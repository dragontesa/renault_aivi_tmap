/*
 * OfflineJobCacheProxy.cpp
 *
 *  Created on: 2017. 7. 19.
 *      Author: njas
 */

#include "OfflineJobCacheProxy.h"
#include "SearchConstants.h"
#include "NavigationControllerConstants.h"
#include "DatabaseHelper.h"
#include "StringHelper.h"
#define OFFLINE_MAX_SEARCH_CACHE_SIZE  36100  // file size is about 10MB

namespace SKT
{

using namespace mvc;

/*! this provides the base class for the offline cache job
 * this class provides interface based on stack storage.
 * so you doen't need to allocate a instance at heap.
 */
class CacheJob: public QObject
{
	Q_OBJECT
protected:
	class CacheWorkerBase: public QThread
	{
	protected:
		virtual void work() = 0;
		bool prepareSql(const QMetaObject& dto, QSqlQuery& query) {
			int ix = dto.indexOfClassInfo(QStringLiteral("table").toLatin1().constData());
			QString table = dto.classInfo(ix).value();
			QStringList columns;
			for (int i=0;i<dto.classInfoCount();i++)
			{
				QMetaClassInfo mci =  dto.classInfo(i);
				QString n(mci.name());
				QString v(mci.value());
				v = v.split(QChar(' ')).at(0);
				if (QStringLiteral("column") == n && QStringLiteral("id") != v )
				{
					QString colName = QString(mci.value()).split(QChar(' ')).at(0);
					columns.append(colName);
				}
			}
			QString sql(QStringLiteral("INSERT OR REPLACE INTO %1 (").arg(table));
			QString cols;
			QString values;
			for (int i=0;i<columns.count()-1;i++)
			{
				cols.append(columns.at(i)).append(QChar(','));
				values.append(QStringLiteral(":%1").arg(columns.at(i))).append(QChar(','));
			}
			cols.append(columns.last());
			values.append(QStringLiteral(":%1").arg(columns.last()));
			sql.append(cols);
			sql.append(QChar(')'));
			sql.append(QStringLiteral("VALUES ("));
			sql.append(values);
			sql.append(QChar(')'));

			QSqlDatabase db = DatabaseHelper::instance()->connection(QStringLiteral("offline_%1.db").arg(table));
			query = QSqlQuery(db);
			return query.prepare(sql);
		}

		void optimizeCacheSize(const QMetaObject& dto, uint optSize) {
		    uint ret = 0;
		    int ix = dto.indexOfClassInfo(QStringLiteral("table").toLatin1().constData());
		    QString table = dto.classInfo(ix).value();
		    QString sql(QStringLiteral("SELECT count(id) FROM %1").arg(table));
		    QSqlDatabase db = DatabaseHelper::instance()->connection(QStringLiteral("offline_%1.db").arg(table));
		    QSqlQuery query(db);
		    if (! query.exec(sql)) {
		    	T_DEBUG("sql failed to cache size:["+QStringLiteral("%1").arg(query.lastError().type())+"]"+query.lastError().text());
		    }
		    ret = query.value(0).toUInt();

		    if (ret > optSize) {
		    	QString sql2(QStringLiteral("DELETE FROM %1 WHERE id < (SELECT id FROM %2 ORDER BY id ASC LIMIT %3,1)").arg(table).arg(table).arg(ret-optSize));
		    	if (!query.exec(sql2)) {
		    		T_DEBUG("sql failed to optimize cache:["+QStringLiteral("%1").arg(query.lastError().type())+"]"+query.lastError().text());
		    	}
		    }

		    query.clear();
		    query.finish();
		}

		void run() override {
			work();
		}

	};

