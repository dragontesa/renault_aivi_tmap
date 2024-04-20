#ifndef RGAUDIOPLAYERINTERFACE_H
#define RGAUDIOPLAYERINTERFACE_H

#include <QtGlobal>

namespace SKT {

class RGAudioPlayerInterface
{
public:
    virtual ~RGAudioPlayerInterface() {}

    virtual bool isPlaying() const = 0;
    virtual bool play(uint wavId, const char* wavBuffer, size_t wavSize) = 0;
    virtual bool playLastAnnouncement() = 0;
    virtual bool stop() = 0;
    virtual bool audioReady() = 0;
    virtual bool playComplete() = 0;
};

}

#endif // RGAUDIOPLAYERINTERFACE_H
