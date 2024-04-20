#include "NavibookmarkAgentProxy.h"
#include "DatabaseHelper.h"
#include "TTableInfo.h"
#include "DatabaseMigration.h"
#include "SearchConstants.h"
#include "ProfileManager.h"

#ifndef TMAP_RECENTSEARCH_MAX
#define TMAP_RECENTSEARCH_MAX TMAP_UI_RECENTSEARCH_MAX
#endif

#ifndef TMAP_RECENTDEST_MAX
#define TMAP_RECENTDEST_MAX TMAP_UI_RECENTDESTINATION_MAX // 250: UX 1.1
#endif

#ifndef TMAP_FAVORITES_MAX
#define TMAP_FAVORITES_MAX TMAP_UI_FAVORITE_MAX
#endif

using namespace SKT;
using namespace SKT::orm;

NavibookmarkAgentProxy::NavibookmarkAgentProxy()
{

}

/*!
 * \brief NavibookmarkAgentProxy::recentDestListByHybrid
 * \details select top3 item base on hit and others base on date
 * \param maximum item's max number to select
 * \param offset begin position in list
 * \return
 */
QList<RecentDestinationDtoPtr> NavibookmarkAgentProxy::recentDestListByHybrid(int maximum, int offset)
{
	QList<RecentDestinationDtoPtr> ret;
    auto db = DatabaseHelper::instance()->connection();
    auto recentDao = orm::TOrm::instance()->dao<RecentDestinationDto>();
    QString tbl1 = recentDao->tableInfo()->tableName();
    QString tbl2 = orm::TOrm::instance()->dao<FavoriteDto>()->tableInfo()->tableName();

    // retrieve top 3 by hit order
    QString sql(QStringLiteral("SELECT *, (SELECT destType FROM %1 WHERE mapX=t1.mapX AND mapY=t1.mapY AND destType>0) AS [favoType] FROM %2 t1 ORDER BY %3 desc"));

    QString sql1(sql.arg(tbl2).arg(tbl1).arg(QStringLiteral("hit")));

    QString sqlTop3(sql1);
    sqlTop3.append(QStringLiteral(" LIMIT 3"));

    TQuery* q0 = new TQuery(db,sqlTop3);
    q0->exec();
    auto q1 = t_new_shared<TTypedQuery<RecentDestinationDto>>(TQueryPtr(q0));

    while (q1->next())
    {
    	auto dto = q1->record();
    	// check favorite type if home or office  2017.7.26
    	QVariant favoType = dto->property("favoType");
    	if (!favoType.isNull())
    		 dto->setDestType(favoType.toInt());
    	else
    		dto->setDestType(0);

    	ret.append(dto);
    }

    // retrieve other still left
    QString sql2(sql.arg(tbl2).arg(tbl1).arg(QStringLiteral("id")));
    if (maximum > 0)
    {
    	sql2.append(QStringLiteral(" LIMIT %1,%2").arg(offset).arg(maximum));
    }
    TQuery* q2 = new TQuery(db,sql2);
    q2->exec();
    auto q3 = t_new_shared<TTypedQuery<RecentDestinationDto>>(TQueryPtr(q2));
    while (q3->next())
    {
    	auto d1 = q3->record();
    	auto it = std::find_if(ret.cbegin(),ret.cend(),[&](RecentDestinationDtoPtr d2) -> bool {
    		return ( d1->id() == d2->id() );
    	});
    	if (it == ret.cend())
    	{
        	// check favorite type if home or office  2017.7.26
        	QVariant favoType = d1->property("favoType");
        	if (!favoType.isNull())
        		 d1->setDestType(favoType.toInt());
            else
                d1->setDestType(0);
    		ret.append(d1);
    	}
    }

    db.close();

    return ret;
}

/*!
 * \brief NavibookmarkAgentProxy::recentDestListBySortType
 * \param maximum item's max number to select
 * \param offset begin position in list
 * \param sortType 0: by date 1: hit
 * \return
 */
QList<RecentDestinationDtoPtr> NavibookmarkAgentProxy::recentDestListBySortType(
		int maximum,
		int offset,
		int sortType)
{
	QList<RecentDestinationDtoPtr> ret;
    auto db = DatabaseHelper::instance()->connection();
    auto recentDao = orm::TOrm::instance()->dao<RecentDestinationDto>();
    auto tbl = recentDao->tableInfo()->tableName();
    QString SORTTYPES[2] = { QStringLiteral("id"), QStringLiteral("hit") };
    QString sql(QStringLiteral("SELECT * FROM %1 ORDER BY %2 desc").arg(tbl).arg(SORTTYPES[sortType%2]));

    // retrieve other still left
        QString sql2(sql);
        if (maximum > 0) {
        	sql2.append(QStringLiteral(" LIMIT %1,%2").arg(offset).arg(maximum));
        }
        TQuery* q2 = new TQuery(db,sql2);
        q2->exec();
        auto q3 = t_new_shared<TTypedQuery<RecentDestinationDto>>(TQueryPtr(q2));
        while (q3->next())
        {
        	auto d1 = q3->record();
       		ret.append(d1);
        }

        db.close();

        return ret;
}


