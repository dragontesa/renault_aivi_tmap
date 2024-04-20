#include "FavoriteMediator.h"
#include "Constants.h"
#include "SearchConstants.h"
#include "ViewRegistry.h"
#include <QString>

namespace SKT
{

FavoriteMediator::FavoriteMediator()
    : AbstractMediator()
    , mAdditionType(None)
{
    // Win10
}

FavoriteMediator::~FavoriteMediator()
{

}

QList<int> FavoriteMediator::listNotificationInterests()
{
    return QList<int> {
        Constants::CreateView,
        SearchConstants::ReturnToFavorite,
    };
}

bool FavoriteMediator::handleNotification(const mvc::INotificationPtr &note)
{
    switch (note->getId()) {
    case Constants::CreateView:
        {
            auto m = note->getBody().toMap();
            auto caller = m[CommonConstants::Caller].toString();
            auto owner = m[CommonConstants::Owner].toString();
            auto url = m[CommonConstants::Url].toString();

            if (caller == ViewRegistry::objectName(ViewRegistry::SearchTabView) &&
                owner == ViewRegistry::objectName(ViewRegistry::BookmarkPageView)) {

                if (url == ViewRegistry::url(ViewRegistry::MapSearchView)) {
                    mAdditionType = MapSearch;
                }
            }
        }
        break;

    case SearchConstants::ReturnToFavorite:
        {
            auto url = ViewRegistry::url(ViewRegistry::SearchTabView);
            int page = 2;
            sendNotification(Constants::CreateView, QVariantMap{{CommonConstants::Url, url}, {SearchConstants::Page, page}});
            mAdditionType = None;
        }
        break;

    default:
        break;
    }

    return true;
}

}//SKT
