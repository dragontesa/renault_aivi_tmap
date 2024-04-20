#ifndef NAVIBOOKMARKAGENTPROXY_H
#define NAVIBOOKMARKAGENTPROXY_H

#include "AbstractProxy.h"
#include "RecentDestinationDto.h"
#include "RecentSearchDto.h"
#include "FavoriteDto.h"
#include "LastRouteDto.h"

namespace SKT {

T_SHAREABLE( RecentDestinationDto );
T_SHAREABLE( RecentSearchDto );
T_SHAREABLE( FavoriteDto );

class NavibookmarkAgentProxy : public AbstractProxy
{
	Q_OBJECT

public:
    NavibookmarkAgentProxy();

    // recent destination implements
    QList<RecentDestinationDtoPtr> recentDestListByHybrid(int maximum, int offset=0);
    QList<RecentDestinationDtoPtr> recentDestListBySortType(int maximum, int offset=0, int sortType=0);
    uint addRecentDestination(const RecentDestinationDto& newDto);
    uint addRecentDestination(const QList<RecentDestinationDtoPtr>& destList);
    QList<RecentDestinationDtoPtr> deleteRecentDest(const QList<uint>& idList);
    void clearRecentDestination();
    void updateRecentDestHit(uint id, uint score);
    void updateRecentDestType(uint id, int destType);
    QList<RecentDestinationDtoPtr> searchRecentDestination(const QString& name);
    QList<RecentDestinationDtoPtr> searchRecentDestination(const QString& name, int limit);

    // favorites
    QList<FavoriteDtoPtr> favoriteList(int maximum, int offset=0, int sortType=0);
    uint addFavorite(const FavoriteDto& newDto, bool silence=false);
    uint addFavorite(const QList<FavoriteDtoPtr>& favList);
    void updateFavoriteAlias(uint id, const QString alias);
    void updateFavoriteType(uint id, int destType);
    int  getFavoriteType(QSqlDatabase db, uint id);
    QList<FavoriteDtoPtr> deleteFavorites(const QList<uint>& idList);
    FavoriteDtoPtr deleteFavorites(int mapX, int mapY, int destType);
    void clearFavorites();
    uint hasFavorite(int mapX, int mapY, int poiId = -1);
    uint hasFavorite(const QSqlDatabase& db, int mapX, int mapY, int poiId = -1);
    FavoriteDtoPtr getHomeFavorite();
    FavoriteDtoPtr getOfficeFavorite();
    FavoriteDtoPtr getHomeFavorite(const QSqlDatabase& db);
    FavoriteDtoPtr getOfficeFavorite(const QSqlDatabase& db);
    FavoriteDtoPtr getFavoriteById(uint id);
    FavoriteDtoPtr getFavoriteByPos(int x, int y);
    QList<FavoriteDtoPtr> searchFavorite(const QString& name);
    QList<FavoriteDtoPtr> searchFavorite(const QString& name, int limit);

    // recent search implements
    QList<RecentSearchDtoPtr> recentSearchList(int maximum, int offset=0);
    void addRecentSearch(const QString& keyword);
    void deleteRecentSearch(const QList<uint>& idList);
    void clearRecentSearch();

    // last route info
    LastRouteDtoPtr lastRouteInfo();
    void setLastRouteInfo(
                          bool isNormallyTerminated ,
                          int selectedRouteIndex    ,
                          int selectedRouteOption   ,
                          int mapLevel              ,
                          int tpX                   ,
                          int tpY                   ,
                          const QString& tpTime     ,
                          const QString& sessionId  ,
                          const QString& address    ,
                          const QString& via
                          );
    void setLastRouteInfo(LastRouteDtoPtr dto);
    void resetLastRouteInfo();

    template<class dtoType>
    uint updateFieldById(const QString& fieldName, const QVariant& value, uint id);

    template<class dtoType>
    uint deleteOldItems(int maxx);

    template<class dtoType>
    uint getTableItemCount(const QSqlDatabase& db) const;

    void setCaller(const QString& caller);

    bool isHome(int destType) const;
    bool isOffice(int destType) const;
    bool isNoramlBookmark(int destType) const;
    bool isPoiBookmark(int destType) const;

private:
    void onRegister();

    void onRemove();

private:
    QString mCaller;
};

}
#endif // NAVIBOOKMARKAGENTPROXY_H
