#include "HighwayTbtViewMediator.h"
#include "Constants.h"
#include "RgConstants.h"
#include "SettingConstants.h"
#include "RgProtocol.h"
#include "mediator/app/MapViewMode.h"
#include "RgDataVo.h"
#include "StringHelper.h"
#include "SettingDto.h"
#include "RgUtils.h"
#include "ViewRegistry.h"
#include "SimulatorConstants.h"
#include "SettingConstants.h"
#include "NavigationControllerConstants.h"
#include "ApplicationConstants.h"
#include "TestConstants.h"
#include <QQmlProperty>

using namespace SKT;

HighwayTbtViewMediator::HighwayTbtViewMediator( QObject* viewComponent )
    : AbstractViewMediator( viewComponent )
{
    mShowSetting = "1";

#ifndef BUILD_TARGET
    QQmlProperty::write( mViewComponent, "isBuildTarget", false );
#endif
}

HighwayTbtViewMediator::~HighwayTbtViewMediator()
{
}

QList<int> HighwayTbtViewMediator::listNotificationInterests()
{
    static QList<int> notes = {
        RgConstants::UpdateHighwayRouteGuidance,
        SettingConstants::ApplySetting,
        SettingConstants::ChangedSetting,
        RgConstants::RequestUiState,
        RgConstants::ReplyToUiState,
        RgConstants::ChangedUiState,
        RgConstants::StartRouteGuidance,
        ApplicationConstants::UpdatedUserProfile,
        ApplicationConstants::ChangedUserProfile,
        NavigationControllerConstants::ChangeLanguage,
        NavigationControllerConstants::ResponseSkyState,
        NavigationControllerConstants::UpdateSkyState,
        TestConstants::RunTest,
        NavigationControllerConstants::ChangeOnlineStatus,
        NavigationControllerConstants::ResponseOnlineStatus,
    };

    return notes;
}

bool HighwayTbtViewMediator::onEvent( int event, const QVariant& data )
{
    switch ( event ) {
    case Mediated:
        {
            sendNotification( SettingConstants::RequestSetting,
                QVariantMap{
                    {CommonConstants::Section, SettingConstants::MapMain},
                    {CommonConstants::Key, QStringLiteral("ExpresswayDrivingGuidance")}
                }
            );
            sendNotification( RgConstants::RequestUiState,
                QVariantMap{
                    {CommonConstants::Caller, ViewRegistry::objectName(ViewRegistry::HighwayTbtView)}
                }
            );            
            sendNotification( SettingConstants::RequestSetting,
                QVariantMap{
                    {CommonConstants::Section, SettingConstants::MapSubOilType},
                    {CommonConstants::Key, SettingConstants::OilKind}
                }
            );
            sendNotification( NavigationControllerConstants::RequestSkyUpdate );
            sendNotification(NavigationControllerConstants::QueryOnlineStatus);
        }
        break;
    case VisibleChanged:
        {
            auto m = data.toMap();
            auto visible = m[QStringLiteral("visible")].toBool();

            qDebug() << "[RG][ViewSwitch][HighwayTbtView]visible: " << visible;

            sendNotification(RgConstants::ChangedUiState,
                QVariantMap{
                    {CommonConstants::Caller, ViewRegistry::objectName(ViewRegistry::HighwayTbtView)},
                    {QStringLiteral("item"), RgConstants::HighwayVisible},
                    {CommonConstants::Value, visible}
                }
            );

            visible = QQmlProperty::read( mViewComponent, QStringLiteral("listWrapVisible") ).toBool();
            sendNotification(RgConstants::ReplyToUiState,
                QVariantMap{
                    {QStringLiteral("item"), RgConstants::HighwayTbtListVisible},
                    {CommonConstants::Value, visible}
                }
            );
        }
        break;
    case ListVisibleChanged:
        {
            auto visible = data.toBool();
            //T_DEBUGV( "ListVisibleChanged %d", visible );

            qDebug() << "[RG][ViewSwitch][HighwayTbtView]list visible: " << visible;

            sendNotification(RgConstants::ChangedUiState,
                QVariantMap{
                    {CommonConstants::Caller, ViewRegistry::objectName(ViewRegistry::HighwayTbtView)},
                    {QStringLiteral("item"), RgConstants::HighwayTbtListVisible},
                    {CommonConstants::Value, visible}
                }
            );
        }
        break;
    case RegistCsw:
        {
            sendNotification(NavigationControllerConstants::RegistCSW, data);
        }
        break;
    case RemoveCsw:
        {
            sendNotification(NavigationControllerConstants::RemoveCSW, data);
        }
        break;
    case SelectCsw:
        {
            sendNotification(NavigationControllerConstants::SelectCSW, data);
        }
        break;
    default:
        return false;
    }

    return false;
}

