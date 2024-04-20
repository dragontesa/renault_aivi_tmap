#include "SettingMapUpdateViewMediator.h"

#include <QTimer>
#include <QWindow>
#include <QDir>

#include "SettingDto.h"
#include "RgConstants.h"
#include "Constants.h"
#include "ZoneConstants.h"
#include "SettingConstants.h"
#include "NavigationUpdateManagerConstants.h"
#include <QJSValueIterator>
#include "SearchConstants.h"
#include "ViewRegistry.h"
#include "ToastsConstants.h"
#include "KeyboardConstants.h"

#include "NavigationControllerProtocol.h"
#include "NavigationControllerConstants.h"

#include "PlatformHelper.h"
#include "LangConstants.h"
#include "ApplicationConstants.h"
#include "../app/SystemPopupMediator.h"

using namespace SKT;

SettingMapUpdateViewMediator::SettingMapUpdateViewMediator( QObject* viewComponent )
    : AbstractViewMediator( viewComponent )
{
}

QList<int> SettingMapUpdateViewMediator::listNotificationInterests()
{
    static QList<int> notes = {
        NavigationControllerConstants::ResponseController,
        NavigationUpdateManagerConstants::Response,
        NavigationControllerConstants::NavigationAppeared,
        ZoneConstants::DoSoftKeyFunction,
        ZoneConstants::DoSoftkeyIndexMenuFunction,
        SettingConstants::UpdateView,
        SettingConstants::DialogAccepted,
        SettingConstants::DialogCanceled,
        NavigationControllerConstants::ChangeLanguage,
        ApplicationConstants::ReplyToViewList,
    };

    return notes;
}

