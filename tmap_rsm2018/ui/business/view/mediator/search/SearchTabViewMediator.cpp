#include "SearchTabViewMediator.h"
#include "Constants.h"
#include "ApplicationConstants.h"
#include "CommonConstants.h"
#include "SearchConstants.h"
#include "NavigationControllerConstants.h"
#include "ViewRegistry.h"
#include "MultiSenseConstants.h"
#include "MapConstants.h"
#include "../app/MapViewMode.h"
#include "LangConstants.h"
#include "NavigationControllerProtocol.h"

using namespace SKT;

SearchTabViewMediator::SearchTabViewMediator( QObject* viewComponent )
    : AbstractViewMediator( viewComponent )
{
    mTabPageViewActivations[ViewRegistry::RecentSearchPageView] = false;
    mTabPageViewActivations[ViewRegistry::RecentDestinationPageView] = false;
    mTabPageViewActivations[ViewRegistry::BookmarkPageView] = false;
}

QList<int> SearchTabViewMediator::listNotificationInterests()
{
    static QList<int> notes = {
        SearchConstants::ChangeSearchTabViewPage,
        SearchConstants::ChangedTabPageViewActivation,
        SearchConstants::ChangeSearchEditMode,
        ZoneConstants::FocusActivatedZone1TextInput,
        SearchConstants::HideTabViewContent,
        MultiSenseConstants::RequestColorState,
        NavigationControllerConstants::ChangeLanguage,
        NavigationControllerConstants::ResponseController,
        ApplicationConstants::BlockSearchTabView,
        ApplicationConstants::ReplyToViewList,
        SearchConstants::UpdateView,
    };

    return notes;
}

bool SearchTabViewMediator::onEvent( int event, const QVariant& data )
{
    //T_DEBUGV( "event: %d", event );

    switch ( event ) {
    case Mediated:
        {
            sendNotification(Constants::CloseView, QVariantMap{{CommonConstants::ObjectName,ViewRegistry::objectName(ViewRegistry::RecentTileGridView)}}); // 2019.05.21 lckxx fixed TMCRSM-3175: Tile view is not closed when csw get unfocused
            sendNotification(MapConstants::ChangeMapViewMode,QVariantMap{{CommonConstants::Mode,MapViewMode::MapInquiry}});
        }
        break;
    case CreateView:
        {
            auto m = data.toMap();
            auto viewId = m[QStringLiteral("viewId")].toInt();
            auto parent = m[QStringLiteral("parent")].value<QObject*>();
            auto caller = QQmlProperty::read( mViewComponent, QStringLiteral("objectName") ).toString();

            QVariantMap arg;
            arg[CommonConstants::Url     ] = ViewRegistry::url(viewId);
            arg[CommonConstants::Parent  ] = QVariant::fromValue(parent);
            arg[CommonConstants::Visible ] = false;
            arg[CommonConstants::Caller  ] = caller;
            if (m.contains(SearchConstants::Query)) {
                arg[SearchConstants::Query] = m[QStringLiteral("query")].toString();
            }
            sendNotification(Constants::CreateView, arg);
        }
        break;
    case CloseView:
        {
            auto m = data.toMap();
            auto viewId = m[QStringLiteral("viewId")].toInt();
            sendNotification(Constants::CloseView,
                    QVariantMap{{CommonConstants::ObjectName,ViewRegistry::objectName(viewId)}});
        }
        break;
    case ResetCsw:
        sendNotification(NavigationControllerConstants::ResetCSW);
        break;
    case RegistCsw:
        sendNotification(NavigationControllerConstants::RegistCSW, data);
        break;
    case ActiveTabPageChanged:
        sendNotification(SearchConstants::ChangedActiveTabPage, data);
        break;
    case TabTouched:
        {
            // 최근 목적지, 즐겨 찾기 탭을 터치하면 검색어 초기화
            auto index = data.toInt();
            const int kRecentSearchPageIndex = 1;
            const int kBookmarkPageIndex = 2;
            if (index == kRecentSearchPageIndex || index == kBookmarkPageIndex) {
                sendNotification(ZoneConstants::UpdateSearchText, "");
            }
        }
        break;

    case RequestLang:
        {
            QVariantMap body;
            body.insert(QStringLiteral("type"), NavigationControllerProtocol::GetLang );
            sendNotification( NavigationControllerConstants::RequestController, body );
        }
        break;

    case RequestViewList: {
            QVariantMap body;
            QString objectName = QQmlProperty::read(mViewComponent, CommonConstants::ObjectName).toString();
            body.insert( CommonConstants::ObjectName,   objectName);
            sendNotification(ApplicationConstants::RequestViewList, body);
        }
        break;
    }

    return true;
}

