#include "RecentDestinationViewMediator.h"
#include "Constants.h"
#include "RouteConstants.h"
#include "CommonConstants.h"
#include "SearchConstants.h"
#include "ZoneConstants.h"
#include <QJSValueIterator>
#include "RecentDestinationDto.h"
#include "BookmarkHelper.h"
#include "StringHelper.h"
#include "ViewRegistry.h"

using namespace SKT;

RecentDestinationViewMediator::RecentDestinationViewMediator( QObject* viewComponent )
    : RecentDestinationPageViewMediator( viewComponent )
{
}
