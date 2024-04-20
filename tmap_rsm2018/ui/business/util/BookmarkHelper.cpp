#include "BookmarkHelper.h"
#include "SearchConstants.h"
#include <QString>
#include <QVariantMap>

namespace SKT
{

void BookmarkHelper::setAddFavoriteDestType(const QString &owner, QVariantMap &m)
{
    int destType = 0;
    if (owner.contains("/home"))
        destType = 2;
    else if (owner.contains("/office"))
        destType = 1;

    m[SearchConstants::DestType] = destType;
}

void BookmarkHelper::setAddFavoriteDestType(bool isHome, bool isOffice, QVariantMap &m)
{
    if (isHome) {
        m[SearchConstants::DestType] = 2;
    } else if (isOffice) {
        m[SearchConstants::DestType] = 1;
    } else {
        m[SearchConstants::DestType] = 0;
    }
}

}