	void start(CacheWorkerBase* worker)
	{
		connect(worker,SIGNAL(finished()),this,SLOT(deleteLater()));
		worker->start();
	}
};



class PoiCacheJob: public CacheJob
{
    friend class SearchCacheJob;
public:
    PoiCacheJob(const PoiCacheInfoListPtr& data): mWorker(data) {}
    void start() {
    	CacheJob::start(&mWorker);
    }

protected:
    class PoiCacheWorker: public CacheWorkerBase
	{
	public:
    	PoiCacheWorker(const PoiCacheInfoListPtr& data):mPoiCacheData(data) {}
    	void work() override {
    		const QList<PoiCacheInfo>* data = mPoiCacheData.data();
    		// optimize cache size
    		optimizeCacheSize(PoiCacheDto::staticMetaObject, OFFLINE_MAX_SEARCH_CACHE_SIZE);

    		QSqlQuery query;
            prepareSql(PoiCacheDto::staticMetaObject,query);
    		for (int i=0;i<data->size();++i)
    		{
    			query.bindValue(0, data->at(i).poiId);
    			query.bindValue(1, data->at(i).dbKind);
    			query.bindValue(2, data->at(i).navX);
    			query.bindValue(3, data->at(i).navY);
    			query.bindValue(4, data->at(i).centerX);
    			query.bindValue(5, data->at(i).centerY);
    			query.bindValue(6, data->at(i).rpFlag);
    			query.bindValue(7, data->at(i).dataKind);
    			query.bindValue(8, data->at(i).mlClass);
    			query.bindValue(9, data->at(i).parkYn);
    			query.bindValue(10, data->at(i).premiumGasolinYn);
    			query.bindValue(11, data->at(i).lowestOilYn);
    			query.bindValue(12, data->at(i).gasolinPrice);
    			query.bindValue(13, data->at(i).dieselPrice);
    			query.bindValue(14, data->at(i).lpgPrice);
    			query.bindValue(15, data->at(i).premiumGasolinPrice);
    			query.bindValue(16, data->at(i).premiumDieselPrice);
    			query.bindValue(17, data->at(i).oilBaseSdt);
    			query.bindValue(18, data->at(i).navSeq);
    			query.bindValue(19, data->at(i).oilCompany);
    			query.bindValue(20, data->at(i).fullName);
    			query.bindValue(21, data->at(i).aliasName);
    			query.bindValue(22, data->at(i).address);
    			query.bindValue(23, data->at(i).primaryBun);
    			query.bindValue(24, data->at(i).secondaryBun);
    			query.bindValue(25, data->at(i).lcdName);
    			query.bindValue(26, data->at(i).mcdName);
    			query.bindValue(27, data->at(i).scdName);
    			query.bindValue(28, data->at(i).dcdName);
    			query.bindValue(29, data->at(i).zipCode);
    			query.bindValue(30, data->at(i).telNum);
    			query.bindValue(31, data->at(i).webSite);
    			query.bindValue(32, data->at(i).radius);
    			query.bindValue(33, data->at(i).roadName);
    			query.bindValue(34, data->at(i).bldNo1);
    			query.bindValue(35, data->at(i).bldNo2);
    			query.bindValue(36, data->at(i).pubTraffic);
    			query.bindValue(37, data->at(i).stationId);

    			if (!query.exec())
    			{
    				T_DEBUG("sql failed to poi cache:["+QString("%1").arg(query.lastError().type())+"]"+query.lastError().text());
    			}
    		}
		    query.clear();
		    query.finish();
    	}

