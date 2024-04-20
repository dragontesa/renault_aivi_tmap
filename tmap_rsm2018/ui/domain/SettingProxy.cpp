#include "SettingProxy.h"
#include "SettingDto.h"
#include "TSettings.h"
#include "DatabaseHelper.h"
#include "TSettings.h"
#include "SettingConstants.h"
#include "HiddenSettingConstants.h"
#include "qctestingfeatures.h"


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 수정 이력
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//chunjae.lee 2018.03.21 - 디폴트값 저장하는 (initialization)함수에서 모든 타입을 String으로 저장하고 있음,
//                         디폴트값을 저장하고 있는  (addItem)함수에서 float을 제외한 모든 타입을 String으로 사용하도록 수정
//                       - 추후 리팩토링시 원래 타입으로 처리하는 방법 모색
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

using namespace SKT;

SettingProxy::SettingProxy()
    : AbstractProxy( "SettingProxy" )
{
    mSettings = t_new_shared<TSettings>();
    //테스트
    addItem( SettingConstants::Section1, SettingConstants::LogEnable, TSettingItem::Bool, true);
    addItem( SettingConstants::Section1, SettingConstants::DisplayOption, TSettingItem::Integer,  1);
    addItem( SettingConstants::Section1, SettingConstants::UserName, TSettingItem::String,  "Foo");

    //환경설정
    //-초기화면 최근 목적지 표시(Default: ON)
    addItem( SettingConstants::MapMain, SettingConstants::RecentDestinations, TSettingItem::Bool,  true);
    //-야간모드(Default: 자동)
    addItem( SettingConstants::MapMain, SettingConstants::NightMode, TSettingItem::Integer,  SettingConstants::NightModeAuto);
    //[TMCRSM-186 leejaeho] 주소 표시 방법 Default는 도로명으로 적용
    //-주소 표시 방법(Default: 도로명)
    addItem( SettingConstants::MapMain, SettingConstants::AddressDisplayMethod, TSettingItem::Integer, SettingConstants::AddressDisplayMethodRoadName);
    addItem( SettingConstants::MapMain, SettingConstants::AddressDisplay2ndMethod, TSettingItem::Integer, SettingConstants::AddressDisplayMethodRoadName);//주소검색부분에서 사용
    //-주변 교통정보 표시(Default: OFF)
    addItem( SettingConstants::MapMain, SettingConstants::DisplayArroundTrafficInfo, TSettingItem::Bool, false );
    //-주행선 교통정보 항상 표시(Default: OFF)
    addItem( SettingConstants::MapMain, SettingConstants::AlwaysDisplayDriveLineTrafficInfo, TSettingItem::Bool, false );
    //-고속도로 주행안내(Default: ON)
    addItem( SettingConstants::MapMain, SettingConstants::ExpresswayDrivingGuidance, TSettingItem::Bool, true );
    //-GPS속도 표시(Default: ON)
    addItem( SettingConstants::MapMain, SettingConstants::GpsSpeed, TSettingItem::Bool, true );
    //-스카이 박스(Default: ON)
    addItem( SettingConstants::MapMain, SettingConstants::SkyeBox, TSettingItem::Bool, true );

#ifndef TMAP_HAVE_CLUSTER
    //-속도 반응 지도 사용(Default: ON)
    addItem( SettingConstants::MapSubSpeedResponse, SettingConstants::SpeedResponseMapUse, TSettingItem::Bool,  true);
#else
    addItem( SettingConstants::MapSubSpeedResponse, SettingConstants::SpeedResponseMapUse, TSettingItem::Bool,  false);
#endif

    //-속도 반응 지도 사용>지도 기본 보기(추천))
    addItem( SettingConstants::MapSubSpeedResponse, SettingConstants::Distance, TSettingItem::Integer,  SettingConstants::DistanceBasicView);
    //-주행 중 지도 글씨 크기(Default: 보통)
    addItem( SettingConstants::MapSubMapCharacter, SettingConstants::Character, TSettingItem::Integer,  SettingConstants::CharacterNormal);
    //-"자동차 아이콘(Default: 기본1)
    addItem( SettingConstants::MapSubCarIcon, SettingConstants::Icon, TSettingItem::Integer,  SettingConstants::CarIconBasic0 );
    //-"유종 선택(Default: 휘발유)
    addItem( SettingConstants::MapSubOilType, SettingConstants::OilKind, TSettingItem::Integer,  SettingConstants::OilTypeGasoline );
    //-"충전 타입(Default: AC3상)
    addItem( SettingConstants::MapSubChargeType, SettingConstants::ChargeType, TSettingItem::Integer,  SettingConstants::ChargeTypeAC3 );
    //과속 경고음 시작 시점(Default: 600m)
    addItem( SettingConstants::GuidMain, SettingConstants::RoadViolation, TSettingItem::Integer,  SettingConstants::RoadViolation1Km);
    //내비 음소거 시 안전운전 경고음 안내(Default: OFF)
    addItem( SettingConstants::GuidMain, SettingConstants::SafeDrivingSoundAtMute, TSettingItem::Bool,  false );
    addItem( SettingConstants::GuidMain, SettingConstants::BeepSoundGuideAtMute,  TSettingItem::Bool,  false );
    addItem( SettingConstants::GuidMain, SettingConstants::SpeedViolationSectionSignalCrackdown, TSettingItem::Integer, 0); // 없음
    //안내 음성 설정(Default: 여자)
    addItem( SettingConstants::GuidSubVoice, SettingConstants::Male,TSettingItem::Bool,  false );
    //경고 카메라 음성 안내(Default: 과속단속, 구간단속, 신호/과속단속, 이동식단속, 버스전용차로단속, 끼어들기단속, 교통정보수집)
    addItem( SettingConstants::GuidSubCamera, SettingConstants::CrackdownOnSpeedViolation, TSettingItem::Bool,  true );
    addItem( SettingConstants::GuidSubCamera, SettingConstants::SectionControl, TSettingItem::Bool, true  );
    addItem( SettingConstants::GuidSubCamera, SettingConstants::SignalControl,  TSettingItem::Bool,  true );
    addItem( SettingConstants::GuidSubCamera, SettingConstants::DetachableCrackdown, TSettingItem::Bool,  true );
    addItem( SettingConstants::GuidSubCamera, SettingConstants::CrackdownWithBusExclusiveCar, TSettingItem::Bool,  true );
    addItem( SettingConstants::GuidSubCamera, SettingConstants::ParkingControl, TSettingItem::Bool, false );
    addItem( SettingConstants::GuidSubCamera, SettingConstants::InterruptRegulation, TSettingItem::Bool, true );
    addItem( SettingConstants::GuidSubCamera, SettingConstants::MonitoringOfRoadShoulder, TSettingItem::Bool, false );
    addItem( SettingConstants::GuidSubCamera, SettingConstants::CollectingTrafficInformation, TSettingItem::Bool, true );
    addItem( SettingConstants::GuidSubCamera, SettingConstants::ControlOfVariableVehicles, TSettingItem::Bool, false );
    //주의구간 음성안내(Default: 사고다발, 어린이보호구역, 안개주의, 급커브)
    addItem( SettingConstants::GuidSubAttentionSection, SettingConstants::SpeedBump, TSettingItem::Bool,  false );
    addItem( SettingConstants::GuidSubAttentionSection, SettingConstants::AccidentFrequent, TSettingItem::Bool, true );
    addItem( SettingConstants::GuidSubAttentionSection, SettingConstants::ChildProtectedArea, TSettingItem::Bool,  true);
    addItem( SettingConstants::GuidSubAttentionSection, SettingConstants::FogCaution, TSettingItem::Bool, true );
    addItem( SettingConstants::GuidSubAttentionSection, SettingConstants::AttentionToWildAnimals, TSettingItem::Bool, false);
    addItem( SettingConstants::GuidSubAttentionSection, SettingConstants::RailroadXing, TSettingItem::Bool, false);
    addItem( SettingConstants::GuidSubAttentionSection, SettingConstants::SteepSlopeSection, TSettingItem::Bool, false );
    addItem( SettingConstants::GuidSubAttentionSection, SettingConstants::SharpCurve, TSettingItem::Bool, true);
    addItem( SettingConstants::GuidSubAttentionSection, SettingConstants::OneWay, TSettingItem::Bool,  false);
    addItem( SettingConstants::GuidSubAttentionSection, SettingConstants::FrequentSignalViolation, TSettingItem::Bool, false );
    addItem( SettingConstants::GuidSubAttentionSection, SettingConstants::SpeedTravelFrequently, TSettingItem::Bool, false );
    addItem( SettingConstants::GuidSubAttentionSection, SettingConstants::TrafficJamArea, TSettingItem::Bool, false );
    addItem( SettingConstants::GuidSubAttentionSection, SettingConstants::ChangeSectionOfCar, TSettingItem::Bool, false );
    addItem( SettingConstants::GuidSubAttentionSection, SettingConstants::ChargingStation, TSettingItem::Bool, false);
    addItem( SettingConstants::GuidSubAttentionSection, SettingConstants::Tunnel, TSettingItem::Bool, false);
    addItem( SettingConstants::GuidSubAttentionSection, SettingConstants::RestArea, TSettingItem::Bool, false);
    addItem( SettingConstants::GuidSubAttentionSection, SettingConstants::FeeStation, TSettingItem::Bool, false );
    addItem( SettingConstants::GuidSubAttentionSection, SettingConstants::BadSight, TSettingItem::Bool,  false);
    addItem( SettingConstants::GuidSubAttentionSection, SettingConstants::SleepShelter, TSettingItem::Bool,  false);
    //음성 길안내 시점(Default: 전체 true)
    addItem( SettingConstants::GuidSubVoiceGuidTime, SettingConstants::Km1, TSettingItem::Bool, true );
    addItem( SettingConstants::GuidSubVoiceGuidTime, SettingConstants::Km2, TSettingItem::Bool,  true );
    addItem( SettingConstants::GuidSubVoiceGuidTime, SettingConstants::M300, TSettingItem::Bool, true );
    addItem( SettingConstants::GuidSubVoiceGuidTime, SettingConstants::M600, TSettingItem::Bool,  true);
    //자동 재탐색(Default: ON)
    addItem( SettingConstants::RouteSearch, SettingConstants::AutoRedetection, TSettingItem::Bool, true);
    //경로 이탈 재탐색(Default: ON)
    addItem( SettingConstants::RouteSearch, SettingConstants::RedetectRouteVeerOff, TSettingItem::Bool, true);
    //하이패스 장착(Default: ON)
    addItem( SettingConstants::RouteSearch, SettingConstants::HipassMount, TSettingItem::Bool, true);
    //경로 대안 선택(Default: 최적길(비본설정으로 변경 불가), 최소시간,  무료도로)
    addItem( SettingConstants::RouteAlternateSelection, SettingConstants::BestPath, TSettingItem::Bool,  true);
    addItem( SettingConstants::RouteAlternateSelection, SettingConstants::MinimumTime, TSettingItem::Bool, true);
    addItem( SettingConstants::RouteAlternateSelection, SettingConstants::FreeRoad, TSettingItem::Bool, true);
    addItem( SettingConstants::RouteAlternateSelection, SettingConstants::ShortestDistance, TSettingItem::Bool,  false);
    addItem( SettingConstants::RouteAlternateSelection, SettingConstants::ExpresswayPriority, TSettingItem::Bool,  false );
    addItem( SettingConstants::RouteAlternateSelection, SettingConstants::BeginnerPass, TSettingItem::Bool,  false);
    //사용 안함 (확인 필요)
    addItem( SettingConstants::NaviInfo, SettingConstants::AppVersion, TSettingItem::String,  CommonConstants::Empty);

    //option
    addItem( SettingConstants::DirectionGuid, SettingConstants::MapMode, TSettingItem::Integer,  SettingConstants::MapModeBirdView );
    addItem( SettingConstants::DirectionGuid, SettingConstants::OtherRoute, TSettingItem::Integer,  SettingConstants::OtherRouteTMapOptimalPath);

    //option: 주변검색,업종검색
    addItem( SettingConstants::SearchOption, SettingConstants::SearchType, TSettingItem::Integer,  SettingConstants::SearchTypeWithinRadius);
    addItem( SettingConstants::SearchOption, SettingConstants::OilKind, TSettingItem::Integer,  SettingConstants::OilTypeGasoline);
    addItem( SettingConstants::SearchOption, SettingConstants::OilBrand, TSettingItem::Integer,  SettingConstants::OilBrandAll);
    addItem( SettingConstants::SearchOption, SettingConstants::OilArea, TSettingItem::Integer,  SettingConstants::OilAreaUpTo2Km);
    addItem( SettingConstants::SearchOption, SettingConstants::ChargeType, TSettingItem::Integer,  SettingConstants::ChargeTypeAll);


    //// hidden test menu setting stuff
    addItem( HiddenSettingConstants::HiddenSetting, HiddenSettingConstants::TnappServerIp, TSettingItem::String,QString());  //  // 0: commercial 1: manually
    addItem( HiddenSettingConstants::HiddenSetting, HiddenSettingConstants::TnappServerPort, TSettingItem::Integer,0);
    addItem( HiddenSettingConstants::HiddenSetting, HiddenSettingConstants::NddsServer, TSettingItem::Integer, HiddenSettingConstants::K_Commercial);
    addItem( HiddenSettingConstants::HiddenSetting, HiddenSettingConstants::AvNextServer, TSettingItem::Integer, 0);
    addItem( HiddenSettingConstants::HiddenSetting, HiddenSettingConstants::NetworkLog, TSettingItem::Bool,false);
//    addItem( HiddenSettingConstants::HiddenSetting, HiddenSettingConstants::HpsWifi, TSettingItem::Bool,false);
//    addItem( HiddenSettingConstants::HiddenSetting, HiddenSettingConstants::Wmds, TSettingItem::Bool,false); // 0: commercial 1: development
    addItem( HiddenSettingConstants::HiddenSetting, HiddenSettingConstants::SafeHelperServerIp, TSettingItem::String,QString()); // 0: commercial 1: development
    addItem( HiddenSettingConstants::HiddenSetting, HiddenSettingConstants::SafeHelperServerPort, TSettingItem::Integer, 0);
    addItem( HiddenSettingConstants::HiddenSetting, HiddenSettingConstants::TmapLog, TSettingItem::Bool,false);
    addItem( HiddenSettingConstants::HiddenSetting, HiddenSettingConstants::GpsMode, TSettingItem::Integer,HiddenSettingConstants::K_SwitchableGps);
    addItem( HiddenSettingConstants::HiddenSetting, HiddenSettingConstants::GpsLog, TSettingItem::Bool,false);
    addItem( HiddenSettingConstants::HiddenSetting, HiddenSettingConstants::FixedRerouteTime, TSettingItem::Integer,0); // default server
    addItem( HiddenSettingConstants::HiddenSetting, HiddenSettingConstants::MapMatching, TSettingItem::Bool,false); // default true
//    addItem( HiddenSettingConstants::HiddenSetting, HiddenSettingConstants::RoadSignVoiceStreamingServer, TSettingItem::String,QString());
    addItem( HiddenSettingConstants::HiddenSetting, HiddenSettingConstants::TvasVersion, TSettingItem::Integer,HiddenSettingConstants::K_Tvas_54);
    addItem( HiddenSettingConstants::HiddenSetting, HiddenSettingConstants::VmsTest, TSettingItem::String, QString());
    addItem( HiddenSettingConstants::HiddenSetting, HiddenSettingConstants::GpsTimeBias, TSettingItem::Integer, 0);
    // vsm hidden setting
    addItem( HiddenSettingConstants::HiddenVsmSetting, HiddenSettingConstants::VsmLevelSettingUi, TSettingItem::Bool, false);
    addItem( HiddenSettingConstants::HiddenVsmSetting, HiddenSettingConstants::VsmDebugSettingUi, TSettingItem::Bool, false);
    addItem( HiddenSettingConstants::HiddenVsmSetting, HiddenSettingConstants::VsmServer, TSettingItem::Integer, 2); // 0: commercial 1: less commercial 2:development
    addItem( HiddenSettingConstants::HiddenVsmSetting, HiddenSettingConstants::VsmConfigId, TSettingItem::String, QString(QStringLiteral("TMAP_RSM_AIVI")));
    addItem( HiddenSettingConstants::HiddenVsmSetting, HiddenSettingConstants::VsmConfigVer, TSettingItem::String, QString());
    addItem( HiddenSettingConstants::HiddenVsmSetting, HiddenSettingConstants::VsmAutoLevelMap, TSettingItem::Integer, 0);
//    addItem( HiddenSettingConstants::HiddenVsmSetting, HiddenSettingConstants::VsmAutoLevelLowLevel, TSettingItem::Float, 11.0f);
//    addItem( HiddenSettingConstants::HiddenVsmSetting, HiddenSettingConstants::VsmAutoLevelMiddleLevel, TSettingItem::Float, 10.5f);
//    addItem( HiddenSettingConstants::HiddenVsmSetting, HiddenSettingConstants::VsmAutoLevelHighLevel, TSettingItem::Float, 10.0f);
//    addItem( HiddenSettingConstants::HiddenVsmSetting, HiddenSettingConstants::VsmAutoLevelLowTilt, TSettingItem::Float, 40.0f);
//    addItem( HiddenSettingConstants::HiddenVsmSetting, HiddenSettingConstants::VsmAutoLevelMiddleTilt, TSettingItem::Float, 25.0f);
//    addItem( HiddenSettingConstants::HiddenVsmSetting, HiddenSettingConstants::VsmAutoLevelHighTilt, TSettingItem::Float, 25.0f);
#ifdef TMAP_TMC_QC_TESTING
    addItem( HiddenSettingConstants::HiddenVsmSetting, HiddenSettingConstants::VsmUseFps, TSettingItem::Bool, false);
    addItem( HiddenSettingConstants::HiddenSetting, HiddenSettingConstants::MapmatchingFeedback, TSettingItem::Bool, true);
    addItem( HiddenSettingConstants::HiddenSetting, HiddenSettingConstants::MapmatchingDebugText, TSettingItem::Bool, false);
    addItem( HiddenSettingConstants::HiddenSetting, HiddenSettingConstants::OnlyTunnelMMFB, TSettingItem::Bool, true);
    addItem( HiddenSettingConstants::HiddenSetting, HiddenSettingConstants::MMFBLog, TSettingItem::Bool, false);
    addItem( HiddenSettingConstants::HiddenSetting, HiddenSettingConstants::GpsLog2Kml, TSettingItem::Bool, false);
    addItem( HiddenSettingConstants::HiddenSetting, HiddenSettingConstants::MmfbGpsMethod, TSettingItem::Integer, 2);
    addItem( HiddenSettingConstants::HiddenSetting, HiddenSettingConstants::MmfbAngleOnTunnel, TSettingItem::Integer, 0);
    addItem( HiddenSettingConstants::HiddenSetting, HiddenSettingConstants::MmfbDrOnly, TSettingItem::Bool, true); // DR only get default just for v0.18.304 (official release shared with Renault and bosch)
    addItem( HiddenSettingConstants::HiddenSetting, HiddenSettingConstants::MmfbDrCep, TSettingItem::Bool, true);
    addItem( HiddenSettingConstants::HiddenSetting, HiddenSettingConstants::MmfbDrCepRange, TSettingItem::Integer, 15);
    addItem( HiddenSettingConstants::HiddenSetting, HiddenSettingConstants::MmfbDrCepMMRange, TSettingItem::Integer, 20);
#endif
//    addItem( HiddenSettingConstants::HiddenVsmSetting, HiddenSettingConstants::LocalMapmatching, TSettingItem::Bool, true);
//    addItem( HiddenSettingConstants::HiddenVsmSetting, HiddenSettingConstants::Crashlytics, TSettingItem::Integer, HiddenSettingConstants::K_Crsh_Server);
//    addItem( HiddenSettingConstants::HiddenVsmSetting, HiddenSettingConstants::ForceCrash, TSettingItem::Bool, false);
    addItem( HiddenSettingConstants::HiddenVsmSetting, HiddenSettingConstants::GpsTrackingPath, TSettingItem::Bool, false);
    addItem( HiddenSettingConstants::HiddenVsmSetting, HiddenSettingConstants::GpsChangeAlarm, TSettingItem::Bool, true);

    addItem( HiddenSettingConstants::HiddenSetting, HiddenSettingConstants::UseScreenCapture, TSettingItem::Bool, true);
}

