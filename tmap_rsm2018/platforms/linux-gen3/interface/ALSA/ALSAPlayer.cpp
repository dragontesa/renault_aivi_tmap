/**
 *  \file      ALSAPlayer.cpp
 *  \brief     TODO
 *  \author    Fang Pengfei (CM/ESW21-CN)<Pengfei.Fang@cn.bosch.com>
 *  \version   1.0
 *  \date      Jan 25, 2018
 *  \copyright (C) 2018 BOSCH Automotive Products (Suzhou) Co. Ltd.
 */

#include "ALSAPlayer.h"
#include <dlt/dlt.h>
#include <pcm_arate.h>
#include <QObject>
#include <QDebug>

#ifndef NDEBUG
#define DEBUG_DETAIL 1
#define DETAIL_TRACE printf
#else
#define DETAIL_TRACE(...) ((void)0)
#endif

static const int _resample = 1; /* enable alsa-lib resampling */
static const snd_pcm_access_t _access_mode = SND_PCM_ACCESS_RW_INTERLEAVED;
static const size_t PCM_BUFF_SIZE = 16384;

/************************************************************************
* 						VARIABLE DEFINITIONS
************************************************************************/
//Defining the Contexts. An application can have multiple contexts
//Context is just a logical grouping of similar functionality
//An application can have any number of Contexts, but with a unique name
//DLT_DECLARE_CONTEXT(NavigationExt_Context);

//DLT_IMPORT_CONTEXT(NavigationExt_Context)


ALSAPlayer::ALSAPlayer() {
    //DLT_LOGNavigationExt_Context,DLT_LOG_INFO,DLT_STRING("ALSAPlayer::ALSAPlayer() Starts"));
    _pcm_handle = NULL;
    _period_size = 0;
    _buffer_size = 0;
    _pcm_buff = new unsigned char[PCM_BUFF_SIZE];
    assert(_pcm_buff);
    _player_state = STREAM_CLOSED;
    int ret = 0;
//    pthread_attr_t attr;
//    pthread_attr_init(&attr);
//    ret = set_thread_attr(&attr, SCHED_RR, 99);
//    if (ret || (ret = pthread_create(&_thread_id, &attr, ALSAPlayer::alsa_working_thread, this))) {
        ret = pthread_create(&_thread_id, NULL, ALSAPlayer::alsa_working_thread, this);
//    }
    if (ret) {
        //DLT_LOGNavigationExt_Context,DLT_LOG_INFO,DLT_STRING("Can NOT create working thread!!!"));
        abort();
    }
    pthread_setname_np(_thread_id, "alsa_working_thread");
//    pthread_attr_destroy(&attr);
    //DLT_LOGNavigationExt_Context,DLT_LOG_INFO,DLT_STRING("ALSAPlayer::ALSAPlayer() Ends"));
}

ALSAPlayer::~ALSAPlayer() {
    if (STREAM_RUNNING == _player_state)
        Stop(true);
    if (STREAM_STOPPED == _player_state)
        Close();

    delete[] _pcm_buff;
    pthread_cancel(_thread_id);
    pthread_join(_thread_id, NULL);
}