uint NavibookmarkAgentProxy::addRecentDestination(const RecentDestinationDto& newDto)
{
    uint newRowId = 0;
	auto recentDao = orm::TOrm::instance()->dao<RecentDestinationDto>();
    QString dest = newDto.dest().toString();
    QString addr = newDto.address().toString();
    int mapX     = newDto.mapX().toInt();
    int mapY     = newDto.mapY().toInt();
    int destType = newDto.destType().toInt();
    int poiId    = newDto.poiID().toInt();
    int navSeq   = newDto.navSeq().toInt();
    int rpFlag   = newDto.rpFlag().toInt();
    int opiPoleCate = newDto.opiPoleCate().toInt();
    int opiPrice    = newDto.opiPrice().toInt();

	// check duplication
	QVariantMap m;
	m.insert(QStringLiteral("mapX"),newDto.mapX());
	m.insert(QStringLiteral("mapY"),newDto.mapY());
    m.insert(QStringLiteral("poiID"),newDto.poiID()); // count as another if poiID is different
    auto cons = DatabaseHelper::instance()->connectableConnections();
    foreach (auto db, cons) {
        RecentDestinationDto dto;
        bool exist = recentDao->find(db, m, &dto);
        if (!exist)
        {
            uint total = getTableItemCount<RecentDestinationDto>(db);
            if (total >= TMAP_RECENTDEST_MAX)
                deleteOldItems<RecentDestinationDto>(TMAP_RECENTDEST_MAX-1);

            auto ib = recentDao->insertBuilder(true);
            RecentDestinationDtoPtr pDto = t_new_shared<RecentDestinationDto>();
            pDto->setDest(dest);
            pDto->setAddress(addr);
            pDto->setMapX(mapX);
            pDto->setMapY(mapY);
            pDto->setDestType(destType);
            pDto->setPoiID(poiId);
            pDto->setNavSeq(navSeq);
            pDto->setRpFlag(rpFlag);
            pDto->setOpiPoleCate(opiPoleCate);
            pDto->setOpiPrice(opiPrice);

            recentDao->insert(db,*pDto);
            dto.setId(pDto->id());
            newRowId = pDto->id().toUInt();
        }
        else
        {
            auto id = dto.id().toUInt();
            auto hit = recentDao->selectSingle(db,"hit",QString("id=%1").arg(id)).toInt();
            updateRecentDestHit(id, hit+1);
            // 2019.07.22 update id to place on top by descent, fix IR 570120 TMCRSM-3504
            auto tbl = recentDao->tableInfo()->tableName();
            QString sqlLatest(QStringLiteral("UPDATE %1 SET id=(SELECT max(id) FROM %2)+1 WHERE id=%3"));
            QString sql=sqlLatest.arg(tbl).arg(tbl).arg(id);
            TQueryPtr q0(new TQuery(db,sql));
            q0->exec();
        }

        // check if home or office is favorite and modify destType
#if 0 // 2019-12-08 Donot use longer hasFavorite to find home or office, hasFavorite should be used only for general favorites
        uint favId = hasFavorite(db, mapX,mapY,poiId);
        if (favId != 0)
        {
            int  favtype = getFavoriteType(db,favId);
            auto id = dto.id().toUInt();
            updateRecentDestType(id,favtype);
        }
#else
        uint id = dto.id().toUInt();
        FavoriteDtoPtr home   = getHomeFavorite();
        FavoriteDtoPtr office = getOfficeFavorite();
        if (!home.isNull() && home->poiID().toInt() == poiId) {
            if ( home->mapX().toInt() == mapX &&
                 home->mapY().toInt() == mapY)
                updateRecentDestType(id, 2);
        }
       else if (!office.isNull() && office->poiID().toInt() == poiId) {
            if ( office->mapX().toInt() == mapX &&
                     office->mapY().toInt() == mapY)
                    updateRecentDestType(id, 1);
        }
       else {
       }
#endif

        db.close();
    }

    return newRowId;
}

uint NavibookmarkAgentProxy::addRecentDestination(const QList<RecentDestinationDtoPtr>& destList)
{
    auto recentDao = orm::TOrm::instance()->dao<RecentDestinationDto>();
    recentDao->insertBuilder(true);
    auto cons = DatabaseHelper::instance()->connectableConnections();
    foreach (auto db, cons) {
        foreach (auto dto, destList) {
            recentDao->insert(db,*dto);
        }
    }

    return destList.size();
}

QList<RecentDestinationDtoPtr> NavibookmarkAgentProxy::deleteRecentDest(const QList<uint>& idList)
{
    QList<RecentDestinationDtoPtr> localDeletedDests;
	auto dao = orm::TOrm::instance()->dao<RecentDestinationDto>();
    auto recentdestOfid = [&dao](const QSqlDatabase& db, uint id) -> RecentDestinationDtoPtr {
        RecentDestinationDtoPtr ret;
        auto s = dao->selectBuilder();
        s->where(QStringLiteral("id == %1").arg(id));
        auto q = dao->exec(db,s);
        if (q->next())
            ret = q->record();

        return ret;
    };

    auto cons = DatabaseHelper::instance()->connectableConnections();
    foreach (auto db, cons) {
        std::for_each(idList.cbegin(),idList.cend(),[&](uint id){
            RecentDestinationDtoPtr rd = recentdestOfid(db, id);
            if (!rd.isNull()) {
                dao->deleteById(db,id);
               localDeletedDests.append(rd);
            }
        });
        db.close();
    }

  return localDeletedDests;
}

