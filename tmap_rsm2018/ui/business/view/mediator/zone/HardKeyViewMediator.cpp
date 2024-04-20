#include "HardKeyViewMediator.h"
#include "Constants.h"
#include "ZoneConstants.h"
#include "NavigationControllerConstants.h"
#include "hardkey.h"

using namespace SKT;

HardKeyViewMediator::HardKeyViewMediator( QObject* viewComponent )
    : AbstractViewMediator( viewComponent )
{
}

QList<int> HardKeyViewMediator::listNotificationInterests()
{
    static QList<int> notes = {
#ifndef BUILD_TARGET
        // Ubuntu only
        NavigationControllerConstants::UpdateCswDebugInfo,
        ZoneConstants::PrintBuffersize
#endif
    };

    return notes;
}

bool HardKeyViewMediator::onEvent( int event, const QVariant& data )
{
    switch ( event ) {
    case Mediated:
#ifdef BUILD_TARGET
        // 우분투에서만 보이도록 한다.
        QQmlProperty::write(mViewComponent, "visible", false);
#endif
        break;

    case RecentTileButtonClicked:
        {
            sendNotification(ZoneConstants::ShowRecentTileGridView);
        }
        break;
    case CswLeftClicked:
        sendNotification(NavigationControllerConstants::ApplyCSW, HardKey::ItcJogDialLeft);
        break;
    case CswRightClicked:
        sendNotification(NavigationControllerConstants::ApplyCSW, HardKey::ItcJogDialRight);
        break;
    case CswUpClicked:
        sendNotification(NavigationControllerConstants::ApplyCSW, HardKey::ItcJogDialUp);
        break;
    case CswDownClicked:
        sendNotification(NavigationControllerConstants::ApplyCSW, HardKey::ItcJogDialDown);
        break;
    case CswSpinCwClicked:
        sendNotification(NavigationControllerConstants::ApplyCSW, HardKey::ItcJogDialSpinCw);
        break;
    case CswSpinCcwClicked:
        sendNotification(NavigationControllerConstants::ApplyCSW, HardKey::ItcJogDialSpinCcw);
        break;
    case CswOkClicked:
        sendNotification(NavigationControllerConstants::ApplyCSW, HardKey::ItcJogDialSelect);
        break;
    case CswBackClicked:
        sendNotification(NavigationControllerConstants::ApplyCSW, HardKey::ItcJogDialBack);
        break;
    case CswOptionClicked:
        sendNotification(NavigationControllerConstants::ApplyCSW, HardKey::ItcJogDialOption);
        break;

    default:
        return false;
    }

    return true;
}

bool HardKeyViewMediator::handleNotification( const mvc::INotificationPtr& noti )
{
    switch ( noti->getId() ) {
    case Constants::None:
        {
        }
        break;
    case NavigationControllerConstants::UpdateCswDebugInfo:
        {
            auto m = noti->getBody().toMap();
            auto section = m["section"].toInt();
            auto sequence = m["sequence"].toInt();
            auto valid = m["valid"].toBool();
            auto count = m["count"].toInt();
            auto name = m["objectName"].toString();
            auto viewName = m[CommonConstants::Caller].toString();
            QString text = QString("[%1]").arg(count);
            text += name;
            if (0 < name.length()) {
                text += ",";
            }
            text += QString("sec:%1").arg(section);
            text += QString(",seq:%1").arg(sequence);
            text += QString(",?:");
            text += valid ? "O" : "X";
            text += "\nviewName:";
            text += viewName;
            QQmlProperty::write(mViewComponent, "cswDebugInfoText", text);
        }
        break;

    case ZoneConstants::PrintBuffersize:
        QQmlProperty::write(mViewComponent, "bufferSize", noti->getBody().toInt());
        break;

    default:
        return false;
    }

    return true;
}