bool ALSAPlayer::Open(const StreamOptions &options, ALSAPlayerCallback *callback) {
    //DLT_LOGNavigationExt_Context,DLT_LOG_INFO,DLT_STRING("ALSAPlayer::Open() Starts"));
    qDebug() << "ALSAPlayer::Open";
    assert(callback != NULL);
    //DLT_LOGNavigationExt_Context,DLT_LOG_INFO,DLT_STRING("-->OpenStream"));
    qDebug() << "ALSAPlayer::OpenStream";

    _sync_event.WaitForThreadReady();
    if (STREAM_CLOSED != _player_state) {
        qDebug() << "ALSAPlayer::Stream already opened";

        //DLT_LOGNavigationExt_Context,DLT_LOG_INFO,DLT_STRING("Stream already opened!"));
        return true;
    }

    _stream_options = options;
    _user_callback = callback;

    std::string device_name;
    if (-1 == _stream_options.device_number)
        device_name = "default";
    else if (-2 == _stream_options.device_number)
        device_name = _stream_options.device_name;
    else if (0 <= _stream_options.device_number) {
        char num[16];
        snprintf(num, 16, "%d", _stream_options.device_number);
        device_name = "plughw:";
        device_name.append(num);
    } else {
        qDebug() << "ALSAPlayer::Unsupported device number";
        //DLT_LOGNavigationExt_Context,DLT_LOG_INFO,DLT_STRING("Unsupported device number!"));
        return false;
    }

    int err = 0;
    do {
        if ((err = snd_pcm_open(&_pcm_handle, device_name.c_str(), SND_PCM_STREAM_PLAYBACK, 0)) < 0) {
            //DLT_LOGNavigationExt_Context,DLT_LOG_INFO,DLT_STRING("Playback open error: "), DLT_STRING(snd_strerror(err)),DLT_STRING("DeviceName: "),DLT_STRING(device_name.c_str()));
            qDebug() << "ALSAPlayer::Playback open error: " << snd_strerror(err) << "DeviceName: " << device_name.c_str();

            break;
        }
        if ((err = set_hwparams()) < 0) {
            //DLT_LOGNavigationExt_Context,DLT_LOG_INFO,DLT_STRING("Setting of hwparams failed: "), DLT_STRING(snd_strerror(err)));
            qDebug() << "Setting of hwparams failed";
            snd_pcm_close(_pcm_handle);
            break;
        }
        if ((err = set_swparams()) < 0) {
            //DLT_LOGNavigationExt_Context,DLT_LOG_INFO,DLT_STRING("Setting of swparams failed: "), DLT_STRING(snd_strerror(err)));
            qDebug() << "Setting of swparams failed";
            snd_pcm_close(_pcm_handle);
            break;
        }
        size_t real_need_bytes = snd_pcm_frames_to_bytes(_pcm_handle, _period_size);
        if (real_need_bytes > PCM_BUFF_SIZE) {
            fprintf(stderr, "Prealloced buff is not enough, realloc the buff\n");
            delete[] _pcm_buff;
            _pcm_buff = new unsigned char[real_need_bytes];
            assert(_pcm_buff);
        }
#if (DEBUG_DETAIL)
        snd_output_t *output = NULL;
        err = snd_output_stdio_attach(&output, stdout, 0);
        if (err < 0) {
            qDebug() << "Output failed";

            //DLT_LOGNavigationExt_Context,DLT_LOG_INFO,DLT_STRING("Output failed: "), DLT_STRING(snd_strerror(err)));
            break;
        }
        snd_pcm_dump(_pcm_handle, output);
#endif
    } while (0);
    _player_state = STREAM_STOPPED;
    qDebug() << "ALSAPlayer::Open() Ends";

    //DLT_LOGNavigationExt_Context,DLT_LOG_INFO,DLT_STRING("ALSAPlayer::Open() Ends"));
    return err == 0 ? true : false;
}

