#ifndef ROUTESEARCHMEDIATOR_H
#define ROUTESEARCHMEDIATOR_H

#include "mediator/AbstractMediator.h"
#include "mediator/app/MapViewMode.h"

namespace SKT
{

class RouteSearchMediator : public AbstractMediator
{
    Q_OBJECT

public:
    RouteSearchMediator();

    virtual QList<int> listNotificationInterests() Q_DECL_OVERRIDE;
    virtual bool handleNotification(const mvc::INotificationPtr &) Q_DECL_OVERRIDE;

private:
    void backup();
    void deleteBackupFiles();
    void deleteTvasFiles();
    void deleteTvasFilesWithoutRouteFile();
    void deleteFiles(const QStringList& files);
    bool restore();
    QString routePath() const;
    void prepareStartRouteGuidance(const QVariantMap&);
    bool sendByDestination(const QVariantMap &data);
    void showConfirmDialog(const QString &address);
    void closeAllView();
    void removeSend2CarData(QVariantMap);
    void applyVoiceGuidanceVocalPoint(bool send = true);
    void applyVoiceGuidance(bool send = true);
    void applyCameraGuidance(bool send = true);
    void applyAttentionGuidance(bool send = true);
    void applyRoadViolation(bool send = true);
    void applySafeDrivingSoundAtMute(bool send = true);
    void applySpeedLevel(bool send = true);
    void applyVoice(bool send = true);
    void responseSendToCar();

private:
    bool mIsInDriving;
    bool mIsTvasBackup;
    int mSelectedTvasId;
    QStringList mTvasFiles;
    QStringList mTvasBackupFiles;
    QTimer mTimer;
    bool mIsRerouteForCalcDone;
    bool mIsContinous = false;
    bool mIsServerRequested = false;
    bool mIsRg;

    QVariantMap mDestData;
    bool mRequestDriveMode;

    QVariantMap mSendToCar;
    bool mForeground = false;

    QVariantMap mDialogData;

    QObject* mapView;
   MapViewMode::ModeK mMapViewMode = MapViewMode::Normal;
};

}//SKT

#endif // ROUTESEARCHMEDIATOR_H
