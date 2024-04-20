#include "CrossImageViewMediator.h"
#include "Constants.h"
#include "RgConstants.h"
#include "MapConstants.h"
#include "SettingConstants.h"
#include "TestConstants.h"
#include "ResourceResolver.h"
#include "SettingDto.h"
#include "ViewRegistry.h"
#include "SimulatorConstants.h"
#include "NavigationControllerConstants.h"
#include "ApplicationConstants.h"
#include "SearchConstants.h"
#include "../common/searchrequest.h"
#include "hostnames.h"
#include "RgProtocol.h"

namespace {
    const QString LocalImagePath = QStringLiteral("localImagePath");
}

using namespace SKT;

CrossImageViewMediator::CrossImageViewMediator(  QObject* viewComponent )
    : AbstractViewMediator( viewComponent ),
    mCurrMapViewMode( MapViewMode::Normal ),
    mIsOnline(false)
{
    mImage = findChild( QStringLiteral("image") );
    mNightMode = "0";

    mIsVnextEnabled = true;

    #ifndef BUILD_TARGET
       mIsVnextEnabled = false;
    #else
       QString acompHost = TmapHostNames::getHostName(TmapHostNames::HOST_COMPLEX_INDEX);
       if (acompHost.indexOf("avnext.renault.com") < 0) {
           mIsVnextEnabled = false;
       }
    #endif
}

CrossImageViewMediator::~CrossImageViewMediator()
{

}

QList<int> CrossImageViewMediator::listNotificationInterests()
{
    static QList<int> notes = {
        RgConstants::UpdateRouteGuidance,
        MapConstants::ApplyMapViewMode,
//        TestConstants::RunTest,
        SettingConstants::ApplySetting,
        SettingConstants::ChangedSetting,
        RgConstants::RequestUiState,
        RgConstants::ReplyToUiState,
        RgConstants::ChangedUiState,
        RgConstants::UpdateView,
		RgConstants::StopRouteGuidance,
		RgConstants::TerminatedRouteGuidance,
        SimulatorConstants::StopSimulator,
        RgConstants::StartRouteGuidance,
        NavigationControllerConstants::ChangeOnlineStatus,
        NavigationControllerConstants::ResponseOnlineStatus,
        ApplicationConstants::UpdatedUserProfile,
        ApplicationConstants::ChangedUserProfile,
        RgConstants::DisplayOnlineCrossImage
    };

    return notes;
}

bool CrossImageViewMediator::onEvent( int event, const QVariant& data )
{
    switch ( event ) {
    case Mediated:
        {
            auto m = data.toMap();
            mRenderer = m["mapView"].value<QObject*>();

            QVariantMap body;
            body[CommonConstants::Section] = SettingConstants::MapMain;
            body[CommonConstants::Key] = QStringLiteral("NightMode");
            sendNotification( SettingConstants::RequestSetting, body );

            sendNotification(MapConstants::NotifyMapViewMode);

            sendNotification( RgConstants::RequestUiState,
                QVariantMap{
                    {CommonConstants::Caller, ViewRegistry::objectName(ViewRegistry::HighwayTbtView)}
                }
            );
        }
        break;
    case VisibleChanged:
        {
            auto m = data.toMap();
            auto visible = m[QStringLiteral("visible")].toBool();

            sendNotification(RgConstants::ChangedUiState,
                QVariantMap{
                    {CommonConstants::Caller, ViewRegistry::objectName(ViewRegistry::CrossImageView)},
                    {QStringLiteral("item"), RgConstants::CrossImageVisible},
                    {CommonConstants::Value, visible}
                }
            );

            if ( !visible ) {
                qDebug() << "[CrossImageView]mIsTbtListExpanded: " << mIsTbtListExpanded;
                if ( mIsTbtListExpanded ) {
                    sendNotification(RgConstants::UpdateView, QVariantMap{
                        {CommonConstants::ObjectName, ViewRegistry::objectName( ViewRegistry::SummaryTbtView )},
                        {CommonConstants::Visible, true},
                        {"cswFocus", false}
                    });
                }
            }
        }
        break;
    default:
        return false;
    }

    return true;
}