	private:
	PoiCacheInfoListPtr mPoiCacheData; // if it run as thread without SearchCacheJob

	} mWorker;
};


class PoiExtraCacheJob: public CacheJob
{
    friend class SearchCacheJob;
public:
    PoiExtraCacheJob(const PoiExtraCacheInfoListPtr& data): mWorker(data) {}
    void start() {
    	CacheJob::start(&mWorker);
    }
protected:
    class PoiExtraCacheWorker: public CacheWorkerBase
	{
	public:
    	PoiExtraCacheWorker(const PoiExtraCacheInfoListPtr& data): mPoiExtraCacheData(data) {}
        void work() override {
        const QList<PoiExtraCacheInfo>* data = mPoiExtraCacheData.data();
        // optimize cache size
        optimizeCacheSize(PoiExtraCacheDto::staticMetaObject, OFFLINE_MAX_SEARCH_CACHE_SIZE);
        QSqlQuery query;
        prepareSql(PoiExtraCacheDto::staticMetaObject,query);
        for (int i=0;i<data->size();++i)
        {
            query.bindValue(0, data->at(i).poiId);
            query.bindValue(1, data->at(i).subGroupList);
            query.bindValue(2, data->at(i).advContents);
            if (!query.exec())
            {
                T_DEBUG("sql failed to poi cache:["+QString("%1").arg(query.lastError().type())+"]"+query.lastError().text());
            }
        }
	    query.clear();
	    query.finish();
    }
	private:
	PoiExtraCacheInfoListPtr mPoiExtraCacheData;  // if it run as thread without SearchCacheJob

	} mWorker;

};


class SearchCacheJob: public CacheJob
{
public:
	SearchCacheJob(const SearchCacheInfoListPtr& data): mWorker(data) {}
	void start() {
		CacheJob::start(&mWorker);
	}

protected:
	class SearchCacheWorker: public CacheWorkerBase
	{
	public:
		SearchCacheWorker(const SearchCacheInfoListPtr& data): mSearchCacheData(data) {}
	protected:
	   void work() override  {
		 QList<TPOI_SEARCH_LIST>* data = mSearchCacheData.data();
		 const int cnt = data->size();
		 // optimize cache size
		 optimizeCacheSize(SearchCacheDto::staticMetaObject, OFFLINE_MAX_SEARCH_CACHE_SIZE);

#ifdef TMAP_OFFLINE_CACHE_POI_FULLSPEC
		// POI info
		 QList<PoiCacheInfo>* poiList = new QList<PoiCacheInfo>;
#endif
		QSqlQuery query;
		prepareSql(SearchCacheDto::staticMetaObject,query);
		for (int i=0;i<cnt;i++)
		{
			query.bindValue(0,data->at(i).nPoiID);
            qulonglong poiIdSeq = QStringLiteral("%1%2").arg(data->at(i).nPoiID).arg(data->at(i).szNavseq).toULongLong();
			query.bindValue(1,poiIdSeq);
			query.bindValue(2,data->at(i).nAddrCode);
			query.bindValue(3,data->at(i).byRPFlag);
			query.bindValue(4,data->at(i).nDist);
			query.bindValue(5,data->at(i).nDeg);
			query.bindValue(6,data->at(i).nKind);
			query.bindValue(7,data->at(i).tp.x);
			query.bindValue(8,data->at(i).tp.y);
			query.bindValue(9,data->at(i).nOPIPoleCate);
			query.bindValue(10,data->at(i).nOPIPrice);
			query.bindValue(11,data->at(i).sGatherTime.nYear);
			query.bindValue(12,data->at(i).sGatherTime.nMonth);
			query.bindValue(13,data->at(i).sGatherTime.nDay);
			query.bindValue(14,data->at(i).sGatherTime.nHour);
			query.bindValue(15,data->at(i).sGatherTime.nMinute);
			query.bindValue(16,data->at(i).sGatherTime.nSecond);
			query.bindValue(17,data->at(i).sGatherTime.nWeek);
			QString fullName(StringHelper::instance()->fromCp949(data->at(i).szName));
			QString aliasName(StringHelper::instance()->fromCp949(data->at(i).szStdName));
			QString addrName(StringHelper::instance()->fromCp949(data->at(i).szAddrName));
			query.bindValue(18,fullName);
			query.bindValue(19,aliasName);
			query.bindValue(20,addrName);
			query.bindValue(21,data->at(i).szPhoneNumber);
			query.bindValue(22,data->at(i).bHighGasoline);
			query.bindValue(23,data->at(i).bLowestOilSt);
			query.bindValue(24,data->at(i).nOPIGasolinePrice);
			query.bindValue(25,data->at(i).nOPIDieselPrice);
			query.bindValue(26,data->at(i).nOPILPGPrice);
			query.bindValue(27,data->at(i).nOPIHiGasolinePrice);
			query.bindValue(28,data->at(i).nOPIHiLPGPrice);
			query.bindValue(29,data->at(i).szNavseq);
			QString oilCompany(StringHelper::instance()->fromCp949(data->at(i).szOilCompany));
			query.bindValue(30,oilCompany);
#ifdef TMAP_OFFLINE_CACHE_POI_FULLSPEC
			PoiCacheInfo poi;
			poi.poiId = data->at(i).nPoiID;
			poi.navX  = data->at(i).tp.x;
			poi.navY  = data->at(i).tp.y;
			poi.rpFlag = data->at(i).byRPFlag;
			poi.premiumGasolinYn = data->at(i).bHighGasoline;
			poi.lowestOilYn  = data->at(i).bLowestOilSt;
			poi.gasolinPrice = data->at(i).nOPIGasolinePrice;
			poi.dieselPrice  = data->at(i).nOPIDieselPrice;
			poi.lpgPrice  = data->at(i).nOPILPGPrice;
			poi.premiumGasolinPrice = data->at(i).nOPIHiGasolinePrice;
			poi.premiumLpgPrice  = data->at(i).nOPIHiLPGPrice;
			poi.navSeq  = data->at(i).szNavseq;
			poi.oilCompany = oilCompany;
			poi.fullName  = fullName;
			poi.aliasName = aliasName;
			poi.address = addrName;
			poi.telNum  = data->at(i).szPhoneNumber;
			poiList->prepend(poi);
#endif
			if (!query.exec())
			{
				T_DEBUG("sql failed to search cache:["+QString("%1").arg(query.lastError().type())+"]"+query.lastError().text());
			}
		}
	    query.clear();
	    query.finish();

#ifdef TMAP_OFFLINE_CACHE_POI_FULLSPEC
		// get POI details
		// TODO: get poi from NDDS with poiID
//			NDDS_getPoiDetailInfo(data[i].nPoiID);

		// get poi extra info

		QList<PoiExtraCacheInfo>* extraList = new QList<PoiExtraCacheInfo>;
		for (int i=0;i<cnt;++i)
		{
			PoiExtraCacheInfo ext;
			ext.poiId = poiList->at(i).poiId;
			ext.subGroupList = poiList->at(i).subGroupList;
			ext.advContents = poiList->at(i).advContents;
			extraList->prepend(ext);
		}

		// save POI into DB
		QScopedPointer<PoiCacheJob>pc(new PoiCacheJob(PoiCacheInfoListPtr(poiList)));
		pc->mWorker.work();

		// save POI Extra into DB
		QScopedPointer<PoiExtraCacheJob>ec(new PoiExtraCacheJob(PoiExtraCacheInfoListPtr(extraList)));
		ec->mWorker.work();
#endif
	}