void NavibookmarkAgentProxy::clearRecentDestination()
{
    auto cons = DatabaseHelper::instance()->connectableConnections();
    foreach (auto db, cons) {
        auto dao = orm::TOrm::instance()->dao<RecentDestinationDto>();
        auto bld = dao->deleteBuilder();
        dao->exec(db,bld);
        db.close();
    }
}

void NavibookmarkAgentProxy::updateRecentDestHit(uint id, uint hit)
{
	updateFieldById<RecentDestinationDto>(QStringLiteral("hit"),hit,id);
}

void NavibookmarkAgentProxy::updateRecentDestType(uint id, int destType)
{
	updateFieldById<RecentDestinationDto>(QStringLiteral("destType"),destType,id);
}

QList<RecentDestinationDtoPtr> NavibookmarkAgentProxy::searchRecentDestination(const QString& name)
{
    return searchRecentDestination(name, 20);
}

QList<RecentDestinationDtoPtr> NavibookmarkAgentProxy::searchRecentDestination(const QString& name, int limit)
{
    QString requestWords=name.trimmed();
    QString condition1,condition2;

    QStringList likeConditions;
    likeConditions << requestWords;
    QStringList words = requestWords.split(' ',QString::SkipEmptyParts);
        // consider matching algorithm if condition1 is like and the request has a few words
    if (words.size() > 1) {
            likeConditions << words.at(1)+char(' ')+words.at(0); // swap first and second with space
            likeConditions << words.at(0)+words.at(1); // trim space between first and second 
            likeConditions << words.at(1)+words.at(0); // swap first and second without space
    }

    const int cnt = likeConditions.size();
    for (int i=0;i<cnt;i++) {
        condition1.append(QStringLiteral("dest LIKE '%1%'").arg(likeConditions.at(i)));
        condition2.append(QStringLiteral("address LIKE '%1%'").arg(likeConditions.at(i)));
        if (i < cnt-1) {
         condition1.append(QStringLiteral(" OR "));   
         condition2.append(QStringLiteral(" OR "));   
        }
    }


    QString sql1(QStringLiteral("SELECT * FROM recent_dest WHERE %1 ORDER BY id DESC LIMIT %2")
                .arg(condition1)
                .arg(limit)
                );
    QString sql2(QStringLiteral("SELECT * FROM recent_dest WHERE %1 ORDER BY id DESC LIMIT %2")
                .arg(condition2)
                .arg(limit)
                );

    QList<RecentDestinationDtoPtr> ret;
    auto db = DatabaseHelper::instance()->connection();

    TQuery* q1 = new TQuery(db,sql1);
    q1->exec();
    auto qry1 = t_new_shared<TTypedQuery<RecentDestinationDto>>(TQueryPtr(q1));

    while (qry1->next()) {
        auto dto = qry1->record();
        ret.append(dto);
    }

    TQuery* q2 = new TQuery(db,sql2);
    q2->exec();

    auto qry2 = t_new_shared<TTypedQuery<RecentDestinationDto>>(TQueryPtr(q2));
    while (qry2->next()) {
        auto dto = qry2->record();
        auto it = std::find_if(ret.cbegin(),ret.cend(),[&](RecentDestinationDtoPtr d)->bool {
            return ( dto->id() == d->id() );
        });
        // add the item if there is not
        if (it == ret.cend()) {
            ret.append(dto);
        }
    }

    return ret;
}


/*!
 * \brief NavibookmarkAgentProxy::favoriteList
 * \param maximum max number to select items once
 * \param offset  begin position in list
 * \param sortType 0: by date 1: by alias
 * \return list selected
 */
QList<FavoriteDtoPtr> NavibookmarkAgentProxy::favoriteList(int maximum, int offset, int sortType)
{
	QList<FavoriteDtoPtr> ret;
    auto db = DatabaseHelper::instance()->connection();
    auto dao = orm::TOrm::instance()->dao<FavoriteDto>();
    // retrieve home, office, normal
    TSelectBuilderPtr s = dao->selectBuilder();
    s->where(QStringLiteral("destType > 0"));
    s->orderBy(QStringLiteral("destType DESC"));
    // s->limitN(maximum);
    // s->limitOffset(offset);  // should take constantly home,office on top by lckxx 2017.11.24
    auto q1 = dao->exec(db,s); // retrieve home or office
    while (q1->next())
    	ret.append(q1->record());

    TSelectBuilderPtr s2 = dao->selectBuilder();
    if (sortType == 0)
        s2->orderBy(QStringLiteral("id DESC"));
    else
    	s2->orderBy(QStringLiteral("alias ASC"));

    s2->where(QStringLiteral("destType == 0"));

    s2->limitN(maximum);
    s2->limitOffset(offset);

    auto q2 = dao->exec(db,s2);
    while (q2->next())
    	ret.append(q2->record());

    db.close();

	return ret;
}

