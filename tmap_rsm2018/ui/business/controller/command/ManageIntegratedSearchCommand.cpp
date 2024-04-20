#include "ManageIntegratedSearchCommand.h"
#include <QQmlProperty>
#include "Constants.h"
#include "SearchConstants.h"
#include "RequestCommand.h"
#include "IntegratedSearchProxy.h"
#include "OfflineJobCacheProxy.h"
#include "NavibookmarkAgentProxy.h"
#include "VsmNaviProxy.h"
#include "UtilConstants.h"
#include "NavigationControllerConstants.h"

#include "RecentDestinationDto.h"
#include "FavoriteDto.h"

#include "NavigationControllerProtocol.h"
#include "TStringUtils.h"

#include "uiconstants.h"
#include "RgConstants.h"
#include "ApplicationConstants.h"

using namespace SKT;

ManageIntegratedSearchCommand::ManageIntegratedSearchCommand(QObject *parent)
    : SimpleCommand(parent)
{
}

ManageIntegratedSearchCommand::~ManageIntegratedSearchCommand()
{
}

void ManageIntegratedSearchCommand::resultCache(const QVariantMap &data, bool always /*=false*/)
{
    auto reqCache = data;
    if (reqCache[SearchConstants::SearchType].toInt() != SearchRequestBase::SearchBySisulNamePrediction &&
        reqCache[SearchConstants::SearchType].toInt() != SearchRequestBase::SearchBySisulName) {
        return;
    }

    if (!reqCache.contains(SearchConstants::TextParam))
        return;

    int startIndex = 0;
    int totalCount = 0;
    SearchCacheInfoListPtr pData;
    pData.clear();

    auto searchProxy = Facade::instance()->retrieveProxy<IntegratedSearchProxy>();
    if (!searchProxy->getOnlineState() || always) {
        QString query = reqCache[SearchConstants::TextParam].toString();
        auto searchProxyCache = Facade::instance()->retrieveProxy<OfflineJobCacheProxy>();        
        int count = 0;
        auto params = reqCache[SearchConstants::Params].value<QVariantMap>();
        if (params.contains(SearchConstants::StartIndex)){
            startIndex = params[SearchConstants::StartIndex].value<int>();
        }
        if (params.contains(SearchConstants::CacheCount)){
            count = params[SearchConstants::CacheCount].value<int>();
        }
        if (startIndex > 0)
            return;
        pData = searchProxyCache->searchCache(query, startIndex, count, totalCount, true);
    }

    reqCache.insert(SearchConstants::Max, QVariant::fromValue(totalCount));

    QVariantMap res;
    res.insert(SearchConstants::Result, QVariant::fromValue(pData));
    res.insert(SearchConstants::ReqData,  QVariant::fromValue(reqCache));
    sendNotification(SearchConstants::ResponseCacheSearch,QVariant::fromValue(res));
}

void ManageIntegratedSearchCommand::resultFavor(const QVariantMap &data)
{
    auto reqFavor = data;
    if (reqFavor[SearchConstants::SearchType].toInt() != SearchRequestBase::SearchBySisulNamePrediction &&
        reqFavor[SearchConstants::SearchType].toInt() != SearchRequestBase::SearchByPOIid)
        return;

    if (!reqFavor.contains(SearchConstants::TextParam))
        return;

    QString query = reqFavor[SearchConstants::TextParam].toString();
    auto searchProxyFavor = Facade::instance()->retrieveProxy<NavibookmarkAgentProxy>();
    int startIndex = 0;
    int count = 0;
    auto params = reqFavor[SearchConstants::Params].value<QVariantMap>();
    if (params.contains(SearchConstants::StartIndex)){
        startIndex = params[SearchConstants::StartIndex].value<int>();
    }
    if (params.contains(SearchConstants::FavorCount)){
        count = params[SearchConstants::FavorCount].value<int>();
    }

    if (startIndex > 0)
        return;

    QList<FavoriteDtoPtr> pData = searchProxyFavor->searchFavorite(query, count);

    reqFavor.insert(SearchConstants::Max, QVariant::fromValue(pData.count()));
    QVariantMap res;
    res.insert(SearchConstants::Result, QVariant::fromValue(pData));
    res.insert(SearchConstants::ReqData,  QVariant::fromValue(reqFavor));
    sendNotification(SearchConstants::ResponseFavorSearch,QVariant::fromValue(res));

}

