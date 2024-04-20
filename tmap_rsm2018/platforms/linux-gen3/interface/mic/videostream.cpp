

#include "videostream.h"

GST_DEBUG_CATEGORY (appsrc_pipeline_debug);
#define GST_CAT_DEFAULT appsrc_pipeline_debug

//GstBuffer *m_buffer;
//#include "QDateTime"
//quint64 tempTime = 0;
//quint64 AccuTime = 0;
//int fps = 0;
int cnt = 0;

//static gboolean read_data (App * app)
//{
//   qDebug() << ":::::: read_data :::::: #1 ";
//   gboolean ok = TRUE;

//   if(app)
//   {
//       app->m_mutex.lock();
//       app->m_bBuffer = false;

////       if(app->m_bRender)
//       {

//               GstFlowReturn ret = GST_FLOW_OK;
//               static GstClockTime timestamp = 0;
//               qDebug() << "20191021 :::::: read_data :::::: #2 " << app->mWidth << app->mHeight << app->mImageSize <<"(" << cnt <<")" ;

//               GstBuffer *m_buffer = gst_buffer_try_new_and_alloc(app->mImageSize);

//               qDebug() << "mic test :: m_buffer =" << (void*)m_buffer;
////               if(m_buffer == NULL)
////               {
////                   app->m_mutex.unlock();
////                   return false;
////               }

////               qDebug() << "mic test :: m_buffer->data =" << (void*)GST_BUFFER_DATA(m_buffer);
////               if(m_buffer->data == NULL)
////               {
////                   app->m_mutex.unlock();
////                   return false;
////               }

//               // 포인터 주소 값 복사가 아닌, 실제 값을 복사하는 메모리 복사 방식으로 변경.
//               // 포인터 주소 값을 복사하게 되면, m_buffer->data 가 원래 갖고 있던 주소를 잃어버리게 되어서 메모리릭이 발생함. 2019.11.30 by hheo
////               memcpy(GST_BUFFER_DATA(m_buffer), app->ImgBuf, app->mImageSize);
//               GST_BUFFER_DATA(m_buffer) = app->ImgBuf;

//               GST_BUFFER_TIMESTAMP(m_buffer) = timestamp;
//               GST_BUFFER_DURATION(m_buffer) = 100 * GST_MSECOND;
////               GST_BUFFER_DURATION(m_buffer) = gst_util_uint64_scale_int(1, GST_SECOND, 10);
//               timestamp += GST_BUFFER_DURATION (m_buffer);

////               ret = gst_app_src_push_buffer((GstAppSrc*)app->appsrc, m_buffer);
//               g_signal_emit_by_name (app->appsrc, "push-buffer", m_buffer, &ret);
////               gst_buffer_unref(m_buffer);
////               gst_object_unref(m_buffer);
////               GST_BUFFER_FREE_FUNC(m_buffer) = g_free;

//               if (ret != GST_FLOW_OK) {
//                   qDebug() << "::::::GST_FLOW_OK ERROR :::::: ";
//                   GST_DEBUG ("some error");
//                  ok = false;
//               }

//           qDebug() << ":::::::::tmcontroller,,onStartMapVideoStream : read_date() #1:" << (long)app->mVideoStream;
//           if (app->mVideoStream != nullptr && cnt >5)
//           {
//               qDebug() << "20191021 hhmictest onDoneStartMapStream()" << app->mWidth << app->mHeight << app->mImageSize <<"("<<app->m_bRender<<")";

//                emit app->mVideoStream->doneStartMapStream();
//           }
//           cnt++;
//           app->m_bStartStream = true;
//           qDebug() << ":::::::::tmcontroller,,onStartMapVideoStream : read_date() #2";
//       }
//       app->m_mutex.unlock();
//   }

//   return ok;
//}

GstBuffer *m_buffer;

