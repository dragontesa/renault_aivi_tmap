#ifndef SIMPLENAVIAUDIO_H
#define SIMPLENAVIAUDIO_H

#include <QObject>
#include <qsound.h>
#include <QBuffer>
#include <QAudioBuffer>
#include <QAudioOutput>
#include <QAudioDeviceInfo>

#include <queue>

#include "VSM_Navi_Audio.h"

VSM_Bool rsm_initAudioTrackCB();
VSM_Bool rsm_quitAudioTrackCB();
VSM_Bool rsm_stopAudioTrackCB(VSM_Int32 value);
VSM_Bool rsm_pauseAudioTrackCB(VSM_Int32 value);
VSM_Bool rsm_resumeAudioTrackCB(VSM_Int32 value);
VSM_Bool rsm_isPlayingCB(VSM_Int32 value);
VSM_Bool rsm_setVolumeCB(VSM_Int32 value);
VSM_Bool rsm_writeAudioBufferCB(VSM_Int32 nChannel,VSM_Int32 nAudioType,VSM_Byte* pWave,VSM_Int32 samplingPerSec,VSM_Int32 nchannel,VSM_Int32 bytesPerSample,VSM_Int32 nSize);

class simpleNaviAudioOutput : public QObject{

public:
    simpleNaviAudioOutput();
    ~simpleNaviAudioOutput();

private:
    QAudioOutput *output;
    QAudioFormat format;
    QBuffer buffer;
    QByteArray data;
    char *_data;

public:
    void setData(const char* pWave,qint64 nSize);
    void setChannel(int channel);
    void setAudioType(int audioType);
    void setSamplingPerSec(int samplingPerSec);
    void setBytesPerSample(int bytesPerSample);

    void play();
    bool isPlaying();
    void stop();

public slots:
    void stateChange(QAudio::State);

signals:

};

class simpleNaviAudio
{
public:
    static simpleNaviAudio& getInstance();
    void initNaviAudio();
    void play();
    bool isPlaying();
    void stop();
    void add(simpleNaviAudioOutput * item);

//Q_SIGNALS:
//    void addPlayList();

private:
    static simpleNaviAudio instance;
    simpleNaviAudio();
    ~simpleNaviAudio();

    simpleNaviAudioOutput *current;
    std::queue<simpleNaviAudioOutput*> outputs;

};

#endif // SIMPLENAVIAUDIO_H