/*!
 * \brief NavibookmarkAgentProxy::addFavorite
 * \param dest poi name
 * \param address poi address
 * \param mapX x coordination in mapview
 * \param mapY y coordination in mapview
 * \param destType 0 ~ 2, 0: normal, 1: office 2: home
 */
uint NavibookmarkAgentProxy::addFavorite(const FavoriteDto& newDto, bool silence)
{
    uint newRowId = 0;
//	auto db = DatabaseHelper::instance()->connection();
	auto dao = orm::TOrm::instance()->dao<FavoriteDto>();


    QString dest = newDto.dest().toString();
    QString addr = newDto.address().toString();
    int mapX     = newDto.mapX().toInt();
    int mapY     = newDto.mapY().toInt();
    int destType = newDto.destType().toInt();
    int poiId    = newDto.poiID().toInt();
    int navSeq   = newDto.navSeq().toInt();
    int rpFlag   = newDto.rpFlag().toInt();
    int opiPoleCate = newDto.opiPoleCate().toInt();
    int opiPrice    = newDto.opiPrice().toInt();
    QString insTime = newDto.insTime().toString();

    auto homeAndOfficeCount = 0;

    auto cons = DatabaseHelper::instance()->connectableConnections();
    foreach (auto db, cons) {
        // delete old one if destType is home or office
        if (destType != 0)
        {
            TDeleteBuilderPtr del = dao->deleteBuilder();
            del->where(QString(QStringLiteral("destType=%1")).arg(destType));
            dao->exec(db,del);
        } else {
            auto home = getHomeFavorite(db);
            if (home != nullptr) {
                ++homeAndOfficeCount;
            }
            auto office = getOfficeFavorite(db);
            if (office != nullptr) {
                ++homeAndOfficeCount;
            }
            uint total = getTableItemCount<FavoriteDto>(db);

            // TMCRSM-3515
            // 19.07.25 ymhong.
            const uint TMAP_FAVORITES_NORMAL_MAX = TMAP_FAVORITES_MAX + homeAndOfficeCount;

            if (total >= TMAP_FAVORITES_NORMAL_MAX && silence==false) {
                db.close();
                QVariantMap m;
                m[QStringLiteral("itemType")] = QStringLiteral("favorite");
                m[QStringLiteral("code")] = 1;
                sendNotification(SearchConstants::AddItemFailure,QVariant::fromValue(m));
                return 0;
            }
        }

        bool isPoi = isPoiBookmark(destType);
        int destTypeForDb = isPoi ? 0 : destType;

        // check duplication
        QVariantMap m;
        m.insert(QStringLiteral("mapX"),mapX);
        m.insert(QStringLiteral("mapY"),mapY);
        m.insert(QStringLiteral("destType"),destTypeForDb);

        FavoriteDto dto;
        bool exist = dao->find(db, m, &dto);
        if (!exist)
        {
            auto ib = dao->insertBuilder(true);

            FavoriteDtoPtr pDto = t_new_shared<FavoriteDto>();
            pDto->setDest(dest);
            pDto->setAddress(addr);
            pDto->setAlias(dest); // first let do same to 'dest'
            pDto->setMapX(mapX);
            pDto->setMapY(mapY);
            pDto->setDestType(destTypeForDb);
            pDto->setPoiID(poiId);
            pDto->setNavSeq(navSeq);
            pDto->setRpFlag(rpFlag);
            pDto->setInsTime(insTime);
            pDto->setOpiPoleCate(opiPoleCate);
            pDto->setOpiPrice(opiPrice);

            dao->insert(db,*pDto);
            newRowId = pDto->id().toUInt();

            if (silence == false) {
                QVariantMap m;
                m[QStringLiteral("itemType")] = QStringLiteral("favorite");
                m[QStringLiteral("destType")] = destType;
                m[QStringLiteral("x")] = mapX;
                m[QStringLiteral("y")] = mapY;
                m[CommonConstants::Caller] = mCaller;
                sendNotification(SearchConstants::AddItemOk,QVariant::fromValue(m));
            }
        }
        else
        {
            // check if the favorite is home or office 2017.7.26
            if (destTypeForDb != 0 )
            {
                // this codes should not be executed
                updateFavoriteType(dto.id().toUInt(), destType);
            }
            else
            {
                if (silence==false) {
                    QVariantMap m;
                    m[QStringLiteral("itemType")] = QStringLiteral("favorite");
                    m[QStringLiteral("code")] = 2;
                    sendNotification(SearchConstants::AddItemFailure,QVariant::fromValue(m));
                }
            }
        }

        db.close();
    }
    return newRowId;
}

uint NavibookmarkAgentProxy::addFavorite(const QList<FavoriteDtoPtr>& favList)
{
    auto dao = orm::TOrm::instance()->dao<FavoriteDto>();
    dao->insertBuilder(true);
    auto cons = DatabaseHelper::instance()->connectableConnections();
    foreach (auto db, cons) {
        foreach (auto dto, favList) {
            dao->insert(db,*dto);
        }
    }
    return favList.size();
}


void NavibookmarkAgentProxy::updateFavoriteAlias(uint id, const QString alias)
{
	updateFieldById<FavoriteDto>(QStringLiteral("alias"),alias,id);
}