bool SettingProxy::addItem(const QString& section, const QString& key, const int& type, const QVariant& defValue)
{
    QVariant value = defValue;
    switch(type) {
    case TSettingItem::Bool:
        value = defValue.toBool() ? 1 : 0;
        //chunjae.lee 2018.03.21 - 환경설정: Qt<->Qml간에 문자열로 주고 받고 있음, 추후 원타입으로 저장하려면 qml 및 이곳 타입변경 필요
        mSettings->addItem( section, TSettingItem( TSettingItem::Bool,   key, value.toString()) );//value.toBool()
        break;
    case TSettingItem::Integer:
        //chunjae.lee 2018.03.21 - 환경설정: Qt<->Qml간에 문자열로 주고 받고 있음, 추후 원타입으로 저장하려면 qml 및 이곳 타입변경 필요
        mSettings->addItem( section, TSettingItem( TSettingItem::Integer, key, value.toString()) );//value.toInt()
        break;
    case TSettingItem::Float:
        //chunjae.lee 2018.03.21 - 환경설정: Qt<->Qml간에 문자열로 주고 받고 있음, 추후 원타입으로 저장하려면 qml 및 이곳 타입변경 필요
        mSettings->addItem( section, TSettingItem( TSettingItem::Float, key, value.toString()) ); //value.toFloat()
         break;
    case TSettingItem::String:
        mSettings->addItem( section, TSettingItem( TSettingItem::String,  key, value.toString()) );
        break;
    default:
        return false;
    }
    mItems[section][key][type] = value;
    return true;
}