static gboolean read_data (App * app)
{
   //qDebug() << ":::::: read_data :::::: #1 ";
   gboolean ok = TRUE;

   app->m_mutex.lock();
   app->m_bBuffer = false;

   if(app->ImgBuf != NULL)
   {
       GstFlowReturn ret;

       m_buffer = gst_buffer_new();

       if(m_buffer == NULL)
       {
           app->m_mutex.unlock();
           return false;
       }

       GST_BUFFER_DATA(m_buffer) = app->ImgBuf;
       GST_BUFFER_SIZE(m_buffer) = app->mImageSize;

       ret = gst_app_src_push_buffer((GstAppSrc*)app->appsrc, m_buffer);
       qDebug() << "::::::read_data " << ret;
 //      GST_BUFFER_FREE_FUNC(m_buffer);

       if (ret != GST_FLOW_OK) {
           qDebug() << "::::::GST_FLOW_OK ERROR :::::: ";
           GST_DEBUG ("some error");
          ok = FALSE;
       }

       if (app->mVideoStream != nullptr && cnt >5)
       {
           qDebug() << "20191021 hhmictest onDoneStartMapStream()" << app->mWidth << app->mHeight << app->mImageSize <<"("<<app->m_bRender<<")";

            emit app->mVideoStream->doneStartMapStream();
       }
       cnt++;
       app->m_bStartStream = true;
   }

   app->m_mutex.unlock();
   return ok;
}

void videostream::setIp(char *ip)
{
    App *app = &s_app;
    qDebug() << "setIp "<< ip;
    app->mIp = QString(ip);

    QHostAddress address;
}

void videostream::setPort(ushort port)
{
    App *app = &s_app;
//    qDebug() << "setPort "<< port;

    app->mPort = port;
}

void videostream::setEncordType(int type)
{
    App *app = &s_app;
    app->mEncodeType = type;
}

void videostream::setBitrate(int bitrate)
{
    App *app = &s_app;
    app->mBitrate = bitrate;
}

void videostream::setVideoFormat(int format)
{
    App *app = &s_app;
    app->mVideoformat = format;
}

void videostream::setImageSize(int width, int height)
{
    App *app = &s_app;
    qDebug() << ":::::::::setImageSize #1 "<< width << height << app->mWidth << app->mHeight;

    if(app != NULL)
    {
        if(app->mWidth != width || app->mHeight != height)
        {
            qDebug() << ":::::::::setImageSize #2";
            free(app->ImgBuf);
            app->ImgBuf = NULL;

            app->mWidth = width;
            app->mHeight = height;
            qDebug() << "::::::initImage << "<< app->mWidth << app->mHeight;
        }
    }
}

videostream::videostream()
{
    //m_bBuffer = false;
    App *app = &s_app;
    if (app->mVideoStream == nullptr)
    {
//        qDebug() << "hhmictest set mVideoStream:" << (long)this;
        app->mVideoStream = this;
    }
}

void videostream::setBuffer(uchar *buf, int size)
{
    App *app = &s_app;
    qDebug() << "20191021 :::::::::::::::::::::::::: setBuffer "<< size << app->m_bRender << app->m_bBuffer;

    if(size > 0)
    {
        if(app->m_bRender)
        {
            if(app->ImgBuf  == NULL)
            {
                app->ImgBuf     = (guint8*)malloc(size);
                qDebug() << "::::::changeMapInClusterImage malloc";
            }

            if(!app->m_bBuffer)
            {
                qDebug() << "20191021 ::::::changeMapInClusterImage setBuffer #2 "<< app->ImgBuf << buf << size;

                app->m_bBuffer = true;

                app->mImageSize = size;
                if(app->ImgBuf != NULL && buf != NULL)
				{
                    app->m_mutex.lock();
                    memcpy(app->ImgBuf, buf, size);
                    app->m_mutex.unlock();

//                    read_data(app);
                }
                else
                {
                    if (app->ImgBuf != NULL)
                    {
                        free(app->ImgBuf);
                        app->ImgBuf = NULL;
                    }
                }
            }
        }
    }
}

bool videostream::isStartStreaming(int width, int height)
{
    App *app = &s_app;
    if(width == app->mWidth && height == app->mHeight)
    {
        return app->m_bStartStream;
    }else{
        return false;
    }
}