int ALSAPlayer::set_hwparams() {
    //DLT_LOGNavigationExt_Context,DLT_LOG_INFO,DLT_STRING("ALSAPlayer::set_hwparams Starts"));
	
	int err = 0;
    do {
        snd_pcm_hw_params_t *hwparams;
        snd_pcm_hw_params_alloca(&hwparams);
        unsigned int rrate;
        /* choose all parameters */
        err = snd_pcm_hw_params_any(_pcm_handle, hwparams);
        if (err < 0) {
            qDebug() << "ALSAPlayer::Broken configuration for playback: no configurations available";

            //DLT_LOGNavigationExt_Context,DLT_LOG_INFO,DLT_STRING("Broken configuration for playback: no configurations available: "), DLT_STRING(snd_strerror(err)));
            return err;
        }

        err = snd_pcm_hw_params_set_rate_resample(_pcm_handle, hwparams, _resample);
        if (err < 0) {
            qDebug() << "ALSAPlayer::Resampling setup failed for playback";

            //DLT_LOGNavigationExt_Context,DLT_LOG_INFO,DLT_STRING("Resampling setup failed for playback: "), DLT_STRING(snd_strerror(err)));
            return err;
        }
        /* set the interleaved read/write format */
        err = snd_pcm_hw_params_set_access(_pcm_handle, hwparams, _access_mode);
        if (err < 0) {
            qDebug() << "ALSAPlayer::Access type not available for playback";

            //DLT_LOGNavigationExt_Context,DLT_LOG_INFO,DLT_STRING("Access type not available for playback: "), DLT_STRING(snd_strerror(err)));
            return err;
        }
        /* set the sample format */
        err = snd_pcm_hw_params_set_format(_pcm_handle, hwparams, _stream_options.format);
        if (err < 0) {
            qDebug() << "ALSAPlayer::Sample format not available for playback";

            //DLT_LOGNavigationExt_Context,DLT_LOG_INFO,DLT_STRING("Sample format not available for playback:"), DLT_STRING(snd_strerror(err)));
            return err;
        }
        /* set the count of channels */
        err = snd_pcm_hw_params_set_channels(_pcm_handle, hwparams, _stream_options.channels);
        if (err < 0) {
            qDebug() << "ALSAPlayer::Channels count:" << _stream_options.channels;
            qDebug() << "ALSAPlayer::snd_strerror: " << snd_strerror(err);


            //DLT_LOGNavigationExt_Context,DLT_LOG_INFO,DLT_STRING("Channels count "), DLT_INT(_stream_options.channels), DLT_STRING(" are not available for playbacks: "), DLT_STRING(snd_strerror(err)));
            return err;
        }
        /* set the stream rate */
        //#ifdef __arm__
        //        switch (m_options.sampleRate) {
        //        case 22050:
        //        case 44100:
        //            periodSize = 220;
        //            break;
        //        case 48000:
        //        case 16000:
        //        default:
        //            periodSize = 160;
        //            break;
        //        }
        //#else
        //        switch (m_options.sampleRate) {
        //        case 22050:
        //            periodSize = 2000;
        //            break;
        //        case 44100:
        //            periodSize = 4000;
        //            break;
        //        case 48000:
        //            periodSize = 4000;
        //            break;
        //        case 16000:
        //        default:
        //            periodSize = 1600;
        //            break;
        //        }
        //#endif

        rrate = _stream_options.sample_rate;
        err = snd_pcm_hw_params_set_rate_near(_pcm_handle, hwparams, &rrate, 0);
        if (err < 0) {
            qDebug() << "ALSAPlayer::is not available for playbacks";

            //DLT_LOGNavigationExt_Context,DLT_LOG_INFO,DLT_STRING("Rate "), DLT_INT(_stream_options.sample_rate), DLT_STRING(" is not available for playbacks: "), DLT_STRING(snd_strerror(err)));
            return err;
        }
        if (rrate != _stream_options.sample_rate) {
            //DLT_LOGNavigationExt_Context, DLT_LOG_INFO, DLT_STRING("Rate does not match. The requested is: "), DLT_INT(_stream_options.sample_rate), DLT_STRING(". But, get: "), DLT_INT(rrate));
            return -EINVAL;
        }

        unsigned int periods = 2;
        err = snd_pcm_hw_params_set_periods_min(_pcm_handle, hwparams, &periods, 0);
        unsigned int buffer_time = 0;
        unsigned int period_time = 0;
        err = snd_pcm_hw_params_get_buffer_time_max(hwparams, &buffer_time, 0);
        assert(err >= 0);
        if (buffer_time > 400000)
            buffer_time = 400000;
        period_time = buffer_time / 4;

        err = snd_pcm_hw_params_set_period_time_near(_pcm_handle, hwparams, &period_time, 0);
        if (err < 0) {
            qDebug() << "ALSAPlayer::Unable to set period time for playback:";

            //DLT_LOGNavigationExt_Context, DLT_LOG_INFO, DLT_STRING("Unable to set period time for playback: "), DLT_STRING(snd_strerror(err)));
            return err;
        }
        err = snd_pcm_hw_params_set_buffer_time_near(_pcm_handle, hwparams, &buffer_time, 0);
        if (err < 0) {
            qDebug() << "ALSAPlayer::Unable to set buffer time for playback";

            //DLT_LOGNavigationExt_Context, DLT_LOG_INFO, DLT_STRING("Unable to set buffer time for playback: "), DLT_STRING(snd_strerror(err)));
            return err;
        }

        snd_pcm_hw_params_get_period_size(hwparams, &_period_size, 0);
        snd_pcm_hw_params_get_buffer_size(hwparams, &_buffer_size);
        snd_pcm_hw_params_get_periods(hwparams, &periods, 0);

        DETAIL_TRACE("buffer size: %lu, period size: %lu, periods: %u\n", _buffer_size, _period_size, periods);
        //DLT_LOGNavigationExt_Context, DLT_LOG_INFO, DLT_STRING("Buffer Size: "), DLT_INT(set_swparams() Starts5_buffer_size));
        //DLT_LOGNavigationExt_Context, DLT_LOG_INFO, DLT_STRING("Period Size: "), DLT_INT(_period_size));
        //DLT_LOGNavigationExt_Context, DLT_LOG_INFO, DLT_STRING("Periods: "), DLT_INT(periods));
        qDebug() << "ALSAPlayer::Buffer Size: " << _buffer_size ;
        qDebug() << "ALSAPlayer::Period Size: " << _period_size;
        qDebug() << "ALSAPlayer::Periods: " << periods;

        /* write the parameters to device */
        err = snd_pcm_hw_params(_pcm_handle, hwparams);
        if (err < 0) {
            qDebug() << "ALSAPlayer::Unable to set hw params for playback";

            //DLT_LOGNavigationExt_Context, DLT_LOG_INFO, DLT_STRING("Unable to set hw params for playback: "), DLT_STRING(snd_strerror(err)));
            return err;
        }
    } while (0);

    return 0;
    //DLT_LOGNavigationExt_Context,DLT_LOG_INFO,DLT_STRING("ALSAPlayer::set_hwparams Ends"));
}