void ManageIntegratedSearchCommand::resultRcntDest(const QVariantMap &data)
{
    auto reqDest = data;
    if (reqDest[SearchConstants::SearchType].toInt() != SearchRequestBase::SearchBySisulNamePrediction)
        return;

    if (!reqDest.contains(SearchConstants::TextParam))
        return;

    QString query = reqDest[SearchConstants::TextParam].toString();
    auto searchProxyDest = Facade::instance()->retrieveProxy<NavibookmarkAgentProxy>();
    int startIndex = 0;
    int count = 0;
    auto params = reqDest[SearchConstants::Params].value<QVariantMap>();
    if (params.contains(SearchConstants::StartIndex)){
        startIndex = params[SearchConstants::StartIndex].value<int>();
    }
    if (params.contains(SearchConstants::DestiCount)){
        count = params[SearchConstants::DestiCount].value<int>();
    }

    if (startIndex > 0)
        return;

    QList<RecentDestinationDtoPtr> pData = searchProxyDest->searchRecentDestination(query, count);

    reqDest.insert(SearchConstants::Max, QVariant::fromValue(pData.count()));
    QVariantMap res;
    res.insert(SearchConstants::Result, QVariant::fromValue(pData));
    res.insert(SearchConstants::ReqData,  QVariant::fromValue(reqDest));
    sendNotification(SearchConstants::ResponseDestSearch,QVariant::fromValue(res));
}

void ManageIntegratedSearchCommand::requestGadgetSearchMode()
{
    auto proxy = Facade::instance()->retrieveProxy<IntegratedSearchProxy>();
    auto isGadgetSearchMode = proxy->isGadgetSearchMode();
    sendNotification(SearchConstants::ReplyToGadgetSearchMode, isGadgetSearchMode);
}

void ManageIntegratedSearchCommand::setGadgetSearchMode(bool b)
{
    auto proxy = Facade::instance()->retrieveProxy<IntegratedSearchProxy>();
    proxy->setGadgetSearchMode(b);
}

void ManageIntegratedSearchCommand::setCurrentGpsTime(QDateTime currentTime)
{
    auto proxy = Facade::instance()->retrieveProxy<IntegratedSearchProxy>();
    proxy->setCurrentGpsTime(currentTime);
}

void ManageIntegratedSearchCommand::requestGpsTime()
{
    auto proxy = Facade::instance()->retrieveProxy<IntegratedSearchProxy>();
    QDateTime currentTime = proxy->getCurrentGpsTime();
    sendNotification(SearchConstants::ResponseGpsTime, currentTime);
}