bool CrossImageViewMediator::handleNotification( const mvc::INotificationPtr& note )
{
    switch ( note->getId() ) {
    case NavigationControllerConstants::ResponseOnlineStatus:
    case NavigationControllerConstants::ChangeOnlineStatus:
    {
        mIsOnline = note->getBody().toBool();
    }
    case RgConstants::UpdateRouteGuidance:
        {
            auto vo = note->getBody().value<RgDataVo>();
            applyRgData( vo );
        }
        break;
    case RgConstants::RequestUiState:
        {
            auto data = note->getBody().value<QVariantMap>();
            auto caller = data[CommonConstants::Caller];
            if ( caller != ViewRegistry::objectName(ViewRegistry::CrossImageView) ) {
                auto visible = QQmlProperty::read( mViewComponent, CommonConstants::Visible ).toBool();
                sendNotification(RgConstants::ReplyToUiState,
                    QVariantMap{
                        {CommonConstants::Caller, ViewRegistry::objectName(ViewRegistry::CrossImageView)},
                        {QStringLiteral("item"), RgConstants::CrossImageVisible},
                        {CommonConstants::Value, visible}
                    }
                );
            }
        }
        break;
    case SettingConstants::ChangedSetting:
        {
            auto data = note->getBody().value<QVariantMap>();
            auto sect = data[CommonConstants::Section].toString();
            auto key = data[CommonConstants::Key].toString();
            if ( sect == SettingConstants::MapMain && key == QStringLiteral("NightMode") ) {
                mNightMode = data[CommonConstants::Setting].value<SettingDtoPtr>()->value().toString();
//                T_DEBUG( QString( "NightMode '%1'" ).arg( mNightMode ) );
            }
        }
        break;
    case SettingConstants::ApplySetting:
        {
            auto data = note->getBody().value<QVariantMap>();
            auto sect = data[CommonConstants::Section].toString();
            auto key = data[CommonConstants::Key].toString();
            if ( sect == SettingConstants::MapMain && key == QStringLiteral("NightMode") ) {
                mNightMode = data[CommonConstants::Setting].value<SettingDtoPtr>()->value().toString();
//                T_DEBUG( QString( "NightMode '%1'" ).arg( mNightMode ) );
            }
        }
        break;
    case MapConstants::ApplyMapViewMode:
        {
            auto data = note->getBody().value<QVariantMap>();
            mCurrMapViewMode = (MapViewMode::ModeK)data[CommonConstants::Mode].toInt();
        }
        break;
    case RgConstants::StartRouteGuidance:
        {
            mNightMode = "";
            QMetaObject::invokeMethod(mViewComponent, "reset");
        }
        break;
    case ApplicationConstants::UpdatedUserProfile:
    case ApplicationConstants::ChangedUserProfile:
        {
            QVariantMap m;
            m[CommonConstants::Section] = SettingConstants::MapMain;
            m[CommonConstants::Key] = SettingConstants::NightMode;
            sendNotification( SettingConstants::RequestSetting, m);
        }
        break;
    case TestConstants::RunTest:
        {
            auto data = note->getBody().value<QVariantMap>();
            mTest = data["test"].toInt();
        }
        break;
    case RgConstants::DisplayOnlineCrossImage:
        {
            static int count = 0;
            auto source = QString( "image://ImageProvider/onlineCross_%1" ).arg( count++ );

            qDebug() << "[Cross]CrossImageViewMediator RgConstants::DisplayOnlineCrossImage #1";

            auto m = note->getBody().toMap();
            auto url = m[CommonConstants::Url];

            if ( mIsVnextEnabled ) {
                if ( mLastVisible ) {
                    if ( mLastImagePath == url ) {
                        qDebug() << "[Cross]CrossImageViewMediator RgConstants::DisplayOnlineCrossImage #2";
                        QQmlProperty::write( mViewComponent, "visibleRequest", true );
                        QQmlProperty::write( mImage, QStringLiteral("source"), source);
                    }
                }
            }
        }
        break;    
    case RgConstants::ReplyToUiState:
    case RgConstants::ChangedUiState:
        {
            // TMCRSM-3965 retains the state of whether tbt list is expanded
            auto visible = QQmlProperty::read( mViewComponent, CommonConstants::Visible ).toBool();
            auto m = note->getBody().toMap();
            auto item = m[QStringLiteral("item")].toInt();
            auto value = m[QStringLiteral("value")].toInt();
            if ( item == RgConstants::SummaryTbtVisible ) {
                mIsTbtListExpanded = value;
                qDebug() << QString( "[CrossImageView]mIsTbtListExpanded: %1" ).arg( mIsTbtListExpanded );
            }
        }
        break;
    default:
        return false;
    }

    return true;
}

#define TMAP_RSM_UI_ENALBE_TEST_CROSSIMAGEVIEW

