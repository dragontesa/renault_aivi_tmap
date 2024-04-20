#include "SpeedMeterViewMediator.h"
#include "Constants.h"
#include "RgConstants.h"
#include "MapConstants.h"
#include "SettingConstants.h"
#include "TestConstants.h"
#include "ResourceResolver.h"
#include "SettingDto.h"
#include "ViewRegistry.h"
#include "RgUtils.h"
#include "ApplicationConstants.h"

using namespace SKT;

SpeedMeterViewMediator::SpeedMeterViewMediator(  QObject* viewComponent )
    : AbstractViewMediator( viewComponent ),
    mCurrMapViewMode( MapViewMode::Normal )
{
}

SpeedMeterViewMediator::~SpeedMeterViewMediator()
{

}

QList<int> SpeedMeterViewMediator::listNotificationInterests()
{
    static QList<int> notes = {
        RgConstants::UpdateRouteGuidance,
        MapConstants::ApplyMapViewMode,
        TestConstants::RunTest,
        SettingConstants::ApplySetting,
        SettingConstants::ChangedSetting,
        RgConstants::RequestSdiStateUpdate,
        ApplicationConstants::UpdatedUserProfile,
        ApplicationConstants::ChangedUserProfile,
        RgConstants::UpdateSdiState
    };

    return notes;
}

bool SpeedMeterViewMediator::onEvent( int event, const QVariant& data )
{
    switch ( event ) {
    case Mediated:
        {                    
            // GPS speed display
            sendNotification( SettingConstants::RequestSetting,
                QVariantMap{
                { CommonConstants::Section, SettingConstants::MapMain },
                { CommonConstants::Key, SettingConstants::GpsSpeed }
            });

            sendNotification( RgConstants::RequestSdiStateUpdate );
        }
        break;
    default:
        return false;
    }

    return true;
}

bool SpeedMeterViewMediator::handleNotification( const mvc::INotificationPtr& note )
{
    switch ( note->getId() ) {
    case RgConstants::UpdateRouteGuidance:
        {
            auto vo = note->getBody().value<RgDataVo>();
            applyRgData( vo );
        }
        break;
    case RgConstants::RequestSdiStateUpdate:
        {
            // TODO 복수이면 위젯이 요청하면 mUpdate를 여러벌로( widget id )
            mUpdate = NotifiableUpdate();
        }
        break;       
    case SettingConstants::ApplySetting:
        {
            auto data = note->getBody().value<QVariantMap>();
            auto section = data[CommonConstants::Section].toString();

            // GPS speed display
            if ( section == SettingConstants::MapMain ) {
                auto key = data[CommonConstants::Key].toString();
                if ( key == SettingConstants::GpsSpeed) {
                    auto gpsSpeedDisplay = data[CommonConstants::Setting].value<SettingDtoPtr>()->value().toInt();
                    QQmlProperty::write(mViewComponent, CommonConstants::Visible, gpsSpeedDisplay);
                }
            }
        }
        break;
    case SettingConstants::ChangedSetting:
        {
            auto data = note->getBody().value<QVariantMap>();
            auto section = data[CommonConstants::Section].toString();

            // GPS speed display
            if ( section == SettingConstants::MapMain ) {
                auto key = data[CommonConstants::Key].toString();
                if ( key == SettingConstants::GpsSpeed) {
                    auto gpsSpeedDisplay = data[CommonConstants::Setting].value<SettingDtoPtr>()->value().toInt();
                    QQmlProperty::write(mViewComponent, CommonConstants::Visible, gpsSpeedDisplay);
                }
            }
        }
        break;
    case MapConstants::ApplyMapViewMode:
        {
            auto data = note->getBody().value<QVariantMap>();
            mCurrMapViewMode = (MapViewMode::ModeK)data[CommonConstants::Mode].toInt();
        }
        break;
    case ApplicationConstants::UpdatedUserProfile:
    case ApplicationConstants::ChangedUserProfile:
        {
            // GPS speed display
            sendNotification( SettingConstants::RequestSetting,
                QVariantMap{
                { CommonConstants::Section, SettingConstants::MapMain },
                { CommonConstants::Key, SettingConstants::GpsSpeed }
            });
        }
        break;
    case RgConstants::UpdateSdiState:
        {
            auto m = note->getBody().toMap();
            auto item = m["item"].toInt();
            switch ( item ) {
            case RgConstants::SdiAlert:
                {
                    mAlert = m["value"].toBool();
                    if ( !mLastSpeed.isEmpty() ) {
                        updateNumbers( mLastSpeed );
                    }
                }
                break;
            case RgConstants::SdiReset:
                {
                    mAlert = false;
                }
                break;
            }
        }
        break;
    default:
        return false;
    }

    return true;
}

void SpeedMeterViewMediator::applyRgData( const RgDataVo& rgDataVo )
{
    if ( rgDataVo.isNull() ) {
        return;
    }

    auto speed = QString::number(rgDataVo.nCurSpeed());
    mLastSpeed = speed;

    updateNumbers( speed );

    if ( mUpdate.speed != speed ){
        mUpdate.speed = speed;
        sendNotification(RgConstants::UpdateSdiState, QVariantMap{
            {QStringLiteral("item"),RgConstants::SdiCurrentSpeed},
            {QStringLiteral("value"), mUpdate.speed},
            {QStringLiteral("alert"), mAlert}
        });
    }
}

void SpeedMeterViewMediator::updateNumbers( const QString& speed )
{
    QStringList images;
    for(int i = 0; i < speed.length(); i++){
        auto ch = speed.at(i);
        images.append( ResourceResolver::instance()->imageUrl( QStringLiteral("sdi"), RgUtils::sdiCurrentSpeedNumberImage(ch, mAlert) ) );
    }

    QQmlProperty::write( mViewComponent, "speedNumberImages", QVariant::fromValue( images ) );
}
