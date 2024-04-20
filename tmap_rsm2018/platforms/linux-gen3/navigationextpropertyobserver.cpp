#include "navigationextpropertyobserver.h"

#include <QDebug>

namespace SKT {

static const char* PropName_NaviPropertyUpdateDimmingStatus         = "NaviPropertyUpdateDimmingStatus";
static const char* PropName_NaviPropertyDevMediaInfo                = "NaviPropertyDevMediaInfo";
static const char* PropName_NaviPropertyUpdateReverseGearState      = "NaviPropertyUpdateReverseGearState";
static const char* PropName_NaviPropertyUpdateLanguageIndex         = "NaviPropertyUpdateLanguageIndex";
static const char* PropName_NaviPropertyDateFormat                  = "NaviPropertyDateFormat";
static const char* PropName_NaviPropertyTimeFormat                  = "NaviPropertyTimeFormat";
static const char* PropName_NaviPropertyMultiAmbienceLightingColorState                  = "NaviPropertyMultiAmbienceLightingColorState";
static const char* PropName_NaviPropertyMexMode                     = "NaviPropertyMexMode";
static const char* PropName_NaviPropertyDialPOICallStatus           = "NaviPropertyDialPOICallStatus";
static const char* PropName_NaviPropertyCallServiceStatus           = "NaviPropertyCallServiceStatus";
static const char* PropName_NaviPropertyUpdateDateTimeDetailInfo    = "NaviPropertyUpdateDateTimeDetailInfo";
static const char* PropName_NaviPropertyUpdateLockStates            = "NaviPropertyUpdateLockStates";
static const char* PropName_NaviPropertyUpdateHmiState              = "NaviPropertyUpdateHmiState";
static const char* PropName_NaviPropertyAppStateNavigation          = "NaviPropertyAppStateNavigation";


NavigationExtPropertyObserver::NavigationExtPropertyObserver(QObject *parent)
    : QObject(parent)
    , m_dimmingStatus(false)
    , m_ReverseGearState(0)
    , m_LanguageIndex(0)
    , m_DateFormat(0)
    , m_TimeFormat(0)
    , m_ColorState(0)
    , m_MexMode(0)
    , m_dialPOICallStatus(0)
    , m_callServiceStatus(0)
    , m_HmiState(0)
    , m_appStateNavigation(0)
{
    if (parent) {
        connect(parent, SIGNAL(propertyChanged(QString,QVariant)),
                this, SLOT(onPropertyChanged(QString,QVariant)));

        // set initial value
        m_dimmingStatus = qvariant_cast<uchar>(parent->property(PropName_NaviPropertyUpdateDimmingStatus)) == 1;
        m_devMediaInfo = qvariant_cast<NXDevMediaInfo>(parent->property(PropName_NaviPropertyDevMediaInfo));
        m_ReverseGearState = qvariant_cast<uchar>(parent->property(PropName_NaviPropertyUpdateReverseGearState));
        m_LanguageIndex = qvariant_cast<uchar>(parent->property(PropName_NaviPropertyUpdateLanguageIndex));
        m_DateFormat = qvariant_cast<uchar>(parent->property(PropName_NaviPropertyDateFormat));
        m_TimeFormat = qvariant_cast<uchar>(parent->property(PropName_NaviPropertyTimeFormat));
        m_ColorState = qvariant_cast<uchar>(parent->property(PropName_NaviPropertyMultiAmbienceLightingColorState));
        m_MexMode = qvariant_cast<uchar>(parent->property(PropName_NaviPropertyMexMode));
        m_dialPOICallStatus = qvariant_cast<uchar>(parent->property(PropName_NaviPropertyDialPOICallStatus));
        m_callServiceStatus = qvariant_cast<uchar>(parent->property(PropName_NaviPropertyCallServiceStatus));
        m_TimeDetailInfo = qvariant_cast<NXDateTimeDetailInfo>(parent->property(PropName_NaviPropertyUpdateDateTimeDetailInfo));
        m_lockStateInfo = qvariant_cast<NXLockStateInfoList>(parent->property(PropName_NaviPropertyUpdateLockStates));
        m_HmiState = qvariant_cast<uint>(parent->property(PropName_NaviPropertyUpdateHmiState));
        m_appStateNavigation = qvariant_cast<uchar>(parent->property(PropName_NaviPropertyAppStateNavigation));

        qDebug() << "[MultiLang] index: " << (int)m_LanguageIndex;

        qDebug() << "NaviExt property hmi state: " << m_HmiState;
    }
}

void NavigationExtPropertyObserver::forceSystemPropertyChanged()
{
    emit languageIndexChanged(m_LanguageIndex);

    emit mexModeChanged(m_MexMode);

    emit colorStateChanged(m_ColorState);
}

void NavigationExtPropertyObserver::onPropertyChanged(const QString &propertyName, const QVariant &value)
{
#if 0 // Too verbose
    qDebug() << Q_FUNC_INFO << "name:" << propertyName << "value:" << value;
#endif
    if (propertyName == QLatin1String(PropName_NaviPropertyUpdateDimmingStatus)) {
        bool dimming = qvariant_cast<uchar>(value) == 1;
        if (m_dimmingStatus != dimming) {
            m_dimmingStatus = dimming;
            emit dimmingStatusChanged(dimming);
        }
    } else if (propertyName == QLatin1String(PropName_NaviPropertyDevMediaInfo)) {
        NXDevMediaInfo devMediaInfo = qvariant_cast<NXDevMediaInfo>(value);
//        qDebug() << "NavigationExtService > naviPropertyDevMediaInfo :" << devMediaInfo.m_mediaPath << devMediaInfo.m_mediaStatus;
        if (m_devMediaInfo != devMediaInfo) {
            m_devMediaInfo = devMediaInfo;
            emit devMediaInfoChanged(devMediaInfo);
        }
    } else if (propertyName == QLatin1String(PropName_NaviPropertyUpdateReverseGearState)) {
        char reverseGearState = qvariant_cast<uchar>(value);
        if (m_ReverseGearState != reverseGearState) {
            m_ReverseGearState = reverseGearState;
            emit reverseGearStateChanged(m_ReverseGearState);
        }
    } else if (propertyName == QLatin1String(PropName_NaviPropertyUpdateLanguageIndex)) {
        char languageIndex = qvariant_cast<uchar>(value);
//        qDebug() << "NavigationExtService > naviPropertyUpdateLanguageIndex :" << (int)languageIndex;

        qDebug() << "[MultiLang] new index: " << (int)languageIndex << " old index: " << (int)m_LanguageIndex;

        if (m_LanguageIndex != languageIndex) {
            m_LanguageIndex = languageIndex;
            emit languageIndexChanged(m_LanguageIndex);
        }
    } else if (propertyName == QLatin1String(PropName_NaviPropertyDateFormat)) {
        char dateFormat = qvariant_cast<uchar>(value);
//        qDebug() << "NavigationExtService > naviPropertDateFormat :" << (int)dateFormat;
        if (m_DateFormat != dateFormat) {
            m_DateFormat = dateFormat;
            emit dateFormatChanged(m_DateFormat);
        }
    } else if (propertyName == QLatin1String(PropName_NaviPropertyTimeFormat)) {
        char timeFormat = qvariant_cast<uchar>(value);
//        qDebug() << "NavigationExtService > naviPropertTimeFormat :" << (int)timeFormat;
        if (m_TimeFormat != timeFormat) {
            m_TimeFormat = timeFormat;
            emit timeFormatChanged(m_TimeFormat);
        }

    } else if (propertyName == QLatin1String(PropName_NaviPropertyMultiAmbienceLightingColorState)) {
        char colorState = qvariant_cast<uchar>(value);
        if (m_ColorState != colorState) {
            m_ColorState = colorState;
            emit colorStateChanged(m_ColorState);
        }
    } else if (propertyName == QLatin1String(PropName_NaviPropertyMexMode)) {
        char mexMode = qvariant_cast<uchar>(value);
        if (m_MexMode != mexMode) {
            m_MexMode = mexMode;
            emit mexModeChanged(m_MexMode);
        }
    } else if (propertyName == QLatin1String(PropName_NaviPropertyDialPOICallStatus)) {
        char dialPOICallStatus = qvariant_cast<uchar>(value);
//        qDebug() << "NavigationExtService > naviPropertyDialPOICallStatus :" << (int)dialPOICallStatus;
        if (m_dialPOICallStatus != dialPOICallStatus) {
            m_dialPOICallStatus = dialPOICallStatus;
            emit dialPOICallStatusChanged(m_dialPOICallStatus);
        }
    } else if (propertyName == QLatin1String(PropName_NaviPropertyCallServiceStatus)) {
        char callServiceStatus = qvariant_cast<uchar>(value);
        if (m_callServiceStatus != callServiceStatus) {
            m_callServiceStatus = callServiceStatus;
            emit callServiceStatusChanged(m_callServiceStatus);
        }
    } else if (propertyName == QLatin1String(PropName_NaviPropertyUpdateDateTimeDetailInfo)) {
        NXDateTimeDetailInfo datimeDetailinfo = qvariant_cast<NXDateTimeDetailInfo>(value);
//        qDebug() << "NavigationExtService > naviPropertyUpdateDateTimeDetailInfo :" << datimeDetailinfo.m_year << datimeDetailinfo.m_month << datimeDetailinfo.m_day << datimeDetailinfo.m_weekday << datimeDetailinfo.m_hours << datimeDetailinfo.m_minutes << datimeDetailinfo.m_seconds;
        if (m_TimeDetailInfo != datimeDetailinfo) {
            m_TimeDetailInfo = datimeDetailinfo;
            emit dateTimeDetailInfoChanged(m_TimeDetailInfo);
        }
    } else if (propertyName == QLatin1String(PropName_NaviPropertyUpdateLockStates)) {
        NXLockStateInfoList updateLockStates = qvariant_cast<NXLockStateInfoList>(value);
//        for (int ii = 0; ii < updateLockStates.size(); ii++) {
//            qDebug() << "NavigationExtService > naviPropertyUpdateLockStates :" << updateLockStates.at(ii).m_lockname << updateLockStates.at(ii).m_lockstate << updateLockStates.at(ii).m_ownerappid << updateLockStates.at(ii).m_userlist.size();
//        }
        if (m_lockStateInfo != updateLockStates) {
            m_lockStateInfo = updateLockStates;
            emit UpdateLockStatesChanged(m_lockStateInfo);
        }
    } else if (propertyName == QLatin1String(PropName_NaviPropertyUpdateHmiState)) {
        uint hmiState = qvariant_cast<uint>(value);
        qDebug() << "NaviExt hmi state: "<< m_HmiState <<" to " << hmiState;
        if (m_HmiState != hmiState) {
            m_HmiState = hmiState;
            emit hmiStateChanged(hmiState);
        }
    } else if (propertyName == QLatin1String(PropName_NaviPropertyAppStateNavigation)) {
        uchar appStateNavigation = qvariant_cast<uchar>(value);
        qDebug() << "NaviExt app state navigation: "<< m_appStateNavigation <<" to " << appStateNavigation;
        if (m_appStateNavigation != appStateNavigation) {
            m_appStateNavigation = appStateNavigation;
            emit appStateNavigationChanged(appStateNavigation);
        }
    }
}
}
