/**
 *  \file      WaveFileReader.cpp
 *  \brief     TODO
 *  \author    Fang Pengfei (CM/ESW21-CN)<Pengfei.Fang@cn.bosch.com>
 *  \version   1.0
 *  \date      Jan 25, 2018
 *  \copyright (C) 2018 BOSCH Automotive Products (Suzhou) Co. Ltd.
 */

#include "WaveFileReader.h"

#include <assert.h>
#include <byteswap.h>
#include <endian.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dlt/dlt.h>
#include <QObject>
#include <QDebug>
#define VERBOSE_OUTPUT 1

#if (VERBOSE_OUTPUT)
#define VTRACE printf
#else
#define VTRACE(...) ((void)0)
#endif

/* Definitions for Microsoft WAVE format */
#if __BYTE_ORDER == __LITTLE_ENDIAN
#define COMPOSE_ID(a, b, c, d) ((a) | ((b) << 8) | ((c) << 16) | ((d) << 24))
#define LE_SHORT(v) (v)
#define LE_INT(v) (v)
#define BE_SHORT(v) bswap_16(v)
#define BE_INT(v) bswap_32(v)
#else
#error "!!! Wrong endian platform !!! This code only for little endian arm or x86."
#endif

#define COMPOSE_ID(a, b, c, d) ((a) | ((b) << 8) | ((c) << 16) | ((d) << 24))

static const u_int WAV_RIFF = COMPOSE_ID('R', 'I', 'F', 'F');
static const u_int WAV_WAVE = COMPOSE_ID('W', 'A', 'V', 'E');
static const u_int WAV_FMT = COMPOSE_ID('f', 'm', 't', ' ');
static const u_int WAV_DATA = COMPOSE_ID('d', 'a', 't', 'a');

#define WAV_FMT_PCM 0x0001

/************************************************************************
* 						VARIABLE DEFINITIONS
************************************************************************/
//Defining the Contexts. An application can have multiple contexts
//Context is just a logical grouping of similar functionality
//An application can have any number of Contexts, but with a unique name
//DLT_DECLARE_CONTEXT(NavigationExt_Context);

//DLT_IMPORT_CONTEXT(NavigationExt_Context)

#pragma pack(push, 1)
typedef struct {
    u_int ckID;   /* 'RIFF' */
    u_int ckSize; /* file length */
    u_int WAVEID; /* 'WAVE' */
} WaveHeader;

typedef struct {
    u_int ckID;
    u_int ckSize;
} WaveChunkHeader;

typedef struct {
    u_short wFormatTag;     /* Format code: WAVE_FORMAT_PCM */
    u_short nChannels;      /* Number of interleaved channels */
    u_int nSamplesPerSec;   /* Sampling rate (blocks per second) */
    u_int nAvgBytesPerSec;  /* Data rate */
    u_short nBlockAlign;    /* Data block size (bytes) */
    u_short wBitsPerSample; /* Bits per sample */
} WaveFormat;

typedef struct {
    u_short wFormatTag;     /* Format code: WAVE_FORMAT_PCM */
    u_short nChannels;      /* Number of interleaved channels */
    u_int nSamplesPerSec;   /* Sampling rate (blocks per second) */
    u_int nAvgBytesPerSec;  /* Data rate */
    u_short nBlockAlign;    /* Data block size (bytes) */
    u_short wBitsPerSample; /* Bits per sample */
    u_short cbSize;         /* Size of the extension (0 or 22) */
} WaveFormatEx;
#pragma pack(pop)

void WaveFileReader::ResetData() {
    fd_wave = -1;
    file_size = 0;
    wave_mmap = NULL;
    data_start = NULL;
    data_end = NULL;
    is_valid = false;
    channels = 0;
    sample_rate = 0;
}

WaveFileReader::WaveFileReader() { ResetData(); }

WaveFileReader::~WaveFileReader() {}