/*!
 * \brief NavibookmarkAgentProxy::updateFavoriteType
 * \details modify the item's dest type
 * \param id item's id to update
 * \param destType type being modified
 */
void NavibookmarkAgentProxy::updateFavoriteType(uint id, int destType)
{
	updateFieldById<FavoriteDto>(QStringLiteral("destType"),destType,id);
}

int  NavibookmarkAgentProxy::getFavoriteType(QSqlDatabase db, uint id)
{
	int type=0;
	QList<FavoriteDtoPtr> ret;
    auto dao = orm::TOrm::instance()->dao<FavoriteDto>();
    auto dto = dao->selectById(db,id);
    if (!dto.isNull())
    	type = dto->destType().toInt();

    return type;
}

QList<FavoriteDtoPtr>  NavibookmarkAgentProxy::deleteFavorites(const QList<uint>& idList)
{
    QList<FavoriteDtoPtr> localDeletedFavs;

	auto dao = orm::TOrm::instance()->dao<FavoriteDto>();

    auto favoriteOfid = [&](const QSqlDatabase& db, uint id) -> FavoriteDtoPtr {
        FavoriteDtoPtr ret;
        auto s = dao->selectBuilder();
        s->where(QStringLiteral("id == %1").arg(id));
        auto q = dao->exec(db,s);
        if (q->next())
            ret = q->record();

        return ret;
    };

    auto cons = DatabaseHelper::instance()->connectableConnections();
    foreach (auto db, cons) {
        std::for_each(idList.cbegin(),idList.cend(),[&](uint id){
            FavoriteDtoPtr fa = favoriteOfid(db,id);
            if (!fa.isNull()) {
                dao->deleteById(db,id);
                localDeletedFavs.append(fa);
            }
        });
        db.close();
    }

  return localDeletedFavs;
}

FavoriteDtoPtr NavibookmarkAgentProxy::deleteFavorites(int mapX, int mapY, int destType)
{
//	auto db = DatabaseHelper::instance()->connection();
    auto cons = DatabaseHelper::instance()->connectableConnections();
    auto dao = orm::TOrm::instance()->dao<FavoriteDto>();
    FavoriteDtoPtr localDeletedFav;
    auto s = dao->selectBuilder();
    QString sql(QStringLiteral("mapX=%1 AND mapY=%2 AND destType=%3").arg(mapX).arg(mapY).arg(destType));
    s->where(sql); // fixed TMCRSM-4086 -> notice! it could deletes both the favorite and home or office if there are at x,y
    foreach (auto db, cons) {
        auto q = dao->exec(db,s);
        if (q->next())
            localDeletedFav = q->record();

        TDeleteBuilderPtr del = dao->deleteBuilder();
        del->where(sql);
        dao->exec(db,del);

        db.close();
    }

    return localDeletedFav;
}

/*!
 * \brief NavibookmarkAgentProxy::clearFavorites
 */
void NavibookmarkAgentProxy::clearFavorites()
{
    auto cons = DatabaseHelper::instance()->connectableConnections();
    foreach (auto db, cons) {
        auto dao = orm::TOrm::instance()->dao<FavoriteDto>();
        auto bld = dao->deleteBuilder();
        dao->exec(db,bld);

        db.close();
    }
}


uint NavibookmarkAgentProxy::hasFavorite(int mapX, int mapY, int poiId)
{
    return hasFavorite(DatabaseHelper::instance()->connection(), mapX, mapY, poiId);
}

/*!
* determine if the favorite is found for mapX,mapY specified 
* @return favorite's row id 
* @param mapX map coord x
* @param mapY map coord y
*/
uint NavibookmarkAgentProxy::hasFavorite(const QSqlDatabase& db, int mapX, int mapY, int poiId)
{
	auto dao = orm::TOrm::instance()->dao<FavoriteDto>();

#if (0)
    if (poiId < 0) {
        poiId = 0;
    }
#endif

	QVariantMap m;
	m.insert(QStringLiteral("mapX"),mapX);
	m.insert(QStringLiteral("mapY"),mapY);
    m.insert(QStringLiteral("poiID"), poiId);
    m.insert(QStringLiteral("destType"), 0);

	FavoriteDto dto;
    bool exist = dao->find(db, m, &dto);

    uint foundId = 0;
    if (exist)
    	foundId = dto.id().toUInt();

    return foundId;
}

FavoriteDtoPtr NavibookmarkAgentProxy::getHomeFavorite()
{
    return getHomeFavorite(DatabaseHelper::instance()->connection());
}

FavoriteDtoPtr NavibookmarkAgentProxy::getOfficeFavorite()
{
    return getOfficeFavorite(DatabaseHelper::instance()->connection());
}

FavoriteDtoPtr NavibookmarkAgentProxy::getHomeFavorite(const QSqlDatabase& db)
{
    FavoriteDtoPtr ret;
    auto dao = orm::TOrm::instance()->dao<FavoriteDto>();
    // retrieve home, office, normal
    TSelectBuilderPtr s = dao->selectBuilder();
    s->where(QStringLiteral("destType == 2"));

    auto q = dao->exec(db,s);
    if (q->next())
        ret = q->record();

    return ret;
}


