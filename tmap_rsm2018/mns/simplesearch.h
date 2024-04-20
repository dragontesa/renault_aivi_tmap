#ifndef SIMPLESEARCH_H
#define SIMPLESEARCH_H

#include <QObject>
#include <QSharedPointer>
#include "searchmanager.h"
#include "searchrequest.h"


namespace SKT {

class SearchReply;

class SimpleSearch
{

public:
    explicit SimpleSearch();
    ~SimpleSearch();

    static SimpleSearch* sharedInstance();

    void searchBySisulName(const QSharedPointer<SearchRequest> &req, SearchReply *reply);
    void searchBySisulAround(const QSharedPointer<SearchRequest> &req, SearchReply *reply);
    void searchByCategory(const QSharedPointer<SearchRequest> &req, SearchReply *reply);
    void searchByAddress(const QSharedPointer<SearchRequest> &req, SearchReply *reply);
    void searchNextAddress(const QSharedPointer<SearchNextAddrRequest> &addrReq, SearchNextAddrReply *reply);
    void searchNextCategory(const QSharedPointer<SearchNextCateRequest> &addrReq, SearchNextCateReply *reply);
    void searchByPOIdetailInfo(const QSharedPointer<SearchRequest> &req, SearchReplyPoiDetail *reply);
    void searchByEvPOIdetailInfo(const QSharedPointer<SearchRequest> &req, SearchReplyEvPoiDetail *reply);
    void searchPoiStationInfo(const QSharedPointer<SearchRequest> &req, SearchReplyPoiStation *reply);
    void searchByAddressVr(const QSharedPointer<SearchRequest> &req, SearchReply *reply);

    TeSearchSort toTeSearchSort(SearchRequestBase::SortType sortType);
    void doSearch(const QSharedPointer<SearchRequest> &req, SearchReply *reply, TS_SEARCH_REQ &sSearchReq);
    void doSearchByRicode(const QSharedPointer<SearchRequest> &req, SearchReply *reply, TS_SEARCH_REQ &sSearchReq);
    void sendFinished(SearchReplyBase *reply);

    TPOI_SEARCH_LIST* conv2TmapSearchList(TS_SEARCH_LIST* result, int resultCount);
    TS_POINT getPosition() { return m_currentTp; }
    void setPosition(TS_POINT tp){ m_currentTp.x = tp.x; m_currentTp.y = tp.y; }

public slots:
    SearchReplyBase* request(const SearchRequestBasePtr &req);

private:
    static SimpleSearch* s_instance;
    TS_POINT m_currentTp;

    Q_DISABLE_COPY(SimpleSearch)
};

}   //namespace SKT

#endif // SIMPLESEARCH_H