QHash<QString,QHash<QString,QHash<int,QVariant>> > SettingProxy::getItems()
{
    return mItems;
}

void SettingProxy::initialization()
{
    QHash<QString,QHash<QString,QHash<int,QVariant>>> hash_i = mItems;
    QHash<QString,QHash<QString,QHash<int,QVariant>>>::const_iterator i = hash_i.constBegin();
    while (i != hash_i.constEnd()) {
       QHash<QString,QHash<int,QVariant>> hash_j = i.value();
       QHash<QString,QHash<int,QVariant>>::const_iterator j = hash_j.constBegin();
       while (j != hash_j.constEnd()) {
           QHash<int,QVariant> hash_k = j.value();
           QHash<int,QVariant> ::const_iterator k = hash_k.constBegin();
           while (k != hash_k.constEnd()) {
               QString section = i.key();
               QString key = j.key();
               //chunjae.lee 2018.03.21 - 환경설정: Qt<->Qml간에 문자열로 주고 받고 있음, 추후 원타입으로 저장하려면 qml 및 이곳 타입변경 필요
               QString value = k.value().toString();
               setSetting(section, key, value);
               k++;
           }
           j++;
       }
       ++i;
    }
}


QList<SettingDtoPtr> SettingProxy::settings( const QString& section )
{
    QList<SettingDtoPtr> ret;

    // setting에서 기본 값을 가져와 리스트 구성
    auto db = DatabaseHelper::instance()->connectionOfCurrentUser();

    auto keys = mSettings->keys( section );
    for ( auto key : keys ) {
        auto item = mSettings->item( section, key );
        auto dto = t_new_shared<SettingDto>();
        dto->setType( item.type );
        dto->setSection( section );
        dto->setKey( item.name );
        dto->setValue( item.defValue );
        ret.append( dto );
    }

    // db에서 유저 설정 값을 가져와 리스트에 존재하면 replace
    auto settingDao = orm::TOrm::instance()->dao<SettingDto>();
    auto qry = settingDao->selectByField( db, CommonConstants::Section, section );
    while( qry->next() ) {
        auto dto = qry->record();
        auto str = dto->key().toString();
        if ( keys.contains( str ) ) {
            auto index = keys.indexOf( str );
            ret.replace( index, dto );
        }
    }

    for ( auto item : ret ) {
//        T_DEBUG(
//            QString( "%1 %2 %3 %4" )
//                .arg( item->section().toString() )
//                .arg( item->type().toInt() )
//                .arg( item->key().toString() )
//                .arg( item->value().toString() )
//        );
    }

//    db.close();

    return ret;
}