bool WaveFileReader::Open(const char *file_name) {
    ////DLT_LOGNavigationExt_Context,DLT_LOG_INFO, DLT_STRING("WaveFileReader::Open() Starts."), DLT_STRING(file_name));
    qDebug() << "WaveFileReader::Open:" << file_name;
    assert(file_name);
    fd_wave = open(file_name, O_RDONLY);
    if (fd_wave == -1) {
        //DLT_LOGNavigationExt_Context,DLT_LOG_INFO, DLT_STRING("Error opening file: "), DLT_STRING(strerror(errno)));
        qDebug() << "WaveFileReader::Error opening file";

        return false;
    }

    struct stat fileInfo = {0};
    if (fstat(fd_wave, &fileInfo) == -1) {
        //DLT_LOGNavigationExt_Context,DLT_LOG_INFO, DLT_STRING("Error getting the file size: "), DLT_STRING(strerror(errno)));
        qDebug() << "WaveFileReader::Error getting the file size";
        return false;
    }
    if (fileInfo.st_size < 44) {
        //DLT_LOGNavigationExt_Context,DLT_LOG_INFO, DLT_STRING("Error: File is too small, nothing to do. "));
        qDebug() << "WaveFileReader::Error: File is too small";
        return false;
    }

    //DLT_LOGNavigationExt_Context,DLT_LOG_INFO, DLT_STRING("File size is: "), DLT_INT(fileInfo.st_size));
    file_size = fileInfo.st_size;
    qDebug() << "WaveFileReader::file_size: "<< file_size;

    wave_mmap = (const char *)mmap(NULL, fileInfo.st_size, PROT_READ, MAP_SHARED, fd_wave, 0);
    if (wave_mmap == MAP_FAILED) {
        close(fd_wave);
        //DLT_LOGNavigationExt_Context,DLT_LOG_INFO, DLT_STRING("Error mmapping the file: "), DLT_STRING(strerror(errno)));
        qDebug() << "WaveFileReader::Error mmapping the file";

        fd_wave = -1;
        file_size = 0;
        return false;
    }

    if (CheckWaveFormat()) {
        is_valid = true;
    } else {
        //DLT_LOGNavigationExt_Context,DLT_LOG_INFO, DLT_STRING("CheckWaveFormat Error, close and do nothing."));
        qDebug() << "WaveFileReader::CheckWaveFormat Error";
        Close();
        return false;
    }

    //DLT_LOGNavigationExt_Context,DLT_LOG_INFO, DLT_STRING("WaveFileReader::Open() Ends."));
    return true;
}

int WaveFileReader::ReadOutData(void *output_buffer, int wanted_bytes) {
    //DLT_LOGNavigationExt_Context,DLT_LOG_INFO, DLT_STRING("WaveFileReader::ReadOutData() Starts."));
    assert(wanted_bytes > 0);
    if (wave_mmap && data_start && data_end) {
        int remain_bytes = data_end - data_start;
        if (remain_bytes <= 0)
            return -1;
        int read_bytes = wanted_bytes < remain_bytes ? wanted_bytes : remain_bytes;
        memcpy(output_buffer, data_start, read_bytes);
        data_start += read_bytes;
        //DLT_LOGNavigationExt_Context,DLT_LOG_INFO, DLT_STRING("Return read bytes."));
        return read_bytes;
    }
    //DLT_LOGNavigationExt_Context,DLT_LOG_INFO, DLT_STRING("WaveFileReader::ReadOutData() Ends."));
    return -1;
}

void WaveFileReader::Close() {
    //DLT_LOGNavigationExt_Context,DLT_LOG_INFO,DLT_STRING("WaveFileReader::Close() Starts."));
    int err = 0;
    if (wave_mmap) {
        if ((err = munmap((void *)wave_mmap, file_size)) == -1) {
            //DLT_LOGNavigationExt_Context,DLT_LOG_INFO,DLT_STRING("Error un-mmapping the file: "), DLT_STRING(strerror(errno)));
        }
    }
    if (fd_wave > 0)
        close(fd_wave);
    ResetData();
    //DLT_LOGNavigationExt_Context,DLT_LOG_INFO,DLT_STRING("WaveFileReader::Close() Ends."));
}

