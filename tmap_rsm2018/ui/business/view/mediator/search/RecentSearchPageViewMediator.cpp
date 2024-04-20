#include "RecentSearchPageViewMediator.h"
#include "Constants.h"
#include "CommonConstants.h"
#include "ZoneConstants.h"
#include "KeyboardConstants.h"
#include "NavigationControllerConstants.h"
#include "SearchConstants.h"
#include "MapConstants.h"
#include "RouteConstants.h"
#include "RgConstants.h"
#include "ViewRegistry.h"
#include "RecentSearchDto.h"
#include "mediator/app/MapViewMode.h"
#include "ApplicationConstants.h"
#include "PlatformHelper.h"

using namespace SKT;

RecentSearchPageViewMediator::RecentSearchPageViewMediator( QObject* viewComponent )
    : AbstractViewMediator( viewComponent )
{
}

QList<int> RecentSearchPageViewMediator::listNotificationInterests()
{
    static QList<int> notes = {
        SearchConstants::ResponseRecentSearch,
        SearchConstants::ReplyToGadgetSearchMode,
        ZoneConstants::DoSoftKeyFunction,
        ZoneConstants::DoSoftkeyIndexMenuFunction,
        KeyboardConstants::ProcessKeyboard,
        KeyboardConstants::PullUpKeyboard,
        SearchConstants::ChangedActiveTabPage,
        NavigationControllerConstants::ChangeLanguage,
        ApplicationConstants::ReplyToViewList,
        SearchConstants::UpdateView,
        ZoneConstants::ReplyToZone1Text,
        SearchConstants::ResponseLockStatus,
    };
    return notes;
}

void RecentSearchPageViewMediator::startRouteGuidance()
{
    auto m = mStartRouteGuidanceMap;
    auto x = m[CommonConstants::X].toInt();
    auto y = m[CommonConstants::Y].toInt();
    auto name = m[CommonConstants::Name].toString();

    if (x > 0 && y > 0) {
        QVariantMap body;
        body[SearchConstants::Clear] = true;
        sendNotification(RouteConstants::SetDeparture, body);
        body.clear();

        body[CommonConstants::X] = x;
        body[CommonConstants::Y] = y;
        body[CommonConstants::Name] = name;

        sendNotification(RouteConstants::SetArrival, body);

        sendNotification(Constants::CreateView,QVariantMap{ {CommonConstants::Url,ViewRegistry::url(ViewRegistry::RouteCandidateView)} });
        close();
    }
}

void RecentSearchPageViewMediator::close()
{
    QMetaObject::invokeMethod(mViewComponent, "closeView");
}

void RecentSearchPageViewMediator::pulldownKeyboard()
{
    QMetaObject::invokeMethod(mViewComponent, "__pullDownKeyboard");

    sendNotification(ZoneConstants::RequestZone1Text); // update mText

#ifdef BUILD_TARGET
    PlatformHelper::instance()->setZone1(SEARCH_TEXT_2, QVariantMap{{CommonConstants::Text, mText}});//QVariantMap{});
#endif
}

void RecentSearchPageViewMediator::pullupKeyboard()
{
    QMetaObject::invokeMethod(mViewComponent, "__pullUpKeyboard");

#ifdef BUILD_TARGET
    auto owner = ViewRegistry::objectName(ViewRegistry::RecentSearchPageView);
    sendNotification(KeyboardConstants::ShowKeyboard, QVariantMap{{CommonConstants::Owner, owner}});
#endif
}

