#ifndef NAVIGATIONEXTPROPERTYOBSERVER_H
#define NAVIGATIONEXTPROPERTYOBSERVER_H

#include <QObject>
#include "navigationexttype.h"

namespace SKT {

class NavigationExtPropertyObserver : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool m_dimmingStatus READ dimmingStatus NOTIFY dimmingStatusChanged)
    Q_PROPERTY(NXDevMediaInfo m_devMediaInfo READ devMediaInfo NOTIFY devMediaInfoChanged)
    Q_PROPERTY(char m_ReverseGearState READ reverseGearState NOTIFY reverseGearStateChanged)
    Q_PROPERTY(char m_LanguageIndex READ LanguageIndex NOTIFY languageIndexChanged)
    Q_PROPERTY(char m_DateFormat READ DateFormat NOTIFY dateFormatChanged)
    Q_PROPERTY(char m_TimeFormat READ TimeFormat NOTIFY timeFormatChanged)
    Q_PROPERTY(char m_ColorState READ ColorState NOTIFY colorStateChanged)
    Q_PROPERTY(char m_MexMode READ MexMode NOTIFY mexModeChanged)
    Q_PROPERTY(char m_dialPOICallStatus READ dialPOICallStatus NOTIFY dialPOICallStatusChanged)
    Q_PROPERTY(char m_callServiceStatus READ callServiceStatus NOTIFY callServiceStatusChanged)
    Q_PROPERTY(uint m_HmiState READ hmiState NOTIFY hmiStateChanged)
    Q_PROPERTY(uchar m_appStateNavigation READ appStateNavigation NOTIFY appStateNavigationChanged)
	Q_PROPERTY(NXDateTimeDetailInfo m_TimeDetailInfo READ DateTimeDetailInfo NOTIFY dateTimeDetailInfoChanged)
    Q_PROPERTY(NXLockStateInfoList m_lockStateInfo READ lockStateInfoList NOTIFY UpdateLockStatesChanged)


public:
    explicit NavigationExtPropertyObserver(QObject *parent = 0);

    bool dimmingStatus()                        const { return m_dimmingStatus; }
    NXDevMediaInfo devMediaInfo()               const { return m_devMediaInfo; }
    char reverseGearState()                     const { return m_ReverseGearState; }
    char LanguageIndex()                        const { return m_LanguageIndex; }
    char DateFormat()                           const { return m_DateFormat; }
    char TimeFormat()                           const { return m_TimeFormat; }
    char ColorState()                           const { return m_ColorState; }
    char MexMode()                              const { return m_MexMode; }
    char dialPOICallStatus()                    const { return m_dialPOICallStatus; }
    char callServiceStatus()                    const { return m_callServiceStatus; }
    NXDateTimeDetailInfo DateTimeDetailInfo()   const { return m_TimeDetailInfo; }
    NXLockStateInfoList lockStateInfoList()     const { return m_lockStateInfo; }
    uint hmiState()                             const { return m_HmiState; }
    uchar appStateNavigation()                  const { return m_appStateNavigation; }

    Q_INVOKABLE void forceSystemPropertyChanged();

signals:
    void dimmingStatusChanged(bool dimmingStatus);
    void devMediaInfoChanged(NXDevMediaInfo devMediaInfo);
    void reverseGearStateChanged(char reverseGearState);
    void languageIndexChanged(char LanguageIndex);
    void dateFormatChanged(char DateFormat);
    void timeFormatChanged(char TimeFormat);
    void colorStateChanged(char ColorState);
    void mexModeChanged(char MexMode);
    void dialPOICallStatusChanged(char dialPOICallStatus);
    void callServiceStatusChanged(char callServiceStatus);
    void dateTimeDetailInfoChanged(NXDateTimeDetailInfo dateTimeDetailInfo);
	void UpdateLockStatesChanged(NXLockStateInfoList lockStateInfoList);
    void hmiStateChanged(uint hmiState); // this comes up from SPM, 1:SPM_U32_HMISTATE_SHOW_OFF(ACC OFF), 2: SPM_U32_HMISTATE_SHOW_HMI (ACC ON)
    void appStateNavigationChanged(uchar appStateNavigation);

public slots:

private slots:
    void onPropertyChanged(const QString &propertyName, const QVariant &value);

private:
    bool m_dimmingStatus;
    NXDevMediaInfo m_devMediaInfo;
    char m_ReverseGearState;
    char m_LanguageIndex;
    char m_DateFormat;
    char m_TimeFormat;
    char m_ColorState;
    char m_MexMode;
    char m_dialPOICallStatus;
    char m_callServiceStatus;
    uint m_HmiState; // 1: show off(acc off) , 2: show on(acc on)
    uchar m_appStateNavigation;
    NXDateTimeDetailInfo m_TimeDetailInfo;
    NXLockStateInfoList m_lockStateInfo;
};

}

#endif // NAVIGATIONEXTPROPERTYOBSERVER_H