bool ManageIntegratedSearchCommand::execute(const mvc::INotificationPtr &noti)
{
    switch (noti->getId()) {
    case SearchConstants::RequestSearch: {
        auto owner = noti->getSource().data();
        QVariantMap data = noti->getBody().value<QVariantMap>();
        data[CommonConstants::Owner] = QVariant(reinterpret_cast<qulonglong>(owner));
        if (data.contains(SearchConstants::SearchType)) {
            const int searchType = data[SearchConstants::SearchType].toInt();
#if 0
            switch(searchType) {
                case SearchRequestBase::SearchByPOIid:
                case SearchRequestBase::SearchByEvPOIid:
                case SearchRequestBase::SearchLowestOilSt:
                case SearchRequestBase::SearchBySisulCategory:
                case SearchRequestBase::SearchBySisulAround:
                case SearchRequestBase::SearchAddress:
                case SearchRequestBase::SearchByEvAround:
                case SearchRequestBase::SearchBySisulNamePrediction:
                case SearchRequestBase::SearchBySisulName:
                case SearchRequestBase::SearchAQI:
                case SearchRequestBase::RemoveSearchQueue:
                     break;
                case SearchRequestBase::SearchNextAddress:
                default:
                    sendNotification( UtilConstants::ShowProgressIndicator );
                    break;
            }
#endif

            if (searchType != SearchRequestBase::RemoveSearchQueue) {
                ///////////////////////////////////////////////////////////////////////
                //[MonkeyTest]를 위하여
                QVariantMap m;
                m.insert(CommonConstants::Name,    SearchConstants::SearchView);
                m.insert(CommonConstants::Value,   true);
                m.insert(CommonConstants::Type,    data[SearchConstants::SearchType]);
                m.insert(CommonConstants::Keyword, data[SearchConstants::Query] );
                sendNotification( SearchConstants::SearchWait, m);
                //[MonkeyTest]를 위하여
                ///////////////////////////////////////////////////////////////////////
            }

            if (searchType == SearchRequestBase::SearchBySisulNamePrediction ||
                searchType == SearchRequestBase::SearchBySisulName) {
                //캐시 검색
                qDebug() << "[Perf][ManageIntegratedSearchCommand][RequestSearch]before resultCache";
                resultCache(data);
                qDebug() << "[Perf][ManageIntegratedSearchCommand][RequestSearch]after resultCache";
                //즐겨찾기 검색
                qDebug() << "[Perf][ManageIntegratedSearchCommand][RequestSearch]before resultFavor";
                resultFavor(data);
                qDebug() << "[Perf][ManageIntegratedSearchCommand][RequestSearch]after resultFavor";
                //최근목적지 검색
                qDebug() << "[Perf][ManageIntegratedSearchCommand][RequestSearch]before resultRcntDest";
                resultRcntDest(data);
                qDebug() << "[Perf][ManageIntegratedSearchCommand][RequestSearch]after resultRcntDest";
            }

            //온/오프라인검색
            auto notifyMethod = std::bind(&ManageIntegratedSearchCommand::onResponseCommand, this, std::placeholders::_1);
            auto req = t_new_shared<RequestCommand>( notifyMethod, noti->getDestination() , data) ;
            auto searchProxy = Facade::instance()->retrieveProxy<IntegratedSearchProxy>();
            qDebug() << "[Perf][ManageIntegratedSearchCommand][RequestSearch]before requestCommand";
            searchProxy->requestCommand( req );
            qDebug() << "[Perf][ManageIntegratedSearchCommand][RequestSearch]after requestCommand";
        } else {
            QVariantMap body;
            body.empty();
            if (data.contains(SearchConstants::Page)) {
                qDebug() << "[Perf][ManageIntegratedSearchCommand][ResponseSearch]Page";
                 auto page = data[SearchConstants::Page].toInt();
                 body.insert( SearchConstants::Page, page);
            }
            if (data.contains(SearchConstants::Query)) {
                qDebug() << "[Perf][ManageIntegratedSearchCommand][ResponseSearch]Query";
                auto query = data[SearchConstants::Query].toString();
                body.insert(SearchConstants::Query, query);
            }
            qDebug() << "[Perf][ManageIntegratedSearchCommand][ResponseSearch]";
            sendNotification( SearchConstants::ResponseSearch, body );
        }
    }
    break;

    case SearchConstants::RequestGadgetSearchMode:
        requestGadgetSearchMode();
        break;

    case SearchConstants::SetGadgetSearchMode:
        setGadgetSearchMode(noti->getBody().toBool());
        break;

    case SearchConstants::UpdateGpsTime:
        setCurrentGpsTime(noti->getBody().toDateTime());
        break;

    case SearchConstants::RequestGpsTime:
        requestGpsTime();
        break;

    case SearchConstants::RequestCacheSearch: {
            QVariantMap data = noti->getBody().value<QVariantMap>();
            resultCache(data, true);
        }
        break;

    default:
        return false;
    }
    return true;
}