bool HighwayTbtViewMediator::handleNotification( const mvc::INotificationPtr& note )
{
    switch ( note->getId() ) {
    case RgConstants::UpdateHighwayRouteGuidance:
        {
            auto vo = note->getBody().value<RgDataVo>();
            updateView( vo );
        }
        break;
    case RgConstants::RequestUiState:
        {
            auto data = note->getBody().value<QVariantMap>();
            auto caller = data[CommonConstants::Caller];
            if ( caller != ViewRegistry::objectName(ViewRegistry::HighwayTbtView) ) {
                auto visible = QQmlProperty::read( mViewComponent, CommonConstants::Visible ).toBool();
                sendNotification(RgConstants::ReplyToUiState,
                    QVariantMap{
                        {QStringLiteral("item"), RgConstants::HighwayVisible},
                        {CommonConstants::Value, visible}
                    }
                );

                visible = QQmlProperty::read( mViewComponent, QStringLiteral("listWrapVisible") ).toBool();
                sendNotification(RgConstants::ReplyToUiState,
                    QVariantMap{
                        {QStringLiteral("item"), RgConstants::HighwayTbtListVisible},
                        {CommonConstants::Value, visible}
                    }
                );
            }
        }
        break;
    // 복잡교차로 -> 고속도로뷰 -> TBT 상세 -> 교통정보바
    case RgConstants::ReplyToUiState:
    case RgConstants::ChangedUiState:
        {
            auto m = note->getBody().toMap();
            auto item = m[QStringLiteral("item")].toInt();
            auto value = m[QStringLiteral("value")].toInt();

            if ( item == RgConstants::CrossImageVisible ) {
                mCrossImageVisible = value;
                if ( mCrossImageVisible ) {
                    QQmlProperty::write( mViewComponent, QStringLiteral("visible"), false );
                }
            }
        }
        break;     
    case SettingConstants::ApplySetting:
    case SettingConstants::ChangedSetting:
        {
            auto data = note->getBody().value<QVariantMap>();
            auto sect = data[CommonConstants::Section].toString();
            auto key = data[CommonConstants::Key].toString();
            if ( sect == SettingConstants::MapMain && key == QStringLiteral("ExpresswayDrivingGuidance") ) {
                mShowSetting = data[CommonConstants::Setting].value<SettingDtoPtr>()->value().toString();
                qDebug() << QString( "[HighwayTbtView][Setting]ExpresswayDrivingGuidance '%1'" ).arg( mShowSetting );
            }
            if ( sect == SettingConstants::MapSubOilType && key == SettingConstants::OilKind ) {
                mCurrentOilType = data[CommonConstants::Setting].value<SettingDtoPtr>()->value().toInt();
                qDebug() << QString( "[HighwayTbtView][Setting]OilKind '%1'" ).arg( mCurrentOilType );
            }
        }
        break;    
    case RgConstants::StartRouteGuidance:
        {
            //T_DEBUG( QString( "StartRouteGuidance #1" ) );
            mLastDistances = QStringList();
            QMetaObject::invokeMethod(mViewComponent, "reset");
        }
        break;
    case ApplicationConstants::UpdatedUserProfile:
    case ApplicationConstants::ChangedUserProfile:
        {
            sendNotification( SettingConstants::RequestSetting,
                QVariantMap{
                    {CommonConstants::Section, SettingConstants::MapMain},
                    {CommonConstants::Key, QStringLiteral("ExpresswayDrivingGuidance")}
                }
            );
            sendNotification( SettingConstants::RequestSetting,
                QVariantMap{
                    {CommonConstants::Section, SettingConstants::MapSubOilType},
                    {CommonConstants::Key, SettingConstants::OilKind}
                }
            );
        }
        break;
    case NavigationControllerConstants::ChangeLanguage: {
            auto m = note->getBody().toMap();
            QMetaObject::invokeMethod(mViewComponent, "retranslateUi");
        }
        break;
    //case NavigationControllerConstants::ResponseSkyState:
    case NavigationControllerConstants::UpdateSkyState: {
            auto m = note->getBody().toMap();
            auto nightMode = m["nightMode"].toBool();
            //qDebug() << "[Highway]nightMode: " << nightMode;
            QQmlProperty::write( mViewComponent, "dayOrNight", nightMode ? QStringLiteral("night") : QStringLiteral("day") );
        }
        break;
    case TestConstants::RunTest: {
            auto data = note->getBody().value<QVariantMap>();
            mTest = data["test"].toInt();
        }
        break;
    case NavigationControllerConstants::ResponseOnlineStatus:
    case NavigationControllerConstants::ChangeOnlineStatus:
        {
            mIsOnline = note->getBody().toBool();
            QQmlProperty::write( mViewComponent, QStringLiteral("onlineOrOffline"), mIsOnline ? QStringLiteral("online") : QStringLiteral("offline"));
        }
        break;
    default:
        return false;
    }

    return true;
}