FavoriteDtoPtr NavibookmarkAgentProxy::getOfficeFavorite(const QSqlDatabase& db)
{
    FavoriteDtoPtr ret;
    auto dao = orm::TOrm::instance()->dao<FavoriteDto>();
    // retrieve home, office, normal
    TSelectBuilderPtr s = dao->selectBuilder();
    s->where(QStringLiteral("destType == 1"));

    auto q = dao->exec(db,s);
    if (q->next())
        ret = q->record();

    return ret;
}


FavoriteDtoPtr NavibookmarkAgentProxy::getFavoriteById(uint id)
{
    FavoriteDtoPtr ret;
    auto db = DatabaseHelper::instance()->connection();
    auto dao = orm::TOrm::instance()->dao<FavoriteDto>();
    // retrieve home, office, normal
    TSelectBuilderPtr s = dao->selectBuilder();
    s->where(QStringLiteral("id == %1 AND destType == 0").arg(id));

    auto q = dao->exec(db,s);
    if (q->next())
        ret = q->record();

    db.close();
    return ret;
}

FavoriteDtoPtr NavibookmarkAgentProxy::getFavoriteByPos(int x, int y)
{
    FavoriteDtoPtr ret;
    auto db = DatabaseHelper::instance()->connection();
    auto dao = orm::TOrm::instance()->dao<FavoriteDto>();
    // retrieve home, office, normal
    TSelectBuilderPtr s = dao->selectBuilder();
    s->where(QStringLiteral("destType == 0 AND mapX=%1 AND mapY=%2").arg(x).arg(y));

    auto q = dao->exec(db,s);
    if (q->next())
        ret = q->record();

    db.close();
    return ret;
}

QList<FavoriteDtoPtr> NavibookmarkAgentProxy::searchFavorite(const QString& name)
{
    return searchFavorite(name, 20);
}

QList<FavoriteDtoPtr> NavibookmarkAgentProxy::searchFavorite(const QString& name, int limit)
{
    QString requestWords=name.trimmed();
    QString condition1,condition2;

    QStringList likeConditions;
    likeConditions << requestWords;
    QStringList words = requestWords.split(' ',QString::SkipEmptyParts);
        // consider matching algorithm if condition1 is like and the request has a few words
    if (words.size() > 1) {
            likeConditions << words.at(1)+char(' ')+words.at(0); // swap first and second with space
            likeConditions << words.at(0)+words.at(1); // trim space between first and second 
            likeConditions << words.at(1)+words.at(0); // swap first and second without space
    }

    const int cnt = likeConditions.size();
    for (int i=0;i<cnt;i++) {
        condition1.append(QStringLiteral("dest LIKE '%1%'").arg(likeConditions.at(i)));
        condition2.append(QStringLiteral("address LIKE '%1%'").arg(likeConditions.at(i)));
        if (i < cnt-1) {
         condition1.append(QStringLiteral(" OR "));   
         condition2.append(QStringLiteral(" OR "));   
        }
    }


    QString sql1(QStringLiteral("SELECT * FROM favorites WHERE %1 ORDER BY id DESC LIMIT %2")
                .arg(condition1)
                .arg(limit)
                );
    QString sql2(QStringLiteral("SELECT * FROM favorites WHERE %1 ORDER BY id DESC LIMIT %2")
                .arg(condition2)
                .arg(limit)
                );

    QList<FavoriteDtoPtr> ret;
    auto db = DatabaseHelper::instance()->connection();

    TQuery* q1 = new TQuery(db,sql1);
    q1->exec();
    auto qry1 = t_new_shared<TTypedQuery<FavoriteDto>>(TQueryPtr(q1));

    while (qry1->next()) {
        auto dto = qry1->record();
        ret.append(dto);
    }

    TQuery* q2 = new TQuery(db,sql2);
    q2->exec();

    auto qry2 = t_new_shared<TTypedQuery<FavoriteDto>>(TQueryPtr(q2));
    while (qry2->next()) {
        auto dto = qry2->record();
        auto it = std::find_if(ret.cbegin(),ret.cend(),[&](FavoriteDtoPtr d) -> bool {
            return ( dto->id() == d->id() );
        });
        // add the item if there is not
        if (it == ret.cend()) {
            ret.append(dto);
        }
    }

    return ret;
}


QList<RecentSearchDtoPtr> NavibookmarkAgentProxy::recentSearchList(int maximum, int offset)
{
	QList<RecentSearchDtoPtr> ret;
	auto db  = DatabaseHelper::instance()->connection();
	auto dao = orm::TOrm::instance()->dao<RecentSearchDto>();
	TSelectBuilderPtr sb = dao->selectBuilder();
    if (maximum > 0)
    {
    	sb->limitN(maximum);
    	sb->limitOffset(offset);
    }
    sb->orderBy(QStringLiteral("accessTime DESC")); // fixed TMCRSM-2118
	auto qry = dao->exec(db,sb);
	while (qry->next())
	{
		auto r = qry->record();
		ret.append(r);
	}

	db.close();

	return ret;
}

