#include "ManageRecentDestinationCommand.h"
#include "NavibookmarkAgentProxy.h"
#include "RouteSearchProxy.h"
#include "RecentDestinationDto.h"
#include "Constants.h"
#include "SearchConstants.h"
#include "NavigationPropertyProtocol.h"
#include "NavigationPropertyConstants.h"
#include "navigationproperty.h" // for TS_ROUTE_POS
#include "StringHelper.h"
#include "NavigationControllerConstants.h"
#include "UserProfileConstants.h"
#include <QtConcurrent/QtConcurrent>
#include "ProfileManager.h"

using namespace SKT;

ManageRecentDestinationCommand::ManageRecentDestinationCommand(QObject* parent) :
		SimpleCommand(parent)
{

}

bool ManageRecentDestinationCommand::execute( const mvc::INotificationPtr& note )
{
	auto proxy = Facade::instance()->retrieveProxy<NavibookmarkAgentProxy>();

	switch ( note->getId() ) {
	    case SearchConstants::RequestRecentDestination: // request recent destination list, TODO: add a new constant into the Constnats map
	        {
	        	auto data = note->getBody().value<QVariantMap>();
                auto max  = data[CommonConstants::Max].toInt(); // number of list to select
                auto offset = data[CommonConstants::Offset].toInt(); // start row number to select
                auto sortType = data[CommonConstants::Type].toInt(); // 0 : by regist desc, 1: by hit desc, 2: hybrid (Grid View)
                auto owner = data[CommonConstants::Owner]; // identify owner view such SearchTabView or BookmarkRegistView 2017.9.13
                if (data["service.sds"].toBool() == false) {
                	QList<RecentDestinationDtoPtr> dests;
                	if (sortType == 2)
                		dests = proxy->recentDestListByHybrid(max,offset);
                	else
                		dests = proxy->recentDestListBySortType(max,offset, sortType);

                	QVariantMap value;
                	value.insert(SearchConstants::Data, QVariant::fromValue(dests));
                	value.insert(CommonConstants::Owner, owner);
                	sendNotification(SearchConstants::ResponseRecentDestination, QVariant::fromValue(value));
                }
                else {
                	mvc::ICommandPtr command = qSharedPointerDynamicCast<mvc::ICommand>(note->getDestination());
                    QtConcurrent::run(this,&ManageRecentDestinationCommand::initNaviProperty,command);
                }
	        }
	        break;
        case SearchConstants::AddRecentDestination:
	    {
	    	auto data = note->getBody().value<QVariantMap>();
  
            auto dest = data[SearchConstants::Dest].toString();
            auto addr = data[CommonConstants::Address].toString();
            auto mapX    = data[CommonConstants::X].toInt();
            auto mapY    = data[CommonConstants::Y].toInt();
            auto poiId   = data[SearchConstants::PoiID].toInt();
            auto navSeq  = data[SearchConstants::NavSeq].toInt();
            auto rpFlag  = data[SearchConstants::RpFlag].toInt();
            auto poleCate  = data[SearchConstants::OpiPoleCate].toInt();

            RecentDestinationDto dto;

            dto.setDest(dest);
            dto.setAddress(addr);
	    	dto.setMapX(mapX);
	    	dto.setMapY(mapY);
	    	dto.setPoiID(poiId);
	    	dto.setNavSeq(navSeq);
	    	dto.setRpFlag(rpFlag);
            dto.setOpiPoleCate(poleCate);

	    	// get address for map search
            if (addr.isEmpty() && dest.isEmpty())
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

	    	uint rowid = proxy->addRecentDestination(dto); // dest type 0, hit 0

            if (rowid > 0) sendNaviPropertyChanged();

		    // notify what user data modified (doing for server ) if a new item added
            if (rowid) {
		       QList<RecentDestinationDtoPtr> data2;
			   #ifdef IVI_NDDS_PROVIDE_TO_REGIST_SINGLE // 2019-08-16 fix TMCRSM-3492, TMCRSM-3425
		       RecentDestinationDto* dst = new RecentDestinationDto;
		       dst->setDest(dest);
		       dst->setAddress(addr);
		       dst->setMapX(mapX);
		       dst->setMapY(mapY);
		       dst->setPoiID(poiId);
		       dst->setNavSeq(navSeq);
		       dst->setRpFlag(rpFlag);
               dst->setOpiPoleCate(poleCate);
			   data2.append(RecentDestinationDtoPtr(dst));
			   #else
               data2 = proxy->recentDestListBySortType(-1);   // # M1 NDDS 동기화는 무조건 모든 데이터를 올려줘야 한다. 서버는 기존 것을 삭제하기 때문이다.
			   #endif


               // 2019-6-13 don't sync in adding a item downloaed from server -> M1에 의해 이 내용은 무시한다.
			   auto owner = note->getSource();
			   if (owner.data() != ProfileManager::instance()) {
				   QVariantMap body2;
				   body2[CommonConstants::Type] = UserProfileConstants::MultiRecentDestsK;
				   body2[QStringLiteral("data")] = QVariant::fromValue(data2);
				   sendNotification(NavigationControllerConstants::ModifiedUserData, body2);
			   }
		   }
	    }
	    	break;
	    case SearchConstants::DeleteRecentDestination:
	    {
	    	auto body = note->getBody();
	    	if (body.isValid())
	    	{
	    		QVariantList ids = body.toList();
	    		QList<uint> idList;
	    		std::for_each(ids.cbegin(),ids.cend(),[&](const QVariant& va) {
	    			idList.append(va.toUInt());
	    		});
	    	   QList<RecentDestinationDtoPtr> destsDeleted = proxy->deleteRecentDest(idList);
	           sendNaviPropertyChanged();

	           // doing for server
	           QVariantMap body;
	           body[CommonConstants::Type]  = UserProfileConstants::RemoveMultiRecentDestsK;
               body[QStringLiteral("data")] = QVariant::fromValue(destsDeleted);
	           sendNotification(NavigationControllerConstants::ModifiedUserData,body);
	    	}
	    }
	    	break;
	    case SearchConstants::ClearRecentDestination:
	    {
	    	proxy->clearRecentDestination();
	        sendNaviPropertyChanged();

	        // doing for server
	           QVariantMap body;
	           body[CommonConstants::Type]  = UserProfileConstants::ClearRecentDestinationsK;
	           sendNotification(NavigationControllerConstants::ModifiedUserData,body);
	    }
	    	break;
//	    case SearchConstants::UpdateRecentDestinationScore: // hit updated if already exist while adding
//	    {
//	    	auto data = note->getBody().value<QVariantMap>();
//	    	uint id    = data[SearchConstants::Id].toUInt();
//	    	uint score = data[SearchConstants::score].toUInt();
//	    	proxy->updateRecentDestHit(id,score);
//	    }
	    	break;
	    default:
	    	return false;
	 }

	return true;
}