bool RecentSearchPageViewMediator::onEvent( int event, const QVariant& data )
{
    switch (event) {
    case Activated: {
#ifdef BUILD_TARGET
            QQmlProperty::write(mViewComponent, "keyboardHeight", SYSTEM_KEYBOARD_HEIGHT);
#endif
        sendNotification(SearchConstants::ChangedTabPageViewActivation,
            QVariantMap{{QStringLiteral("viewId"),ViewRegistry::RecentSearchPageView},
                {QStringLiteral("active"),true}});

        auto objectName = QQmlProperty::read(mViewComponent, CommonConstants::ObjectName).toString();
        sendNotification(ZoneConstants::RegistSoftkey, objectName);

        sendNotification(ZoneConstants::SelectSoftkeyMenuMode, QStringList{"noOption"});
        sendNotification(SearchConstants::ChangeSearchEditMode, false);
        sendNotification(RgConstants::BlockReroute, true);
        break;
    }
    case Deactivated:
        sendNotification(SearchConstants::ChangedTabPageViewActivation,
            QVariantMap{{QStringLiteral("viewId"),ViewRegistry::RecentSearchPageView},
                {QStringLiteral("active"),false}});
        break;
    case BackwardClicked:
        sendNotification(SearchConstants::RequestGadgetSearchMode);
        if (mIsGadgetSearchMode) {
            PlatformHelper::instance()->switchToSystem();
            sendNotification(SearchConstants::SetGadgetSearchMode, false);
        }
        sendNotification(ZoneConstants::UpdateSearchText, "");
        break;
    case SelectZone1TextInput:
        sendNotification(ZoneConstants::SelectZone1TextInput);
        break;
    case FocusZone1TextInput:
        sendNotification(ZoneConstants::FocusZone1TextInput);
        break;
        // check
//    case ActivatedForNavi:
//        {
//            auto objectName = QQmlProperty::read(mViewComponent, CommonConstants::ObjectName).toString();
//            sendNotification(ZoneConstants::RegistSoftkey, objectName);
//            sendNotification(ZoneConstants::SelectSoftkeyNaviMode);
//        }
//        break;
    case CreateView: {
            auto m = data.toMap();
            auto url = m[CommonConstants::Url].toString();
            if (url == ViewRegistry::url(ViewRegistry::RecentSearchEditView)) {
                sendNotification(SearchConstants::ChangeSearchEditMode, true);
            } else if (url == ViewRegistry::url(ViewRegistry::SearchResultView)) {
                if (m.contains(SearchConstants::Query)) {
                    auto query = m[SearchConstants::Query].toString();
                    sendNotification(ZoneConstants::UpdateSearchText, query);
                    sendNotification(SearchConstants::UpdateSearchResultText);
                }
            }
            sendNotification(Constants::CreateView, data);
        }
        break;
    case CloseView:
        // Deactivate 이벤트가 발생하지 않으므로 CSW처리는 이곳에서 함
        sendNotification(NavigationControllerConstants::ResetCSW);
        if (mIsGadgetSearchMode) {
            sendNotification(NavigationControllerConstants::ReserveSystemSwitchJob, QVariantMap{
                                 {CommonConstants::Command, Constants::CloseView},
                                 {CommonConstants::Arg, data}
                             });
        } else {
            sendNotification(Constants::CloseView, data);
        }
        break;
    case ShowKeyboard:
        sendNotification(KeyboardConstants::ShowKeyboard, data);
        break;
    case PulldownKeyboard:
        pulldownKeyboard();
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
    case RequestRecentSearch:
       sendNotification(SearchConstants::RequestRecentSearch, data);
        break;
    case ChangeSearchTabViewPage:
        sendNotification(SearchConstants::ChangeSearchTabViewPage, data);
        break;
    case QueryInDriveMode://v
        mStartRouteGuidanceMap = data.toMap();
        sendNotification(MapConstants::RequestDriveMode);
        break;
    case RequestViewList: {
            QVariantMap body;
            QString objectName = QQmlProperty::read(mViewComponent, CommonConstants::ObjectName).toString();
            body.insert( CommonConstants::ObjectName,   objectName);
            sendNotification(ApplicationConstants::RequestViewList, body);
        }
        break;
    case FocusCsw:
        sendNotification(NavigationControllerConstants::SelectCSWByObjectName, data);
        break;

    case UpdateView:
        sendNotification(SearchConstants::UpdateView, data);
        break;

    }

    return false;
}

