#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include <QDebug>

#include "searchmanager.h"
#include "serversearch.h"
#include "simplesearch.h"
#include "textdecoder.h"
#include "MNS_Interface.h"
#include "NDDS_Interface.h"
#include "searchmanagertest.h"


namespace SKT {

class SearchWorker : public QThread
{
public:
    SearchWorker() : m_quit(false), m_owner(nullptr) {
        serverSearch = ServerSearch::sharedInstance();
        localSearch = SimpleSearch::sharedInstance();
    }

    virtual ~SearchWorker() {

    }

    SearchReplyBase* post(SearchReplyBase *reply) {
        if (reply) {
            QMutexLocker locker(&m_mutex);
            m_jobs.push_back(reply);
            m_jobNotEmpty.wakeAll();
        }
        return reply;
    }

    void stop() {
        QMutexLocker locker(&m_mutex);
        m_quit = true;
        m_jobNotEmpty.wakeAll();
    }

    void clear() {
        QMutexLocker locker(&m_mutex);
        SearchReplyBase *reply = NULL;
        while (!m_quit && !m_jobs.isEmpty()) {
            reply = m_jobs.first();
            m_jobs.pop_front();
            if (reply) {
                delete reply;
            }
        }
    }

    void setOwner(const QObject *owner) {
        m_owner = const_cast<QObject*> (owner);
    }