bool SettingMapUpdateViewMediator::onEvent( int event, const QVariant& data )
{
    T_DEBUGV( "event: %d", event );

    switch ( event ) {
    case Mediated:
        break;

    case Activated: {
            // CommonDialog 사용 이전 필요했던 조건문. 현재는 필요 없고 시스템 화면에서 내비로 진입 시
            // 맵업데이트 화면에서 내비메뉴가 노출 됨.
            // 삭제 처리함
            // 20.02.19 cjlee

//            QString caller = QQmlProperty::read(mViewComponent, CommonConstants::Caller).toString();
//            if (caller == CommonConstants::IncompleteMapData) {
//                break;
//            }
            sendNotification(ZoneConstants::RegistSoftkey, ViewRegistry::objectName(ViewRegistry::SettingMapUpdateView));
            sendNotification(ZoneConstants::SelectSoftkeyIndexMenuMode, data);
        }
        break;

    case Deactivated: {
//            QString caller = QQmlProperty::read(mViewComponent, CommonConstants::Caller).toString();
//            if (caller == CommonConstants::IncompleteMapData)
//                break;
            sendNotification(ZoneConstants::SelectSoftkeyIndexMenuMode, QStringList{});
        }
        break;

    case SelectZone1Title:
        sendNotification(ZoneConstants::SelectZone1Title, data);
        break;

    case BlockSoftkeyMenu:
        sendNotification(ZoneConstants::BlockSoftkeyMenu, data);
        break;
    case CreateView:
        {
            auto m = data.toMap();
            auto url = m[CommonConstants::Url].toString();
            if (url == ViewRegistry::url(ViewRegistry::CommonDialogView)) {
                m["popupType"] = SystemPopupMediator::MapUpdate;
                auto messageType = m[CommonConstants::MessageType].toInt();
                qDebug() << "SettingMapUpdateViewMediator.CreateSystemPopup(" << messageType << ")";
            }
            sendNotification(ApplicationConstants::CreateSystemPopup, m);
        }
        break;
    case CloseView: {
        auto m = data.toMap();
        auto viewName = m[CommonConstants::ObjectName].toString();
        if (viewName == ViewRegistry::objectName(ViewRegistry::CommonDialogView)) {
            // CommonDialogView는 CloseView로 닫기 전에
            // 취소 버튼이 있으면 취소 동작을
            // 확인 버튼만 있다면 확인 동작을 한다.
            // 각 뷰에서 시나리오상 이상 동작이 있을 수가 있음
            // 예. 경유지 설정 후 경로 탐색 했을 때 없는 경로 팝업의 경우... 그냥 닫으면 경로 요약 화면에
            // 머물러 있게 됨.
            sendNotification(ApplicationConstants::CloseCommonDialogForSystemPopup);
        }
        sendNotification(Constants::CloseView, data);

        auto updateStatus = QQmlProperty::read(mViewComponent, "updataStatus").toInt();
        switch (updateStatus) {
        case UpdateStart:
        case UpdateProgress:
        case UpdateEnd:
            sendNotification(SettingConstants::ExitMapUpdate);
            break;

        default:
            break;
        }
        break;
    }

    case ModalChanged: {
            sendNotification(ZoneConstants::UpdateModal,data.toMap());
        }
        break;

    case ShowToast: {
            sendNotification(ToastsConstants::ShowToast, data);
        }
        break;

    case RequestSetting: {
            QVariantMap body;
            body.insert( CommonConstants::Type, NavigationControllerProtocol::GetAppVersion );
            sendNotification( NavigationControllerConstants::RequestController, body );
        }
        break;

    case RequestPackageStatus: {
            QVariantMap body;
            body.insert( CommonConstants::Type, NavigationUpdateManagerConstants::RequestPackageStatus );
            sendNotification( NavigationUpdateManagerConstants::Request, body );
        }
        break;

    case RequestNeedRestart: {
            QVariantMap body;
            body.insert( CommonConstants::Type, NavigationUpdateManagerConstants::RequestNeedRestart );
            sendNotification( NavigationUpdateManagerConstants::Request, body );
        }
        break;

    case RequestStartUpdate: {
            QString ignorePopup = SettingConstants::MapUpdatePath + QDir::separator() + SettingConstants::IgnorePopupFileName;
            QFile::remove(ignorePopup);
            QVariantMap body;
            body.insert( CommonConstants::Type, NavigationUpdateManagerConstants::ApplyStartUpdate );
            sendNotification( NavigationUpdateManagerConstants::Apply, body );
        }
        break;
    case RequestStopUpdate: {
            QVariantMap body;
            body.insert( CommonConstants::Type, NavigationUpdateManagerConstants::ApplyStopUpdate );
            sendNotification( NavigationUpdateManagerConstants::Apply, body );
        }
        break;
    case RequestCancelUpdate: {
            QVariantMap body;
            body.insert( CommonConstants::Type, NavigationUpdateManagerConstants::ApplyCancelUpdate );
            sendNotification( NavigationUpdateManagerConstants::Apply, body );
        }
        break;
    case RequestApplyUpdate: {
            QVariantMap body;
            body.insert( CommonConstants::Type, NavigationUpdateManagerConstants::ApplyUpdate );
            sendNotification( NavigationUpdateManagerConstants::Apply, body );
        }
        break;
    case HideKeyboard:
        sendNotification(KeyboardConstants::HideKeyboard, data);
        break;

    case ResetCsw:
        sendNotification(NavigationControllerConstants::ResetCSW);
        break;

    case RegistCsw:
        sendNotification(NavigationControllerConstants::RegistCSW, data);
        break;

    case RequestExit:
        // exit(0);
        break;

    case Back:
#ifdef BUILD_TARGET
        // UID 1.12
        PlatformHelper::instance()->switchToSystem();
#endif
        break;
     case RequestPopupInCompleteMapDataCopy: {
            QDir dir(SettingConstants::MapUpdatePath);
            QString ignorePopup = SettingConstants::MapUpdatePath + QDir::separator() + SettingConstants::IgnorePopupFileName;
            QMetaObject::invokeMethod(mViewComponent, "__popupInCompleteMapDataCopy", Q_ARG(QVariant, QVariant::fromValue(dir.exists() && !QFile(ignorePopup).exists())));
        }
        break;
     case RequestIgnoreMapUpdate: {
            QString ignorePopup = SettingConstants::MapUpdatePath + QDir::separator() + SettingConstants::IgnorePopupFileName;
            QFile out(ignorePopup);
            if (!out.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
                out.close();
            }
        }
        break;
    case RequestViewList: {
            QVariantMap body;
            body.insert( CommonConstants::ObjectName,   ViewRegistry::objectName(ViewRegistry::SettingMapUpdateView));
            sendNotification(ApplicationConstants::RequestViewList, body);
        }
        break;

    case RequestLang: {
            QVariantMap body;
            body.insert(QStringLiteral("type"), NavigationControllerProtocol::GetLang );
            sendNotification( NavigationControllerConstants::RequestController, body );
        }
        break;
    case RequestDlgAccepted: {
            sendNotification( ApplicationConstants::RequestCommonDialogAccepted, data);
        }
        break;
    case RequestDlgCanceled: {
            sendNotification( ApplicationConstants::RequestCommonDailogCanceled, data);
        }
        break;
    default:
        return false;
    }
    return true;
}