void SettingProxy::setSetting( const QString& section, const QString& key, const QString& value)
{
    auto item = mSettings->item( section, key );

    QList<SettingDtoPtr> setting;
    auto dto = t_new_shared<SettingDto>( item.type,
      section,
      key,
      value
    );
    setting.append( dto );
    setSettings(section, setting);
}


void SettingProxy::setSettings( const QString& section, const QList<SettingDtoPtr>& values )
{
    auto db = DatabaseHelper::instance()->connectionOfCurrentUser();

    auto settingDao = orm::TOrm::instance()->dao<SettingDto>();
    auto ib = settingDao->insertBuilder( true );
    auto qry = ib->buildQuery( db );

    for ( auto value : values ) {
        settingDao->insert( db, qry, *value );
    }
//    db.close();
}


SettingDtoPtr SettingProxy::setting( const QString& section, const QString& key ){
    SettingDtoPtr ret;

   // setting에서 기본 값을 가져와 리스트 구성
   auto db = DatabaseHelper::instance()->connectionOfCurrentUser();

   auto item = mSettings->item( section, key );
   ret = t_new_shared<SettingDto>();
   ret->setType( item.type );
   ret->setSection( section );
   ret->setKey( item.name );
   ret->setValue( item.defValue );

   QString where;
   where.append("section='");
   where.append(section);
   where.append("' and ");
   where.append("key='");
   where.append(key);
   where.append("' ");

   auto settingDao = orm::TOrm::instance()->dao<SettingDto>();
   auto v = settingDao->selectSingle( db, CommonConstants::Value,  where );
   if(!v.toString().isEmpty())
        ret->setValue(v);


//    T_DEBUG(
//        QString( "%1 %2 %3 %4" )
//            .arg( ret->section().toString() )
//            .arg( ret->type().toInt() )
//            .arg( ret->key().toString() )
//            .arg( ret->value().toString() )
//    );

   //db.close();

   return ret;
}


void SettingProxy::setSetting( const QString& section, const SettingDtoPtr& value ){
    auto db = DatabaseHelper::instance()->connectionOfCurrentUser();

    auto settingDao = orm::TOrm::instance()->dao<SettingDto>();
    auto ib = settingDao->insertBuilder( true );
    auto qry = ib->buildQuery( db );

    settingDao->insert( db, qry, *value );
    //db.close();
}

void SettingProxy::onRegister()
{
    auto db = DatabaseHelper::instance()->connectionOfCurrentUser();

    orm::TOrm::instance()->create<SettingDto>( db );

    //db.close();
}

void SettingProxy::onRemove()
{

}
