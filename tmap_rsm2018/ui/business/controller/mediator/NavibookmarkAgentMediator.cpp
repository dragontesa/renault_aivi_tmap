#include "NavibookmarkAgentMediator.h"
#include "Constants.h"
#include "CommonConstants.h"
#include "SearchConstants.h"
#include "NavibookmarkConstants.h"

using namespace SKT;

NavibookmarkAgentMediator::NavibookmarkAgentMediator()
    : AbstractMediator(nullptr),
	  mTransactionId(QString::null),
      mTransactionType(NavibookmarkProtocol::TransactionUnknown),
      mBookmarkType(NavibookmarkProtocol::BookmarkUnknown),
      mSearchType(NavibookmarkProtocol::SearchUnknown)
{

}

NavibookmarkAgentMediator::~NavibookmarkAgentMediator()
{

}


QList<int> NavibookmarkAgentMediator::listNotificationInterests()
{
    static QList<int> notes = {
            NavibookmarkConstants::NewNavibookSession,
//			NavibookmarkConstants::StopNavibookSession,
            NavibookmarkConstants::SelectNavibookSearch,
            NavibookmarkConstants::SelectNavibookAddress,
            NavibookmarkConstants::RegistNavibookBookmark,
    };

    return notes;
}

bool NavibookmarkAgentMediator::handleNotification(const mvc::INotificationPtr& note)
{
	auto caller = note->getSource();
	auto data = note->getBody().value<QVariantMap>();
    auto transId = (data.contains(SearchConstants::Transaction))?data[SearchConstants::Transaction].toString():QString();

	switch ( note->getId() ) {
    case NavibookmarkConstants::NewNavibookSession:
		// clear transaction id on old session
//		mTransactionId.clear();
		reset();
        mTransactionType = (NavibookmarkProtocol::NavibookTransactionType)data[CommonConstants::Type].toInt();
		mTransactionId = newTransactionId(mTransactionType);
		mCallerId = caller->metaObject()->className();

        if (data.contains(SearchConstants::SearchType ))
            mSearchType = (NavibookmarkProtocol::NavibookSearchType) data[SearchConstants::SearchType ].toInt();

//		sendNotification(Constants::NavibookSessionStarted,QVariant::fromValue(mTransactionId));
		break;

       case NavibookmarkConstants::SelectNavibookSearch:
//    	   if (mTransactionId.isEmpty() || mTransactionId != transId) return false;
            mSearchType = (NavibookmarkProtocol::NavibookSearchType) data[SearchConstants::SearchType ].toInt();
            if (data.contains(CommonConstants::Keyword ))
                mSearchKeyword = data[CommonConstants::Keyword ].toString();

			break;
       case NavibookmarkConstants::SelectNavibookAddress:
       {
//    	   if (mTransactionId.isEmpty() || mTransactionId != transId) return false;
           if ( data.contains(CommonConstants::Name) )
               mBookmark.name = data[CommonConstants::Name].toString();

           if ( data.contains(CommonConstants::Address) )
               mBookmark.address = data[CommonConstants::Address].toString();

           if ( data.contains(CommonConstants::X ) )
               mBookmark.mapX  = data[CommonConstants::X ].toInt();

           if ( data.contains(CommonConstants::Y) )
                mBookmark.mapY  = data[CommonConstants::Y].toInt();

           if ( data.contains(SearchConstants::DestType ) )
               mBookmark.destType = data[SearchConstants::DestType ].toInt();

           if ( data.contains(SearchConstants::PoiID) )
             mBookmark.poiID = data[SearchConstants::PoiID].toInt();

           if ( data.contains(SearchConstants::NavSeq) )
             mBookmark.navSeq = data[SearchConstants::NavSeq].toInt();

           if (data.contains(SearchConstants::RpFlag))
               mBookmark.rpFlag = data[SearchConstants::RpFlag].toInt();

           if ( data.contains(SearchConstants::FavInsTime) )
             mBookmark.favInsTime = data[SearchConstants::FavInsTime].toString();

           if ( data.contains(SearchConstants::OpiPoleCate) )
             mBookmark.opiPoleCate = data[SearchConstants::OpiPoleCate].toInt();

           if ( data.contains(SearchConstants::OpiPrice) )
             mBookmark.opiPrice = data[SearchConstants::OpiPrice].toInt();
       }
			break;
       case NavibookmarkConstants::RegistNavibookBookmark:
//    	   if (mTransactionId.isEmpty() || mTransactionId != transId) return false;
            mBookmarkType = (NavibookmarkProtocol::NavibookBookmarkType) data[CommonConstants::BookmarkType].toInt();
			if (isValidBookmark())
			{
                // 지도에서 즐겨찾기하는 경우인지 구분용.
                // 지도에서 추가한 경우 추가로 즐겨찾기 화면을 띄우는 작업을 수행.
                // 18.01.09 ymhong.
                auto caller = data[CommonConstants::Caller].toString();

                registNewBookmark(caller);
				reset();
			}
			break;
		default :
			return false;
		}
	return true;
}

