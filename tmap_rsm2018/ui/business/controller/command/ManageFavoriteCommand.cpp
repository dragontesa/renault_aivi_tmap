#include "ManageFavoriteCommand.h"
#include "NavibookmarkAgentProxy.h"
#include "RouteSearchProxy.h"
#include "FavoriteDto.h"
#include "Constants.h"
#include "SearchConstants.h"
#include "ToastsConstants.h"
#include "navigationproperty.h"
#include "NavigationPropertyProtocol.h"
#include "NavigationPropertyConstants.h"
#include "NavigationControllerConstants.h"
#include "UserProfileConstants.h"
#include "ViewRegistry.h"
#include "ProfileManager.h"
#include <QtConcurrent/QtConcurrent>
#include "textdecoder.h"
#include "PlatformHelper.h"

using namespace SKT;

ManageFavoriteCommand::ManageFavoriteCommand(QObject* parent) :
		SimpleCommand(parent)
{

}

bool ManageFavoriteCommand::execute( const mvc::INotificationPtr& note )
{
	auto proxy = Facade::instance()->retrieveProxy<NavibookmarkAgentProxy>();

	switch ( note->getId() ) {
	case SearchConstants::RequestFavorites:
        {
            auto m = note->getBody().toMap();
            auto forProperty = m["isPropertyValue"].toBool();

            if (m["service.sds"].toBool() == false)
                requestFavoiteList(m, forProperty);
            else {
                mvc::ICommandPtr command = qSharedPointerDynamicCast<mvc::ICommand>(note->getDestination());
                QtConcurrent::run(this, &ManageFavoriteCommand::initNaviProperty, command, FavoriteDtoPtr());
            }
        }
	    break;

    case SearchConstants::AddFavorite:
        {
            auto data = note->getBody().value<QVariantMap>();
            auto dest = data[SearchConstants::Dest];
            auto addr = data[CommonConstants::Address];
            auto destType = data[SearchConstants::DestType];
            auto mapX    = data[CommonConstants::X];
            auto mapY    = data[CommonConstants::Y];
            auto poiId   = data[SearchConstants::PoiID];
            auto navSeq  = data[SearchConstants::NavSeq];
            auto rpFlag  = data[SearchConstants::RpFlag];
            auto insTime = data[SearchConstants::FavInsTime];
            auto alias   = data[QStringLiteral("alias")];
            auto poleCate= data[SearchConstants::OpiPoleCate];
            bool silence = data[QStringLiteral("silence")].toBool();

            FavoriteDto dto;

            if (dest.isValid())      dto.setDest(dest.toString());
            if (addr.isValid())      dto.setAddress(addr.toString());
            if (destType.isValid())  dto.setDestType(destType);
            if (mapX.isValid())      dto.setMapX(mapX.toInt());
            if (mapY.isValid())      dto.setMapY(mapY.toInt());
            if (poiId.isValid())     dto.setPoiID(poiId.toInt());
            if (navSeq.isValid())    dto.setNavSeq(navSeq.toInt());
            if (rpFlag.isValid())    dto.setRpFlag(rpFlag.toInt());
            if (insTime.isValid())   dto.setInsTime(insTime.toString());
            if (alias.isValid())     dto.setAlias(alias.toString());

            if (poleCate.isValid())  dto.setOpiPoleCate(poleCate.toInt());

            // get address for map search
            if (!addr.isValid() && dest.toString().isEmpty())
            {
                auto searchProxy = Facade::instance()->retrieveProxy<RouteSearchProxy>();
                QPoint pos = searchProxy->viaWorldPos(RouteItem_End);
                auto a = searchProxy->viaAddress(RouteItem_End);
                auto x = pos.x();
                auto y = pos.y();

                dto.setAddress(a);
                 // fixed poi loss right coordination received from server
                if (x != 0 && y != 0) {
                    dto.setMapX(x);
                    dto.setMapY(y);
                }
            }


            auto caller = data[CommonConstants::Caller].toString();
            proxy->setCaller(caller);

            uint newRowId = proxy->addFavorite(dto,silence);

            // show toast. 이동.

            // Home, Office의 경우 D-Bus로 전달.
            int dtype = dto.destType().toInt();
            if (0 != dtype) {
                QVariantMap m;
                TS_ROUTE_POS pos;
                memset(&pos, 0, sizeof(TS_ROUTE_POS));
                pos.tp.x = dto.mapX().toInt();
                pos.tp.y = dto.mapY().toInt();
                pos.nPoiID = poiId.toInt();
                pos.byRPFlag = rpFlag.toInt();
                qDebug() << "fav command: add home-office:" << dto.dest().toString();
                qstrncpy(pos.szName, TextDecoder::instance()->fromUnicode(dto.dest().toString()).constData(),TD_POSNAME_LEN);

                m["propertyValue"] = QVariant::fromValue(pos);

                if (destType == 1) {
                    m[CommonConstants::Type] = NavigationPropertyProtocol::TmapWorkLocation;
                    sendNotification(NavigationPropertyConstants::setNavigationProperty, m);

                } else {
                    m[CommonConstants::Type] = NavigationPropertyProtocol::TmapHomeLocation;
                    sendNotification(NavigationPropertyConstants::setNavigationProperty, m);
                }

               // 2019-6-13 don't sync in adding a item downloaed from server
               auto owner = note->getSource();
			   if (owner.data() != ProfileManager::instance()) {
                // notify user home/office modified 2017.10.10 (doing for server)
                QVariantMap body;
                body[CommonConstants::Type] = UserProfileConstants::RegistHomeOfficeK;
                FavoriteDtoPtr home = proxy->getHomeFavorite();
                FavoriteDtoPtr office = proxy->getOfficeFavorite();
                QList<FavoriteDtoPtr> data;
                data.append(home);
                data.append(office);
                body[QStringLiteral("data")] = QVariant::fromValue(data);
                sendNotification(NavigationControllerConstants::ModifiedUserData,body);
               }
            }
            else if (newRowId > 0 ) {
                QVariantMap body;
                QList<FavoriteDtoPtr> data = proxy->favoriteList(TMAP_UI_FAVORITE_MAX); // M1 must upload all items but not single item.
                FavoriteDto* fav  = new FavoriteDto;
                fav->setDest(dest);
                fav->setAddress(addr);
                fav->setDestType(destType);
                fav->setMapX(mapX);
                fav->setMapY(mapY);
                fav->setPoiID(poiId);
                fav->setNavSeq(navSeq);
                fav->setRpFlag(rpFlag);
                fav->setInsTime(insTime);
                fav->setAlias(alias);
                fav->setOpiPoleCate(poleCate);

                data.append(FavoriteDtoPtr(fav));

               // 2019-6-13 don't sync in adding a item downloaed from server  -> M1에 의해 이 내용은 무시한다.
               auto owner = note->getSource();
               if (owner.data() != ProfileManager::instance()) {
                   body[CommonConstants::Type] = UserProfileConstants::RegistFavoriteK;
                   body[QStringLiteral("data")] = QVariant::fromValue(data);
                   sendNotification(NavigationControllerConstants::ModifiedUserData, body);
               }
            }

            requestFavoiteListForProperty();
        }
		break;
    case SearchConstants::DeleteFavorite:
        {
            auto body = note->getBody();
            if (body.isValid())
            {
                auto m = body.toMap();
                QList<FavoriteDtoPtr> favsDeleted;
                if (m.contains("list")) {
                    QVariantList ids = m["list"].toList();
                    QList<uint> idList;
                    std::for_each(ids.cbegin(),ids.cend(),[&](const QVariant& va) {
                        idList.append(va.toUInt());
                    });
                    
                    favsDeleted = proxy->deleteFavorites(idList);

                } else if (m.contains("x") && m.contains("y")) {
                    auto x = m["x"].toInt();
                    auto y = m["y"].toInt();
                    auto desttype = m["destType"].toInt();
                    FavoriteDtoPtr fav = proxy->deleteFavorites(x, y, desttype);
                    favsDeleted.append(fav);
                }

                auto caller = m[CommonConstants::Caller].toString();
                QVariantMap body;
                body[CommonConstants::Position] = ToastsConstants::ToastUp;
                if (caller == ViewRegistry::objectName(ViewRegistry::SearchResultDetailView)) {
                    body[CommonConstants::Message] = tr("즐겨찾기가 해제되었습니다.");
                } else {
                    body[CommonConstants::Message] = tr("선택한 즐겨찾기가 삭제되었습니다.");
                }
                sendNotification(ToastsConstants::ShowToast, body);

                qDebug() << "favorite deleted !! count: " << favsDeleted.size();

                // doing for server
                bool hasHomeOffice = false;
                bool hasNormalFavorite = false;
                for (int i=0;i<favsDeleted.size();i++) {
                     // fixed TMCRSM-2412, TMCRSM-2413   BEGIN
                    int destType = favsDeleted.at(i)->destType().toInt();
                     qDebug() << "favorite deleted type: " << destType;
                    if (destType == 2)
                        resetHomeLocationProperty();
                    
                    else if (destType == 1)
                        resetWorkLocationProperty();

                    if (!hasHomeOffice) hasHomeOffice = destType > 0;
                     // fixed TMCRSM-2412, TMCRSM-2413   END

                    if (!hasNormalFavorite) hasNormalFavorite = destType == 0;
                }

                requestFavoiteListForProperty();
                
                // to get sync with server
                if (hasHomeOffice) { // notify for resetting home/office if the item is home or office 2017.10.10
                    FavoriteDtoPtr home = proxy->getHomeFavorite();
                    FavoriteDtoPtr office = proxy->getOfficeFavorite();
                    QVariantMap body;
                    if (home.isNull() && office.isNull())
                       body[CommonConstants::Type] = UserProfileConstants::ResetHomeOfficeK;
                    else {
                        body[CommonConstants::Type] = UserProfileConstants::RegistHomeOfficeK;
                        QList<FavoriteDtoPtr> data;
                        data.append(home);
                        data.append(office);
                        body[QStringLiteral("data")] =  QVariant::fromValue(data);
                    }
                    sendNotification(NavigationControllerConstants::ModifiedUserData,body);
                }

                if (hasNormalFavorite) {
                    QVariantMap body;
                    body[CommonConstants::Type] = UserProfileConstants::RemoveMultiFavoritesK;
                    body[QStringLiteral("data")] = QVariant::fromValue(favsDeleted);
                    sendNotification(NavigationControllerConstants::ModifiedUserData, body);
                }
            }
        }
		break;
    case SearchConstants::ClearFavorites: {
            qDebug() << "favorite cleared !!";
           FavoriteDtoPtr home   =  proxy->getHomeFavorite();
           FavoriteDtoPtr office =  proxy->getOfficeFavorite();
		   proxy->clearFavorites();

           if (!home.isNull()) {
               resetHomeLocationProperty();
           }
            
           if (!office.isNull()) {
               resetWorkLocationProperty();
           }

           requestFavoiteListForProperty();
           // doing to server
            QVariantMap body;
            body[CommonConstants::Type] = UserProfileConstants::ResetHomeOfficeK;
            sendNotification(NavigationControllerConstants::ModifiedUserData,body);

            QVariantMap body2;
            body2[CommonConstants::Type] = UserProfileConstants::ClearFavoritesK;
            sendNotification(NavigationControllerConstants::ModifiedUserData,body2);
        }
		break;

    case SearchConstants::RequestHomeFavorite: {
        auto home = proxy->getHomeFavorite();
        sendNotification(SearchConstants::ResponseHomeFavorite, QVariant::fromValue(home));
        auto data = note->getBody().toMap();
        if (data["service.sds"].toBool() == true) {
            mvc::ICommandPtr command = qSharedPointerDynamicCast<mvc::ICommand>(note->getDestination());
            QtConcurrent::run(this,&ManageFavoriteCommand::initNaviProperty,command, home);
        }
    }
        break;
    case SearchConstants::RequestOfficeFavorite: {
        auto office = proxy->getOfficeFavorite();
        sendNotification(SearchConstants::ResponseOfficeFavorite, QVariant::fromValue(office));
        auto data = note->getBody().toMap();
         if (data["service.sds"].toBool() == true) {
            mvc::ICommandPtr command = qSharedPointerDynamicCast<mvc::ICommand>(note->getDestination());
            QtConcurrent::run(this,&ManageFavoriteCommand::initNaviProperty,command, office);
        }
    }
        break;

    case SearchConstants::UpdateFavorite:
        {
            auto data = note->getBody().value<QVariantMap>();
            auto id  = data[SearchConstants::Id].toInt();
            auto alias = data[CommonConstants::Alias].toString();
            auto desttype  = data[SearchConstants::DestType].toInt();
            FavoriteDtoPtr oldfav = proxy->getFavoriteById(id);
            proxy->updateFavoriteAlias(id,alias);
            proxy->updateFavoriteType(id,desttype);
            requestFavoiteListForProperty();

            // notify user profile data modification
            FavoriteDtoPtr fav = proxy->getFavoriteById(id);
            if (!fav.isNull()) {
                QList<FavoriteDtoPtr> favors;
                favors.append(oldfav);
                favors.append(fav);
                QVariantMap body;
                body[CommonConstants::Type] = UserProfileConstants::ModifyFavoriteK;
                body[QStringLiteral("data")] = QVariant::fromValue(favors);
                sendNotification(NavigationControllerConstants::ModifiedUserData, body);
            }
        }
		break;

    case SearchConstants::QueryIsFavorite:
        {
            auto m = note->getBody().toMap();
            auto x = m["x"].toInt();
            auto y = m["y"].toInt();
            auto poiId = -1;
            if (m.contains(SearchConstants::PoiID)){
                poiId = m[SearchConstants::PoiID].toInt();
            }
            sendNotification(SearchConstants::ResponseIsFavorite, proxy->hasFavorite(x, y, poiId));
        }
        break;

    case SearchConstants::AddItemOk:
        {
            auto m = note->getBody().toMap();
            auto itemType = m[QStringLiteral("itemType")].toString();
            auto destType = m[QStringLiteral("destType")].toInt();

            if (itemType == QStringLiteral("favorite")) {
                QString message;
                auto caller = m[CommonConstants::Caller].toString();
                auto callerIsSearchResultDetailView = caller == ViewRegistry::objectName(ViewRegistry::SearchResultDetailView);

                switch (destType) {
                case 1:
                    message = QObject::tr("회사가 등록되었습니다.");
                    break;
                case 2:
                    message = QObject::tr("집이 등록되었습니다.");
                    break;
                case 0:
                case 3:
                    if (callerIsSearchResultDetailView) {
                        message = "즐겨찾기로 등록되었습니다";
                    } else {
                        message = QObject::tr("즐겨찾기에 추가되었습니다.");
                    }
                    break;
                }
                QVariantMap m;
                m[CommonConstants::Position] = ToastsConstants::ToastUp;
                m[CommonConstants::Message] = message;
                sendNotification(ToastsConstants::ShowToast, m);
            }
        }
        break;

    case SearchConstants::AddItemFailure:
        {
            auto m = note->getBody().toMap();
            auto itemType = m["itemType"].toString();
            auto failCode = m["code"].toInt();

            if (itemType == QStringLiteral("favorite")) {
                QVariantMap m;

                switch (failCode) {
                case 1:
                    // Popup 다이얼로그를 띄움
                    m[CommonConstants::Url] = ViewRegistry::url(ViewRegistry::CommonDialogView);
                    m[QStringLiteral("title")] = tr("알림");
                    m[QStringLiteral("message")] = tr("등록할 수 있는 최대 개수를 초과하였습니다.\n기존 목록 삭제 후 다시 시도해주세요.");
                    PlatformHelper::instance()->setParentForSystemPopup(m);
                    sendNotification(Constants::CreateView, m);
                    break;

                case 2:
                    // 이미 등록된 즐겨찾기의 경우 토스트로 알림
                    m[CommonConstants::Position] = ToastsConstants::ToastUp;
                    m[QStringLiteral("message")] = tr("이미 등록되어 있습니다.");
                    sendNotification(ToastsConstants::ShowToast, m);
                    break;
                }
            }
        }
        break;

	    default:
	    	return false;
	 }

    return true;
}

