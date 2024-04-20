/**
 *  \file      WaveFilePlayer.cpp
 *  \brief     TODO
 *  \author    Fang Pengfei (CM/ESW21-CN)<Pengfei.Fang@cn.bosch.com>
 *  \version   1.0
 *  \date      Jan 26, 2018
 *  \copyright (C) 2018 BOSCH Automotive Products (Suzhou) Co. Ltd.
 */

#include "WaveFilePlayer.h"
#include <dlt/dlt.h>
#include <QDebug>
#include <QObject>

#ifdef __arm__
static const char *device = "AdevLxcSrcSelect"; /* playback device */
#else
static const char *device = "plughw:0,0"; /* playback device */
#endif

/************************************************************************
* 						VARIABLE DEFINITIONS
************************************************************************/
//Defining the Contexts. An application can have multiple contexts
//Context is just a logical grouping of similar functionality
//An application can have any number of Contexts, but with a unique name
//DLT_DECLARE_CONTEXT(NavigationExt_Context);

//DLT_IMPORT_CONTEXT(NavigationExt_Context)

WaveFilePlayer::WaveFilePlayer(WaveFilePlayerObserver *observer) : _observer(observer) {}

WaveFilePlayer::~WaveFilePlayer() {}

void WaveFilePlayer::Play(const char *file_name, const char *pDeviceName) {
    //DLT_LOGNavigationExt_Context,DLT_LOG_INFO,DLT_STRING("WaveFilePlayer::Play() Starts"), DLT_STRING(file_name), DLT_STRING(pDeviceName));
    qDebug() << "WaveFilePlayer::Play";
    boost::mutex::scoped_lock lock(_mutex);
    if (_wave_file_reader.Open(file_name)) {
        //DLT_LOGNavigationExt_Context,DLT_LOG_INFO,DLT_STRING("Wave Fiel Opens."));
        StreamOptions stream_options;
        stream_options.device_name = pDeviceName;
        stream_options.channels = _wave_file_reader.GetChannels();
        stream_options.sample_rate = _wave_file_reader.GetSampleRate();
        qDebug() << "WaveFilePlayer::device_name: " << QString::fromStdString(stream_options.device_name);
        qDebug() << "WaveFilePlayer::channels: " << stream_options.channels;
        qDebug() << "WaveFilePlayer::sample_rate:" << stream_options.sample_rate;
        if (_pcm_player.Open(stream_options, this)) {
            qDebug() << "_pcm_player.Open Success";
            _pcm_player.Start();
        }
    } else {
        qDebug() << "WaveFilePlayer::_wave_file_reader.Open Fail";
    }

    //DLT_LOGNavigationExt_Context,DLT_LOG_INFO,DLT_STRING("WaveFilePlayer::Play() Ends"));
}

void WaveFilePlayer::AbortAndWait() {
	boost::mutex::scoped_lock lock(_mutex);

    _pcm_player.Stop(true);
    _pcm_player.Close();
    _wave_file_reader.Close();
}

void WaveFilePlayer::readyPlayer(){
    boost::mutex::scoped_lock lock(_mutex);
    _pcm_player.Stop(false);

    _pcm_player.Close();
    _wave_file_reader.Close();
    _observer->onPlaybackCompleted();
}

int WaveFilePlayer::onRequestData(void *buffer, int need_bytes) {
	boost::mutex::scoped_lock lock(_mutex);
    int read_bytes = _wave_file_reader.ReadOutData(buffer, need_bytes);
    if (read_bytes <= 0) {
        // data end
        _pcm_player.Stop(false);
        _pcm_player.Close();
        _wave_file_reader.Close();
        _observer->onPlaybackCompleted();
    }
    return read_bytes;
}
