#pragma once
/**
 *  \file      ALSAPlayer.h
 *  \brief     TODO
 *  \author    Fang Pengfei (CM/ESW21-CN)<Pengfei.Fang@cn.bosch.com>
 *  \version   1.0
 *  \date      Jan 25, 2018
 *  \copyright (C) 2018 BOSCH Automotive Products (Suzhou) Co. Ltd.
 */

#ifndef ALSAPLAYER_H_
#define ALSAPLAYER_H_

#include <alsa/asoundlib.h>
#include <string>

#include "ThreadSync.h"
#include "use_boost_header_only.hpp"

class ALSAPlayerCallback {
public:
    ALSAPlayerCallback(){};
    virtual ~ALSAPlayerCallback(){};

    /// DON'T BLOCK this Callback, DON'T call any ALSAPlayer API in Callback, such as Open(), Start()
    virtual int onRequestData(void *buffer, int need_bytes) = 0;
};

struct StreamOptions {
    int device_number;       /*-2:use name, -1, use default, >=0:use plughw:x */
    std::string device_name; // optional
    snd_pcm_format_t format;
    unsigned int sample_rate;
    unsigned int channels;
    // Default constructor.
    StreamOptions() : device_number(-2), device_name("AdevExtNavMixOut"), format(SND_PCM_FORMAT_S16_LE), sample_rate(48000), channels(1) {}
    // Copy constructor
    StreamOptions(const StreamOptions &src) {
        device_number = src.device_number;
        device_name = src.device_name;
        format = src.format;
        sample_rate = src.sample_rate;
        channels = src.channels;
    }
    StreamOptions &operator=(const StreamOptions &src) {
        device_number = src.device_number;
        device_name = src.device_name;
        format = src.format;
        sample_rate = src.sample_rate;
        channels = src.channels;
        return *this;
    }
};

class ALSAPlayer : boost::noncopyable {
public:
    ALSAPlayer();
    virtual ~ALSAPlayer();

    bool Open(const StreamOptions &options, ALSAPlayerCallback *callback);
    bool Start();
    bool Stop(bool immediately);
    bool Close();

private:
    typedef enum { STREAM_CLOSED = 0, STREAM_STOPPED, STREAM_RUNNING } StreamState;
    boost::atomic<StreamState> _player_state;

    snd_pcm_t *_pcm_handle;
    StreamOptions _stream_options;
    int set_hwparams();
    int set_swparams();
    snd_pcm_uframes_t _period_size;
    snd_pcm_uframes_t _buffer_size;
    unsigned char *_pcm_buff;
    bool __Stop(bool immediately);

    ALSAPlayerCallback *_user_callback;
    int xrun_recovery(int err);

    pthread_t _thread_id;
    ThreadSync _sync_event;
    static void *alsa_working_thread(void *arg);
    void working_loop();
};

#endif /* ALSAPLAYER_H_ */