    void run() override {
        forever {
            SearchReplyBase *reply = NULL;

            m_mutex.lock();
            while (!m_quit && m_jobs.isEmpty()) {
                m_jobNotEmpty.wait(&m_mutex);
            }
            if (m_quit) {
                m_mutex.unlock();
                break;
            }
            reply = m_jobs.first();
            m_jobs.pop_front();
            m_mutex.unlock();

            if (reply) {
                const SearchRequestBasePtr &req = reply->request();
                switch (req->searchType()) {
                case SearchRequestBase::SearchBySisulName:
                    if(req->getOnline()) {
                        if (serverSearchAgent(req->searchType(), req, reply)){
                            break;
                        }
                    }                    
                    if (qSharedPointerCast<SearchRequest>(req)->categoryCode() == 30101) {//아파트
                        localSearch->searchByCategory(qSharedPointerCast<SearchRequest>(req), static_cast<SearchReply*>(reply));
                    } else {
                        localSearch->searchBySisulName(qSharedPointerCast<SearchRequest>(req), static_cast<SearchReply*>(reply));
                    }
                    // REM [testcode : sangbong.woo] serverSearch->searchByPOIdetailInfo(qSharedPointerCast<SearchRequest>(req), static_cast<SearchReply*>(reply));
                    break;
                case SearchRequestBase::SearchBySisulNamePrediction:
                   if(req->getOnline()) {
                       serverSearch->searchBySisulNamePrediction(qSharedPointerCast<SearchNotNddsRequest>(req), static_cast<SearchSisulNamePredictionReply*>(reply));
                       break;
                   }
                    localSearch->searchBySisulName(qSharedPointerCast<SearchRequest>(req), static_cast<SearchReply*>(reply));

                    //serverSearch->searchByServerSisulName(qSharedPointerCast<SearchRequest>(req), static_cast<SearchReply*>(reply));
                    break;
                case SearchRequestBase::SearchGeoCoding:
                case SearchRequestBase::SearchAddress:
                      serverSearch->searchByGeoCoding(qSharedPointerCast<SearchNotNddsRequest>(req), static_cast<SearchGeoCodingReply*>(reply));
                    break;
                case SearchRequestBase::SearchComplexImage:
                     if (req->getOnline()) {
                         serverSearch->searchByComplexImage(qSharedPointerCast<SearchNotNddsRequest>(req), static_cast<SearchComplexImageReply*>(reply));
                         break;
                     }
                    break;
                case SearchRequestBase::SearchAQI:
                      serverSearch->searchByAQI(qSharedPointerCast<SearchNotNddsRequest>(req), static_cast<SearchAqiReply*>(reply));
                    break;
                    
                case SearchRequestBase::SearchBySisulCategory:
                    localSearch->searchByCategory(qSharedPointerCast<SearchRequest>(req), static_cast<SearchReply*>(reply));
                    break;
                case SearchRequestBase::SearchBySisulAround:
                    if(req->getOnline()) {
                        if (serverSearchAgent(req->searchType(), req, reply)){
                            break;
                        }
                    }
                    localSearch->searchBySisulAround(qSharedPointerCast<SearchRequest>(req), static_cast<SearchReply*>(reply));
                    break;
                case SearchRequestBase::SearchByAddress:
                    if(req->getOnline()) {
                        if (serverSearchAgent(req->searchType(), req, reply)){
                            break;
                        }
                    }                    
                    localSearch->searchByAddress(qSharedPointerCast<SearchRequest>(req), static_cast<SearchReply*>(reply));
                    break;
                case SearchRequestBase::SearchByAddressVr:
                    if(req->getOnline()) {
                        serverSearch->searchByAddressVr(qSharedPointerCast<SearchRequest>(req), static_cast<SearchReply*>(reply));
                    }
                    else {
                        localSearch->searchByAddressVr(qSharedPointerCast<SearchRequest>(req), static_cast<SearchReply*>(reply));
                    }
                    break;
                case SearchRequestBase::SearchNextAddress:
                    localSearch->searchNextAddress(qSharedPointerCast<SearchNextAddrRequest>(req), static_cast<SearchNextAddrReply*>(reply));
                    break;
                case SearchRequestBase::SearchNextCategory:
                    localSearch->searchNextCategory(qSharedPointerCast<SearchNextCateRequest>(req), static_cast<SearchNextCateReply*>(reply));
                    break;
                case SearchRequestBase::SearchByPOIid:
                    if(req->getOnline()) {
                        serverSearch->searchByPOIdetailInfo(qSharedPointerCast<SearchRequest>(req), static_cast<SearchReplyPoiDetail*>(reply));
                    } else {
                        // Local은 정보를 얻을 수 없음
                        localSearch->searchByPOIdetailInfo(qSharedPointerCast<SearchRequest>(req), static_cast<SearchReplyPoiDetail*>(reply));
                    }
                    break;
                case SearchRequestBase::SearchByEvPOIid:
                    if(req->getOnline()) {
                        if (serverSearchAgent(req->searchType(), req, reply)){
                            break;
                        }
                    }
                    localSearch->searchByEvPOIdetailInfo(qSharedPointerCast<SearchRequest>(req), static_cast<SearchReplyEvPoiDetail*>(reply));
                    break;
                case SearchRequestBase::SearchLowestOilSt:
                    if(req->getOnline()) {
                        serverSearch->searchPoiStationInfo(qSharedPointerCast<SearchRequest>(req), static_cast<SearchReplyPoiStation*>(reply));
                    } else {
                        // Local은 정보를 얻을 수 없음
                        localSearch->searchPoiStationInfo(qSharedPointerCast<SearchRequest>(req), static_cast<SearchReplyPoiStation*>(reply));
                    }
                    break;
                case SearchRequestBase::SearchRecentDestination:
                    serverSearch->searchRecentDestination(qSharedPointerCast<SearchRequest>(req), static_cast<SearchRecentDestinationReply*>(reply));
                    break;
                case SearchRequestBase::SearchFavorites:
                	serverSearch->searchFavorites(req, static_cast<SearchFavoriteReply*>(reply));
                    break;
                case SearchRequestBase::SearchMyHomeOffice:
                    serverSearch->searchMyHomeOffice(req, static_cast<SearchMyHomeOfficeReply*>(reply));
                    break;
                case SearchRequestBase::SearchRegisterSendToCar:
                    serverSearch->searchRegisterSendToCar(req, static_cast<SearchSendToCarReply*>(reply));
                    break;
                case SearchRequestBase::SearchFindSendToCar:
                    serverSearch->searchFindSendToCar(req, static_cast<SearchSendToCarReply*>(reply));
                    break;
                case SearchRequestBase::SearchRemoveSendToCar:
                    serverSearch->searchRemoveSendToCar(req, static_cast<SearchSendToCarReply*>(reply));
                    break;
                case SearchRequestBase::UploadAllRecentDestination:
                     serverSearch->uploadAllRecentDestination(req.staticCast<UploadAllRecentDestinationRequest>(), static_cast<UploadReplyBase*>(reply));
                    break;
                case SearchRequestBase::UploadAllFavorite:
                     serverSearch->uploadAllFavorite(req.staticCast<UploadAllFavoriteRequest>(), static_cast<UploadReplyBase*>(reply));
                    break;
                case SearchRequestBase::RegistMultiFavorites:
                    serverSearch->registMultiFavorites(req.staticCast<RegistMultiFavoritesRequest>(), static_cast<UploadReplyBase*>(reply));
                    break;
                case SearchRequestBase::RemoveMultiFavorites:
                    serverSearch->removeMultiFavorites(req.staticCast<RemoveMultiFavoritesRequest>(), static_cast<UploadReplyBase*>(reply));
                    break;
                case SearchRequestBase::RemoveMultiRecentDestinations:
                    serverSearch->removeMultiRecentDestinations(req.staticCast<RemoveMultiRecentDestinationsRequest>(), static_cast<UploadReplyBase*>(reply));
                    break;
                case SearchRequestBase::ModifyMultiFavorites:
                    serverSearch->modifyFavorite(req.staticCast<ModifyFavoriteRequest>(), static_cast<UploadReplyBase*>(reply));
                    break;
                case SearchRequestBase::SearchByEvAround:
                    if(req->getOnline()) {
                        if (serverSearchAgent(req->searchType(), req, reply)){
                            break;
                        }
                    }
                    localSearch->searchBySisulAround(qSharedPointerCast<SearchRequest>(req), static_cast<SearchReply*>(reply));
                    break;
                default:
                    qDebug() << "Not implemented search type:" << req->searchType();
                    break;
                }
            }
            if (m_owner) {
                QMetaObject::invokeMethod(m_owner, "queueChanged", Qt::QueuedConnection, Q_ARG(int, m_jobs.count()));
            }
        }
    }