void NavibookmarkAgentProxy::addRecentSearch(const QString& keyword)
{
	// check duplication
	QVariantMap m;
	RecentSearchDto dto;
	m.insert(QStringLiteral("keyword"),keyword);

    auto cons = DatabaseHelper::instance()->connectableConnections();
    auto dao = orm::TOrm::instance()->dao<RecentSearchDto>();
    foreach (auto db, cons) {
        bool exist = dao->find(db,m,&dto);
        QString now = QDateTime::currentDateTime().toString("yyyyMMdd hh:mm:ss");

        if (!exist) // add new recent search
        {
            // truncate db size
            uint total = getTableItemCount<RecentSearchDto>(db);
            if (total >= TMAP_RECENTSEARCH_MAX)
                deleteOldItems<RecentSearchDto>(TMAP_RECENTSEARCH_MAX-1);

            auto ib = dao->insertBuilder(true);
            RecentSearchDtoPtr dt = t_new_shared<RecentSearchDto>();
            dt->setKeyword(keyword);
            dt->setAccessTime(QVariant::fromValue(now));
            dao->insert(db,*dt);
        }
        else // update access time
        {
            QList<TFieldInfoPtr> fields;
            auto tbinfo = dao->tableInfo();
            foreach (TFieldInfoPtr fi, tbinfo->fieldInfos() )
            {
                if (fi->name() == "accessTime") {
                    fields.append(fi);
                    break;
                }
            }

            RecentSearchDtoPtr dt = t_new_shared<RecentSearchDto>();
            dt->setId(dto.id());
            dt->setAccessTime(now);
            dao->updateById(db,*dt,fields);
        }
        //	db.close();
    }
}


void NavibookmarkAgentProxy::deleteRecentSearch(const QList<uint>& idList)
{
    auto cons = DatabaseHelper::instance()->connectableConnections();
    foreach (auto db, cons) {
        auto dao = orm::TOrm::instance()->dao<RecentSearchDto>();
        std::for_each(idList.cbegin(),idList.cend(),[&](uint id){
            dao->deleteById(db,id);
        });
    }

//	db.close();
}

void NavibookmarkAgentProxy::clearRecentSearch()
{
    auto dao = orm::TOrm::instance()->dao<RecentSearchDto>();
    auto cons = DatabaseHelper::instance()->connectableConnections();
    foreach (auto db, cons) {
        auto bld = dao->deleteBuilder();
        dao->exec(db,bld);
        db.close();
    }
}

void NavibookmarkAgentProxy::setCaller(const QString &caller)
{
    mCaller = caller;
}

bool NavibookmarkAgentProxy::isHome(int destType) const
{
    return destType == 2;
}

bool NavibookmarkAgentProxy::isOffice(int destType) const
{
    return destType == 1;
}

bool NavibookmarkAgentProxy::isNoramlBookmark(int destType) const
{
    return !isHome(destType) && !isOffice(destType);
}

bool NavibookmarkAgentProxy::isPoiBookmark(int destType) const
{
    return destType == 3;
}

#define MAPVIEW_DEFAULT_VIEWLEVEL           10  // defined in MapViewQuick.cpp

LastRouteDtoPtr NavibookmarkAgentProxy::lastRouteInfo()
{
    LastRouteDtoPtr lastRoute;
    auto db   = DatabaseHelper::instance()->connection();
    auto dao  = orm::TOrm::instance()->dao<LastRouteDto>();
    TSelectBuilderPtr sb = dao->selectBuilder();
    auto qry = dao->exec(db,sb);
    if (qry->next()) {
        lastRoute = qry->record();
    } else {
        lastRoute = t_new_shared<LastRouteDto>();
        lastRoute->setIsNormallyTerninated(true);
        lastRoute->setSelectedRouteIndex(0);
        lastRoute->setSelectedRouteOption(1);
        lastRoute->setMapLevel(MAPVIEW_DEFAULT_VIEWLEVEL);
        lastRoute->setTpX(0);
        lastRoute->setTpY(0);
        lastRoute->setTpTime(QString{});
        lastRoute->setSessionId(QString{});
        lastRoute->setAddress(QString{});
        lastRoute->setVia(QString{});
    }

    db.close();

    return lastRoute;
}

void NavibookmarkAgentProxy::setLastRouteInfo(
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
                              )
{
    LastRouteDtoPtr  dto = t_new_shared<LastRouteDto>();
    dto->setIsNormallyTerninated(isNormallyTerminated);
    dto->setSelectedRouteIndex(selectedRouteIndex);
    dto->setSelectedRouteOption(selectedRouteOption);
    dto->setMapLevel(mapLevel);
    dto->setTpX(tpX);
    dto->setTpY(tpY);
    dto->setTpTime(tpTime);
    dto->setSessionId(sessionId);
    dto->setAddress(address);
    dto->setVia(via);

    setLastRouteInfo(dto);
}