bool SearchTabViewMediator::handleNotification( const mvc::INotificationPtr& note )
{
    switch ( note->getId() ) {
    case ApplicationConstants::BlockSearchTabView:
        {
            auto block = note->getBody().toBool();
            auto tabEnable = !block;
            QQmlProperty::write(mViewComponent, "tabEnable", tabEnable);
        }
        break;

    case SearchConstants::HideTabViewContent:
        {
            QMetaObject::invokeMethod(mViewComponent, "__hideContent");
        }
        break;
    case SearchConstants::ChangeSearchTabViewPage:
        {
            int page = -1;
            auto m = note->getBody().toMap();
            if (m.isEmpty()) {
                page = note->getBody().toInt();
                QQmlProperty::write(mViewComponent, SearchConstants::Page, page);
            } else {
                page = m[SearchConstants::Page].toInt();
                QMetaObject::invokeMethod(mViewComponent, "changePageByCsw", Q_ARG(QVariant, page));                
            }
        }
        break;
    case SearchConstants::ChangedTabPageViewActivation:
        {
            auto m = note->getBody().toMap();
            auto viewId = m[QStringLiteral("viewId")].toInt();
            auto active = m[QStringLiteral("active")].toBool();

            //T_DEBUG( QString("ChangedTabPageViewActivation objectName: %1 active: %2").arg( ViewRegistry::objectName( viewId ) ).arg( active ) );

            mTabPageViewActivations[viewId] = active;
        }
        break;
    case SearchConstants::ChangeSearchEditMode:
        {
            // 최근 검색, 최근 목적지, 즐겨 찾기 편집 모드로 이동 하면
            // tab view의 csw를 비활성화 시킨다.
            // 편집 화면을 종료 하고 이전 뷰로 이동 하면 csw를 활성화 시킴.
            // 19.03.26 ymhong.
            auto tabCswEnable = !note->getBody().toBool();
            QQmlProperty::write(mViewComponent, "tabCswEnable", tabCswEnable);
        }
        break;

    case ZoneConstants::FocusActivatedZone1TextInput:
        {
            auto found = mTabPageViewActivations.key( true, false );
            if ( found ) {
                QMetaObject::invokeMethod(mViewComponent, "__focusActivatedZone1TextInput");
            }
        }
        break;
    case NavigationControllerConstants::ChangeLanguage: {
            auto data = note->getBody().value<QVariantMap>();
            auto lang = data[LangConstants::Lang].toString();
            QMetaObject::invokeMethod(mViewComponent, "__retranslateUi", Q_ARG(QVariant, QVariant::fromValue(lang)));
        }
        break;
    case NavigationControllerConstants::ResponseController: {
            QVariantMap data = note->getBody().value<QVariantMap>();
            if (data.contains(CommonConstants::Lang)) {
                QString value = data[CommonConstants::Lang].toString();
                QQmlProperty::write( mViewComponent, "__lang", QVariant::fromValue(value) );
            }
        }
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
            if (objectName != ViewRegistry::objectName(ViewRegistry::SearchTabView))
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
    }

    return true;
}