bool videostream::isUsingBuffer()
{
    App *app = &s_app;
    return app->m_bBuffer;
//    return false;
}

void videostream::setStartStream(bool isResume)
{    
    qDebug() << ":::::::::setStartStream :::::::::::::::::::::::::::::: #0";
    qDebug() << "setStartStream onDoneStartMapStream()" << isResume;
    App *app = &s_app;
    GstStateChangeReturn gstReturn;

//    app->m_mutex.lock();

    if(app->appsrc != NULL)
    {
        qDebug() << ":::::::::setStartStream :::::::::::::::::::::::::::::: #1";

        if(isResume)
        {
            gstReturn = gst_element_change_state( (GstElement*)app->pipeline, GST_STATE_CHANGE_PAUSED_TO_PLAYING );
        }else{
            gstReturn = gst_element_set_state ((GstElement*)app->pipeline, GST_STATE_PLAYING);
        }
        qDebug() << "20191105 setStartStream onDoneStartMapStream() return #1: " << gstReturn << app->ImgBuf << app->m_bBuffer ;

//        app->sourceid = g_timeout_add(25, (GSourceFunc)read_data, app);
        app->m_bRender = true;
    }
//    app->m_mutex.unlock();
}

void videostream::setStopStream()
{
    qDebug() << ":::::::::setStopStream #0";

    cnt = 0;
    App *app = &s_app;
    app->m_mutex.lock();
//    if(app->m_bRender)
    {
        app->m_bRender = false;
        qDebug() << "20191105 :::::::::setStopStream #0" << app->mWidth << app->mHeight;

//        app->m_bPause = true;
        app->m_bStartStream = false;
//        gst_element_send_event(app->pipeline, gst_event_new_eos());
//        gst_element_set_state((GstElement*)app->pipeline, GST_STATE_NULL);
//        GstStateChangeReturn gstReturn = gst_element_change_state( (GstElement*)app->pipeline, GST_STATE_CHANGE_PLAYING_TO_PAUSED );
//        qDebug() << "20191105 setStopStream onDoneStartMapStream() return /  " << gstReturn;
//        g_source_remove(app->sourceid);
//        app->sourceid = 0;
    }
    app->m_mutex.unlock();
}

static void start_feed (GstElement * pipeline, guint size, App * app)
{
    if(app->sourceid == 0)
    {
        qDebug() << ":::::: start_feed() g_timeout_add";
//        app->sourceid = g_timeout_add((1000/app->framerate), (GSourceFunc)read_data, app);
        app->sourceid = g_timeout_add(100, (GSourceFunc)read_data, app);
//        app->sourceid = g_idle_add((GSourceFunc)read_data, app);
    }
}

static void stop_feed (GstElement * pipeline, App * app)
{
    if (app->sourceid != 0)
    {
        g_source_remove (app->sourceid);
        app->sourceid = 0;
    }
}

static gboolean bus_message (GstBus * bus, GstMessage * message, App * app)
{
    switch (GST_MESSAGE_TYPE (message)) {
        case GST_MESSAGE_ERROR: {
            GError *err = NULL;
            gchar *dbg_info = NULL;

            gst_message_parse_error (message, &err, &dbg_info);
            g_error_free (err);
            g_free (dbg_info);
            g_main_loop_quit (app->loop);
        break;
        }
        case GST_MESSAGE_EOS:
            g_main_loop_quit (app->loop);
        break;
        default:
        break;
    }
    return TRUE;
}