    bool serverSearchAgent(const int type, const SearchRequestBasePtr &req, SearchReplyBase *reply) {
        QVariantMap reqData = req->extra();
        reqData["goLocalSearch"] = true;
        req->setExtra(reqData);
        switch(type) {
        case SearchRequestBase::SearchBySisulName:
            serverSearch->searchByServerSisulName(qSharedPointerCast<SearchRequest>(req), static_cast<SearchReply*>(reply));
            break;
        case SearchRequestBase::SearchBySisulAround:
            serverSearch->searchByServerSisulAround(qSharedPointerCast<SearchRequest>(req), static_cast<SearchReply*>(reply));
            break;
        case SearchRequestBase::SearchByEvAround:
           serverSearch->searchByEvAround(qSharedPointerCast<SearchRequest>(req), static_cast<SearchReply*>(reply));
           break;
        case SearchRequestBase::SearchByAddress:
             serverSearch->searchByServerAddress(qSharedPointerCast<SearchRequest>(req), static_cast<SearchReply*>(reply));
             break;
        case SearchRequestBase::SearchByEvPOIid:
            serverSearch->searchByEvPOIdetailInfo(qSharedPointerCast<SearchRequest>(req), static_cast<SearchReplyEvPoiDetail*>(reply));
            break;
        }
        return !goLocalSearch(type, req, reply);
    }

