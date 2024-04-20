#ifndef SERVERSEARCH_H
#define SERVERSEARCH_H

#include <QObject>
#include <QSharedPointer>
#include "searchmanager.h"
#include "searchmanagertest.h"
#include "searchrequest.h"


namespace SKT {

class ServerSearch
{

public:
    explicit ServerSearch();
    ~ServerSearch();

    static ServerSearch* sharedInstance();
    void searchByServerSisulName(const QSharedPointer<SearchRequest> &req, SearchReply *reply);
    void searchByServerSisulAround(const QSharedPointer<SearchRequest> &req, SearchReply *reply);
    void searchByAddressVr(const QSharedPointer<SearchRequest> &req, SearchReply *reply);
    void searchByServerAddress(const QSharedPointer<SearchRequest> &req, SearchReply *reply);
    void searchByServerNextAddress(const QSharedPointer<SearchRequest> &req, SearchReply *reply);
    void searchByPOIdetailInfo(const QSharedPointer<SearchRequest> &req, SearchReplyPoiDetail *reply);
    void searchByEvPOIdetailInfo(const QSharedPointer<SearchRequest> &req, SearchReplyEvPoiDetail *reply);
    void searchPoiStationInfo(const QSharedPointer<SearchRequest> &req, SearchReplyPoiStation *reply);
    void searchRecentDestination(const QSharedPointer<SearchRequest> &req, SearchRecentDestinationReply *reply);
    void searchFavorites(const SearchRequestBasePtr &req, SearchFavoriteReply *reply);
    void searchMyHomeOffice(const SearchRequestBasePtr &req, SearchMyHomeOfficeReply *reply);
    void searchRegisterSendToCar(const SearchRequestBasePtr &req, SearchSendToCarReply *reply);
    void searchFindSendToCar(const SearchRequestBasePtr &req, SearchSendToCarReply *reply);
    void searchRemoveSendToCar(const SearchRequestBasePtr &req, SearchSendToCarReply *reply);
    void searchByEvAround(const QSharedPointer<SearchRequest> &req, SearchReply *reply);
    void searchBySisulNamePrediction(const QSharedPointer<SearchNotNddsRequest> &req, SearchSisulNamePredictionReply *reply);
    void searchByGeoCoding(const QSharedPointer<SearchNotNddsRequest> &req, SearchGeoCodingReply *reply);
    void searchByComplexImage(const QSharedPointer<SearchNotNddsRequest> &req, SearchComplexImageReply *reply);
    void searchByAQI(const QSharedPointer<SearchNotNddsRequest> &req, SearchAqiReply *reply);
    const DS_SEARCH_POI_DETAIL_INFO* getPoiDetailInfoById(int poiid, const char* navseq);

    int toServer(int id);

    DeServerSearchSort toDeSearchSort(SearchRequestBase::SortType sortType);

    void doServerSearch(const QSharedPointer<SearchRequest> &req, SearchReply *reply, DS_SEARCH_REQ &sNddsSearchReq);
    void sendFinished(SearchReplyBase *reply);

    void uploadAllRecentDestination(QSharedPointer<UploadAllRecentDestinationRequest> req, UploadReplyBase *reply );
    void uploadAllFavorite(QSharedPointer<UploadAllFavoriteRequest> req, UploadReplyBase* reply );
    void registMultiFavorites(QSharedPointer<RegistMultiFavoritesRequest> req, UploadReplyBase* reply );
    void removeMultiFavorites(QSharedPointer<RemoveMultiFavoritesRequest> req, UploadReplyBase* reply );
    void modifyFavorite(QSharedPointer<ModifyFavoriteRequest> req, UploadReplyBase* reply );
    void removeMultiRecentDestinations(QSharedPointer<RemoveMultiRecentDestinationsRequest> req, UploadReplyBase* reply);

    // void uploadHomeOffice(UploadRequestBasePtr req, UploadReplyBase* reply );
    // void uploadMySetting(UploadRequestBasePtr req, UploadReplyBase* reply );

    TPOI_SEARCH_LIST* conv2TmapSearchList(DS_SEARCH_LIST* result, int resultCount);
    TPOI_DETAIL_INFO* conv2TmapPoiDetailInfo(DS_SEARCH_POI_DETAIL_INFO* result, int resultCount);
    TPOI_EvDETAIL_INFO* conv2TmapPoiEvDetailInfo(DS_EVSTATION_INFO* result, int resultCount);
    TPOI_STATION_INFO* conv2TmapPoiStationInfo(DS_SEARCH_POI_STATION_INFO* result, int resultCount);

    const TPOI_FAVORITE_INFO* conv2TmapFavoriteInfo(const DS_FAVORITE_LIST* result, int resultCount);
    const TPOI_MYHOMEOFFICE_INFO* conv2TmapMyHomeOfficeInfo(const DS_MYFAVORITE* result);
    const TPOI_RECENTDESTINATION_INFO* conv2TmapRecentDestinationInfo(const DS_SEARCH_RECENT_DESTNATION* result, int resultCount);
    const TPOI_SENDTOCAR_INFO* conv2TmapFindSendToCarInfo(const DS_SENDTOCARINFO* result, int resultCount);
    const TPOI_SENDTOCAR_INFO* conv2TmapRemoveSendToCarInfo(const DS_SENDTOCARINFO* result, int resultCount);
    bool checkStrCompare(QString reqStr, const char* token);
    int conv2OPIPoleCate(const char* reqStr, int poleKind);

    DS_POINT getPosition() { return m_currentTp; }
    void setPosition(DS_POINT tp){ m_currentTp.x = tp.x; m_currentTp.y = tp.y; }

public slots:
    SearchReplyBase* request(const SearchRequestBasePtr &req);

private:
    static ServerSearch* s_instance;
    DS_POINT m_currentTp;

    Q_DISABLE_COPY(ServerSearch)
};

}   //namespace SKT
#endif // SERVERSEARCH_H