	private:
	   SearchCacheInfoListPtr mSearchCacheData;

	} mWorker;
};


OfflineJobCacheProxy::OfflineJobCacheProxy() :
		mIsOnline(false)
{

}

OfflineJobCacheProxy::~OfflineJobCacheProxy()
{
}

void OfflineJobCacheProxy::createDB(const QMetaObject* dto)
{
	QString table;
	QStringList schema;
	QList<QStringList> indexclause;
	int cnt = dto->classInfoCount();
	for (int i=0;i<cnt;i++)
	{
		QMetaClassInfo mci = dto->classInfo(i);
		if (QString(mci.name()) == QStringLiteral("column"))
			schema.append(mci.value());
		else if (QString(mci.name()) == QStringLiteral("table"))
			table = mci.value();
		else if ( QString(mci.name()).indexOf(QStringLiteral("index")) > -1 )
		{
			QStringList s;
			s<<mci.name()<<mci.value();
			indexclause.append(s);
		}
	}


	// create table
	QString sql(QStringLiteral("CREATE TABLE IF NOT EXISTS %1(%2)").arg(table).arg(schema.join(QChar(','))));
	QSqlDatabase db = DatabaseHelper::instance()->connection(QStringLiteral("offline_%1.db").arg(table));
	db.exec(sql);

	// create index
	for (int i=0;i<indexclause.size();++i)
	{
		QString index = indexclause.at(i).at(0);
		QString column = indexclause.at(i).at(1);
		QString sql2(QStringLiteral("CREATE %1 IF NOT EXISTS %2_index_%3").arg(index).arg(table).arg(column));
		sql2.append(QStringLiteral(" ON %1(%2)").arg(table).arg(column));
		db.exec(sql2);
	}
}

void OfflineJobCacheProxy::onRegister()
{
	auto notifyMethod = std::bind(&OfflineJobCacheProxy::handleJobNotification, this, std::placeholders::_1);
	auto observer = IObserverPtr(new mvc::TObserver(notifyMethod, shareSelf()));
	observer->setObserverName(staticMetaObject.className());
	Facade::instance()->registerObserver(SearchConstants::ResponseSearch, observer);
	Facade::instance()->registerObserver(NavigationControllerConstants::ResponseOnlineStatus, observer);
	Facade::instance()->registerObserver(NavigationControllerConstants::ChangeOnlineStatus, observer);

	createDB(&SearchCacheDto::staticMetaObject);
	createDB(&PoiCacheDto::staticMetaObject);
	createDB(&PoiExtraCacheDto::staticMetaObject);
}

bool OfflineJobCacheProxy::handleJobNotification( const mvc::INotificationPtr& note )
{
	switch (note->getId())
	{
	case SearchConstants::ResponseSearch:
		if (mIsOnline)
		{
			auto data = note->getBody().toMap();
            if (!data.contains(SearchConstants::Result))
                break;

            // skip for searching keyword auto completion 2017.11.20 by lckxx
            auto reqdata = data[SearchConstants::ReqData].toMap();
            if (reqdata[SearchConstants::SearchType].toInt() == SearchRequestBase::SearchBySisulNamePrediction)
                 break;

            QObject* res = data[SearchConstants::Result].value<QObject*>();
            if (!res) {
                break;
            }
            SearchReplyBaseInterface* reply = qobject_cast<SearchReplyBaseInterface*>(res);
            if (!reply || reply->error()) {
                break;
            }
            SearchReplyInterface* searchReply = qobject_cast<SearchReplyInterface*>(res);
            if (!searchReply) {
                break;
            }

            int resCnt = searchReply->resultCount();
            if ( resCnt < 1) {
            	break;
            }

            QList<TPOI_SEARCH_LIST>* jobData = new QList<TPOI_SEARCH_LIST>;
            const TPOI_SEARCH_LIST *resData = searchReply->results();
            for (int i=resCnt-1;i>=0;--i)
            	jobData->prepend(resData[i]);

            SearchCacheJob* job = new SearchCacheJob(SearchCacheInfoListPtr(jobData));
            job->start();

            // move into search cache job
//            CacheJob<PoiCacheJob,QList<PoiCacheInfo>> job2;
//            job2.start(...);

            // move into search cache job
//            CacheJob<PoiExtraCacheJob,QList<PoiExtraCacheInfo>> job3;
//            job3.start(...);
		}
		break;
#if OFFLINE_CACHE_SEARCH_NOTI
    case SearchConstants::RequestCacheSearch: {
            auto req = note->getBody().toMap();
            if (req.contains(SearchConstants::Query)) {
                QString query = req[SearchConstants::Query].toString();
                SearchCacheInfoListPtr res = searchCache(query);
                sendNotification(SearchConstants::ResponseCacheSearch,QVariant::fromValue(res));
            }
        }
		break;
    case SearchConstants::RequestPoiCacheInfo: {
            auto poiId = note->getBody().toInt();
            PoiCacheInfoPtr poi = popPoiCacheInfo(poiId);
            sendNotification(SearchConstants::ResponsePoiCacheInfo,QVariant::fromValue(poi));
        }
		break;
#endif
	case NavigationControllerConstants::ResponseOnlineStatus:
	case NavigationControllerConstants::ChangeOnlineStatus:
		mIsOnline = note->getBody().toBool();
		break;
	default:
		return false;
	}
	return true;
}

PoiCacheInfoPtr OfflineJobCacheProxy::popPoiCacheInfo(int poiId) const
{
	PoiCacheInfoPtr poi;
	QString schema;
	QList<QStringList> indexclause;
	const QMetaObject* dto = &PoiCacheDto::staticMetaObject;
	QStringList cols;
	for (int i=0;i<dto->classInfoCount();i++)
	{
		QMetaClassInfo mci =  dto->classInfo(i);
		if (QStringLiteral("column") == mci.name())
		{
			QString colName = QString(mci.value()).split(QChar(' ')).at(0);
			cols.append(colName);
		}
	}

	QString sql(QStringLiteral("SELECT %1 FROM poi_cache WHERE poiId=%2").arg(cols.join(QChar(','))).arg(poiId));
	QSqlQuery query(DatabaseHelper::instance()->connection(QStringLiteral("offline_poi_cache.db")));
	if (query.exec(sql))
	{
		if (query.next())
		{
			PoiCacheInfo* pci = new PoiCacheInfo;
			pci->poiId = poiId;
			pci->dbKind = query.value(2).toInt();
			pci->navX   = query.value(3).toInt();
			pci->navY   = query.value(4).toInt();
			pci->centerX   = query.value(5).toInt();
			pci->centerY   = query.value(6).toInt();
			pci->rpFlag    = query.value(7).toInt();
			pci->dataKind  = query.value(8).toInt();
			pci->mlClass   = query.value(9).toInt();
			pci->parkYn   = query.value(10).toInt();
			pci->premiumGasolinYn   = query.value(11).toInt();
			pci->lowestOilYn        = query.value(12).toInt();
			pci->gasolinPrice       = query.value(13).toInt();
			pci->dieselPrice        = query.value(14).toInt();
			pci->lpgPrice           = query.value(15).toInt();
			pci->premiumGasolinPrice  = query.value(16).toInt();
			pci->premiumDieselPrice   = query.value(17).toInt();
			pci->premiumLpgPrice      = query.value(18).toInt();
			pci->oilBaseSdt           = query.value(19).toString();
			pci->navSeq               = query.value(20).toString();
			pci->oilCompany           = query.value(21).toString();
			pci->fullName             = query.value(22).toString();
			pci->aliasName            = query.value(23).toString();
			pci->address              = query.value(24).toString();
			pci->primaryBun           = query.value(25).toString();
			pci->secondaryBun         = query.value(26).toString();
			pci->lcdName              = query.value(27).toString();
			pci->mcdName              = query.value(28).toString();
			pci->scdName              = query.value(29).toString();
			pci->dcdName              = query.value(30).toString();
			pci->zipCode              = query.value(31).toString();
			pci->telNum               = query.value(32).toString();
			pci->webSite              = query.value(33).toString();
			pci->radius               = query.value(34).toString();
			pci->roadName             = query.value(35).toString();
			pci->bldNo1               = query.value(36).toString();
			pci->bldNo2               = query.value(37).toString();
			pci->pubTraffic           = query.value(38).toString();
			pci->stationId            = query.value(39).toString();
			poi.reset(pci);
		}
	}

	query.clear();
	query.finish();
	return poi;
}


SearchCacheInfoListPtr OfflineJobCacheProxy::searchCache(const QString& request) const
{
    int totalCount = 0;
    return searchCache(request, 0, 20, totalCount);
}

SearchCacheInfoListPtr OfflineJobCacheProxy::searchCache(const QString& request, int startIndex, int limit, int &totalCount, bool isLike/*=true*/) const
{
	SearchCacheInfoListPtr results;
	const QMetaObject* dto = &SearchCacheDto::staticMetaObject;
	QStringList cols;
	for (int i=0;i<dto->classInfoCount();i++)
	{
		QMetaClassInfo mci =  dto->classInfo(i);
		if (QStringLiteral("column") == mci.name())
		{
			QString colName = QString(mci.value()).split(QChar(' ')).at(0);
			cols.append(colName);
		}
	}

	QString requestWords=request.trimmed();
	QString condition;

	if (isLike) {
	    QStringList likeConditions;
	    likeConditions << requestWords;
	    QStringList words = requestWords.split(' ',QString::SkipEmptyParts);
	    // consider matching algorithm if condition is like and the request has a few words
	    if (words.size() > 1) {
	    	likeConditions << words.at(1)+char(' ')+words.at(0); // swap first and second with space
	    	likeConditions << words.at(0)+words.at(1); // trim space between first and second 
	    	likeConditions << words.at(1)+words.at(0); // swap first and second without space
	    }
	    const int cnt = likeConditions.size();
	    for (int i=0;i<cnt;i++) {
	    	condition.append(QStringLiteral("stdName LIKE '%%1%'").arg(likeConditions.at(i)));
            if (i < cnt-1) condition.append(QStringLiteral(" OR "));
	    }
    } else {
    	condition.append(QStringLiteral("stdName = %1").arg(requestWords));
    }

    QString sql(QStringLiteral("SELECT  %1 FROM search_cache WHERE %2 LIMIT %3, %4")
                .arg(cols.join(QChar(',')))
                .arg(condition)
                .arg(startIndex)
                .arg(limit)
                );

    QSqlQuery query(DatabaseHelper::instance()->connection(QStringLiteral("offline_search_cache.db")));
	if (query.exec(sql))
	{
        {
			QList<TPOI_SEARCH_LIST>* res = new QList<TPOI_SEARCH_LIST>;
			while (query.next())
			{
				TPOI_SEARCH_LIST search;
				memset(&search,0,sizeof(TPOI_SEARCH_LIST));
				search.nPoiID   = query.value(1).toInt();
				search.nAddrCode  = query.value(3).toInt();
				search.byRPFlag   = query.value(4).toInt();
				search.nDist      = query.value(5).toInt();
				search.nDeg       = query.value(6).toInt();
				search.nKind      = query.value(7).toInt();
				search.tp.x       = query.value(8).toInt();
				search.tp.y       = query.value(9).toInt();
				search.nOPIPoleCate = query.value(10).toInt();
				search.nOPIPrice    = query.value(11).toInt();
				search.sGatherTime.nYear  = query.value(12).toInt();
				search.sGatherTime.nMonth = query.value(13).toInt();
				search.sGatherTime.nDay   = query.value(14).toInt();
				search.sGatherTime.nHour  = query.value(15).toInt();
				search.sGatherTime.nMinute  = query.value(16).toInt();
				search.sGatherTime.nSecond  = query.value(17).toInt();
				search.sGatherTime.nWeek    = query.value(18).toInt();
				QString fullName = query.value(19).toString();
				QString stdName  = query.value(20).toString();
				QString address  = query.value(21).toString();
				QString phoneNum = query.value(22).toString();

				search.bHighGasoline = query.value(23).toBool();
				search.bLowestOilSt  = query.value(24).toBool();
				search.nOPIGasolinePrice = query.value(25).toInt();
				search.nOPIDieselPrice   = query.value(26).toInt();
				search.nOPILPGPrice      = query.value(27).toInt();
				search.nOPIHiGasolinePrice    = query.value(28).toInt();
				search.nOPIHiLPGPrice         = query.value(29).toInt();
				QString navSeq                = query.value(30).toString();
				QString oilCompany            = query.value(31).toString();

				if (!fullName.isEmpty())
					qstrncpy(search.szName,fullName.toUtf8().constData(),80);
				if (!stdName.isEmpty())
					qstrncpy(search.szStdName,stdName.toUtf8().constData(),80);
				if (!address.isEmpty())
					qstrncpy(search.szAddrName,address.toUtf8().constData(),80);
				if (!phoneNum.isEmpty())
					qstrncpy(search.szPhoneNumber,phoneNum.toLatin1().constData(),14);

				if (!navSeq.isEmpty())
					qstrncpy(search.szNavseq,navSeq.toLatin1().constData(),6);
				if (!oilCompany.isEmpty())
					qstrncpy(search.szOilCompany,oilCompany.toUtf8().constData(),15);

				res->prepend(search);
			}
			results.reset(res);
		}
	}
	query.clear();

    int totCount = 0;
    if(startIndex == 0) {
        QString sqlTxt(QStringLiteral("SELECT count(*) FROM search_cache WHERE %1")
                    .arg(condition) );

        if (query.exec(sqlTxt)) {
             if (query.next()) {
                totCount = query.value(0).toInt();
            }
        }
        query.clear();
        totalCount = totCount;
    }

    query.finish();

	return results;
}

} /* namespace SKT */

#include "OfflineJobCacheProxy.moc"