    bool goLocalSearch(const int type, const SearchRequestBasePtr &req, SearchReplyBase *reply) {
        switch(type) {
        case SearchRequestBase::SearchBySisulName:
        case SearchRequestBase::SearchBySisulAround:
        case SearchRequestBase::SearchByEvAround:
        case SearchRequestBase::SearchByAddress: {
                if (req->startIndex() > 0) {
                    return false;
                }
                SearchReply *rep = static_cast<SearchReply*>(reply);
                if (!rep->error()) {
                    qDebug()<<"SearchWorker:serverSearch=>[success]type:"<<type<<", error:"<<rep->error()<<", cause:"<<rep->errorCause()<<",totalCount:"<<rep->totalCount()<<",resultCount:"<<rep->resultCount()<<",return:false";
                    if (rep->totalCount() > 0) {
                        return false;
                    }
                }
                qDebug()<<"SearchWorker:serverSearch=>[fail]type:"<<type<<", error:"<<rep->error()<<", cause:"<<rep->errorCause()<<",totalCount:"<<rep->totalCount()<<",resultCount:"<<rep->resultCount()<<",return:true";
            }
            break;
        case SearchRequestBase::SearchByEvPOIid: {
                SearchReplyEvPoiDetail *rep = static_cast<SearchReplyEvPoiDetail*>(reply);
                if (!rep->error()) {
                    qDebug()<<"SearchWorker:serverSearch=>[success]type:"<<type<<", error:"<<rep->error()<<", cause:"<<rep->errorCause()<<",totalCount:"<<rep->totalCount()<<",resultCount:"<<rep->resultCount()<<",return:false";
                    if (rep->totalCount() > 0) {
                        return false;
                    }
                }
                qDebug()<<"SearchWorker:serverSearch=>[fail]type:"<<type<<", error:"<<rep->error()<<", cause:"<<rep->errorCause()<<",totalCount:"<<rep->totalCount()<<",resultCount:"<<rep->resultCount()<<",return:true";
            }
            break;
        }
        //로컬 검색을 위하여 초기화
        reply->m_error = false;
        reply->m_errorCause = DE_ERROR_SUCCESS;
        req->setOnline(false);
        req->setLineChanged(true);
        if (m_owner) {
            QMetaObject::invokeMethod(m_owner, "queueChanged", Qt::QueuedConnection, Q_ARG(int, 1));
            QMetaObject::invokeMethod(m_owner, "queueChanged", Qt::QueuedConnection, Q_ARG(int, 0));
        }
        return true;
    }

private:
    ServerSearch *serverSearch;
    SimpleSearch *localSearch;
    bool m_quit;
    QMutex m_mutex;
    QWaitCondition m_jobNotEmpty;
    QList<SearchReplyBase*> m_jobs;
    QObject *m_owner;
};

SearchManager::SearchManager()
{
    m_worker = new SearchWorker();
    m_worker->start();
}

SearchManager::~SearchManager()
{
    m_worker->stop();
    m_worker->wait();
    delete m_worker;
}

SearchReplyBase* SearchManager::createReply(const SearchRequestBasePtr &req, QObject *parent)
{
    SearchReplyBase* reply = NULL;

    switch (req->searchType()) {
    case SearchRequestBase::SearchBySisulNamePrediction: {
        if (req->getOnline()) {
            reply = new SearchSisulNamePredictionReply(req,parent);
            break;
        }
    }
    case SearchRequestBase::SearchBySisulName:
    case SearchRequestBase::SearchBySisulAround:
    case SearchRequestBase::SearchByAddress:
    case SearchRequestBase::SearchBySisulCategory:
    case SearchRequestBase::SearchByServerAddress:
    case SearchRequestBase::SearchByAddressVr:
    case SearchRequestBase::SearchByEvAround:
        reply = new SearchReply(req, parent);
        break;
    case SearchRequestBase::SearchAddress:
    case SearchRequestBase::SearchGeoCoding:
         reply = new SearchGeoCodingReply(req, parent);
         break;
    case SearchRequestBase::SearchComplexImage:
         reply = new SearchComplexImageReply(req, parent);
         break;
    case SearchRequestBase::SearchAQI:
         reply = new SearchAqiReply(req, parent);
         break;
    case SearchRequestBase::SearchRecentDestination:
        reply = new SearchRecentDestinationReply(req, parent);
        break;
    case SearchRequestBase::SearchNextAddress:
        reply = new SearchNextAddrReply(req, parent);
        break;
    case SearchRequestBase::SearchNextCategory:
        reply = new SearchNextCateReply(req, parent);
        break;
    case SearchRequestBase::SearchByPOIid:
        reply = new SearchReplyPoiDetail(req, parent);
        break;
    case SearchRequestBase::SearchByEvPOIid:
        reply = new SearchReplyEvPoiDetail(req, parent);
        break;
    case SearchRequestBase::SearchLowestOilSt:
        reply = new SearchReplyPoiStation(req, parent);
        break;
    case SearchRequestBase::SearchFavorites:
        reply = new SearchFavoriteReply(req, parent);
        break;
    case SearchRequestBase::SearchMyHomeOffice:
        reply = new SearchMyHomeOfficeReply(req, parent);
        break;
    case SearchRequestBase::SearchRegisterSendToCar:
        reply = new SearchSendToCarReply(req, parent);
        break;
    case SearchRequestBase::SearchFindSendToCar:
        reply = new SearchSendToCarReply(req, parent);
        break;
    case SearchRequestBase::SearchRemoveSendToCar:
        reply = new SearchSendToCarReply(req, parent);
        break;
    case SearchRequestBase::UploadAllRecentDestination:
    case SearchRequestBase::UploadAllFavorite:
    case SearchRequestBase::UploadAllFavoritesAndResetHomeOffice:
    case SearchRequestBase::UploadAllFavoritesAndRegistHomeOffice:
    case SearchRequestBase::RegistNewFavorite:
    case SearchRequestBase::RegistMultiFavorites:
    case SearchRequestBase::ModifyFavorite:
    case SearchRequestBase::ModifyMultiFavorites:
    case SearchRequestBase::RemoveMultiFavorites:
    case SearchRequestBase::RemoveMultiRecentDestinations:
        reply = new UploadReplyBase(req, parent);
        break;
    }

    return reply;
}

SearchReplyBaseInterface* SearchManager::request(const SearchRequestBasePtr &req)
{
    return m_worker->post(createReply(req, this));
}

void SearchManager::clear()
{
    return m_worker->clear();
}

void SearchManager::setOwner(const QObject *owner)
{
    m_worker->setOwner(owner);
}

}