int ALSAPlayer::set_swparams() {
    qDebug() << "ALSAPlayer::set_swparams() Starts";

    //DLT_LOGNavigationExt_Context, DLT_LOG_INFO, DLT_STRING("ALSAPlayer::set_swparams() Starts"));
    snd_pcm_sw_params_t *swparams;
    snd_pcm_sw_params_alloca(&swparams);

    int err;
    /* get the current swparams */
    err = snd_pcm_sw_params_current(_pcm_handle, swparams);
    if (err < 0) {
        qDebug() << "ALSAPlayer::set_swparams() Starts1";

        //DLT_LOGNavigationExt_Context, DLT_LOG_INFO, DLT_STRING("Unable to determine current swparams for playback: "), DLT_STRING(snd_strerror(err)));
        return err;
    }
    err = snd_pcm_sw_params_set_start_threshold(_pcm_handle, swparams, _buffer_size);
    if (err < 0) {
        qDebug() << "ALSAPlayer::set_swparams() Starts2";
        //DLT_LOGNavigationExt_Context, DLT_LOG_INFO, DLT_STRING("Unable to set start threshold mode for playback: "), DLT_STRING(snd_strerror(err)));
        return err;
    }
    err = snd_pcm_sw_params_set_avail_min(_pcm_handle, swparams, _period_size);
    if (err < 0) {
        qDebug() << "ALSAPlayer::set_swparams() Starts3";
        //DLT_LOGNavigationExt_Context, DLT_LOG_INFO, DLT_STRING("Unable to set avail min for playback: "), DLT_STRING(snd_strerror(err)));
        return err;
    }

    /* write the parameters to the playback device */
    err = snd_pcm_sw_params(_pcm_handle, swparams);
    if (err < 0) {
        qDebug() << "ALSAPlayer::set_swparams() Starts4";
        //DLT_LOGNavigationExt_Context, DLT_LOG_INFO, DLT_STRING("Unable to set sw params for playback: "), DLT_STRING(snd_strerror(err)));
        return err;
    }
    qDebug() << "ALSAPlayer::set_swparams() Starts5";
    //DLT_LOGNavigationExt_Context, DLT_LOG_INFO, DLT_STRING("ALSAPlayer::set_swparams() Ends"));
    return 0;
}

