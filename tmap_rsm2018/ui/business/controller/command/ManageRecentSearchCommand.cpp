#include "ManageRecentSearchCommand.h"
#include "NavibookmarkAgentProxy.h"
#include "RecentSearchDto.h"
#include "Constants.h"
#include "SearchConstants.h"
#include "NavigationPropertyProtocol.h"
#include "NavigationPropertyConstants.h"
#include "navigationproperty.h" // for TS_ROUTE_POS
#include "StringHelper.h"

using namespace SKT;

ManageRecentSearchCommand::ManageRecentSearchCommand(QObject* parent) :
		SimpleCommand(parent)
{

}

bool ManageRecentSearchCommand::execute( const mvc::INotificationPtr& note )
{
	auto proxy = Facade::instance()->retrieveProxy<NavibookmarkAgentProxy>();

	switch ( note->getId() ) {
	case SearchConstants::RequestRecentSearch:
	{
    	auto data = note->getBody().value<QVariantMap>();
        auto max  = data[CommonConstants::Max].toInt(); // number of list to select
        auto offset = data[CommonConstants::Offset].toInt(); // start row number to select
    	auto dests = proxy->recentSearchList(max,offset);
    	sendNotification(SearchConstants::ResponseRecentSearch, QVariant::fromValue(dests));
	}
		break;
	case SearchConstants::AddRecentSearch:
	{
		auto data = note->getBody();
		if (data.isValid())
		{
			proxy->addRecentSearch(data.toString());
		}
		sendNaviPropertyChanged();
	}
		break;
	case SearchConstants::DeleteRecentSearch:
	{
		auto data = note->getBody();
		if (data.isValid())
		{
			QVariantList ids = data.toList();
			QList<uint> idList;
			std::for_each(ids.cbegin(),ids.cend(),[&](const QVariant& va) {
				idList.append(va.toUInt());
			});
			proxy->deleteRecentSearch(idList);
		}
		sendNaviPropertyChanged();
	}
		break;
	case SearchConstants::ClearRecentSearch:
		proxy->clearRecentSearch();
		sendNaviPropertyChanged();
		break;
	default:
		return false;
		break;
	}
	return true;
}

/*!
 * provides navigation property to export called 'TmapRecentDestinationChange' by lckxx 2017.7.25
 */
void ManageRecentSearchCommand::sendNaviPropertyChanged()
{
	// TODO: note! it needs to run in a thread
	auto proxy = Facade::instance()->retrieveProxy<NavibookmarkAgentProxy>();
	auto dests = proxy->recentSearchList(-1);
	QList<QString>* data = new QList<QString>();
	for (RecentSearchDtoPtr d : dests)
	{
		data->append(d->keyword().toString());
	}
	QVariantMap body;
	body.insert(QStringLiteral("type"),NavigationPropertyProtocol::TmapSearchWordChange);
	body.insert(QStringLiteral("propertyValue"),QVariant::fromValue(QSearchWordListPtr(data)));
	sendNotification(NavigationPropertyConstants::setNavigationProperty,body);
}