/*!
 * provides navigation property to export called 'TmapRecentDestinationChange' by lckxx 2017.7.25
 */
void ManageRecentDestinationCommand::sendNaviPropertyChanged()
{
	// TODO: note! it needs to run in a thread
	auto proxy = Facade::instance()->retrieveProxy<NavibookmarkAgentProxy>();
	auto dests = proxy->recentDestListBySortType(-1);
	QList<TS_ROUTE_POS>* data = new QList<TS_ROUTE_POS>();
	for (RecentDestinationDtoPtr d : dests)
	{
		TS_ROUTE_POS rp;
        memset(&rp, 0, sizeof(TS_ROUTE_POS));
		rp.tp.x = d->mapX().toInt();
		rp.tp.y = d->mapY().toInt();
		QString szName = d->dest().toString();
		if (szName.isEmpty())
			szName = d->address().toString();

        rp.byRPFlag = d->rpFlag().toInt();
        rp.nPoiID = d->poiID().toInt();

		qstrncpy(rp.szName,StringHelper::instance()->toCp949(szName).constData(),100);
		data->append(rp);
	}
	QVariantMap body;
	body.insert(QStringLiteral("type"),NavigationPropertyProtocol::TmapRecentDestinationChange);
	body.insert(QStringLiteral("propertyValue"),QVariant::fromValue(QRoutePosListPtr(data)));
	#if 0
    mvc::INotificationPtr note = t_new_shared<Notification>(nullptr,
                                           NavigationPropertyConstants::setNavigationProperty,
                                           body);
    Facade::instance()->notifyObservers(note);
    #else
    sendNotification(NavigationPropertyConstants::setNavigationProperty,body);
    #endif
}

/*!
 * initialize navigation recent destination property
 */
void ManageRecentDestinationCommand::initNaviProperty(const mvc::ICommandPtr& command)
{
	sendNaviPropertyChanged();
}