bool ALSAPlayer::Start() {
    qDebug() << "ALSAPlayer::Start() starts. -->StartStream";
    //DLT_LOGNavigationExt_Context, DLT_LOG_INFO, DLT_STRING("ALSAPlayer::Start() starts. -->StartStream"));
    if (STREAM_CLOSED == _player_state) {
        qDebug() << "ALSAPlayer::Start() starts. -->STREAM_CLOSED";
        //DLT_LOGNavigationExt_Context, DLT_LOG_INFO, DLT_STRING("ALSAPlayer::Start(): Haven't opened stream!"));
        return false;
    }
    if (STREAM_RUNNING == _player_state) {
        qDebug() << "ALSAPlayer::Start() starts. -->STREAM_RUNNING";
        //DLT_LOGNavigationExt_Context, DLT_LOG_INFO, DLT_STRING("The stream is already running!"));
        return true;
    }
    _player_state = STREAM_RUNNING;
    _sync_event.SignalEvent();
    qDebug() << "ALSAPlayer::Start() Ends";
    //DLT_LOGNavigationExt_Context, DLT_LOG_INFO, DLT_STRING("ALSAPlayer::Start() Ends."));
    return true;
}

bool ALSAPlayer::Stop(bool immediately) {
    qDebug() << "ALSAPlayer::Stop() starts. -->StopStream";
    //DLT_LOGNavigationExt_Context, DLT_LOG_INFO, DLT_STRING("ALSAPlayer::Stop() starts. -->StopStream"));
    if (STREAM_CLOSED == _player_state) {
        qDebug() << "ALSAPlayer::Stop() starts. -->StopStream1";
        //DLT_LOGNavigationExt_Context, DLT_LOG_INFO, DLT_STRING("ALSAPlayer::Stop(): Haven't opened stream"));
        return false;
    }
    if (STREAM_STOPPED == _player_state) {
        qDebug() << "ALSAPlayer::Stop() starts. -->StopStream2";
        //DLT_LOGNavigationExt_Context, DLT_LOG_INFO, DLT_STRING("The stream is already stopped!"));
        return true;
    }
    _player_state = STREAM_STOPPED;

    if (pthread_self() != _thread_id) {
        _sync_event.SignalEvent(true);
        _sync_event.WaitForEventReturn();
    }

    //DLT_LOGNavigationExt_Context, DLT_LOG_INFO, DLT_STRING("ALSAPlayer::Stop() Ends."));
    return __Stop(immediately);
}

bool ALSAPlayer::__Stop(bool immediately) {
    qDebug() << "ALSAPlayer::__Stop() Starts";
    //DLT_LOGNavigationExt_Context, DLT_LOG_INFO, DLT_STRING("ALSAPlayer::__Stop() Starts"));
    int err = 0;
    if (immediately)
        err = snd_pcm_drop(_pcm_handle);
    else
        err = snd_pcm_drain(_pcm_handle);
    if (err < 0) {
        qDebug() << "ALSAPlayer::__Stop() Starts1";
        //DLT_LOGNavigationExt_Context, DLT_LOG_INFO, DLT_STRING("Error drop/drain: "), DLT_STRING(snd_strerror(err)));
        return false;
    }
    qDebug() << "ALSAPlayer::__Stop() Starts2";
    //DLT_LOGNavigationExt_Context, DLT_LOG_INFO, DLT_STRING("ALSAPlayer::__Stop() Ends"));
    return true;
}