bool WaveFileReader::CheckWaveFormat() {
    //DLT_LOGNavigationExt_Context,DLT_LOG_INFO,DLT_STRING("WaveFileReader::CheckWaveFormat() Starts."));
    qDebug() << "WaveFileReader::CheckWaveFormat() Starts";
    WaveHeader *header = (WaveHeader *)wave_mmap;
    if (header->ckID != WAV_RIFF) {
        //DLT_LOGNavigationExt_Context,DLT_LOG_INFO,DLT_STRING("Not a little endian RIFF file! Doesn't Support!"));
        qDebug() << "WaveFileReader::CheckWaveFormat::Not a little endian RIFF file! Doesn't Support";

        return false;
    }
    if (header->WAVEID != WAV_WAVE) {
        //DLT_LOGNavigationExt_Context,DLT_LOG_INFO,DLT_STRING("Not a little endian WAVE file! Doesn't Support!"));
        qDebug() << "WaveFileReader::CheckWaveFormat::Not a little endian WAVE file! Doesn't Support";

        return false;
    }

    const char *fmt_chunk = NULL;
    const char *next_chunk = wave_mmap + sizeof(WaveHeader);

    bool fmt_got = false;
    bool data_got = false;
    while (1) {
        WaveChunkHeader *pChunk = (WaveChunkHeader *)next_chunk;
        if (pChunk->ckID == WAV_FMT) {
            fmt_chunk = next_chunk;
            fmt_got = true;
        }
        if (pChunk->ckID == WAV_DATA) {
            data_start = next_chunk + sizeof(WaveChunkHeader);
            data_end = wave_mmap + file_size;
            u_int wave_data_size = pChunk->ckSize;
            //DLT_LOGNavigationExt_Context,DLT_LOG_INFO,DLT_STRING("Data size"), DLT_INT(wave_data_size));
            qDebug() << "WaveFileReader::CheckWaveFormat::wave_data_size: "<< wave_data_size;
            if (data_start + wave_data_size > data_end) {
                //DLT_LOGNavigationExt_Context,DLT_LOG_INFO,DLT_STRING("Wave Data size: "), DLT_INT(wave_data_size), DLT_STRING(" ! It overflow file size, try to fix data end to file end."));
                qDebug() << "WaveFileReader::CheckWaveFormat::wave_data_size: "<< wave_data_size << " ! It overflow file size, try to fix data end to file end.";
            }
            data_got = true;
        }
        if (fmt_got && data_got)
            break;

        next_chunk = next_chunk + sizeof(WaveChunkHeader) + pChunk->ckSize;
        if (next_chunk > wave_mmap + file_size)
            break;
    }
    if (!fmt_got) {
        //DLT_LOGNavigationExt_Context,DLT_LOG_INFO,DLT_STRING("Read PCM format error!"));
        qDebug() << "WaveFileReader::CheckWaveFormat::Read PCM format error";
        return false;
    }
    if (!data_got) {
        //DLT_LOGNavigationExt_Context,DLT_LOG_INFO,DLT_STRING("Read PCM data error!"));
        qDebug() << "WaveFileReader::CheckWaveFormat::Read PCM data error";

        return false;
    }

    WaveFormat *fmt = (WaveFormat *)(fmt_chunk + sizeof(WaveChunkHeader));
    if (fmt->wFormatTag != WAV_FMT_PCM) {
        //DLT_LOGNavigationExt_Context,DLT_LOG_INFO,DLT_STRING("NOT a supported raw PCM file!"));
        qDebug() << "WaveFileReader::CheckWaveFormat::NOT a supported raw PCM file";

        return false;
    }
    channels = fmt->nChannels;
    if (channels < 1 || channels > 2) {
        //DLT_LOGNavigationExt_Context,DLT_LOG_INFO,DLT_STRING("Can't play WAVE-files with tracks: "), DLT_INT(channels));
        qDebug() << "WaveFileReader::CheckWaveFormat::Can't play WAVE-files with tracks: " << channels;

        return false;
    }

    if (fmt->wBitsPerSample != 16) {
        //DLT_LOGNavigationExt_Context,DLT_LOG_INFO,DLT_STRING("Can't play WAVE-files with sample bits wide: "), DLT_INT(fmt->wBitsPerSample));
        qDebug() << "WaveFileReader::CheckWaveFormat::Can't play WAVE-files with sample bits wide:" << fmt->wBitsPerSample;

        return false;
    }
    sample_rate = fmt->nSamplesPerSec;
    if (sample_rate != 16000 && sample_rate != 22050 && sample_rate != 44100 && sample_rate != 48000) {
        fprintf(stderr, "Can't play WAVE-files with sample rate %u Hz\n", sample_rate);
        //DLT_LOGNavigationExt_Context,DLT_LOG_INFO,DLT_STRING("Can't play WAVE-files with sample rate: "), DLT_INT(sample_rate), DLT_STRING(" Hz."));
        qDebug() << "WaveFileReader::CheckWaveFormat::Can't play WAVE-files with sample rate: " << sample_rate << " Hz. ";

        return false;
    }

    //DLT_LOGNavigationExt_Context,DLT_LOG_INFO,DLT_STRING("WaveFileReader::CheckWaveFormat() Ends."));
    qDebug() << "WaveFileReader::CheckWaveFormat::WaveFileReader::CheckWaveFormat() Ends.";

    return true;
}