void NavibookmarkAgentProxy::setLastRouteInfo(LastRouteDtoPtr dto)
{
    auto db  = DatabaseHelper::instance()->connection();
    auto dao = orm::TOrm::instance()->dao<LastRouteDto>();

    // check if it exist
    qlonglong tpX = dto->tpX().toLongLong();
    qlonglong tpY = dto->tpY().toLongLong();
    QVariantMap m;
    m.insert(QStringLiteral("tpX"),tpX);
    m.insert(QStringLiteral("tpY"), tpY);
    LastRouteDto findDto;
    bool exist = dao->find(db,m,&findDto);
    // update
    if (exist) {
        TUpdateBuilderPtr up = dao->updateBuilder();
        up->where(QString(QStringLiteral("tpX=%1 and tpY=%2")).arg(tpX).arg(tpY));
        QSqlQuery q(db);
        q.prepare(up->build());
        q.bindValue(0,dto->isNormallyTerminated());
        q.bindValue(1,dto->selectedRouteIndex());
        q.bindValue(2,dto->selectedRouteOption());
        q.bindValue(3,dto->mapLevel());
        q.bindValue(4,dto->tpX());
        q.bindValue(5,dto->tpY());
        q.bindValue(6,dto->tpTime());
        q.bindValue(7,dto->sessionId());
        q.bindValue(8,dto->address());
        q.bindValue(9,dto->via());

        if (!q.exec())
        {
            T_DEBUG("sql failed to update last route:["+QString("%1").arg(q.lastError().type())+"]"+q.lastError().text());
        }
        q.clear();
        q.finish();
    }
    else {
        dao->insert(db, *dto);
    }
    db.close();
}

void NavibookmarkAgentProxy::resetLastRouteInfo()
{
    auto db = DatabaseHelper::instance()->connection();
    auto dao = orm::TOrm::instance()->dao<LastRouteDto>();
    auto bld = dao->deleteBuilder();
    dao->exec(db,bld);

    db.close();
}

/*!
 * \brief NavibookmarkAgentProxy::UpdateFieldById<T>
 * \details common template to update the field specified with fieldName by id
 */
template<class dtoType>
uint NavibookmarkAgentProxy::updateFieldById(const QString& fieldName, const QVariant& value, uint id)
{
    auto dao = orm::TOrm::instance()->dao<dtoType>();
	auto tbinfo = dao->tableInfo();

	QList<TFieldInfoPtr> fields;

	foreach (TFieldInfoPtr fi, tbinfo->fieldInfos() )
	{
		if (fi->name() == fieldName) {
			fields.append(fi);
			break;
		}
	}

	auto dto = t_new_shared<dtoType>();
	QObject* ob = reinterpret_cast<QObject*>(dto.data());
	ob->setProperty(tbinfo->idField()->name().toLatin1().constData(),id);
	ob->setProperty(fieldName.toLatin1().constData(),value);

    auto cons = DatabaseHelper::instance()->connectableConnections();
    foreach (auto db, cons) {
        dao->updateById(db,*dto,fields);
//        db.close(); // 2020-1-9 fixed TMCRSM-4473, DONOT close, the db might be sharing on other stuff like the caller.
    }

	return 0;
}

template<class dtoType>
uint NavibookmarkAgentProxy::deleteOldItems(int maxx)
{
    auto dao = orm::TOrm::instance()->dao<dtoType>();
    auto tbinfo = dao->tableInfo();

    const QString table = tbinfo->tableName();
    auto cons = DatabaseHelper::instance()->connectableConnections();
    int cnt = 0;
    foreach (auto db, cons) {
        cnt = getTableItemCount<dtoType>(db) - maxx;
        if (cnt > 0) {
            QString sql(QStringLiteral("DELETE FROM %1 WHERE id < (SELECT id FROM %2 ORDER BY id ASC LIMIT %3,1)").arg(table).arg(table).arg(cnt));
            TQuery qry ( db, sql);
            qry.exec();
        }
    }

    return cnt;
}


template<class dtoType>
uint NavibookmarkAgentProxy::getTableItemCount(const QSqlDatabase& db) const
{
    uint total=0;
    auto dao = orm::TOrm::instance()->dao<dtoType>();

    total  = dao->selectSingle(db, QStringLiteral("count(id)"), QString()).toUInt();

    return total;
}

void NavibookmarkAgentProxy::onRegister()
{
    // migration RecentDestinationDto with adding new poiID field
    #ifdef TMAP_RSM_UI_ENABLE_DB_MIGRATION
        //  get migration for another user profile 
        for (int i=0;i<6;i++) {
            QString profilePath = DatabaseHelper::instance()->getUserDBFilePath(i);
            if (QFile::exists(profilePath)) {
                auto db = DatabaseHelper::instance()->connection(i);
                DatabaseMigration mg(db);
                mg.migrateTable(DatabaseHelper::instance()->getDriver(),RecentDestinationDto::staticMetaObject);
                mg.migrateTable(DatabaseHelper::instance()->getDriver(), FavoriteDto::staticMetaObject);
            }
        }
    #endif

    auto db = DatabaseHelper::instance()->connection();
    orm::TOrm::instance()->create<RecentDestinationDto>( db );
    orm::TOrm::instance()->create<RecentSearchDto>( db );
    orm::TOrm::instance()->create<FavoriteDto>( db );
    orm::TOrm::instance()->create<LastRouteDto>( db );

    //	db.close();
}


void NavibookmarkAgentProxy::onRemove()
{
}