bool ALSAPlayer::Close() {
    qDebug() << "ALSAPlayer::Close() Starts: -->CloseStream";
    //DLT_LOGNavigationExt_Context, DLT_LOG_INFO, DLT_STRING("ALSAPlayer::Close() Starts: -->CloseStream"));
    if (STREAM_CLOSED == _player_state) {
        qDebug() << "ALSAPlayer::Close() Starts: -->CloseStream1";
        //DLT_LOGNavigationExt_Context, DLT_LOG_INFO, DLT_STRING("No open stream to close!"));
        return true;
    }
    if (STREAM_RUNNING == _player_state) {
        Stop(true);
    }
    int err = 0;
    if ((err = snd_pcm_close(_pcm_handle)) < 0) {
        qDebug() << "ALSAPlayer::Close() Starts: -->CloseStream2";
        //DLT_LOGNavigationExt_Context, DLT_LOG_INFO, DLT_STRING("Close PCM Failed: "), DLT_STRING(snd_strerror(err)));
        return false;
    }
    _player_state = STREAM_CLOSED;
    qDebug() << "ALSAPlayer::Close() Starts: -->CloseStream3";
    //DLT_LOGNavigationExt_Context, DLT_LOG_INFO, DLT_STRING("ALSAPlayer::Close() Ends."));
    return true;
}

int ALSAPlayer::xrun_recovery(int err) {
    qDebug() << "ALSAPlayer::xrun_recovery() Starts";
    //DLT_LOGNavigationExt_Context, DLT_LOG_INFO, DLT_STRING("ALSAPlayer::xrun_recovery() Starts."));
    err = err > 0 ? err : -err;
    err = snd_pcm_recover(_pcm_handle, err, 0);
    if (err < 0)
        qDebug() << "ALSAPlayer::xrun_recovery() Starts1";
        //DLT_LOGNavigationExt_Context, DLT_LOG_INFO, DLT_STRING("Can't recovery from underrun, prepare failed: "), DLT_STRING(snd_strerror(err)));
    //DLT_LOGNavigationExt_Context, DLT_LOG_INFO, DLT_STRING("ALSAPlayer::xrun_recovery() Ends."));
    return err;
}

void *ALSAPlayer::alsa_working_thread(void *arg) {
    qDebug() << "*ALSAPlayer::alsa_working_thread() Starts.";
    //DLT_LOGNavigationExt_Context, DLT_LOG_INFO, DLT_STRING("*ALSAPlayer::alsa_working_thread() Starts."));
    if (arg != NULL) {
        ALSAPlayer *pThis = static_cast<ALSAPlayer *>(arg);
        pThis->working_loop();
    }
    qDebug() << "*ALSAPlayer::alsa_working_thread() Starts1.";
    //DLT_LOGNavigationExt_Context, DLT_LOG_INFO, DLT_STRING("*ALSAPlayer::alsa_working_thread() Ends."));
    return NULL;
}

