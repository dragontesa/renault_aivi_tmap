#pragma once
/**
 *  \file      WaveFilePlayer.h
 *  \brief     TODO
 *  \author    Fang Pengfei (CM/ESW21-CN)<Pengfei.Fang@cn.bosch.com>
 *  \version   1.0
 *  \date      Jan 26, 2018
 *  \copyright (C) 2018 BOSCH Automotive Products (Suzhou) Co. Ltd.
 */

#ifndef WAVEFILEPLAYER_H_
#define WAVEFILEPLAYER_H_

#include "ALSAPlayer.h"
#include "WaveFileReader.h"

#include "ThirdPartyVoiceIF.h"

class WaveFilePlayer : public ALSAPlayerCallback, boost::noncopyable {
public:
    static WaveFilePlayer &GetInst(WaveFilePlayerObserver *observer) {
        static WaveFilePlayer tmp(observer);
        return tmp;
    }

    virtual ~WaveFilePlayer();

    void Play(const char *file_name, const char *pDeviceName);
    void AbortAndWait();
    void readyPlayer();

private:
    WaveFilePlayer(WaveFilePlayerObserver *observer);
    WaveFileReader _wave_file_reader;
    ALSAPlayer _pcm_player;

    virtual int onRequestData(void *buffer, int need_bytes);

    WaveFilePlayerObserver *_observer;
    boost::mutex _mutex;// To protect the members
};

#endif /* WAVEFILEPLAYER_H_ */
