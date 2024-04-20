#include "simplenaviaudio.h"

#include <qsound.h>
#include <QBuffer>
#include <QAudioBuffer>
#include <QAudioOutput>
#include <QAudioDeviceInfo>

simpleNaviAudio simpleNaviAudio::instance;
simpleNaviAudio& simpleNaviAudio::getInstance(){
    return instance;
}

simpleNaviAudio::simpleNaviAudio(){
    current = 0;
}

simpleNaviAudio::~simpleNaviAudio(){

}

void simpleNaviAudio::initNaviAudio(){
    VSMNavi_SetinitAudioTrackCB(rsm_initAudioTrackCB);
    VSMNavi_SetwriteAudioBufferCB(rsm_writeAudioBufferCB);
    VSMNavi_SetstopAudioTrackCB(rsm_stopAudioTrackCB);
    VSMNavi_SetisPlayingCB(rsm_isPlayingCB);
    //...
}

void simpleNaviAudio::play(){
    stop();
    current = (simpleNaviAudioOutput*) outputs.front();
    if(current != 0){
        outputs.pop();
        current->play();
    }
}

bool simpleNaviAudio::isPlaying(){
    if(current != 0){
        return current->isPlaying();
    }
}

void simpleNaviAudio::stop(){
    if(current != 0){
        current->stop();
        free(current);
        current = 0;
    }
}

void simpleNaviAudio::add(simpleNaviAudioOutput * item){
    outputs.push(item);
    play();
}

simpleNaviAudioOutput::simpleNaviAudioOutput(){
    format.setCodec("audio/pcm");
    format.setByteOrder(QAudioFormat::LittleEndian);
    format.setSampleType(QAudioFormat::UnSignedInt);
}

simpleNaviAudioOutput::~simpleNaviAudioOutput(){
    buffer.close();
    data.clear();
    delete output;
}

void simpleNaviAudioOutput::setData(const char* wave,qint64 size){
    _data = new char[size];
    memcpy(_data,wave,size);
    data.setRawData(_data,size);
    buffer.setData(data);
}

void simpleNaviAudioOutput::setChannel(int channel){
    format.setChannelCount(channel);
}

void simpleNaviAudioOutput::setAudioType(int audioType){

}

void simpleNaviAudioOutput::setSamplingPerSec(int samplingPerSec){
    format.setSampleRate(samplingPerSec);
}

void simpleNaviAudioOutput::setBytesPerSample(int bytesPerSample){
    format.setSampleSize(bytesPerSample*8);
}

void simpleNaviAudioOutput::play(){
    buffer.open(QIODevice::ReadOnly);
    output = new QAudioOutput(format);
//    connect(output,SIGNAL(stateChanged(QAudio::State)),this,SLOT(simpleNaviAudioOutput::stateChange(QAudio::State)));
    output->start(&buffer);
}

bool simpleNaviAudioOutput::isPlaying()
{
    return output->state() == QAudio::ActiveState ? true : false;
}

void simpleNaviAudioOutput::stop()
{
    output->stop();
}

void simpleNaviAudioOutput::stateChange(QAudio::State newState){
    switch(newState){
    case QAudio::IdleState :
        output->stop();
        delete this;
        break;
    case QAudio::StoppedState :
        if(output->error() != QAudio::NoError)
        {
            delete this;
        }
        delete this;
        break;
    }
}






VSM_Bool rsm_initAudioTrackCB()
{
    return VSM_TRUE;
}
VSM_Bool rsm_quitAudioTrackCB()
{
    return VSM_TRUE;
}
VSM_Bool rsm_stopAudioTrackCB(VSM_Int32 value)
{
    simpleNaviAudio::getInstance().stop();
    return VSM_TRUE;
}
VSM_Bool rsm_pauseAudioTrackCB(VSM_Int32 value)
{
    return VSM_TRUE;
}
VSM_Bool rsm_resumeAudioTrackCB(VSM_Int32 value)
{
    return VSM_TRUE;
}
VSM_Bool rsm_isPlayingCB(VSM_Int32 value)
{
    return (VSM_Bool)simpleNaviAudio::getInstance().isPlaying();
}
VSM_Bool rsm_setVolumeCB(VSM_Int32 value)
{
    return VSM_TRUE;
}

VSM_Bool rsm_writeAudioBufferCB(VSM_Int32 nChannel,VSM_Int32 nAudioType,VSM_Byte* pWave,VSM_Int32 samplingPerSec,VSM_Int32 nchannel,VSM_Int32 bytesPerSample,VSM_Int32 nSize)
{
    simpleNaviAudioOutput *play = new simpleNaviAudioOutput();
    play->setBytesPerSample(bytesPerSample);
    play->setChannel(nChannel);
    play->setSamplingPerSec(samplingPerSec);
    play->setData((const char*)pWave,nSize);
    simpleNaviAudio::getInstance().add(play);

    return VSM_TRUE;
}

