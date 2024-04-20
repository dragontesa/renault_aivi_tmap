#include "ManageNavigationPropertyCommand.h"
#include <QQmlProperty>
#include "Constants.h"
#include "RequestCommand.h"
#include "NavigationPropertyProxy.h"
#include "NavigationPropertyConstants.h"

using namespace SKT;

ManageNavigationPropertyCommand::ManageNavigationPropertyCommand(QObject *parent)
    : SimpleCommand(parent)
{
}

ManageNavigationPropertyCommand::~ManageNavigationPropertyCommand()
{
    //T_DEBUG( "I'm done..." );
}

bool ManageNavigationPropertyCommand::execute(const mvc::INotificationPtr &noti)
{
    switch (noti->getId()) {
    case NavigationPropertyConstants::setNavigationProperty: {
        auto propertyProxy = Facade::instance()->retrieveProxy<NavigationPropertyProxy>();
        auto data = noti->getBody().value<QVariantMap>();
        auto notifyMethod = std::bind(&ManageNavigationPropertyCommand::onResponseCommand, this, std::placeholders::_1);
        auto req = t_new_shared<RequestCommand>( notifyMethod, noti->getDestination() , data) ;
        propertyProxy->setNavigationProperty( req );
    }
    break;

    default:
        return false;
    }
    return true;
}

bool ManageNavigationPropertyCommand::onResponseCommand( const mvc::INotificationPtr& note )
{
    auto req = note->getBody().value<RequestCommandPtr>();
    QVariantMap reqData = req->getReqData();
    QVariantMap resData = req->getResData();
    QVariantMap body;
    return true;
}
