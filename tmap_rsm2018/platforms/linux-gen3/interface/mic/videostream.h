#ifndef VIDEOSTREAM_H
#define VIDEOSTREAM_H

#include <QThread>
#include <QDebug>
#include <glib.h>

#include <gst/gst.h>
#include <gst/gstbuffer.h>
#include <gst/app/gstappbuffer.h>
#include <gst/app/gstappsrc.h>
#include <gst/gstelement.h>
#include <gst/video/video.h>

#include <QUdpSocket>
#include <QMutex>

class videostream;
struct _App
{
    _App()
    {
        pipeline = NULL;
        appsrc = NULL;
        loop = NULL;
        sourceid = 0;
        timer = NULL;
        mWidth = 800;
        mHeight = 400;
        ImgBuf = NULL;
        m_bBuffer = false;
        framerate = 30;
        m_bRender = false;
        m_bPause = false;
        #ifdef BUILD_TARGET
            mIp = "172.17.0.5";
            mPort =8554;
        #else
            mIp = "127.0.0.1";
            mPort = 5000;
        #endif

        m_bStartStream = false;
        mVideoStream = nullptr;
    }

    GstElement *pipeline;
    GstElement *appsrc;

    GMainLoop *loop;
    guint sourceid;
    int mWidth;
    int mHeight;
    int mImageSize;
    int mBusId;

    QString mIp;
    ushort mPort;
    int mBitrate;
    int mVideoformat;
    int mEncodeType;

    guint8 *ImgBuf;
    guint framerate;

    bool m_bBuffer;
    bool m_bRender;
    bool m_bPause;

    bool m_bStartStream;

    GTimer *timer;

    videostream* mVideoStream;
    QMutex m_mutex;
};

typedef struct _App App;
class videostream : public QThread
{
    Q_OBJECT

public:
    videostream();

    void initPipeline();
    void setStopStream();
    void setStartStream(bool isResume);
    bool isUsingBuffer();
    bool isStartStreaming(int width, int height);

    void setImageSize(int width, int height);
    void setBuffer(uchar *buf, int buffersize);
    void setIp(char *ip);
    void setPort(ushort port);
    void setEncordType(int type);
    void setBitrate(int bitrate);
    void setVideoFormat(int type);

signals:
    void doneStartMapStream();

private:
    void run();
    App s_app;

//    QUdpSocket *udpSocket;
private slots:
    void readPendingDatagrams();

};

#endif // VIDEOSTREAM_H