bool RecentSearchPageViewMediator::handleNotification( const mvc::INotificationPtr& note )
{
    switch ( note->getId() ) {
    case SearchConstants::ChangedActiveTabPage:
        {
            auto m = note->getBody().toMap();
            auto page = m[SearchConstants::Page].toInt();
            T_DEBUGV( "ApplyTabPageActivation #1 page: %d", page );
            QQmlProperty::write(mViewComponent, CommonConstants::Visible, page == 0);
        }
        break;

    case SearchConstants::ReplyToGadgetSearchMode:
        mIsGadgetSearchMode = note->getBody().toBool();
        break;

    case SearchConstants::ResponseRecentSearch:
    {
        auto data = note->getBody().value<QList<RecentSearchDtoPtr> >();
        QVariantMap r;
        int i = 0;
        for ( RecentSearchDtoPtr dto : data )
        {
            QVariantList v;
            uint arrivalTime = 0; // TODO : reqeust rp to arrival time from MNS
            v << dto->id() << dto->keyword();
            r.insert(QString::number(i),QVariant::fromValue(v));
            i++;
        }

        //T_DEBUG("Responsed to recent search: "+QString::number(i));
        QMetaObject::invokeMethod(mViewComponent,"pushRecentSearchList",Q_ARG(QVariant,QVariant::fromValue(r)),Q_ARG(QVariant,i));
    }
        break;
    case ZoneConstants::DoSoftKeyFunction:
    case ZoneConstants::DoSoftkeyIndexMenuFunction:{
            // Donot follow this notification if the view is hidden, by lckxx 2017.11.14
        if (!QQmlProperty::read(mViewComponent, CommonConstants::Visible).toBool()) break;


    	auto objectName = QQmlProperty::read(mViewComponent, CommonConstants::ObjectName).toString();
    	auto data = note->getBody().value<QVariantMap>();
    	if (data.contains(CommonConstants::Name)) {
    		auto objName = data[CommonConstants::Name].toString();
    		if (objName != objectName)
    			return false;
    	}

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

    case KeyboardConstants::ProcessKeyboard: {
        if (!isVisible() || !isActive())
            break;
         auto args = note->getBody().value<QVariantMap>();
         auto key = args[CommonConstants::Key];
         auto text = args[CommonConstants::Text];
         QMetaObject::invokeMethod(mViewComponent, "__processKeyboard", Q_ARG(QVariant, QVariant::fromValue(key)), Q_ARG(QVariant, QVariant::fromValue(text)) );
     }
     break;

    case KeyboardConstants::PullUpKeyboard:
        if (isActive()) {
            pullupKeyboard();
        }
        break;
    case NavigationControllerConstants::ChangeLanguage:
        if (isActive())
            QMetaObject::invokeMethod(mViewComponent, "retranslateUi");
        break;
    case ApplicationConstants::ReplyToViewList:
        {
            auto data = note->getBody().toMap();
            if (!data.contains(CommonConstants::ObjectName)) {
                break;
            }
            QString objectName = QQmlProperty::read(mViewComponent, CommonConstants::ObjectName).toString();
            QString requestObjectName = data[CommonConstants::ObjectName].value<QString>();
            if (requestObjectName != objectName)
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

    case SearchConstants::UpdateView: {
            qDebug() << "[Perf][UpdateView]before";
            QVariantMap data = note->getBody().value<QVariantMap>();
            QString objectName = data[CommonConstants::ObjectName].toString();
            if (objectName != ViewRegistry::objectName(ViewRegistry::RecentSearchPageView))
                return false;
            qDebug() << "[Perf][UpdateView]objectName: " << objectName;
            qDebug() << "[Perf][UpdateView]after";
            for ( auto it = data.constBegin(); it != data.constEnd(); it++ ){
                if (CommonConstants::Url == it.key() ||
                    CommonConstants::ObjectName == it.key() ||
                    CommonConstants::ReFocus == it.key() ||
                    CommonConstants::Visible == it.key() ||
                    SearchConstants::Params == it.key()) {
                    continue;
                }
                QQmlProperty::write( mViewComponent, it.key(), QVariant::fromValue( it.value() ) );
            }
            if (data.contains(CommonConstants::Visible)) {
                QQmlProperty::write( mViewComponent, CommonConstants::Visible, QVariant::fromValue( data[CommonConstants::Visible]) );
            }
            if (data.contains(SearchConstants::Params)) {
                QQmlProperty::write( mViewComponent, SearchConstants::Params, QVariant::fromValue( data[SearchConstants::Params]) );
            }
            if (data.contains(CommonConstants::ReFocus)) {
                QQmlProperty::write( mViewComponent, CommonConstants::ReFocus, QVariant::fromValue( data[CommonConstants::ReFocus]) );
            }
        }
        break;

    case ZoneConstants::ReplyToZone1Text:
        mText = note->getBody().toString();
        break;

    case SearchConstants::ResponseLockStatus: {
            bool lock = note->getBody().toBool();
            QMetaObject::invokeMethod(mViewComponent, "__changedSyncLock", Q_ARG(QVariant, lock));
        }
        break;

    default:
        return false;
    }
    return true;
}

bool RecentSearchPageViewMediator::isVisible() const
{
    auto visible = QQmlProperty::read(mViewComponent, CommonConstants::Visible).toBool();
    return visible;
}

bool RecentSearchPageViewMediator::isActive() const
{
    auto active = QQmlProperty::read(mViewComponent, CommonConstants::Active).toBool();
    return active;
}