QVariantMap HighwayTbtViewMediator::makeModel(
    const QString& type,
    bool noPrice,
    bool noData,
    const QString& name, const QString& popIcon, const QString& toll,
    const QVariantList& distance, const QString& progress, const QString& station,
    const QVariantList& oilAndPrice,
    bool isEvRapidCharger,
    bool isEvSlowCharger
    )
{
    QVariantMap model;

    model[QStringLiteral("type")] = type;
    model[QStringLiteral("noPrice")] = noPrice;
    model[QStringLiteral("noData")] = noData;
    model[QStringLiteral("name")] = name;
    model[QStringLiteral("popIcon")] = popIcon;
    model[QStringLiteral("toll")] = toll;
    model[QStringLiteral("distanceLength")] = distance.length();
    model[QStringLiteral("distance")] = distance;
    model[QStringLiteral("progress")] = progress;
    model[QStringLiteral("station")] = station;
    model[QStringLiteral("oilAndPriceLength")] = oilAndPrice.length();
    model[QStringLiteral("oilAndPrice")] = oilAndPrice;
    model[QStringLiteral("isEvRapidCharger")] = isEvRapidCharger;
    model[QStringLiteral("isEvSlowCharger")] = isEvSlowCharger;

    return model;
}

void HighwayTbtViewMediator::updateView( const RgDataVo& rgDataVo )
{    
    if ( rgDataVo.isNull() ) {
        return;
    }

    auto visible = rgDataVo.nShowHighway() == 1 && !mCrossImageVisible;
    if ( mShowSetting == QStringLiteral("0") ) {
        visible = false;
    }

//    qDebug() << QString( "[HighwayTbtView]visible: %1 show: %2 cross: %3 setting: %4 tbtListUpdated: %5 tbtList_length: %6 lastDistances: %7" )
//        .arg( visible )
//        .arg( rgDataVo.nShowHighway() )
//        .arg( mCrossImageVisible )
//        .arg( mShowSetting )
//        .arg( rgDataVo.highwayTbtListUpdated() )
//        .arg( rgDataVo.highwayTbtList_length() )
//        .arg( mLastDistances.count() );

    QQmlProperty::write( mViewComponent, CommonConstants::Visible, visible );
    if ( !visible ) {
        return;
    }    

    QVariant buttonModel;
    QVariantList listModel;
    QString popIcon;
    QString station;
    QString toll;
    QVariantList oilAndPriceList;
    QString congestion;

    auto tbtListLength = rgDataVo.highwayTbtList_length();
    //T_DEBUG( QString( "#3 tbtListLength %1" ).arg( tbtListLength ) );

    bool highwayTbtListUpdated = rgDataVo.highwayTbtListUpdated();
    if ( tbtListLength != mLastDistances.count() ) {
        highwayTbtListUpdated = true;
    }

    if ( !highwayTbtListUpdated ) {
        for ( int i = 0; i < tbtListLength; i++ ) {
            auto isButton = i == tbtListLength - 1;

            int dist = rgDataVo.highwayTbtList_nAccDist(i) - rgDataVo.nAccDist();
            auto distStr = RgUtils::formatDistance( dist );

            if ( distStr != mLastDistances[i] ) {
                mLastDistances[i] = distStr;
                QVariantList distImages;
                for(int j = 0, jLen = distStr.length(); j < jLen; j++){
                    auto ch = distStr.at(j);
                    distImages.append( RgUtils::highwayDistanceNumberImage(ch) );
                    if ( ch == QChar('k') ) {
                        j++;
                    }
                }

                if ( isButton ) {
                    QMetaObject::invokeMethod(mViewComponent, "setButtonModelDistance", Q_ARG(QVariant, QVariant::fromValue( distImages ) ));
                }
                else {
                    QMetaObject::invokeMethod(mViewComponent, "setListModelDistance", Q_ARG(QVariant, i), Q_ARG(QVariant, QVariant::fromValue( distImages ) ));
                }
            }
        }
    }
    else {
        mLastDistances.clear();

        for ( int i = 0; i < tbtListLength; i++ ) {
            popIcon = "";
            station = "";
            toll = "";
            oilAndPriceList.clear();
            congestion = "";

            auto isButton = i == tbtListLength - 1;

            // name
            auto name = rgDataVo.highwayTbtList_szTBTMainText(i);

            // distance			
            int dist = rgDataVo.highwayTbtList_nAccDist(i) - rgDataVo.nAccDist();
            auto distStr = RgUtils::formatDistance( dist );

            mLastDistances.append( distStr );

            QVariantList distImages;
            for(int j = 0, jLen = distStr.length(); j < jLen; j++){
                auto ch = distStr.at(j);
                distImages.append( RgUtils::highwayDistanceNumberImage(ch) );
                if ( ch == QChar('k') ) {
                    j++;
                }
            }

            // pop icon & station
            bool isRapidCharger = false;
            bool isSlowCharger = false;
            QString station;
            bool noPrice = false;
            bool noData = false;
            bool hasOilType = false;
            bool hasOilPrice = false;

            switch (rgDataVo.highwayTbtList_nTBTType(i)) {
            case RgTbtType::SA:
                {
                    auto oilFlag = (int)rgDataVo.highwayTbtList_byOilFlag(i);
                    auto hgGasoline = rgDataVo.highwayTbtList_uOilPrice( i, RgHighwayFuelPriceIndex::HIGH_GRADE_GASOLINE );
                    auto gasoline = rgDataVo.highwayTbtList_uOilPrice( i, RgHighwayFuelPriceIndex::GASOLINE );
                    auto diesel = rgDataVo.highwayTbtList_uOilPrice( i, RgHighwayFuelPriceIndex::DIESEL );
                    auto lpg = rgDataVo.highwayTbtList_uOilPrice( i, RgHighwayFuelPriceIndex::LPG );

                    int oilCompany = VSM_OilCompanyNone;
                    int oilCompany2 = VSM_OilCompanyNone;

                    switch ( mCurrentOilType ) {
                    case SettingConstants::OilTypePremiumGasoline:
                        {
                            oilCompany = rgDataVo.highwayTbtList_eOilCompany(i, 0);
                            oilCompany2 = rgDataVo.highwayTbtList_eOilCompany(i, 1);
                            if ( oilCompany != 0 ) {
                                station = RgUtils::highwayStationImage( oilCompany );
                                QVariantMap oilAndPrice;
                                oilAndPrice[QStringLiteral("icon")] = QStringLiteral("highway_poi_premium.png");
                                if ( rgDataVo.highwayTbtList_byOilFlag(i) & RgFacilityCode::OIL ) {
                                    hasOilType = true;
                                    auto gasolinePrice = rgDataVo.highwayTbtList_uOilPrice( i, RgHighwayFuelPriceIndex::HIGH_GRADE_GASOLINE );
                                    if ( gasolinePrice > 0 ) {
                                        hasOilPrice = true;
                                        oilAndPrice[QStringLiteral("price")] = QLocale().toString(gasolinePrice);
                                        noData = false;
                                    }
                                }
                                oilAndPriceList.append(oilAndPrice);
                            }
                        }
                        break;
                    case SettingConstants::OilTypeGasoline:
                        {
                            oilCompany = rgDataVo.highwayTbtList_eOilCompany(i);
                            if ( oilCompany != 0 ) {                                
                                station = RgUtils::highwayStationImage( oilCompany );
                                QVariantMap oilAndPrice;
                                oilAndPrice[QStringLiteral("icon")] = QStringLiteral("highway_poi_gasoline.png");
                                if ( rgDataVo.highwayTbtList_byOilFlag(i) & RgFacilityCode::OIL ) {                                    
                                    hasOilType = true;
                                    auto gasolinePrice = rgDataVo.highwayTbtList_uOilPrice( i, RgHighwayFuelPriceIndex::GASOLINE );
                                    if ( gasolinePrice > 0 ) {
                                        hasOilPrice = true;
                                        oilAndPrice[QStringLiteral("price")] = QLocale().toString(gasolinePrice);
                                        noData = false;
                                    }
                                }
                                oilAndPriceList.append(oilAndPrice);
                            }
                        }
                        break;
                    case SettingConstants::OilTypeDiesel:
                        {
                            oilCompany = rgDataVo.highwayTbtList_eOilCompany(i);
                            if ( oilCompany != 0 ) {
                                station = RgUtils::highwayStationImage( oilCompany );
                                QVariantMap oilAndPrice;
                                oilAndPrice[QStringLiteral("icon")] = QStringLiteral("highway_poi_disel.png");
                                if ( rgDataVo.highwayTbtList_byOilFlag(i) & RgFacilityCode::OIL ) {
                                    hasOilType = true;
                                    auto dieselPrice = rgDataVo.highwayTbtList_uOilPrice( i, RgHighwayFuelPriceIndex::DIESEL );
                                    if ( dieselPrice > 0 ) {
                                        hasOilPrice = true;
                                        oilAndPrice[QStringLiteral("price")] = QLocale().toString(dieselPrice);
                                        noData = false;
                                    }
                                }
                                oilAndPriceList.append(oilAndPrice);
                            }
                        }
                        break;
                    case SettingConstants::OilTypeLPG:
                        {
                            oilCompany = rgDataVo.highwayTbtList_eOilCompany(i);
                            if ( oilCompany != 0 ) {
                                station = RgUtils::highwayStationImage( oilCompany );
                                QVariantMap oilAndPrice;
                                oilAndPrice[QStringLiteral("icon")] = QStringLiteral("highway_poi_lpg.png");
                                if ( rgDataVo.highwayTbtList_byOilFlag(i) & RgFacilityCode::GAS ) {
                                    hasOilType = true;
                                    auto lpgPrice = rgDataVo.highwayTbtList_uOilPrice( i, RgHighwayFuelPriceIndex::LPG );
                                    if ( lpgPrice > 0 ) {
                                        hasOilPrice = true;
                                        oilAndPrice[QStringLiteral("price")] = QLocale().toString(lpgPrice);
                                        noData = false;
                                    }
                                }
                                oilAndPriceList.append(oilAndPrice);
                            }
                        }
                        break;
                    case SettingConstants::OilTypeElec:
                        {
                            isRapidCharger = rgDataVo.highwayTbtList_Ev_isRapidCharger(i);
                            isSlowCharger = rgDataVo.highwayTbtList_Ev_isSlowCharger(i);
                            if ( !isRapidCharger && !isSlowCharger ) {
                                noData = true;
                            }
                        }
                        break;
                    }

                    switch ( mCurrentOilType ) {
                    case SettingConstants::OilTypePremiumGasoline:
                    case SettingConstants::OilTypeGasoline:
                    case SettingConstants::OilTypeLPG:
                        {
                            if ( !hasOilType ) {
                                noData = true;
                            }
                            else {
                                if ( !hasOilPrice) {
                                    noPrice = true;
                                }
                            }
                        }
                        break;
                    }

                    qInfo() << QString( "[HighwayTbtView]index: %1 name: %2 oilCompany: %3 %4 oilFlag: %5 station: %6 noPrice: %7 noData: %8 OilType: %9 hgGasoline: %10 gasoline: %11 diesel: %12 lpg %13 nDcCHA: %14 nAC3: %15 nDcCombo: %16 nSlowCharge: %17" )
                             .arg( i )
                             .arg( name )
                             .arg( oilCompany )
                             .arg( oilCompany2 )
                             .arg( oilFlag )
                             .arg( station )                               
                             .arg( noPrice )
                             .arg( noData )
                             .arg( mCurrentOilType ).arg( hgGasoline ).arg( gasoline ).arg( diesel ).arg( lpg )
                             .arg( rgDataVo.highwayTbtList_nDcCHA(i) )
                             .arg( rgDataVo.highwayTbtList_nAC3(i) )
                             .arg( rgDataVo.highwayTbtList_nDcCombo(i) )
                             .arg( rgDataVo.highwayTbtList_nSlowCharge(i) );
                }
                break;
            }

            // toll

            switch (rgDataVo.highwayTbtList_nTBTType(i)) {
            case RgTbtType::TG:
            case RgTbtType::IC:
            case RgTbtType::JC:
                {
                    // pop icon
                    // UID 0.97a p.89 회전안내 제공 (JC 또는 IC 출구의 경우)
                    //qDebug() << "[HighwayTbtView]TG/IC/JC " << QString( "i: %1 name: %2 exit: %3 tbtListLength: %4" ).arg(i).arg(name).arg(rgDataVo.highwayTbtList_bHighWayExit(i)).arg( tbtListLength );

                    if (rgDataVo.highwayTbtList_bHighWayExit(i)) {
                        if ( isButton) {
                            popIcon = QStringLiteral("highway_pop_exit.png");
                        }
                        else {
                            popIcon = QStringLiteral("highway_pop_route.png");
                        }
                    }

                    if(rgDataVo.highwayTbtList_nTollFee(i) > 0) {
                        toll = QLocale().toString( rgDataVo.highwayTbtList_nTollFee(i) );
                    }
                }
                break;
            }

            congestion = RgUtils::highwayCongestionImage( rgDataVo.highwayTbtList_nCong(i) );

            auto model = makeModel( QStringLiteral("list"), noPrice, noData, name, popIcon, toll, distImages, congestion, station, oilAndPriceList, isRapidCharger, isSlowCharger );

            if ( isButton ) {
                QMetaObject::invokeMethod(mViewComponent, "setButtonModel", Q_ARG(QVariant, QVariant::fromValue( model ) ));                
                //break;
            }
            else {
                listModel.append( model );
            }
        }

        QMetaObject::invokeMethod(mViewComponent, "setListModel", Q_ARG(QVariant, QVariant::fromValue( listModel ) ));
    }
}