void videostream::initPipeline()
{
    App *app = &s_app;
    qDebug() << ":::::: initPipeline #1 -1";
    gst_init (NULL, NULL);

    app->loop = g_main_loop_new (NULL, TRUE);

    app->timer = g_timer_new();
#ifdef BUILD_TARGET
    QString pipeline;
    QString str1 = "appsrc name=mysource ! ffmpegcolorspace ! vpuenc codec=6 force-framerate=true seqheader-method=3 bitrate=1280000 ! video/x-h264, is-live=true ! rtph264pay pt=96 ! udpsink host=";


    char* str2 = "port=";
    pipeline = QString("%1%2 %3%4").arg(str1).arg(app->mIp).arg(str2).arg(app->mPort);

    qDebug() << "::::::startvideostream "<< pipeline.toUtf8().data();

    app->pipeline = gst_parse_launch(pipeline.toUtf8().data(), NULL);
//    app->pipeline = gst_parse_launch("gstrtpbin name=rtpbin1 videotestsrc udpsrc uri=udp://10.19.229.117:8554 ! ffmpegcolorspace ! vpuenc codec=6 force-framerate=true seqheader-method=3 bitrate=1280000 ! video/x-h264 ! rtph264pay pt=96 ! udpsink host=172.17.0.5 port=8554", NULL);
//    app->pipeline = gst_parse_launch("gstrtpbin name=rtpbin1 videotestsrc ! ffmpegcolorspace ! vpuenc codec=6 force-framerate=true seqheader-method=3 bitrate=1280000 ! video/x-h264 ! rtph264pay pt=96 ! udpsink host=172.17.0.5 port=8554", NULL);
//    app->pipeline = gst_parse_launch("gstrtpbin name=rtpbin1 videotestsrc pattern=snow is-live=true timestamp-offset=100 ! ffmpegcolorspace ! vpuenc codec=6 force-framerate=true seqheader-method=3 ! video/x-h264, stream-format=byte-stream, alignment=nal ! rtph264pay pt=96 mtu=10 ! udpsink host=192.168.40.2 port=5004", NULL);
//    app->pipeline = gst_parse_launch("gstrtpbin name=rtpbin1 videotestsrc is-live=true ! ffmpegcolorspace ! vpuenc codec=6 force-framerate=true seqheader-method=3 bitrate=1280000 ! video/x-h264 background-color=0x111111 background-color=0x111111 pattern = 1 ! rtph264pay pt=96 mtu=1024 ! udpsink host=192.168.40.2 port=5004", NULL);
//    app->pipeline = gst_parse_launch("appsrc name=mysource ! ffmpegcolorspace ! vpuenc codec=6 force-framerate=true seqheader-method=3 bitrate=1280000 ! video/x-h264 ! rtph264pay pt=96 ! multiudpsink clients=172.17.0.5:5004", NULL);
//    app->pipeline = gst_parse_launch("gst-launch-0.10 -v uridecodebin name=uridec uri=file:///home/root/sim.mp4 is-live=true ! ffmpegcolorspace ! vpuenc codec=6 force-framerate=true seqheader-method=3 bitrate=1280000 ! video/x-h264 ! rtph264pay pt=96 ! udpsink host=192.168.40.2 port=5004", NULL);
//    app->pipeline = gst_parse_launch("appsrc name=mysource ! ffmpegcolorspace ! vpuenc codec=6 force-framerate=true seqheader-method=3 ! video/x-h264 ! h264parse ! rtph264pay ! udpsink host=192.168.40.2 port=5004", NULL);
//    app->pipeline = gst_parse_launch("appsrc name=mysource ! ffmpegcolorspace ! vpuenc codec=6 force-framerate=true seqheader-method=3 ! video/x-h264, is-live=true ! rtph264pay pt=96", NULL);
//      app->pipeline = gst_parse_launch("appsrc name=mysource ! ffmpegcolorspace ! vpuenc codec=6 force-framerate=true seqheader-method=3 ! video/x-h264, is-live=true ! rtph264pay pt=96 ! udpsink host=172.17.0.5 port=8554", NULL);
//udpsrc uri=udp://239.194.0.177:1026
//      app->pipeline = gst_parse_launch("appsrc name=mysource ! ffmpegcolorspace ! vpuenc codec=6 force-framerate=true seqheader-method=3 bitrate=1280000 ! video/x-h264 ! rtph264pay pt=96 ! multiudpsink clients=192.168.40.2:5004,10.19.229.117", NULL);
//      app->pipeline = gst_parse_launch("appsrc name=mysource ! ffmpegcolorspace ! vpuenc codec=6 force-framerate=true seqheader-method=3 bitrate=1280000 ! video/x-h264 ! rtph264pay pt=96 ! udpsrc uri=udp://172.17.0.5:8554", NULL);
//#else0
//    app->pipeline = gst_parse_launch("appsrc name=mysource ! ffmpegcolorspace ! x264enc ! rtph264pay ! udpsink host=127.0.0.1 port=5000", NULL);
#endif
//    g_assert (app->pipeline);

    app->m_bRender = true;
}

