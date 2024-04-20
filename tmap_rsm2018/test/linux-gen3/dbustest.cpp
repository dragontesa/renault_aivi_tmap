#include <fstream>
#include <iostream>

#include "dbustest.h"
#include "SharedMemory.h"

#include "navigationserviceadaptor.h"
#include "navigationserver.h"
#include "platformcontroller_gen3.h"

#define PURE_TEXT               "Please go ahead 100 meter, then turn left at the second traffic light."
#define WAV_PATH                "/opt/bosch/navigation_kor/wav/3101.wav"
#define PCM_PATH                "/opt/bosch/navigation_kor/wav/PCM/3101s16.pcm"
#define WAV_TMP_PATH            "/opt/bosch/navigation_kor/wav/tmp.tmp"

namespace SKT {

DbusTest::DbusTest(PlatformControllerGen3 *controller) : m_controller(controller), m_guiControl(NULL), m_navigationExt(NULL)
{

}

DbusTest::~DbusTest()
{

}

void DbusTest::init()
{
    m_guiControl = m_controller->guiControl();

    if (m_guiControl) {
        connect(m_guiControl, SIGNAL(KeyInfoForwarding(GCKeyInfo)),             this, SLOT(onKeyInfoForwarding(GCKeyInfo)));
    }

    m_navigationExt = m_controller->navigationExt();

    if (m_navigationExt) {
        connect(m_navigationExt, SIGNAL(NaviSigNaviAnnouncementStatus(uint)),   this, SLOT(onNaviSigNaviAnnouncementStatus(uint)));
        connect(m_navigationExt, SIGNAL(NaviSignalSpeedValue(ushort)),          this, SLOT(onNaviSignalSpeedValue(ushort)));
        connect(m_navigationExt, SIGNAL(PlayBeepStatus(uchar)),                 this, SLOT(onPlayBeepStatus(uchar)));
        connect(m_navigationExt, SIGNAL(PlayMelodyStatus(uchar)),               this, SLOT(onPlayMelodyStatus(uchar)));
        connect(m_navigationExt, SIGNAL(ResetToFactorySettings()),              this, SLOT(onResetToFactorySettings()));
    }
}

void DbusTest::cleanUp()
{
    if (m_guiControl) {
        disconnect(m_guiControl, 0, 0, 0);
    }

    if (m_navigationExt) {
        disconnect(m_navigationExt, 0, 0, 0);
    }
}

void DbusTest::onKeyInfoForwarding(GCKeyInfo data) {
    qDebug() << "onKeyInfoForwarding: data.m_key=" << data.m_key << "data.m_state=" << data.m_state;



}
void DbusTest::onNaviSigNaviAnnouncementStatus(uint u32NaviAnnouncementStatus) {
    qDebug() << "onNaviSigNaviAnnouncementStatus: u32NaviAnnouncementStatus=" << u32NaviAnnouncementStatus;
}
void DbusTest::onNaviSignalSpeedValue(ushort u8SpeedValue) {
    qDebug() << "onNaviSignalSpeedValue: u8SpeedValue = " << u8SpeedValue;
}
void DbusTest::onPlayBeepStatus(uchar uBeepStatus) {
    qDebug() << "onPlayBeepStatus: uBeepStatus = " << uBeepStatus;
}
void DbusTest::onPlayMelodyStatus(uchar uMelodyStatus) {
    qDebug() << "onPlayMelodyStatus: uMelodyStatus = "<< uMelodyStatus;
}
void DbusTest::onResetToFactorySettings() {
    qDebug() << "onResetToFactorySettings";
}

void DbusTest::runTest()
{
    if (m_navigationExt) {
//property
        NXDevMediaInfo mediaInfo = m_navigationExt->naviPropertyDevMediaInfo();
        qDebug() << "naviPropertyDevMediaInfo: mediaInfo.m_mediaPath = "<< mediaInfo.m_mediaPath;
        qDebug() << "naviPropertyDevMediaInfo: mediaInfo.m_mediaStatus = "<< mediaInfo.m_mediaStatus;

        NXDateTimeDetailInfo dateTimeDetailInfo = m_navigationExt->naviPropertyUpdateDateTimeDetailInfo();
        qDebug() << "naviPropertyUpdateDateTimeDetailInfo: dateTimeDetailInfo.m_weekday = "<< dateTimeDetailInfo.m_weekday;
        qDebug() << "naviPropertyUpdateDateTimeDetailInfo: dateTimeDetailInfo.m_year = "<< dateTimeDetailInfo.m_year;
        qDebug() << "naviPropertyUpdateDateTimeDetailInfo: dateTimeDetailInfo.m_month = "<< dateTimeDetailInfo.m_month;
        qDebug() << "naviPropertyUpdateDateTimeDetailInfo: dateTimeDetailInfo.m_day = "<< dateTimeDetailInfo.m_day;
        qDebug() << "naviPropertyUpdateDateTimeDetailInfo: dateTimeDetailInfo.m_hours = "<< dateTimeDetailInfo.m_hours;
        qDebug() << "naviPropertyUpdateDateTimeDetailInfo: dateTimeDetailInfo.m_minutes = "<< dateTimeDetailInfo.m_minutes;
        qDebug() << "naviPropertyUpdateDateTimeDetailInfo: dateTimeDetailInfo.m_seconds = "<< dateTimeDetailInfo.m_seconds;

        uchar dialPOICallStatus     = m_navigationExt->naviPropertyDialPOICallStatus();
        qDebug() << "naviPropertyDialPOICallStatus = "<< dialPOICallStatus;

        uchar callServiceStatus     = m_navigationExt->naviPropertyCallServiceStatus();
        qDebug() << "naviPropertyCallServiceStatus = "<< callServiceStatus;


        uchar dimmingStatus         = m_navigationExt->naviPropertyUpdateDimmingStatus();
        qDebug() << "naviPropertyUpdateDimmingStatus = "<< dimmingStatus;

        uchar updateHmiState        = m_navigationExt->naviPropertyUpdateHmiState();
        qDebug() << "naviPropertyUpdateHmiState = "<< updateHmiState;

        uchar reverseGearState      = m_navigationExt->naviPropertyUpdateReverseGearState();
        qDebug() << "naviPropertyUpdateReverseGearState = "<< reverseGearState;

        uchar distanceUnit          = m_navigationExt->naviPropertyUpdateDistanceUnit();
        qDebug() << "naviPropertyUpdateDistanceUnit = "<< distanceUnit;

        uchar handBrakeStatus       = m_navigationExt->naviPropertyUpdateHandBrakeStatus();
        qDebug() << "naviPropertyUpdateHandBrakeStatus = "<< handBrakeStatus;

        uchar languageIndex         = m_navigationExt->naviPropertyUpdateLanguageIndex();
        qDebug() << "naviPropertyUpdateLanguageIndex = "<< languageIndex;

        uchar dateFormat         = m_navigationExt->naviPropertyDateFormat();
        qDebug() << "naviPropertyDateFormat = "<< dateFormat;

        uchar timeFormat         = m_navigationExt->naviPropertyTimeFormat();
        qDebug() << "naviPropertyTimeFormat = "<< timeFormat;

        uchar colorState         = m_navigationExt->naviPropertyMultiAmbienceLightingColorState();
        qDebug() << "naviPropertyMultiAmbienceLightingColorState = "<< colorState;

        uchar mexMode         = m_navigationExt->naviPropertyMexMode();
        qDebug() << "naviPropertyMexMode = "<< timeFormat;

//method
        QDBusPendingReply<bool> pendingReplyTypeBool;
        QDBusPendingReply<> pendingReply;
        QDBusReply<QString> replay;

        pendingReplyTypeBool = m_navigationExt->NaviMethDialPOI((const char*)"0101231234");
        pendingReplyTypeBool.waitForFinished();
        if (!pendingReplyTypeBool.isError())
            qDebug() << "NaviMethDialPOI = "<< pendingReplyTypeBool.argumentAt<0>();
        else
            qDebug() << "NaviMethDialPOI error= "<< pendingReplyTypeBool.error();

        replay = m_navigationExt->NaviMethGetDeviceID();
        if (replay.isValid())
            qDebug() << "NaviMethGetDeviceID = "<< replay.value();
        else
            qDebug() << "NaviMethGetDeviceID = "<< replay.error();

        pendingReplyTypeBool = m_navigationExt->NaviMethPlayBeep();
        pendingReplyTypeBool.waitForFinished();
        if (!pendingReplyTypeBool.isError())
            qDebug() << "NaviMethPlayBeep = "<< pendingReplyTypeBool.argumentAt<0>();
        else
            qDebug() << "NaviMethPlayBeep error= "<< pendingReplyTypeBool.error();


//NaviMethPlayNaviAnnouncement start
        QStringList contents;
        NaviAnnouncementType at = NaviAnnouncementType::NAVI_ANNOUNCEMENT_TYPE_PCM;
    /*
        PCM params
        "11025"  ---  11kHz
        "22050"  ---  22kHz
        "44100"  ---  44kHz
    */
        switch (at) {
        case NaviAnnouncementType::NAVI_ANNOUNCEMENT_TYPE_PURE_TEXT:
            contents << PURE_TEXT;
            pendingReplyTypeBool = m_navigationExt->NaviMethPlayNaviAnnouncement(0, contents); //TEXT
            break;
        case NaviAnnouncementType::NAVI_ANNOUNCEMENT_TYPE_RECORD_FILE:
            contents << WAV_PATH;
            pendingReplyTypeBool = m_navigationExt->NaviMethPlayNaviAnnouncement(1, contents); //WAV
            break;
        case NaviAnnouncementType::NAVI_ANNOUNCEMENT_TYPE_PCM:
            contents << "22050";
            pendingReplyTypeBool = m_navigationExt->NaviMethPlayNaviAnnouncement(2, contents); //PCM
            break;
        default:
            break;
        }
        pendingReplyTypeBool.waitForFinished();

        if (!pendingReplyTypeBool.isError())
            qDebug() << "NaviMethPlayNaviAnnouncement = "<< pendingReplyTypeBool.argumentAt<0>();
        else
            qDebug() << "NaviMethPlayNaviAnnouncement error= "<< pendingReplyTypeBool.error();

//NaviMethPlayNaviAnnouncement end

        pendingReplyTypeBool = m_navigationExt->NaviMethAbortNaviAnnouncement();
        pendingReplyTypeBool.waitForFinished();
        if (!pendingReplyTypeBool.isError())
            qDebug() << "NaviMethAbortNaviAnnouncement = "<< pendingReplyTypeBool.argumentAt<0>();
        else
            qDebug() << "NaviMethAbortNaviAnnouncement error= "<< pendingReplyTypeBool.error();

        pendingReply = m_navigationExt->NaviMethThirdPartyNaviStartState(1);
        if (pendingReply.isError())
            qDebug() << "NaviMethThirdPartyNaviStartState error= "<< pendingReply.error();

        pendingReplyTypeBool = m_navigationExt->NaviMethRestartSystem(1);
        pendingReplyTypeBool.waitForFinished();
        if (!pendingReplyTypeBool.isError())
            qDebug() << "NaviMethRestartSystem = "<< pendingReplyTypeBool.argumentAt<0>();
        else
            qDebug() << "NaviMethRestartSystem error= "<< pendingReplyTypeBool.error();

        pendingReply = m_navigationExt->NaviMethContextSwitch(1);
        if (pendingReply.isError())
            qDebug() << "NaviMethContextSwitch error= "<< pendingReply.error();

//NaviMethPlayBeepMelody start

        /*
            chime.mp3
            church_bell_1.mp3
            dog_1.mp3
            horn.mp3
            marimba_2.mp3
            melody.mp3
            music_box_1.mp3
            music_box_2.mp3
            scratch_2.mp3
            sound_1.mp3
            trumpet_1.mp3
            wave_2.mp3
        */

        QString melody = "/opt/bosch/navigation/cfg/mdw/sounds/music_box_1.mp3";
        pendingReplyTypeBool = m_navigationExt->NaviMethPlayBeepMelody(melody);
        pendingReplyTypeBool.waitForFinished();
        if (!pendingReplyTypeBool.isError())
            qDebug() << "NaviMethPlayBeepMelody = "<< pendingReplyTypeBool.argumentAt<0>();
        else
            qDebug() << "NaviMethPlayBeepMelody error= "<< pendingReplyTypeBool.error();

//NaviMethPlayBeepMelody end

        pendingReplyTypeBool = m_navigationExt->NaviMethRestartSystem(0);
        pendingReplyTypeBool.waitForFinished();
        if (!pendingReplyTypeBool.isError())
            qDebug() << "NaviMethRestartSystem = "<< pendingReplyTypeBool.argumentAt<0>();
        else
            qDebug() << "NaviMethRestartSystem error= "<< pendingReplyTypeBool.error();
    }
}

int DbusTest::writeToSM() {
    std::ifstream file(PCM_PATH);
    if (!file.is_open()) return 0;
    file.seekg( 0, std::ios::end );
    int size = file.tellg();
    file.seekg (0, std::ios::beg);
    const size_t BUFFER_SIZE = (size < 0) ? __SIZE_MAX__ : (size_t)((unsigned)size);
    char buffer[BUFFER_SIZE];
    ThirdPartyToolSet::SharedMemoryWriter writer;
    writer.Open();
    file.read(buffer, size);
    writer.Write(buffer, BUFFER_SIZE);
    writer.Close();
    file.close();
    return 1;
}

void DbusTest::readFromSM() {
    ThirdPartyToolSet::SharedMemoryReader reader;  //1
    int reader_ret = reader.Open();  //2
    char buffer[100];

    sprintf(buffer, "%d", reader_ret);
    memset(buffer, 0, sizeof(buffer));

    if (reader_ret) {
        return;
    } else {
        size_t dataSize = reader.GetSize();

        int convertdata = static_cast<int>(dataSize);
        sprintf(buffer, "%d", convertdata);
        memset(buffer, 0, sizeof(buffer));
        if (dataSize > 0) {
            char* pData = new char[dataSize];
            if (pData != NULL) {
                reader.Read(pData, dataSize);  //4,
                const char * destPtr = (const char *)pData;
                //QFuture<int> writedataFunction = QtConcurrent::run(writeData, destPtr, dataSize);
                //writedataFunction.waitForFinished();
                //sprintf(buffer, "%d", writedataFunction.result());
                sprintf(buffer, "%d", writeData(destPtr, dataSize));
                delete[] pData;
            }
        }
    }
    reader.Close();
}

int DbusTest::writeData(const char*data, qint64 len) {
//    struct timespec tp;
//    clock_gettime(CLOCK_MONOTONIC, &tp);
    QFile content(WAV_TMP_PATH);
    QString errMsg;
    QFileDevice::FileError err = QFileDevice::NoError;
    if (!content.open(QIODevice::ReadWrite)) {
        errMsg = content.errorString();
        err = content.error();
        return -1;
    } else {
        content.write(data, len);
    }
    content.close();
    return 0;
}

}