void ManageFavoriteCommand::requestFavoiteList(const QVariantMap &m, bool forProperty)
{
    auto max  = m[CommonConstants::Max].toInt(); // number of list to select
    auto offset = m[CommonConstants::Offset].toInt(); // start row number to select
    auto sortType = 0;
    if ( m.contains(CommonConstants::Type))
    	sortType = m[CommonConstants::Type].toInt(); // 0: by regist date, 1: name (가나다 ASC)

    auto proxy = Facade::instance()->retrieveProxy<NavibookmarkAgentProxy>();
    auto favorites = proxy->favoriteList(max,offset,sortType);

    if (forProperty) {
        auto favoriteList = new QList<TS_ROUTE_POS>();
        TS_ROUTE_POS p;
        for (int i=0; i < favorites.count(); i++) {
            memset(&p, 0, sizeof(TS_ROUTE_POS));
            auto item = favorites.at(i);
            p.tp.x = item->mapX().toInt();
            p.tp.y = item->mapY().toInt();
            // fixed, missing poi name, address
            p.nPoiID = item->poiID().toInt();
            p.byRPFlag = item->rpFlag().toInt();
            if (item->alias().toString().isEmpty())
               qstrncpy(p.szName, TextDecoder::instance()->fromUnicode(item->dest().toString()).constData(),TD_POSNAME_LEN);
            else
               qstrncpy(p.szName, TextDecoder::instance()->fromUnicode(item->alias().toString()).constData(),TD_POSNAME_LEN);

            favoriteList->append(p);
        }

        auto favoriteListPtr = QRoutePosListPtr(favoriteList);

        QVariantMap m2;
        m2["propertyValue"] = QVariant::fromValue(favoriteListPtr);
        m2["type"] = NavigationPropertyProtocol::TmapFavoriteChange;

        sendNotification(NavigationPropertyConstants::setNavigationProperty, m2);
    } else {
        sendNotification(SearchConstants::ResponseFavorites, QVariant::fromValue(favorites));
    }
}

