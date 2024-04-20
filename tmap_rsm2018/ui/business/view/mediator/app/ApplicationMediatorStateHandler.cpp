#include "ApplicationMediator.h"
#include "Constants.h"
#include "CommonConstants.h"
#include "SettingConstants.h"
#include "ApplicationConstants.h"
#include "ViewRegistry.h"
#include "RgConstants.h"
#include "ToastsConstants.h"
#include "PlatformHelper.h"
#include "NavigationControllerConstants.h"
#include "NavigationControllerProtocol.h"
#include "../environment.h"

#include <QDir>

using namespace SKT;

void ApplicationMediator::handleStateChange( ApplicationConstants::StateK newState )
{
    // preprocessing of state change

    switch ( mState ) {
    case ApplicationConstants::StateWithMapData:
        break;
    case ApplicationConstants::StateNoMapData:
        break;
    case ApplicationConstants::StateNone:
        break;
    }

    mState = newState;
    QQmlProperty::write( mViewComponent, QStringLiteral("appState"), mState );

    // postprocessing of state change

    switch ( mState ) {
    case ApplicationConstants::StateWithMapData:
    case ApplicationConstants::StateNoMapData:
        {
            mBlankWrap = findChild( "blankWrap" );
            mZoneWrap = findChild( CommonConstants::ZoneWrap );
            mContentWrap = findChild( CommonConstants::ContentWrap );
            mContentRoot = findChild( CommonConstants::ContentRoot );
            mContentRootWithoutSoftkey = findChild( CommonConstants::ContentRootWithoutSoftkey );
            mMapViewWrap = findChild( CommonConstants::MapViewWrap );
            mToastWrap = findChild( CommonConstants::ToastWrap );
            mAlertWrap = findChild( CommonConstants::AlertWrap );
            mProgressWrap = findChild( CommonConstants::ProgressWrap );
            mWidgetWrap = findChild( CommonConstants::WidgetWrap );
            QObject* hardKeyWrap = findChild("hardKeyWrap");

#ifdef BUILD_TARGET
            // 우분투에서만 보이도록 한다.
            QQmlProperty::write(hardKeyWrap, "visible", false);
#endif

            bool noMap = mState == ApplicationConstants::StateNoMapData;

            // Zone1
            createView(
                QVariantMap{
                    {CommonConstants::Url,ViewRegistry::url(ViewRegistry::Zone1View)},
                    {QStringLiteral("parent"),QVariant::fromValue(mZoneWrap)},
                    {"noMap", noMap}
                });

            // Zone3
            createView(
                QVariantMap{
                    {CommonConstants::Url,ViewRegistry::url(ViewRegistry::Zone3View)},
                    {QStringLiteral("parent"),QVariant::fromValue(mZoneWrap)}
                });

            // create views if map data validated (engine init successful)
            if (newState == ApplicationConstants::StateWithMapData)
            {
                // MapView
                createView(
                        QVariantMap{
                    {CommonConstants::Url,ViewRegistry::url(ViewRegistry::MapView)},
                    {QStringLiteral("parent"),QVariant::fromValue(mMapViewWrap)}
                });

                // HardKeyView
                createView(
                        QVariantMap{
                    {CommonConstants::Url,ViewRegistry::url(ViewRegistry::HardKeyView)},
                    {QStringLiteral("parent"),QVariant::fromValue(hardKeyWrap)}
                });

                // Check if the map is not fully copied
                 createView(
                         QVariantMap{
                             {CommonConstants::Url,ViewRegistry::url(ViewRegistry::SettingMapUpdateView)},
                             {CommonConstants::Caller, CommonConstants::IncompleteMapData},
                             {CommonConstants::Visible, false},
                          });
            }
            // create view in Map data not found
            else {
                QObject* parent = PlatformHelper::instance()->popupRoot();
                if (parent) {
                }

                QVariantMap body;
                body.insert(QStringLiteral("type"), NavigationControllerProtocol::GetMapError );
                sendNotification( NavigationControllerConstants::RequestController, body );
            }
        }
        break;
    case ApplicationConstants::StateNone:
        break;
    }
}