void CrossImageViewMediator::applyRgData( const RgDataVo& rgDataVo )
{
    if ( rgDataVo.isNull() ) {
        return;
    }

    auto bExtcImage = rgDataVo.bExtcImage();
    auto nExtcImageCode = rgDataVo.nExtcImageCode();
    auto nExtcImageShowDist = rgDataVo.nExtcImageShowDist();

    auto szImageBaseUrl = rgDataVo.szImageBaseUrl();
    auto szImageDayUri = rgDataVo.szImageDayUri();
    auto szImageNightUri = rgDataVo.szImageNightUri();

    auto stGuidePoint_nTBTDist = rgDataVo.stGuidePoint_nTBTDist();

    auto turnType = rgDataVo.stGuidePoint_nTBTTurnType();
    if (turnType == RgTbtCode::VIA_1 || turnType == RgTbtCode::VIA_2 || turnType == RgTbtCode::VIA_3) {
        auto nextDist = rgDataVo.stGuidePointNext_nSvcLinkDist();
        stGuidePoint_nTBTDist = stGuidePoint_nTBTDist + nextDist;
    }
	
#ifdef TMAP_RSM_UI_ENALBE_TEST_CROSSIMAGEVIEW
    if ( mTest == 0 ) {
        bExtcImage = true;
        //nExtcImageCode = 467;
        nExtcImageCode = 12244;
        nExtcImageShowDist = 310;
        stGuidePoint_nTBTDist = 150;
    }
    else if ( mTest == 1 ) {
        bExtcImage = true;
        nExtcImageCode = 467;
        nExtcImageShowDist = 300;
        stGuidePoint_nTBTDist = 150;
    }
#endif

    auto visible = false;
	
    // TBT까지의 남은 거리가 복잡교차로 표출 가능 거리 안에 들어왔는가?
	bool isInDisplayableDist = nExtcImageShowDist >= stGuidePoint_nTBTDist;
    QString imgPath;
    QString localImgPath;
	
    if ( bExtcImage && isInDisplayableDist ) { // 복잡교차로가 나오지 않아 일단 T map 사양으로 변경
        QString nightMode = "1";

        switch (mNightMode.toInt()) {
            case 0: {
                auto mRendererNightMode = QQmlProperty::read(mRenderer, QStringLiteral("nightMode")).toBool();
                if (mRendererNightMode) {
                    nightMode = "0";
                } else {
                    nightMode = "1";
                }
            }
                break;
            case 1:
                nightMode = "1";
                break;
            case 2:
                nightMode = "0";
                break;
        }

        imgPath = ResourceResolver::instance()->imageUrl( QStringLiteral("cross"), nightMode, QString( QStringLiteral("%1") ).arg( nExtcImageCode ) );
        localImgPath = imgPath;

        if (mIsOnline) {
            auto szImageBaseUrl = rgDataVo.szImageBaseUrl();
            auto szImageDayUri = rgDataVo.szImageDayUri();
            auto szImageNightUri = rgDataVo.szImageNightUri();

            if (szImageBaseUrl.length() > 0) {
                if (nightMode.toInt() == 0 && szImageNightUri.length() > 0) {
                    imgPath = QString( QStringLiteral("%1%2") ).arg( szImageBaseUrl ).arg( szImageNightUri );
                } else if (nightMode.toInt() == 1 && szImageDayUri.length() > 0) {
                    imgPath = QString( QStringLiteral("%1%2") ).arg( szImageBaseUrl ).arg( szImageDayUri );
                }
            }
        }

        visible = true;
    }

    mLastVisible = visible;

    if ( visible ) {
        if ( mIsOnline && mIsVnextEnabled ) {
            if ( mLastImagePath != imgPath ) {
                mLastImagePath = imgPath;

                QVariantMap body;
                body.insert( CommonConstants::Url, imgPath );
                body.insert( LocalImagePath, localImgPath );
                body.insert( SearchConstants::SearchType, SearchRequestBase::SearchComplexImage );

                sendNotification(SearchConstants::RequestSearch, body);
            }
        }
        else {
            QQmlProperty::write( mViewComponent, "visibleRequest", true );
            QQmlProperty::write( mImage, QStringLiteral("source"), imgPath);
        }
        QQmlProperty::write( mViewComponent, QStringLiteral("showDistance"), nExtcImageShowDist );
        QQmlProperty::write( mViewComponent, QStringLiteral("distance"), stGuidePoint_nTBTDist );
    }
    else {
        QQmlProperty::write( mViewComponent, "visibleRequest", false );
        QQmlProperty::write( mViewComponent, "visible", false );
        QQmlProperty::write( mImage, QStringLiteral("source"), "");
    }
}
