#pragma once
/**
 *  \file      WaveFileReader.h
 *  \brief     TODO
 *  \author    Fang Pengfei (CM/ESW21-CN)<Pengfei.Fang@cn.bosch.com>
 *  \version   1.0
 *  \date      Jan 25, 2018
 *  \copyright (C) 2018 BOSCH Automotive Products (Suzhou) Co. Ltd.
 */

#ifndef WAVEFILEREADER_H_
#define WAVEFILEREADER_H_

#include <boost/noncopyable.hpp>

class WaveFileReader : boost::noncopyable {
public:
    WaveFileReader();
    virtual ~WaveFileReader();

    bool Open(const char *file_name);

    bool IsValidPCMWave() { return is_valid; }
    unsigned short GetChannels() { return channels; }
    unsigned int GetSampleRate() { return sample_rate; }

    int ReadOutData(void *output_buffer, int wanted_bytes);

    void Close();

private:
    void ResetData();
    bool CheckWaveFormat();

    int fd_wave;
    unsigned long file_size;
    const char *wave_mmap;
    const char *data_start;
    const char *data_end;
    bool is_valid;
    unsigned short channels;
    unsigned int sample_rate;
};

#endif /* WAVEFILEREADER_H_ */