void ALSAPlayer::working_loop() {
    qDebug() << "ALSAPlayer::working_loop() Starts.";
    //DLT_LOGNavigationExt_Context, DLT_LOG_INFO, DLT_STRING("ALSAPlayer::working_loop() Starts"));
    _sync_event.SignalThreadReady();

    int err = 0;

    while (1) { // Loop for waiting
        qDebug() << "alsa_working_thread is waiting...";
        //DLT_LOGNavigationExt_Context, DLT_LOG_INFO, DLT_STRING("alsa_working_thread is waiting..."));
        int normal_end = 0;
        _sync_event.WaitForEvent();
        qDebug() << "alsa_working_thread wakes up...";
        //DLT_LOGNavigationExt_Context, DLT_LOG_INFO, DLT_STRING("alsa_working_thread wakes up..."));
        pthread_testcancel();
        if (_player_state != STREAM_RUNNING) {
            continue;
        }

        int first_block = 1;
        while (2) { // read and play loop0
            if (_player_state != STREAM_RUNNING)
                break;
            if (err < 0) {
                const snd_pcm_state_t state = snd_pcm_state(_pcm_handle);
                if (state == SND_PCM_STATE_XRUN || state == SND_PCM_STATE_SUSPENDED) {
                    err = (state == SND_PCM_STATE_XRUN) ? -EPIPE : -ESTRPIPE;
                    if (xrun_recovery(err) < 0) {
                        qDebug() << "ALSAPlayer::working_loop::Stream Recovery Error: " << snd_strerror(err);
                        //DLT_LOGNavigationExt_Context, DLT_LOG_INFO, DLT_STRING("Stream Recovery Error: "), DLT_STRING(snd_strerror(err)));
                        __Stop(true);
                        _player_state = STREAM_STOPPED;
                        break;
                    }
                } else {
                    qDebug() << "alsa_working_thread is waiting...2";
                    //DLT_LOGNavigationExt_Context, DLT_LOG_INFO, DLT_STRING("Wait for poll failed"));
                    __Stop(true);
                    _player_state = STREAM_STOPPED;
                    break;
                }
            }

            int read_bytes = _user_callback->onRequestData(_pcm_buff, snd_pcm_frames_to_bytes(_pcm_handle, _period_size));
//            qDebug() << "working_loop read_bytes: " << read_bytes;

            if (_player_state != STREAM_RUNNING)
                break;
            if (read_bytes <= 0) {
                __Stop(false);
                break;
            }

            snd_pcm_sframes_t real_frames = snd_pcm_bytes_to_frames(_pcm_handle, read_bytes);
//            qDebug() << "working_loop real_frames: " << (long)real_frames;

            unsigned char *ptr = _pcm_buff;
            while (real_frames > 0) {
                err = snd_pcm_writei(_pcm_handle, ptr, real_frames);
                if (err == -EAGAIN)
                    continue;
                if (err < 0) {
                    if (xrun_recovery(err) < 0) {
//                        qDebug() << "ALSAPlayer::working_loop::Write error: " << snd_strerror(err);
                        fprintf(stderr, "Write error: %s\n", snd_strerror(err));
                        __Stop(true);
                        _player_state = STREAM_STOPPED;
                        break;
                    }
                    break; /* skip one period */
                }
                ptr += snd_pcm_frames_to_bytes(_pcm_handle, _period_size);
                real_frames -= err;
            }

            if (first_block) {
                err = snd_pcm_start(_pcm_handle);
                if (err < 0) {
                    const snd_pcm_state_t state = snd_pcm_state(_pcm_handle);
                    if (state == SND_PCM_STATE_XRUN || state == SND_PCM_STATE_SUSPENDED) {
                        err = (state == SND_PCM_STATE_XRUN) ? -EPIPE : -ESTRPIPE;
                        if (xrun_recovery(err) < 0) {
                            qDebug() << "alsa_working_thread is waiting...3";
                         //DLT_LOGNavigationExt_Context, DLT_LOG_INFO, DLT_STRING("Stream recovery error: "), DLT_STRING(snd_strerror(err)));
						}
                    } else {
//                        qDebug() << "alsa_working_thread is waiting...4";
                        //DLT_LOGNavigationExt_Context, DLT_LOG_INFO, DLT_STRING("Start Error: "), DLT_STRING(snd_strerror(err)));
                        //DLT_LOGNavigationExt_Context, DLT_LOG_INFO, DLT_STRING("State: "), DLT_STRING(snd_pcm_state_name(state)));
                    }
                }
                first_block = 0;
            }
            pthread_testcancel();
        }
        if (_sync_event.NeedReturn())
            _sync_event.SignalEventReturn();
        qDebug() << "alsa_working_thread Out of Poll Loop";
        //DLT_LOGNavigationExt_Context, DLT_LOG_INFO, DLT_STRING("Out of Poll Loop."));
    }
    //DLT_LOGNavigationExt_Context, DLT_LOG_INFO, DLT_STRING("Out of Working Loop."));
    //DLT_LOGNavigationExt_Context, DLT_LOG_INFO, DLT_STRING("ALSAPlayer::working_loop() Ends"));
}
