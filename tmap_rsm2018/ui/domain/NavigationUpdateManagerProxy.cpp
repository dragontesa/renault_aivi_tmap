#include "NavigationUpdateManagerProxy.h"
#include "RequestCommand.h"
#include "Constants.h"
#include "CommonConstants.h"

#include <QDebug>
#include "StringHelper.h"
#include "NavigationUpdateManagerProtocol.h"
#include "NavigationUpdateManagerConstants.h"
#include "NavigationPropertyProtocol.h"
#include "NavigationPropertyConstants.h"
#include "QmlAppHelper.h"
#include "SettingConstants.h"
#include "SearchConstants.h"
#include "ViewRegistry.h"
#include "RouteGuidanceProxy.h"
#include "ApplicationConstants.h"
#include <QFile>
#include "../environment.h"

namespace SKT {

NavigationUpdateManagerProxy::NavigationUpdateManagerProxy(QObject* controller, QObject* updateManager, QObject* property)
    : mController(qobject_cast<NavigationController*>(controller) ),
      mUpdateManager(updateManager ),
      mProperty(property),
      mMounted(false),
      mStatus(State::StateIdle),
      mProgress(0.0),
      mIsRg(false)
{

    connect(mUpdateManager, SIGNAL(updatePackageMounted(bool)), this, SLOT(onUpdatePackageMounted(bool)));
    connect(mUpdateManager, SIGNAL(updatePackageUnmounted()), this, SLOT(onUpdatePackageUnmounted()));
    connect(mUpdateManager, SIGNAL(updatePackageProgress(quint64, quint64, quint32, quint32)),
                      this, SLOT(onUpdatePackageProgress(quint64, quint64, quint32, quint32)));
    connect(mUpdateManager, SIGNAL(updatePackageStored()), this, SLOT(onUpdatePackageStored()));
    connect(mUpdateManager, SIGNAL(updatePackageError(SKT::UpdateManagerInterface::ErrorType)),
                      this, SLOT(onUpdatePackageError(SKT::UpdateManagerInterface::ErrorType)));
    connect(mUpdateManager, SIGNAL(packageStatusChanged(SKT::UpdateManagerInterface::PackageStatus)),
                      this, SLOT(onPackageStatusChanged(SKT::UpdateManagerInterface::PackageStatus))); //TMCRSM-1377
    connect(mUpdateManager, SIGNAL(updateApplied()), this, SLOT(onUpdateApplied()));
}

void NavigationUpdateManagerProxy::applyCommand(const RequestCommandPtr& req)
{
    QVariantMap reqData = req->getReqData();
    reqData.insert(CommonConstants::RequestCommandPtr,QVariant::fromValue(req));

    const int type = reqData[CommonConstants::Type].toInt();
    switch(type) {
        case NavigationUpdateManagerConstants::ApplyRgStatus:
            mIsRg = reqData[CommonConstants::Value].toBool();
            break;
        case NavigationUpdateManagerConstants::ApplyStartUpdate:  {
                UpdateManagerInterface* pUpdateManager = qobject_cast<UpdateManagerInterface*>(mUpdateManager);
                if (pUpdateManager) {
                    pUpdateManager->startUpdate();

                    QVariantMap data;
                    data["type"] = NavigationPropertyProtocol::TmapNaviStatus;
                    data["propertyValue"] = TMAPNAVISTATUS_MAP_UPDATING;
                    sendNotification(NavigationPropertyConstants::setNavigationProperty, data);
                }
            }
            break;
        case NavigationUpdateManagerConstants::ApplyStopUpdate:{
                UpdateManagerInterface* pUpdateManager = qobject_cast<UpdateManagerInterface*>(mUpdateManager);
                if (pUpdateManager) {
                    pUpdateManager->stopUpdate();

                    QVariantMap data;
                    data["type"] = NavigationPropertyProtocol::TmapNaviStatus;
                    data["propertyValue"] = mIsRg ?  TMAPNAVISTATUS_PLAYROUTE : TMAPNAVISTATUS_ANDO;
                    sendNotification(NavigationPropertyConstants::setNavigationProperty, data);
                }
            }
            break;
        case NavigationUpdateManagerConstants::ApplyCancelUpdate:{
        UpdateManagerInterface* pUpdateManager = qobject_cast<UpdateManagerInterface*>(mUpdateManager);
                if (pUpdateManager) {
                    pUpdateManager->cancelUpdate();
                }
            }
            break;
        case NavigationUpdateManagerConstants::ApplyUpdate: {
                UpdateManagerInterface* pUpdateManager = qobject_cast<UpdateManagerInterface*>(mUpdateManager);
                if (pUpdateManager) {
                    mStatus = State::StateStart;
                    mProgress = 0.0;
                    pUpdateManager->applyUpdate();

                    QVariantMap data;
                    data["type"] = NavigationPropertyProtocol::TmapNaviStatus;
                    data["propertyValue"] = TMAPNAVISTATUS_MAP_UPDATING;
                    sendNotification(NavigationPropertyConstants::setNavigationProperty, data);
                }
            }
            break;
        default:
            break;
    }
}


void NavigationUpdateManagerProxy::getCommand(const RequestCommandPtr& req)
{
    QVariantMap reqData = req->getReqData();
    reqData.insert(CommonConstants::RequestCommandPtr,QVariant::fromValue(req));

    const int type = reqData[CommonConstants::Type].toInt();
    switch(type) {
        case NavigationUpdateManagerConstants::GetStatusUpdateCommitted: {
                UpdateManagerInterface* pUpdateManager = qobject_cast<UpdateManagerInterface*>(mUpdateManager);
                if (pUpdateManager) {
                    if (pUpdateManager->getStatusUpdateCommitted()) {
                        QVariantMap body;
                        body[CommonConstants::Position] = ToastsConstants::ToastUp;
                        body[CommonConstants::Duration] = 5 * 1000;
                        body[CommonConstants::Message] = tr("새 지도 데이터가 있어 지도를 업데이트 하였습니다");
                        sendNotification(ToastsConstants::ShowToast, body);
                    }
                }
            }
            break;
    }
}


void NavigationUpdateManagerProxy::requestCommand(const RequestCommandPtr& req)
{
    QVariantMap reqData = req->getReqData();
    reqData.insert(CommonConstants::RequestCommandPtr,QVariant::fromValue(req));

    const int type = reqData[CommonConstants::Type].toInt();
    switch(type) { 
        case NavigationUpdateManagerConstants::RequestPackageStatus: {
            UpdateManagerInterface* pUpdateManager = qobject_cast<UpdateManagerInterface*>(mUpdateManager);
            if (pUpdateManager) {
                sendNotification(Constants::CreateView, QVariantMap{
                                     {CommonConstants::Url, ViewRegistry::url(ViewRegistry::SettingMapUpdateView)},
                                     {CommonConstants::Visible, false}});

                int status = (int)pUpdateManager->packageStatus();
                sendNotification(NavigationUpdateManagerConstants::Response,
                                QVariantMap{
                                     {CommonConstants::Type, NavigationUpdateManagerConstants::ResponsePackageStatus},
                                     {CommonConstants::Value, status}});

            }
        }
        break;                
        case NavigationUpdateManagerConstants::RequestNeedRestart: {
            sendNotification(NavigationUpdateManagerConstants::Response,
                            QVariantMap{
                                 {CommonConstants::Type, NavigationUpdateManagerConstants::ResponseNeedRestart},
                                 {CommonConstants::Value, (mStatus == State::StateDone)}});
        }
        break;
        default:
            break;
    }
}

void NavigationUpdateManagerProxy::onUpdatePackageMounted(bool locked)
{
    mMounted = true;
    sendNotification(Constants::CreateView, QVariantMap{
                         {CommonConstants::Url, ViewRegistry::url(ViewRegistry::SettingMapUpdateView)},
                         {CommonConstants::Visible, false}});


    sendNotification(NavigationUpdateManagerConstants::Response,
                     QVariantMap{{CommonConstants::Type, NavigationUpdateManagerConstants::ResponseUpdatePackageMounted},
                                 {CommonConstants::Value, locked}});

}


void NavigationUpdateManagerProxy::onUpdatePackageUnmounted()
{
     mMounted = false;
     sendNotification(NavigationUpdateManagerConstants::Response,
                      QVariantMap{{CommonConstants::Type, NavigationUpdateManagerConstants::ResponseUpdatePackageUnmounted}});

}

void NavigationUpdateManagerProxy::requestToViewList(int viewId)
{
    QVariantMap body;
    body.insert( CommonConstants::ObjectName,   ViewRegistry::objectName( ViewRegistry::MapView ));
    body.insert( CommonConstants::Relay, "NavigationUpdateManagerProxy");
    body.insert( CommonConstants::ViewId, viewId);
    sendNotification(ApplicationConstants::RequestViewList, body);
}

void NavigationUpdateManagerProxy::sendReplyToViewList(const QList<int> data, const int findId)
{
    emit responseToViewList(data);
}


void NavigationUpdateManagerProxy::onUpdatePackageProgress(quint64 storedSize, quint64 totalSize, quint32 count, quint32 totalCount)
{
    mStatus = State::StateCopy;    
    sendNotification(NavigationUpdateManagerConstants::Response,
                     QVariantMap{{CommonConstants::Type, NavigationUpdateManagerConstants::ResponseUpdatePackageProgress},
                                 {SettingConstants::StoredSize, storedSize},
                                 {SettingConstants::TotalSize, totalSize},
                                 {SettingConstants::Count, count},
                                 {SettingConstants::TotalCount, totalCount }});

    mProgress = (((float)storedSize)/((float)totalSize))*100.0;

    QVariantMap data;
    data["type"] = NavigationPropertyProtocol::TmapNaviStatus;
    data["propertyValue"] = TMAPNAVISTATUS_MAP_UPDATING;
    sendNotification(NavigationPropertyConstants::setNavigationProperty, data);
}

void NavigationUpdateManagerProxy::onUpdatePackageStored()
{
    mStatus = State::StateDone;
    sendNotification(Constants::CreateView, QVariantMap{
                         {CommonConstants::Url, ViewRegistry::url(ViewRegistry::SettingMapUpdateView)},
                         {CommonConstants::Visible, false}});


    sendNotification(NavigationUpdateManagerConstants::Response,
                     QVariantMap{{CommonConstants::Type, NavigationUpdateManagerConstants::ResponseUpdatePackageStored}});


    QVariantMap data;
    data["type"] = NavigationPropertyProtocol::TmapNaviStatus;
    data["propertyValue"] = mIsRg ?  TMAPNAVISTATUS_PLAYROUTE : TMAPNAVISTATUS_ANDO;
    sendNotification(NavigationPropertyConstants::setNavigationProperty, data);
}

void NavigationUpdateManagerProxy::onUpdatePackageError(SKT::UpdateManagerInterface::ErrorType errorType)
{
    if (errorType == UpdateManagerInterface::ErrorTypeMediaUnmounted)
        mMounted = false;

    sendNotification(Constants::CreateView, QVariantMap{
                         {CommonConstants::Url, ViewRegistry::url(ViewRegistry::SettingMapUpdateView)},
                         {CommonConstants::Visible, false}});


    int err = (int) errorType;
    sendNotification(NavigationUpdateManagerConstants::Response,
                    QVariantMap{
                         {CommonConstants::Type, NavigationUpdateManagerConstants::ResponseUpdatePackageError},
                         {CommonConstants::Value, err}});

    mStatus = State::StateError;

    QVariantMap data;
    data["type"] = NavigationPropertyProtocol::TmapNaviStatus;
    data["propertyValue"] = mIsRg ?  TMAPNAVISTATUS_PLAYROUTE : TMAPNAVISTATUS_ANDO;
    sendNotification(NavigationPropertyConstants::setNavigationProperty, data);
}

//TMCRSM-1377
void NavigationUpdateManagerProxy::onPackageStatusChanged(SKT::UpdateManagerInterface::PackageStatus packageStatus)
{
    if (packageStatus != UpdateManagerInterface::PackageStatusError) {
        return;
    }
    sendNotification(Constants::CreateView, QVariantMap{{CommonConstants::Url, ViewRegistry::url(ViewRegistry::SettingMapUpdateView)},
                                                        {CommonConstants::Caller, CommonConstants::ErrorMapData},
                                                        {CommonConstants::Visible, false}});
}

void NavigationUpdateManagerProxy::onUpdateApplied()
{
    sendNotification(Constants::CreateView, QVariantMap{
                         {CommonConstants::Url, ViewRegistry::url(ViewRegistry::SettingMapUpdateView)},
                         {CommonConstants::Visible, false}});


    sendNotification(NavigationUpdateManagerConstants::Response,
                     QVariantMap{{CommonConstants::Type, NavigationUpdateManagerConstants::ResponseUpdateApplied}});

}

}
