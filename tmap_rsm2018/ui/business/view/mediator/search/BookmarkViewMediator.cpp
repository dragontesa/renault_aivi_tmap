#include "BookmarkViewMediator.h"
#include "Constants.h"
#include "CommonConstants.h"
#include "FavoriteDto.h"
#include "RouteConstants.h"
#include "SearchConstants.h"
#include "ZoneConstants.h"
#include <QJSValueIterator>
#include "NavigationControllerConstants.h"

using namespace SKT;

BookmarkViewMediator::BookmarkViewMediator( QObject* viewComponent )
    : BookmarkPageViewMediator( viewComponent )
{
    auto p = this;
}