void NavibookmarkAgentMediator::reset()
{
	mTransactionId.clear();
	mCallerId.clear();
    mTransactionType = NavibookmarkProtocol::TransactionUnknown;
    mBookmarkType = NavibookmarkProtocol::BookmarkUnknown;
    mSearchType = NavibookmarkProtocol::SearchUnknown;
	mSearchKeyword.clear();
	mBookmark.name.clear();
	mBookmark.address.clear();
	mBookmark.mapX = -1;
	mBookmark.mapY = -1;
	mBookmark.destType = 0;
	mBookmark.poiID = -1;
  mBookmark.navSeq = 0;
  mBookmark.rpFlag = 0;
  mBookmark.opiPoleCate = 0;
  mBookmark.opiPrice = 0;
  mBookmark.favInsTime.clear();
}

QString NavibookmarkAgentMediator::newTransactionId(int transactionType)
{
	// create new transaction id(32 bytes) :
	    //  RSMNB[2digit reserved(default:00)][3digit transaction type][6 digit reserved(default:000000)]T[timestamp]
	QDateTime now = QDateTime::currentDateTime();
	QString timestamp=now.toString("yyMMddhhmmsszzz");
	return QString("RSMNB00%1%2T%3").arg(transactionType,2,10,QChar('0')).arg("000000").arg(timestamp);
}


bool NavibookmarkAgentMediator::registNewBookmark(const QString& caller)
{
	// request address info for map search
	switch (mBookmarkType)
	{
    case NavibookmarkProtocol::BookmarkRecentDestination:
	{
		QVariantMap body;
        body.insert(SearchConstants::Dest, mBookmark.name);
        body.insert(CommonConstants::Address, mBookmark.address);
        body.insert(CommonConstants::X , mBookmark.mapX);
        body.insert(CommonConstants::Y, mBookmark.mapY);
        body.insert(SearchConstants::DestType , 0); // first default must get 0
        body.insert(SearchConstants::PoiID, mBookmark.poiID);
        body.insert(SearchConstants::NavSeq, mBookmark.navSeq);
        body.insert(SearchConstants::RpFlag, mBookmark.rpFlag);
        body.insert(SearchConstants::OpiPoleCate, mBookmark.opiPoleCate);
        body.insert(SearchConstants::OpiPrice, mBookmark.opiPrice);

    	sendNotification(SearchConstants::AddRecentDestination,body);
	}
        break;
    case NavibookmarkProtocol::BookmarkRecentSearch:
	{
		if (!mSearchKeyword.isEmpty()) // keyword must has value
		    sendNotification(SearchConstants::AddRecentSearch,QVariant::fromValue(mSearchKeyword));
	}
		break;
    case NavibookmarkProtocol::BookmarkFavorite:
	{
		QVariantMap body;
        body.insert(SearchConstants::Dest, mBookmark.name);
        body.insert(CommonConstants::Address, mBookmark.address);
        body.insert(CommonConstants::X , mBookmark.mapX);
        body.insert(CommonConstants::Y, mBookmark.mapY);
        body.insert(SearchConstants::DestType , mBookmark.destType);
        body.insert(SearchConstants::PoiID, mBookmark.poiID);
        body.insert(SearchConstants::NavSeq, mBookmark.navSeq);
        body.insert(SearchConstants::RpFlag, mBookmark.rpFlag);
        body.insert(SearchConstants::FavInsTime, mBookmark.favInsTime);
        body.insert(SearchConstants::OpiPoleCate, mBookmark.opiPoleCate);
        body.insert(SearchConstants::OpiPrice, mBookmark.opiPrice);
        body.insert(CommonConstants::Caller, caller);

        sendNotification(SearchConstants::AddFavorite,body);
	}
		break;
	default: return false;
	}

	return true;
}

bool NavibookmarkAgentMediator::isValidBookmark()
{
	bool ret = false;
	switch (mBookmarkType)
	{
    case NavibookmarkProtocol::BookmarkRecentSearch:
		ret = !mSearchKeyword.isEmpty();
		break;
    case NavibookmarkProtocol::BookmarkRecentDestination:
    case NavibookmarkProtocol::BookmarkFavorite:
        ret = (mBookmark.mapX >= 0 && mBookmark.mapY >=0 );
		break;
	default: ret = false;
	}

	return ret;
}