bool SettingMapUpdateViewMediator::handleNotification( const mvc::INotificationPtr& note )
{
    switch ( note->getId() ) {    
    case NavigationControllerConstants::NavigationAppeared: {
            auto caller = QQmlProperty::read(mViewComponent, "caller").toString();
            if (caller == SettingConstants::NoMapData) {
                QMetaObject::invokeMethod(mViewComponent, "__showZone1Title");
            }
        }
        break;
    case NavigationControllerConstants::ResponseController: {
            QVariantMap data = note->getBody().value<QVariantMap>();
            if (data.contains(CommonConstants::Lang)) {
                QString value = data[CommonConstants::Lang].toString();
                QQmlProperty::write( mViewComponent, "__lang", QVariant::fromValue(value) );
            } else if (data.contains(SettingConstants::AppVersion)) {
                auto owner = reinterpret_cast<void*>(data[CommonConstants::Owner].toULongLong());
                if (owner != this)
                    break;
                QString appVersion = data[SettingConstants::AppVersion].toString();
                QString mapVersion = data[SettingConstants::MapVersion].toString();
                QString sdiVersion = data[SettingConstants::SDIVersion].toString();
                QString localNetworkVersion = data[SettingConstants::LocalNetworkVersion].toString();
                QString localPoiVersion = data[SettingConstants::LocalPoiVersion].toString();
                QString localMapVersion = data[SettingConstants::LocalMapVersion].toString();
                QVariantList result;
                for(int i=0; i < ResultEnd; i++) {
                    result.append(CommonConstants::Empty);
                }
                //프로그램 버전
                result.insert(ResultAppVersion,          appVersion);
                //지도 버전
                result.insert(ResultMapVersion,          mapVersion);
                //내장 지도 버전
                result.insert(ResultLocalMapVersion,     localMapVersion);
                //내장 전국 도로 버전
                result.insert(ResultLocalNetworkVersion, localNetworkVersion);
                //내장 명칭 데이타 버전
                result.insert(ResultLocalPoiVersion,     localPoiVersion);
                //안전 운전 버전
                result.insert(ResultSDIVersion,          sdiVersion);
                //지도성과심업체
                result.insert(ResultCompany,        CommonConstants::Empty);
                //디버깅
                result.insert(ResultDebuging,       CommonConstants::Empty);
                QQmlProperty::write( mViewComponent, CommonConstants::Settings, QVariant::fromValue( result ) );
            }
        }
        break;
    case NavigationUpdateManagerConstants::Response: {
            QVariantMap data = note->getBody().value<QVariantMap>();
            if (!data.contains(CommonConstants::Type))
                return false;
            switch(data[CommonConstants::Type].toInt()) {
                case NavigationUpdateManagerConstants::ResponseUpdatePackageMounted: {
                        bool locked = data[CommonConstants::Value].toBool();
                        QMetaObject::invokeMethod(mViewComponent, "__updatePackageMounted",  Q_ARG(QVariant, QVariant::fromValue(locked)));
                    }
                    break;
                case NavigationUpdateManagerConstants::ResponseUpdatePackageUnmounted:
                    QMetaObject::invokeMethod(mViewComponent, "__updatePackageUnmounted");
                    break;
                case NavigationUpdateManagerConstants::ResponsePackageStatus: {
                    int value = data[CommonConstants::Value].toInt();
                    QMetaObject::invokeMethod(mViewComponent, "__packageStatus", Q_ARG(QVariant, QVariant::fromValue(value)));
                }
                break;
                case NavigationUpdateManagerConstants::ResponseUpdatePackageProgress: {
                    auto storedSize = data[SettingConstants::StoredSize].toLongLong();
                    auto totalSize = data[SettingConstants::TotalSize].toLongLong();
                    int count = data[SettingConstants::Count].toInt();
                    int totalCount = data[SettingConstants::TotalCount].toInt();
                    QMetaObject::invokeMethod(mViewComponent, "__updatePackageProgress",
                                              Q_ARG(QVariant, QVariant::fromValue(storedSize)),
                                              Q_ARG(QVariant, QVariant::fromValue(totalSize)),
                                              Q_ARG(QVariant, QVariant::fromValue(count)),
                                              Q_ARG(QVariant, QVariant::fromValue(totalCount))  );
                }
                break;
                case NavigationUpdateManagerConstants::ResponseUpdatePackageStored: {
                    QMetaObject::invokeMethod(mViewComponent, "__updatePackageStored");
                }
                break;
                case NavigationUpdateManagerConstants::ResponseUpdatePackageError: {
                    int err = data[CommonConstants::Value].toInt();
                    QMetaObject::invokeMethod(mViewComponent, "__updatePackageError", Q_ARG(QVariant, QVariant::fromValue(err)));

                }
                break;
                case NavigationUpdateManagerConstants::ResponseUpdateApplied: {
                    QMetaObject::invokeMethod(mViewComponent, "__updateApplied");
                }
                break;                
                case NavigationUpdateManagerConstants::ResponseNeedRestart: {
                    bool success = data[CommonConstants::Value].toBool();
                    QMetaObject::invokeMethod(mViewComponent, "__updatePackageComplete",  Q_ARG(QVariant, QVariant::fromValue(success)));
                }
                break;
            }//swich
        }
        break;

    case SettingConstants::ApplySetting:
        {
            QVariantMap data = note->getBody().value<QVariantMap>();
            auto owner = reinterpret_cast<void*>(data[CommonConstants::Owner].toULongLong());
            if (owner != this)
                break;
            QString section = data[CommonConstants::Section].toString();
            if ( section == SettingConstants::RouteSearch || section == SettingConstants::RouteAlternateSelection ) {
                auto settings = data[CommonConstants::Settings].value<QList<SettingDtoPtr>>();

                QVariantMap value;
                for ( auto setting : settings ) {
                    value[setting->key().toString()] =
                    QVariantMap({
                        {CommonConstants::Type,setting->type()},
                        {CommonConstants::Section,setting->section()},
                        {CommonConstants::Key,setting->key()},
                        {CommonConstants::Value,setting->value()}
                    });
                }

                QQmlProperty::write( mViewComponent, CommonConstants::Settings, QVariant::fromValue( value ) );
            }
        }
        break;
    case ZoneConstants::DoSoftKeyFunction:
    case ZoneConstants::DoSoftkeyIndexMenuFunction:{
            QVariantMap data = note->getBody().value<QVariantMap>();
            QString objectName = data[CommonConstants::Name].toString();
            if (objectName != ViewRegistry::objectName(ViewRegistry::SettingMapUpdateView))
                return false;

            QString objFunction = CommonConstants::Empty;
            QString objMenu = CommonConstants::Empty;
            if (data.contains(ZoneConstants::Function))
                objFunction = data[ZoneConstants::Function].toString();
            if (data.contains(ZoneConstants::Menu) )
                objMenu = data[ZoneConstants::Menu].toString();
            int  objIndex  = -1;
            if (data.contains(ZoneConstants::Index))
                objIndex = data[ZoneConstants::Index].toInt();
            QMetaObject::invokeMethod(mViewComponent, "__doSoftKeyFunction", Q_ARG(QVariant, QVariant::fromValue(objFunction)), Q_ARG(QVariant, QVariant::fromValue(objMenu)), Q_ARG(QVariant, QVariant::fromValue(objIndex)) );
        }
        break;
    case SettingConstants::UpdateView: {
            QVariantMap data = note->getBody().value<QVariantMap>();
            QString objectName = data[CommonConstants::ObjectName].toString();
            if (objectName != ViewRegistry::objectName(ViewRegistry::SettingMapUpdateView))
                return false;

            for ( auto it = data.constBegin(); it != data.constEnd(); it++ ){
                if (CommonConstants::Url == it.key() || CommonConstants::ObjectName == it.key())
                    continue;
               QQmlProperty::write( mViewComponent, it.key(), QVariant::fromValue( it.value() ) );
            }
        }
        break;

    case SettingConstants::DialogAccepted: {
            QVariantMap data = note->getBody().value<QVariantMap>();
            QString caller = data[CommonConstants::Caller].toString();
            QVariantMap m = data[CommonConstants::Value].toMap();
            QString objectName = QQmlProperty::read(mViewComponent, CommonConstants::ObjectName).toString();
            if (caller != objectName)
                break;
            QMetaObject::invokeMethod(mViewComponent, "__closeDialog", Q_ARG(QVariant, true));
        }
        break;

    case SettingConstants::DialogCanceled: {
            QVariantMap data = note->getBody().value<QVariantMap>();
            QString caller = data[CommonConstants::Caller].toString();
            QVariantMap m = data[CommonConstants::Value].toMap();
            if (caller != ViewRegistry::objectName(ViewRegistry::SettingMapUpdateView))
                break;
            QMetaObject::invokeMethod(mViewComponent, "__closeDialog", Q_ARG(QVariant, false));
        }
        break;

    case NavigationControllerConstants::ChangeLanguage: {
            auto data = note->getBody().value<QVariantMap>();
            auto lang = data[LangConstants::Lang].toString();
            QMetaObject::invokeMethod(mViewComponent, "__retranslateUi", Q_ARG(QVariant, QVariant::fromValue(lang)));
        }
        break;

    case ApplicationConstants::ReplyToViewList:
        {
            auto data = note->getBody().toMap();
            QString objectName = data[CommonConstants::ObjectName].value<QString>();
            if (objectName != ViewRegistry::objectName(ViewRegistry::SettingMapUpdateView))
                break;

            auto viewList = data[ApplicationConstants::ViewIdList].value<QList<int>>();
            auto viewDataMap = data[ApplicationConstants::ViewDataMap].value<QMap<int,QVariantMap>>();
            QVariantList values = QVariantList{};
            for ( auto viewId :viewList ) {
                QVariantList result;
                CommonConstants::resultViewInit(result);
                result[CommonConstants::ResultViewId        ] = viewId;
                result[CommonConstants::ResultViewName      ] = ViewRegistry::objectName( viewId );
                result[CommonConstants::ResultViewVisible   ] = viewDataMap[viewId][ApplicationConstants::Visible].toBool();
                result[CommonConstants::ResultViewActive    ] = viewDataMap[viewId][ApplicationConstants::Active].toBool();
                values.append(QVariant::fromValue(result));
            }
            QMetaObject::invokeMethod(mViewComponent, "__onResultViewList",Q_ARG(QVariant, QVariant::fromValue(values)) );
        }
        break;

    default:
        return false;
    }
    return true;
}
