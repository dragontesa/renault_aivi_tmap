#include "SystemPopupMediator.h"
#include "ApplicationConstants.h"
#include "CommonConstants.h"
#include "DebugUtils.h"
#include "NavigationControllerConstants.h"
#include "PlatformHelper.h"
#include "SettingConstants.h"
#include "ViewRegistry.h"
#include <QTimer>
#include "../setting/SettingMapUpdateViewMediator.h"

namespace SKT
{

const int WAITING_CLOSE_VIEW_MSEC = 500;
const QString KEY_MESSAGE_TYPE = CommonConstants::MessageType;

SystemPopupMediator::SystemPopupMediator()
    : AbstractMediator()
{
    mTimer.setInterval(WAITING_CLOSE_VIEW_MSEC);
    mTimer.setSingleShot(true);
    connect(&mTimer, &QTimer::timeout, this, [&](){
        create1stView();
    });
}

QList<int> SystemPopupMediator::listNotificationInterests()
{
    return QList<int> {
        ApplicationConstants::CreateSystemPopup,
        ApplicationConstants::SystemPopupAccepted,
        ApplicationConstants::SystemPopupCanceled,
        ApplicationConstants::ReplyToCommonDialogExist,
        SettingConstants::ExitMapUpdate,
    };
}

bool SystemPopupMediator::handleNotification(const mvc::INotificationPtr &note)
{
    switch (note->getId()) {
    case ApplicationConstants::CreateSystemPopup:
        createSystemView(note->getBody());
        break;

    case ApplicationConstants::SystemPopupAccepted:
        accepted(note->getBody());
        break;

    case ApplicationConstants::SystemPopupCanceled:
        canceled(note->getBody());
        break;

    case ApplicationConstants::ReplyToCommonDialogExist:
        mIsCommonDialogViewExist = true;
        break;

    case SettingConstants::ExitMapUpdate:
        mPopupsMap.remove(MapUpdate);
        mCreateMessageType = -1;
        delayedCreate1stView();
        break;
    }
    return true;
}

void SystemPopupMediator::accepted(const QVariant &v)
{
    auto m = v.toMap();
    auto type = m["popupType"].toInt();
    auto notiSystemPopupClosed = false;

    switch (type) {
    case MapUpdate:
        delayedCreate1stView();
        break;

    case ContinueRg:
        // 경로 이어하기를 수행 하면 Send2Car는 무시된다.
        if (mPopupsMap.contains(Send2Car)) {
            mPopupsMap.remove(Send2Car);
            notiSystemPopupClosed = true;
        }
        break;

    case Send2Car:
        notiSystemPopupClosed = true;
        break;
    }

    if (isMapUpdate()) {
        qDebug() << "isMapUpdate.";
    } else {
        if (type == MapUpdate) {
            mCreateMessageType = -1;
        }
        mPopupsMap.remove(type);
        if (notiSystemPopupClosed) {
            sendNotification(ApplicationConstants::SystemPopupClosed);
        }
    }

    sendNotification(NavigationControllerConstants::SwitchToAppIfNecessary);
}

void SystemPopupMediator::canceled(const QVariant &v)
{
    auto m = v.toMap();
    auto type = m["popupType"].toInt();
    if (type == MapUpdate) {
        mCreateMessageType = -1;
    }

    mPopupsMap.remove(type);
    delayedCreate1stView();
}

void SystemPopupMediator::createSystemView(const QVariant &v)
{
    auto m = v.toMap();
    auto type = m["popupType"].toInt();

    if (mPopupsMap.contains(type)) {
        auto messageType = m[KEY_MESSAGE_TYPE].toInt();
        auto m2 = mPopupsMap.value(type);
        auto messageType2 = m2[KEY_MESSAGE_TYPE].toInt();
        if (messageType == messageType2) {
            qDebug() << "MapUpdatePopup is already opend. messageType: " << messageType;

            DebugUtils::printMap("SystemPopupMediator::createSystemView()", m);
            return;
        }
    }

    m.insert("isSystemPopup", true);
    PlatformHelper::instance()->setParentForSystemPopup(m);

    switch (type) {
    case MapUpdate:
        qDebug() << "createMapUpdateView ##########################   1";
        createMapUpdateView(m);
        break;

    case ContinueRg:
        qDebug() << "createContinueRgView ##########################  2";
        createContinueRgView(m);
        break;

    case Send2Car:
        qDebug() << "createSend2CarView ##########################    3";
        createSend2CarView(m);
        break;
    }
    mPopupsMap.insert(type, m);
}

bool SystemPopupMediator::isMapUpdate() const
{
    auto m = mPopupsMap.value(MapUpdate);
    if (m.isEmpty()) {
        return false;
    }
    auto messageType = m[KEY_MESSAGE_TYPE].toInt();
    switch (messageType) {
    case SettingMapUpdateViewMediator::CopyMapData1Ko:
    case SettingMapUpdateViewMediator::SameVersionKo:
        return true;

    default:
        return false;
    }
}

void SystemPopupMediator::createMapUpdateView(const QVariantMap &m)
{
    if (mPopupsMap.contains(ContinueRg) || mPopupsMap.contains(Send2Car) || mPopupsMap.contains(MapUpdate)) {
        closeCommonDialog();
        delayedCreate1stView();

        // 이미 맵업데이트 팝업이 등록 되어 있는 경우 교체를 위해 제거한다.
        if (mPopupsMap.contains(MapUpdate)) {
            mPopupsMap.remove(MapUpdate);
        }
    } else {
        // 시스템 팝업이 아닌 일반 CommonDialogView가 떠 있는 지 검사.
        mIsCommonDialogViewExist = false;
        sendNotification(ApplicationConstants::RequestCommonDialogExist);

        if (mIsCommonDialogViewExist) {
            // 현재 떠 있는 CommonDialogView가 있다면 기본 처리를 하도록 호출.
            closeCommonDialog();
            delayedCreate1stView();
        } else {
            sendNotification(ApplicationConstants::SystemPopupCreated);
            if (m.contains(KEY_MESSAGE_TYPE)) {
                mCreateMessageType = m[KEY_MESSAGE_TYPE].toInt();
            }
            sendNotification(Constants::CreateView, m);
        }
    }
}

void SystemPopupMediator::createContinueRgView(const QVariantMap &m)
{
    if (mPopupsMap.contains(MapUpdate)) {
        return;
    }

    if (mPopupsMap.contains(Send2Car)) {
        closeCommonDialog();
        delayedCreate1stView();
    } else {
        sendNotification(ApplicationConstants::SystemPopupCreated);
        sendNotification(Constants::CreateView, m);
    }
}

void SystemPopupMediator::createSend2CarView(const QVariantMap &m)
{
    if (mPopupsMap.contains(MapUpdate) || mPopupsMap.contains(ContinueRg)) {
        return;
    }
    sendNotification(ApplicationConstants::SystemPopupCreated);
    sendNotification(Constants::CreateView, m);
}

void SystemPopupMediator::create1stView()
{
    QVariantMap m;
    if (mPopupsMap.contains(MapUpdate)) {
        m = mPopupsMap.value(MapUpdate);

        // 같은 맵업데이트 다이얼로그를 중복 생성 하는지 체크.
        auto messageType = m[KEY_MESSAGE_TYPE].toInt();
        if (messageType == mCreateMessageType) {
            m.clear();
        } else {
            mCreateMessageType = messageType;
        }
    } else if (mPopupsMap.contains(ContinueRg)) {
        m = mPopupsMap.value(ContinueRg);
    } else if (mPopupsMap.contains(Send2Car)) {
        m = mPopupsMap.value(Send2Car);
    }
    if (!m.isEmpty()) {
        DebugUtils::printMap("SystemPopupMediator::CreateView", m);
        sendNotification(ApplicationConstants::SystemPopupCreated);
        sendNotification(Constants::CreateView, m);
    } else {
        if (!isMapUpdate()) {
            sendNotification(ApplicationConstants::SystemPopupClosed);
        }
    }
}

void SystemPopupMediator::delayedCreate1stView()
{
    if (mTimer.isActive()) {
        mTimer.stop();
    }
    mTimer.start();
}

void SystemPopupMediator::closeCommonDialog()
{
    // CommonDialogView가 닫기 전 기본 동작 처리를 하도록 요청한다.
    sendNotification(ApplicationConstants::CloseCommonDialogForSystemPopup);

    auto viewName = ViewRegistry::objectName(ViewRegistry::CommonDialogView);
    sendNotification(Constants::CloseView, QVariantMap{
                         {CommonConstants::ObjectName, viewName}
                     });
}

}