void ManageFavoriteCommand::requestFavoiteListForProperty()
{
    QVariantMap m;
    m[CommonConstants::Offset] = 0;
    m[CommonConstants::Max] = TMAP_UI_FAVORITE_MAX;
    requestFavoiteList(m, true);

    // notify what user data modified
    // QVariantMap body;
    // body[CommonConstants::Type] = UserProfileConstants::FavoriteList;

    // sendNotification(NavigationControllerConstants::ModifiedUserData,body);
}


void ManageFavoriteCommand::initNaviProperty(const mvc::ICommandPtr& command, const FavoriteDtoPtr& favorite)
{
    if (favorite.isNull()) 
       requestFavoiteListForProperty();
    else {
            QVariantMap m;
            TS_ROUTE_POS pos;
            memset(&pos, 0, sizeof(TS_ROUTE_POS));
            pos.tp.x = favorite->mapX().toInt();
            pos.tp.y = favorite->mapY().toInt();
            pos.nPoiID = favorite->poiID().toInt();
            pos.byRPFlag = favorite->rpFlag().toInt();
            if (favorite->alias().toString().isEmpty())
              qstrncpy(pos.szName, TextDecoder::instance()->fromUnicode(favorite->dest().toString()).constData(),TD_POSNAME_LEN);
            else
              qstrncpy(pos.szName, TextDecoder::instance()->fromUnicode(favorite->alias().toString()).constData(),TD_POSNAME_LEN);

            m["propertyValue"] = QVariant::fromValue(pos);

            if (favorite->destType() == 1)
                m[CommonConstants::Type] = NavigationPropertyProtocol::TmapWorkLocation;
            else
                m[CommonConstants::Type] = NavigationPropertyProtocol::TmapHomeLocation;

            sendNotification(NavigationPropertyConstants::setNavigationProperty, m);
    }
}



void ManageFavoriteCommand::resetHomeLocationProperty()
{
    QVariantMap m;
    TS_ROUTE_POS pos;
    memset(&pos, 0, sizeof(TS_ROUTE_POS));
    pos.tp.x = -1;
    pos.tp.y = -1;
    pos.nPoiID = -1;
    m["propertyValue"] = QVariant::fromValue(pos);
    m[CommonConstants::Type] = NavigationPropertyProtocol::TmapHomeLocation;
    sendNotification(NavigationPropertyConstants::setNavigationProperty, m);
}

void ManageFavoriteCommand::resetWorkLocationProperty()
{
    QVariantMap m;
    TS_ROUTE_POS pos;
    memset(&pos, 0, sizeof(TS_ROUTE_POS));
    pos.tp.x = -1;
    pos.tp.y = -1;
    pos.nPoiID = -1;
    m["propertyValue"] = QVariant::fromValue(pos);
    m[CommonConstants::Type] = NavigationPropertyProtocol::TmapWorkLocation;
    sendNotification(NavigationPropertyConstants::setNavigationProperty, m);
}