void videostream::readPendingDatagrams()
{
    qDebug() << "readPendingDatagrams :::: #2";
}

void videostream::run()
{
    App *app = &s_app;
    GstBus *bus;

    GstElement *colorformat, *enc, *rtph, *videosink, *udpsink;

//    app->pipeline = gst_parse_launch("appsrc name=mysource ! ffmpegcolorspace ! vpuenc codec=6 force-framerate=true seqheader-method=3 ! video/x-h264, is-live=true ! rtph264pay pt=96", NULL);

    app->pipeline = gst_pipeline_new ("pipeline");
    app->appsrc = gst_element_factory_make ("appsrc", "source");
    colorformat = gst_element_factory_make ("ffmpegcolorspace", "colorformat");
    enc = gst_element_factory_make ("vpuenc", "enc");
    rtph = gst_element_factory_make ("rtph264pay", "rtph");
    videosink = gst_element_factory_make ("udpsink", "videosink");

    bool isPC = false;

    if(isPC)
    {
        g_object_set(G_OBJECT(videosink),
                        "host", "172.17.0.5",
                        "port", 8554,
                        NULL);
    }else{
        g_object_set(G_OBJECT(videosink),
                        "host", "192.168.40.2",   //cluster ip address
                        "port", 5004,             //cluster port
                        NULL);
    }

//    app->pipeline = gst_parse_launch("appsrc name=mysource ! ffmpegcolorspace ! vpuenc codec=6 force-framerate=true seqheader-method=3 bitrate=1280000 ! video/x-h264 ! rtph264pay pt=96 ! multiudpsink clients=172.17.0.5:5004", NULL);
//appsrc name=myappsrc stream-type=0 is-live=TRUE ! v4l_csc devicename=/dev/video0 use-pad-alloc=TRUE vflip=TRUE ! queue name=encqueue !
//vpuenc codec=6 seqheader-method=3 cbr=false quant=0 gopsize=1 framerate-nu=25 force-framerate=TRUE ! queue name=payqueue ! rtph264pay ! queue name=sinkqueue ! udpsink host=

    g_object_set(G_OBJECT(app->appsrc),
                     "stream-type", 0,
                     "is-live", TRUE);

    g_object_set(G_OBJECT(enc),
                     "codec", 6,
                     "force-framerate", TRUE,
                     "seqheader-method", 3,
                     "gopsize", 1,
                     "framerate-nu", 25,
                     "cbr", false,
                     "quant", 0,
//                     "async-depth", 4,
//                     "async-finalize", true,
//                     "preset", "veryfast",
//                     "bitrate", 1280000,
                     NULL);

    g_object_set(G_OBJECT(rtph),
                 "pt", 96,
//                 "mtu", 1400,
                  NULL);

      gst_bin_add_many (GST_BIN (app->pipeline), app->appsrc, colorformat, enc, rtph, videosink, NULL);
      gst_element_link_many (app->appsrc, colorformat, enc, rtph, videosink, NULL);

      g_assert(app->pipeline);

      GstMessage *msg;
      gboolean terminate = FALSE;

      bus = gst_pipeline_get_bus(GST_PIPELINE(app->pipeline));
//      do{
//          msg = gst_bus_timed_pop_filtered(bus, GST_CLOCK_TIME_NONE, (GstMessageType)(GST_MESSAGE_STATE_CHANGED | GST_MESSAGE_ERROR | GST_MESSAGE_EOS));

//          if(msg != NULL)
//          {
//              GError *err;
//              gchar *debug_info;
//              switch(GST_MESSAGE_TYPE(msg))
//              {
//              case GST_MESSAGE_ERROR:
//                    qDebug() << "20191201 GST_MESSAGE_ERROR:";
////                  gst_message_parse_error(msg,&err, &debug_info);
////                  g_printerr("Error received from element %s: %s\n", GST_OBJECT_NAME(msg->src), err->message);
////                  g_printerr("Debugging information: %s\n", debug_info ? debug_info : "none");
////                  g_clear_error(&err); g_free(debug_info);
//                  terminate = TRUE;
//                  break;
//              case GST_MESSAGE_EOS:
//                    qDebug() << "20191201 GST_MESSAGE_EOS:";
//                  g_print("End-Of-Stream reached.\n");
//                  terminate = TRUE;
//                  break;
//              case GST_MESSAGE_STATE_CHANGED:
//                  qDebug() << "20191201 GST_MESSAGE_STATE_CHANGED:";
//                  if(GST_MESSAGE_SRC(msg) == GST_OBJECT(app->pipeline))
////                  {
////                      GstState old_state, new_state, pending_state;
////                      gst_message_parse_state_changed(msg, &old_state, &new_state, &pending_state);
////                      g_print("Pipeline state changed from %s to %s:\n", gst_element_state_get_name(old_state), gst_element_state_get_name(new_state));
////                  }
//                  break;
//              default:
//                qDebug() << "20191201 default:";
////                  g_printerr("Unexpected message received.\n");
//                  break;
//              }
//              gst_message_unref(msg);
//          }
//      }while(!terminate);
//      gst_object_unref(bus);

         g_assert(bus);
         /* add watch for messages */
      app->mBusId = gst_bus_add_watch(bus, (GstBusFunc)bus_message, app);

         qDebug() << "::::::startvideostream #2";

//         g_assert(GST_IS_APP_SRC(app->appsrc));

         g_signal_connect(app->appsrc, "need-data", G_CALLBACK(start_feed), app);
         g_signal_connect(app->appsrc, "enough-data", G_CALLBACK(stop_feed), app);


////         /* set the caps on the source */
////         GstCaps *caps = gst_video_format_new_caps(GST_VIDEO_FORMAT_RGB, app->mWidth, app->mHeight, 0, 1, 1, 1);
         GstCaps *caps = gst_video_format_new_caps(GST_VIDEO_FORMAT_BGRA, app->mWidth, app->mHeight, 0, 1, 1, 1);

         gst_app_src_set_caps(GST_APP_SRC((GstElement*)app->appsrc), caps);
         gst_app_src_set_stream_type((GstAppSrc*)app->appsrc, GST_APP_STREAM_TYPE_STREAM);
         gst_app_src_set_max_bytes((GstAppSrc*)app->appsrc, app->mWidth * app->mHeight *3*sizeof(guchar));

         gboolean sendevent = gst_element_send_event((GstElement*)app->pipeline, gst_event_new_eos());
         qDebug() << "::::::startvideostream #5 sendevent : " << sendevent;
         printf("::::::startvideostream #5\n");
         /* go to playing and wait in a mainloop. */
         GstStateChangeReturn gstReturn = gst_element_set_state((GstElement*)app->pipeline, GST_STATE_PLAYING);
         qDebug() << "onDoneStartMapStream() GST_STATE_PLAYING " << gstReturn;

//         if (gst_element_get_state (app->pipeline, NULL, NULL, -1) == GST_STATE_CHANGE_FAILURE) {
//            g_error ("Failed to go into PLAYING state");
//          }
         qDebug() << "::::::startvideostream #6";
         printf("::::::startvideostream #6\n");
         /* this mainloop is stopped when we receive an error or EOS */
         g_main_loop_run(app->loop);
         qDebug() << "::::::startvideostream #7";
         GST_DEBUG("stopping");

         gst_element_set_state((GstElement*)app->pipeline, GST_STATE_NULL);

         gst_object_unref(bus);
         g_main_loop_unref(app->loop);
}
