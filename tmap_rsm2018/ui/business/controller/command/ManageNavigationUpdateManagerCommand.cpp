#include "ManageNavigationUpdateManagerCommand.h"
#include <QQmlProperty>
#include "Constants.h"
#include "RequestCommand.h"
#include "NavigationUpdateManagerProxy.h"
#include "NavigationUpdateManagerConstants.h"

using namespace SKT;

MangeNavigationUpdateManagerCommand::MangeNavigationUpdateManagerCommand(QObject *parent)
    : SimpleCommand(parent)
{
}

MangeNavigationUpdateManagerCommand::~MangeNavigationUpdateManagerCommand()
{
    //T_DEBUG( "I'm done..." );
}

bool MangeNavigationUpdateManagerCommand::execute(const mvc::INotificationPtr &noti)
{
    auto updateManagerProxy = Facade::instance()->retrieveProxy<NavigationUpdateManagerProxy>();
    auto data = noti->getBody().value<QVariantMap>();
    auto notifyMethod = std::bind(&MangeNavigationUpdateManagerCommand::onResponseCommand, this, std::placeholders::_1);
    auto req = t_new_shared<RequestCommand>( notifyMethod, noti->getDestination() , data) ;

    switch (noti->getId()) {
    case NavigationUpdateManagerConstants::Apply:
        updateManagerProxy->applyCommand( req );
        break;
    case NavigationUpdateManagerConstants::Request:
        updateManagerProxy->requestCommand( req );
        break;
    case NavigationUpdateManagerConstants::Get:
        updateManagerProxy->getCommand( req );
        break;
    default:
        return false;
    }
    return true;
}

bool MangeNavigationUpdateManagerCommand::onResponseCommand( const mvc::INotificationPtr& note )
{
    auto req = note->getBody().value<RequestCommandPtr>();
    QVariantMap reqData = req->getReqData();
    QVariantMap resData = req->getResData();
    QVariantMap body;    
    return true;
}