bool ManageIntegratedSearchCommand::onResponseCommand( const mvc::INotificationPtr& note )
{
    qDebug() << "[Perf][ManageIntegratedSearchCommand][onResponseCommand]";

    auto req = note->getBody().value<RequestCommandPtr>();
    QVariantMap reqData = req->getReqData();
    QVariantMap resData = req->getResData();
    QVariantMap body;

    int respNotiId = SearchConstants::ResponseSearch;

    if ( reqData.contains(SearchConstants::ResponseNotificationId) ) {
        respNotiId = reqData[SearchConstants::ResponseNotificationId].toInt();
    }

    body.insert( SearchConstants::ReqData, reqData );
    if (resData.contains(SearchConstants::Result)) {
        body.insert( SearchConstants::Result, resData[SearchConstants::Result] );

        QObject* res = body[SearchConstants::Result].value<QObject*>();
        if (res){
            SearchReplyBaseInterface* reply = qobject_cast<SearchReplyBaseInterface*>(res);
            if (reply) {
                //검색후 에러 여부 확인을 위한 트레이스 로그를 남김
                int type = - 1;
                int totalCount = -1;
                int resultCount = -1;
                QString query = CommonConstants::Empty;
                if (reqData.contains(SearchConstants::SearchType)) {
                    type = reqData[SearchConstants::SearchType].toInt();
                }
                if (reqData.contains(SearchConstants::Query)) {
                   query = reqData[SearchConstants::Query].toString();
                }
                switch (type) {
                case SearchRequestBase::SearchBySisulNamePrediction: {
                    if (reply->request()->getOnline()) {
                        reqData.insert(CommonConstants::Error, reply->error());
                        SearchSisulNanmePredictionReplyInterface* searchReply = qobject_cast<SearchSisulNanmePredictionReplyInterface*>(res);
                        QByteArray result(searchReply->results()->rawdata);
                        auto searchProxy = Facade::instance()->retrieveProxy<IntegratedSearchProxy>();
                        searchProxy->onResultSearchBySisulNamePrediction2(reqData,result);
                        return true;
                    }
                }
                case SearchRequestBase::SearchBySisulName:
                case SearchRequestBase::SearchBySisulAround:
                case SearchRequestBase::SearchByAddress:
                case SearchRequestBase::SearchByAddressVr:
                case SearchRequestBase::SearchBySisulCategory:
                case SearchRequestBase::SearchByServerAddress: {
                    SearchReplyInterface* searchReply = qobject_cast<SearchReplyInterface*>(res);
                    totalCount = searchReply->totalCount();
                    resultCount = searchReply->resultCount();
                  }
                  break;
                case SearchRequestBase::SearchRecentDestination: {
                    SearchRecentDestinationReplyInterface* searchReply = qobject_cast<SearchRecentDestinationReplyInterface*>(res);
                    totalCount = searchReply->totalCount();
                    resultCount = searchReply->resultCount();
                    resultCount = searchReply->resultCount();
                   }
                   break;
                case SearchRequestBase::SearchNextAddress: {
                    SearchNextAddrReplyInterface* searchReply = qobject_cast<SearchNextAddrReplyInterface*>(res);
                    totalCount = searchReply->totalCount();
                    resultCount = searchReply->resultCount();
                  }
                  break;
                case SearchRequestBase::SearchNextCategory: {
                    SearchNextCateReplyInterface* searchReply = qobject_cast<SearchNextCateReplyInterface*>(res);
                    totalCount = searchReply->totalCount();
                    resultCount = searchReply->resultCount();
                  }
                  break;
                case SearchRequestBase::SearchByPOIid: {
                    SearchPoiReplyInterface* searchReply = qobject_cast<SearchPoiReplyInterface*>(res);
                    totalCount = searchReply->resultCount();
                    resultCount = searchReply->resultCount();
                  }
                  break;
                case SearchRequestBase::SearchByEvPOIid: {
                    SearchEvPoiReplyInterface* searchReply = qobject_cast<SearchEvPoiReplyInterface*>(res);
                    totalCount = searchReply->resultCount();
                    resultCount = searchReply->resultCount();
                  }
                  break;
                case SearchRequestBase::SearchLowestOilSt: {
                    SearchPoiStationReplyInterface* searchReply = qobject_cast<SearchPoiStationReplyInterface*>(res);
                    totalCount = searchReply->resultCount();
                    resultCount = searchReply->resultCount();
                  }
                  break;
                case SearchRequestBase::SearchFavorites: {
                    SearchFavoriteReplyInterface* searchReply = qobject_cast<SearchFavoriteReplyInterface*>(res);
                    totalCount = searchReply->totalCount();
                    resultCount = searchReply->resultCount();
                   }
                   break;
                case SearchRequestBase::SearchMyHomeOffice: {
                    SearchMyHomeOfficeReplyInterface* searchReply = qobject_cast<SearchMyHomeOfficeReplyInterface*>(res);
                    }
                    break;
                case SearchRequestBase::SearchRegisterSendToCar:
                case SearchRequestBase::SearchFindSendToCar:
                case SearchRequestBase::SearchRemoveSendToCar: {
                    SearchSendToCarReplyInterface* searchReply = qobject_cast<SearchSendToCarReplyInterface*>(res);
                    totalCount = searchReply->totalCount();
                    resultCount = searchReply->resultCount();
                    }
                    break;
                case SearchRequestBase::SearchAddress:
                case SearchRequestBase::SearchGeoCoding: {
                    reqData.insert(CommonConstants::Error, reply->error());
                    SearchGeoCodingReplyInterface* geocodingReply = qobject_cast<SearchGeoCodingReplyInterface*>(res);
                    QByteArray result(geocodingReply->results()->rawdata);
                    auto searchProxy = Facade::instance()->retrieveProxy<IntegratedSearchProxy>();
                    searchProxy->onResultSearchAddress2(reqData,result);
                }
                    return true;
                case SearchRequestBase::SearchAQI: {
                    SearchAqiReplyInterface* aqiReply = qobject_cast<SearchAqiReplyInterface*>(res);
                    QByteArray result(aqiReply->results()->rawdata);
                    auto searchProxy = Facade::instance()->retrieveProxy<IntegratedSearchProxy>();
                    searchProxy->onResultSearchAQI2(reqData,result);
                }
                    return true;
                case SearchRequestBase::UploadAllRecentDestination:
                case SearchRequestBase::UploadAllFavorite:
                case SearchRequestBase::UploadAllFavoritesAndResetHomeOffice:
                case SearchRequestBase::UploadAllFavoritesAndRegistHomeOffice:
                case SearchRequestBase::RegistNewFavorite:
                case SearchRequestBase::RegistMultiFavorites:
                case SearchRequestBase::ModifyFavorite:
                case SearchRequestBase::ModifyMultiFavorites:
                case SearchRequestBase::RemoveMultiFavorites:
                case SearchRequestBase::RemoveMultiRecentDestinations: {
                    UploadReplyBaseInterface* searchReply = qobject_cast<UploadReplyBaseInterface*>(res);
                    totalCount = searchReply->uploadedCount();
                    resultCount = searchReply->uploadedCount();
                    }
                    break;
                case SearchRequestBase::SearchComplexImage: {
                        SearchComplexImageReplyInterface* searchReply = qobject_cast<SearchComplexImageReplyInterface*>(res);                        
                        const TComplexImage* img = searchReply->results();
                        if (img) {
                            //auto str = TStringUtils::charArrayToHexString( img->rawdata, img->size  );
                            qDebug() << "[Cross]ManageIntegratedSearchCommand SearchComplexImage";
                            QImage image;
                            image.loadFromData( (const uchar*)img->rawdata, img->size  );
                            //image.save( "/var/opt/bosch/navdata/log/screenshot/cross.png" );

                            sendNotification( ApplicationConstants::UpdateImageProvider, QVariantMap{{CommonConstants::Type, ApplicationConstants::OnlineCrossImage}, {CommonConstants::Image, image}} ) ;
                            sendNotification( RgConstants::DisplayOnlineCrossImage, reqData );
                        }
                    }
                    break;
                default:
                  break;
                }
                auto searchProxy = Facade::instance()->retrieveProxy<IntegratedSearchProxy>();
                if (!reply->error()) {
                    qDebug()<<"ManageIntegratedSearchCommand::onResponseCommand=>[success]type:"<< type << ",query:" << query <<",error:"<<reply->error() <<",errorCause:"<<reply->errorCause() <<",totalCount:"<<totalCount<<",resultCount:"<<resultCount<<",online:"<<searchProxy->getOnlineState();
                } else {
                    qDebug()<<"ManageIntegratedSearchCommand::onResponseCommand=>[fail]type:"<< type << ",query:" << query <<",error:"<<reply->error() <<",errorCause:"<<reply->errorCause() <<",totalCount:"<<totalCount<<",resultCount:"<<resultCount<<",online:"<<searchProxy->getOnlineState();
                }
            }
        }
    }

    sendNotification( respNotiId, body );

#if 0
    const int searchType = reqData[SearchConstants::SearchType].toInt();
    switch(searchType) {
        case SearchRequestBase::SearchByPOIid:
        case SearchRequestBase::SearchByEvPOIid:
        case SearchRequestBase::SearchLowestOilSt:
        case SearchRequestBase::SearchBySisulCategory:
        case SearchRequestBase::SearchBySisulAround:
        case SearchRequestBase::SearchAddress:
        case SearchRequestBase::SearchByEvAround:
        case SearchRequestBase::SearchBySisulNamePrediction:
        case SearchRequestBase::SearchBySisulName:
        case SearchRequestBase::SearchAQI:
        case SearchRequestBase::RemoveSearchQueue:
            break;
        case SearchRequestBase::SearchNextAddress:
        default:
            sendNotification( UtilConstants::HideProgressIndicator );
            break;
    }
#endif


    ///////////////////////////////////////////////////////////////////////
    //[MonkeyTest]를 위하여
    QVariantMap m;
    m.insert(CommonConstants::Name,    SearchConstants::SearchView);
    m.insert(CommonConstants::Value,   false);
    m.insert(CommonConstants::Type,    reqData[SearchConstants::SearchType]);
    m.insert(CommonConstants::Keyword, reqData[SearchConstants::Query]);
    sendNotification(SearchConstants::SearchWait, m);
    //[MonkeyTest]를 위하여
    ///////////////////////////////////////////////////////////////////////
    return true;
}
