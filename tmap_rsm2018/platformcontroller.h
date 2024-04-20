#ifndef PLATFORMCONTROLLER_H
#define PLATFORMCONTROLLER_H

#include <QObject>
#include <QString>

// TODO: DO NOT USE PLATFORM-SPECIFIC DATA TYPES

#ifdef PLATFORM_GEN3
#include "hmiZonetype.h"
#include "guicontroltype.h"
#include "navigationexttype.h"
#else
#include "platforms/linux-gen3/interface/navigationext/navigationexttype.h"
#endif
#include "common/rgdataptr.h"

namespace SKT {

class PlatformController : public QObject
{
    Q_OBJECT
public:
    virtual ~PlatformController() {}

public:
    Q_INVOKABLE virtual void init() = 0;
    Q_INVOKABLE virtual void startService() {};
    Q_INVOKABLE virtual void finishService() {};

    virtual QString deviceId() const = 0;

    public:
    virtual bool dimmingStatus() const = 0;
    virtual bool networkOnline() const = 0;
    virtual bool appForeground() const = 0;
    virtual char reverseGearState() const = 0;
    virtual char LanguageIndex() const = 0;
    virtual char DateFormat() const = 0;
    virtual char TimeFormat() const = 0;
    virtual char ColorState() const = 0;
    virtual char MexMode() const = 0;
    virtual char DialPOICallStatus() const = 0;
    virtual char CallServiceStatus() const = 0;
    virtual uint zone3PullUpListState() const = 0;

    Q_INVOKABLE virtual bool remount(const QString &path, bool readOnly) = 0;
    Q_INVOKABLE virtual bool remountCommand(uint type, const QString &srcPath, const QString &destPath) = 0;
    Q_INVOKABLE virtual bool restart() = 0;
    Q_INVOKABLE virtual bool callNumber(const QString &number) = 0;
    Q_INVOKABLE virtual uint carconfigurationValue(uchar key) const = 0;

    Q_INVOKABLE virtual uint doRegisterApp(const QString &AppName) = 0;
    Q_INVOKABLE virtual uint doSetProfileName(const QString &newName, uchar userID) = 0;

    virtual void sendGadgetImage(const QImage& image, int type) = 0;

signals:
    void gotitCarConfigValue(uint value);
    void gotitProfileImages(const QByteArray& images);
    void gotitProfileNames(const QStringList& names);
    void gotitProfileTypes(const types& typeList);
    void doneRemount(bool success);
    void doneRemountCommand(bool success);
    void doneRestart(bool success);

public:
    Q_INVOKABLE virtual QByteArray getProfileImages() const = 0;
    Q_INVOKABLE virtual QStringList getProfileNames() const = 0;
    Q_INVOKABLE virtual types getProfileTypes() const = 0;
    Q_INVOKABLE virtual void setNavigationDataUpdateStatusInfo(int status, float progress) const = 0;

    public Q_SLOTS:
    virtual void onProfileDataChanged(uint result) const = 0;
    virtual void onProfileCopied(uint result) const = 0;
    virtual void onProfileCreated(uint result) const = 0;
    virtual void onProfileDeleted(uint result) const = 0;
    virtual void onPlatformSpmCentralLocStateRequested(int state) = 0; // called from package manager

    public:
    virtual void setZoneVisibility(uchar appId, bool visibilityState, uchar displayRegionType) const = 0;
    virtual void setEditfieldText(uchar appId, const QString &searchBarText, bool isHighLightRequired, uchar highlightStartIndex, uchar highlightCount) const = 0;
    virtual void setEditFieldUpdate(uchar appId, uchar cursorPosition, const QString &searchbarText) const = 0;
    virtual void setKeyboard(uchar appId, uchar keyboardType, const QString &validCharacters)const = 0;
    virtual void setSDSType(uint sdsType, uchar value)const = 0;

    virtual void showPopup(const QString &viewname)const = 0;
    virtual void hidePopup(const QString &viewname)const = 0;
    virtual void beepForwading(int beepType) const = 0;
    virtual void beforePlatformContextSwitchRequested() = 0;

signals:
    void platformInitialized();
    void platformUIRequested();
    void platformMediaMounted(QString mountPath);
    void platformSpmCentralLockStateUpdated(int lockState); // this is invoked when navi owns the central lock while media keep mounted
    void platformDiagDataUpdateRequested();
    void platformDiagDataDeleteRequested();
    void platformMediaUnmounted();
    void platformRemountStatus(bool success);
    void platformDimmingStatusChanged(bool dimming);
    void platformNetworkOnlineChanged(bool online);
    void platformAppForegroundChanged(bool foreground);
    void platformReverseGearStateChanged(char gearState);
    void platformLanguageIndexStatusChanged(char languageIndex);
    void platformDateFormatStatusChanged(char dateFormat);
    void platformTimeFormatStatusChanged(char timeFormat);
    void platformColorStateStatusChanged(char colorState);
    void platformMexModeStatusChanged(char mexMode);
    void platformDialPOICallStatusChanged(char dialPOICallStatus);
    void platformCallServiceStatusChanged(char callServiceStatus);
    void platformZone3PullUpListStateChanged(uint zone3PullUpListState);
    void platformIsHFPAvailable(bool PhoneCallStatus);
    void platformRemountDone(bool success);
    void platformCallNumberDone(bool success);
    void platformPrivateDataEncryptionRegistered(QObject* interface);
    void platformActivateExternalImageProvider(bool activate, int instanceId);
    void platformExternalImageProviderTouch(quint8 touchState, uint xPos, uint yPos, uint timeStamp, int instanceId);
    void platformHmiStateChanged(uint showOnOff); // acc off, acc on
    void platfromNaviSignalRFAntennaStatus(uchar u8RFAntenna, uchar u8RFAntennaStatus, uchar u8ConnectionCurrent);
    void platformCurrentStreetChanged(RGDataPtr value);
    void platformfocusConfigChanged(char focusConfig);
    void platformNaviGadgetFocusStatusChanged(uint instanceId, bool focus);
    void platformNaviGadgetFocusInteractionSignal(uint instanceId, uint interactionType);

    void platformContextSwitchRequested(QVariant event);
};

}

#endif // PLATFORMCONTROLLER_H
