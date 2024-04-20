#include "IntegratedSearchProxy.h"
#include "RequestCommand.h"
#include "Constants.h"
#include "RouteConstants.h"
#include "SearchHelper.h"
#include "VSM_Navi.h"
#include "VSM_Map.h"
#include "VSM_Pos.h"

#include <QDebug>
#include "StringHelper.h"
#include "CommonConstants.h"
#include "SearchConstants.h"
#include "UtilConstants.h"

#include "RouteSearchProxy.h"
#include "VsmMapProxy.h"

#include "SettingConstants.h"
#include "SettingProxy.h"
#include "SettingDto.h"

#include "hostnames.h"
#include <QtNetwork/qsslsocket.h>

#include "../mns/searchmanager.h"
#include "../../tmapproperty.h"


namespace SKT {

IntegratedSearchProxy::IntegratedSearchProxy(QObject* controller)
 : mController(qobject_cast<NavigationController*>(controller) )
 , mIsOnline(false)
{
    connect(mController->qobject(), SIGNAL(onlineChanged(bool)), this, SLOT(onlineChanged(bool)));
    connect(controller, SIGNAL(currentStreetChanged(RGDataPtr)), this, SLOT(onCurrentStreetChanged(RGDataPtr)));
    connect(controller, SIGNAL(nddsServerPolicyFinished(bool)), this, SLOT(onChannelServerPolicyFinished(bool)));
    connect(this, SIGNAL(queueChanged(int)), this, SLOT(onQueueChanged(int)));

    SearchHelper::instance()->setOwner(this);

    // 2019.8.21 place non-ndds search fetcher by vNext on/off   (Autocompleted, Geo, Aqi);
    // IVI sdc-ecm에 의한 vnext 통신 구분
    bool vNextEnabled = true;
    #ifndef BUILD_TARGET
       vNextEnabled = false;
    #else
       QString acompHost = TmapHostNames::getHostName(TmapHostNames::HOST_AUTOCOMP_INDEX);
       if (acompHost.indexOf("avnext.renault.com") < 0) {
           vNextEnabled = false;
       }
    #endif

    if (vNextEnabled) {
        mSearchSisulnamPredictionFntr = &IntegratedSearchProxy::searchBySisulNamePredictionViaNddsBundle;
        mSearchAddressFntr = &IntegratedSearchProxy::searchAddressViaNddsBundle;
        mSearchAqiFntr = &IntegratedSearchProxy::searchAQIViaNddsBundle;
    } else {
        mSearchSisulnamPredictionFntr = &IntegratedSearchProxy::searchBySisulNamePrediction;
        mSearchAddressFntr = &IntegratedSearchProxy::searchAddress;
        mSearchAqiFntr = &IntegratedSearchProxy::searchAQI;
    }

}

IntegratedSearchProxy::~IntegratedSearchProxy(){
}

void IntegratedSearchProxy::onlineChanged(bool online)
{
    mIsOnline = online;
}

void IntegratedSearchProxy::onChannelServerPolicyFinished(bool success)
{
    mServerPolicySuccess = success;
}

bool IntegratedSearchProxy::getOnlineState() const
{
    return mIsOnline;
}

void IntegratedSearchProxy::setGadgetSearchMode(bool b)
{
    mIsGadgetSearch = b;
}

void IntegratedSearchProxy::setCurrentGpsTime(QDateTime currentTime)
{
    mCurrentTime = currentTime;
}

QDateTime IntegratedSearchProxy::getCurrentGpsTime() const
{
    return mCurrentTime;
}


bool IntegratedSearchProxy::isGadgetSearchMode() const
{
    return mIsGadgetSearch;
}

void IntegratedSearchProxy::requestCommand(const RequestCommandPtr& req)
{
    SearchHelper* searchHelper = SearchHelper::instance();

    QVariantMap reqData = req->getReqData();
    reqData.insert(CommonConstants::RequestCommandPtr,QVariant::fromValue(req));

    const int searchType = reqData[SearchConstants::SearchType].toInt();
    QString textParam = reqData[SearchConstants::TextParam].toString();
    QVariantMap params = reqData[SearchConstants::Params].value<QVariantMap>();

    SearchRequestBase* r = NULL;
    switch (searchType) {
    case SearchRequestBase::SearchByPOIid: {
        qDebug() << "[Perf][IntegratedSearchProxy][SearchByPOIid]";
        SearchRequest* textReq = new SearchRequest(SearchRequestBase::SearchByPOIid);
        int poiId = params[SearchConstants::PoiID].toInt();
        QString navSeq = params[SearchConstants::NavSeq].toString();
        bool isOnline = mIsOnline;
        if (params.contains(SearchConstants::SearchType)) {
            isOnline = params[SearchConstants::SearchType].toBool();
        }
        if (!mServerPolicySuccess) {
            isOnline = false;
        }
        textReq->setPoiId(poiId); //버거킹 252640; // detail info를 얻어올 poi id를 세팅
        textReq->setNavSeq(navSeq);
        int wx , wy;
        mController->currentPosition(wx,wy);
        textReq->setX(wx);
        textReq->setY(wy);
        textReq->setOnline(isOnline);  // (참고) poi detail info는 online상태에서만 값을 가져오게 됨
        r = textReq;
        break;
    }

    case SearchRequestBase::SearchByEvPOIid: {
        qDebug() << "[Perf][IntegratedSearchProxy][SearchByEvPOIid]";
        SearchRequest* textReq = new SearchRequest(SearchRequestBase::SearchByEvPOIid);
        int poiId = params[SearchConstants::PoiID].toInt();
        QString navSeq = params[SearchConstants::NavSeq].toString();
        bool isOnline = mIsOnline;
        if (params.contains(SearchConstants::SearchType)) {
            isOnline = params[SearchConstants::SearchType].toBool();
        }
        if (!mServerPolicySuccess) {
            isOnline = false;
        }
        textReq->setPoiId(poiId);
        textReq->setNavSeq(navSeq);
        int wx , wy;
        mController->currentPosition(wx,wy);
        textReq->setX(wx);
        textReq->setY(wy);
        textReq->setOnline(isOnline);
        r = textReq;
        break;
    }

    case SearchRequestBase::SearchLowestOilSt: {
        qDebug() << "[Perf][IntegratedSearchProxy][SearchLowestOilSt]";
        SearchRequest* textReq = new SearchRequest(SearchRequestBase::SearchLowestOilSt);
        int distance = params[SearchConstants::Distance].toInt();
        int wx , wy;
        bool isOnline = mIsOnline;
        if (params.contains(SearchConstants::SearchType)) {
            isOnline = params[SearchConstants::SearchType].toBool();
        }
        if (!mServerPolicySuccess) {
            isOnline = false;
        }
        mController->currentPosition(wx,wy);
        textReq->setDistance(distance);
        textReq->setX(wx);
        textReq->setY(wy);
        textReq->setOnline(isOnline);  // (참고) poi station info는 online상태에서만 값을 가져오게 됨
        r = textReq;
        break;
    }

    case SearchRequestBase::SearchBySisulNamePrediction: {
        qDebug() << "[Perf][IntegratedSearchProxy][SearchBySisulNamePrediction]";
        bool isOnline = mIsOnline;
        if (params.contains(SearchConstants::SearchType)) {
            isOnline = params[SearchConstants::SearchType].toBool();
        }
        if(isOnline) {    // disable autocomplete temporary. 2019.8.9
            (this->*mSearchSisulnamPredictionFntr)(reqData);
        return;
        }
        int code = params[SearchConstants::AddrCode].toInt();
        SearchRequest* textReq = new SearchRequest(SearchRequestBase::SearchBySisulNamePrediction);
        textReq->setKeyword(textParam);
        textReq->setSortType(SearchRequestBase::SortTypeByName);
        textReq->setNewAddr(reqData[SearchConstants::RoadSearch].toBool());
        textReq->setAddrCode(code);
        int distance = params[SearchConstants::Distance].toInt();
        int wx , wy;
        mController->currentPosition(wx,wy);
        textReq->setDistance(distance);
        textReq->setX(wx);
        textReq->setY(wy);
        textReq->setPrediction(true);
        textReq->setOnline(false);  // prediction은 false로 고정
        r = textReq;
        break;
    }
    case SearchRequestBase::SearchBySisulName: {
        qDebug() << "[Perf][IntegratedSearchProxy][SearchBySisulName]";
        int wx, wy;
        int code = params[SearchConstants::AddrCode].toInt();
        int distance = params[SearchConstants::Distance].toInt();
        bool isOnline = mIsOnline;
        if (params.contains(SearchConstants::SearchType)) {
            isOnline = params[SearchConstants::SearchType].toBool();
        }
        if (!mServerPolicySuccess) {
            isOnline = false;
        }
        bool bNewAddress = reqData[SearchConstants::RoadSearch].toBool();
        mController->currentPosition(wx,wy);

        SearchRequest* textReq = new SearchRequest(SearchRequestBase::SearchBySisulName);
        textReq->setKeyword(textParam);
        textReq->setNewAddr(bNewAddress);
        ///////////////////////////////////////////////////////////
        //서버 지역설정 검색
        auto lcdName = params[SearchConstants::LcdName].toString();
        auto mcdName = params[SearchConstants::McdName].toString();
        auto scdName = params[SearchConstants::ScdName].toString();
        //TMCRSM-1600 - 지역검색 중 1Depth + "전체"에 해당하는 지역검색을 위하여  mcdName 및 scdName의 Empty를 체크하지 않음
        if (!lcdName.isEmpty() && !textParam.isEmpty()) {
            textReq->setNewAddrLocal(false);
            if (isOnline) {
                QString addressName = QString("%1 %2 %3").arg(lcdName).arg(mcdName).arg(scdName);
                textReq->setAddressName(addressName);
            }
        }
        //서버 지역설정 검색
        ///////////////////////////////////////////////////////////
        textReq->setSortType(reqData[SearchConstants::SortBy] == SearchConstants::DistanceOrder ? SearchRequestBase::SortTypeByDist : SearchRequestBase::SortTypeByName);
        textReq->setAddrCode(code);
        textReq->setDistance(distance);
        textReq->setX(wx);
        textReq->setY(wy);
        textReq->setPrediction(false);
        textReq->setOnline(isOnline);
        r = textReq;
        break;
    }
    case SearchRequestBase::SearchBySisulAround: {
        qDebug() << "[Perf][IntegratedSearchProxy][SearchBySisulAround]";
        int category = params[SearchConstants::Kind].toInt();
        int distance = params[SearchConstants::Distance].toInt();
        bool isOnline = mIsOnline;
        if (params.contains(SearchConstants::SearchType)) {
            isOnline = params[SearchConstants::SearchType].toBool();
        }
        if (!mServerPolicySuccess) {
            isOnline = false;
        }
        int wx , wy;
        mController->currentPosition(wx,wy);
        SearchRequest* textReq = new SearchRequest(SearchRequestBase::SearchBySisulAround);
        //< 카테고리 인덱스 (0:주유소, 1:충전소, 2:주차장, 3:은행, 4:쇼핑, 5:음식점, 6:관광, 7:의료, 8:공공기관, 9: 금융, 10:전기차 충전소)
        //< 주변 검색 거리 (0:0~2km, 1:2~5km, 2:5~7km, 3:7~10km)
        // sample) textReq->setDistance(3);  textReq->setCategoryIdx(0); ==> 반경 7~10Km내에서 주유소 검색
        textReq->setSortType(reqData[SearchConstants::SortBy] == SearchConstants::DistanceOrder ?  SearchRequestBase::SortTypeByDist : SearchRequestBase::SortTypeByName);
        textReq->setNewAddr(reqData[SearchConstants::RoadSearch].toBool());
        textReq->setCategoryIdx(category);
        textReq->setDistance(distance);
        textReq->setX(wx);
        textReq->setY(wy);
        textReq->setOnline(isOnline);
        r = textReq;
        break;
    }

    case SearchRequestBase::SearchByAddressVr: {
        qDebug() << "[Perf][IntegratedSearchProxy][SearchByAddressVr]";
        SearchRequest* req = new SearchRequest(SearchRequestBase::SearchByAddressVr);
        int wx , wy;
        mController->currentPosition(wx,wy);
        req->setX(wx);
        req->setY(wy);
        req->setNewAddr(reqData[SearchConstants::RoadSearch].toBool());
        req->setAddressVrParams(params);
        req->setOnline(false);
        r = req;
    }
    break;

    case SearchRequestBase::SearchByAddress: {
        qDebug() << "[Perf][IntegratedSearchProxy][SearchByAddress]";
        QString text = reqData[CommonConstants::Text].toString();
        auto lcdName = params[SearchConstants::LcdName].toString();
        auto mcdName = params[SearchConstants::McdName].toString();
        auto scdName = params[SearchConstants::ScdName].toString();
        QString keyword = reqData[CommonConstants::Keyword].toString();
        QString keywordServer = QString("%1 %2 %3").arg(lcdName).arg(mcdName).arg(scdName);
        bool isSan = reqData[SearchConstants::IsSan].toBool();
        bool isApt = reqData[SearchConstants::IsApt].toBool();
        int code = params[SearchConstants::AddrCode ].toInt();
        QString name = params[ CommonConstants::Name ].toString();
        int jibunStIdx = params[ SearchConstants::JibunStIdx ].toInt();
        int jibunCnt = params[ SearchConstants::JibunCnt ].toInt();
        int aptStIdx = params[ SearchConstants::AptStIdx ].toInt();
        int aptCnt = params[ SearchConstants::AptCnt ].toInt();
        int newAddrStIdx = params[ SearchConstants::NewAddrStIdx ].toInt();
        int newAddrCnt = params[ SearchConstants::NewAddrCnt ].toInt();
        int jibunRicode = params[ SearchConstants::JibunRicode ].toInt();
        T_DEBUG( QString( "LcdName: %1" ).arg( lcdName ) );
        T_DEBUG( QString( "McdName: %1" ).arg( mcdName ) );
        T_DEBUG( QString( "ScdName: %1" ).arg( scdName ) );

        bool isOnline = mIsOnline;
        if (params.contains(SearchConstants::SearchType)) {
            isOnline = params[SearchConstants::SearchType].toBool();
        }
        if (!mServerPolicySuccess) {
            isOnline = false;
        }

        // hskim 17.7.13 apt search
        T_DEBUGV( "isApt: %d", isApt );

        if ( isApt ) {
            isSan = false;
            textParam = QString("%1 %2 %3").arg(mcdName).arg(name).arg(QStringLiteral("아파트"));
            //qInfo() << "[Addr]textParam: " << textParam << " lcdName: " << lcdName << " mcdName: " << mcdName << " scdName: " << scdName;
            SearchRequest* textReq = new SearchRequest(SearchRequestBase::SearchBySisulName);
            textReq->setKeyword(textParam);
            textReq->setSortType(reqData[SearchConstants::SortBy] == SearchConstants::DistanceOrder ? SearchRequestBase::SortTypeByDist : SearchRequestBase::SortTypeByName);
            textReq->setNewAddr(reqData[SearchConstants::RoadSearch].toBool());

            //로컬검색인 경우 업종검색을 할수 있도록 추가
            textReq->setCategoryCode(30101);//아파트
            textReq->setAddrCode(code);//지역코드

            int distance = params[SearchConstants::Distance].toInt();
            int wx , wy;
            mController->currentPosition(wx,wy);
            textReq->setDistance(distance);
            textReq->setX(wx);
            textReq->setY(wy);
            textReq->setPrediction(false);
            textReq->setOnline(isOnline);
            r = textReq;
        }
        else {
            char sanFlag = isSan ? '2' : '1';
            SearchRequest* textReq = new SearchRequest(SearchRequestBase::SearchByAddress);
            textReq->setSortType(reqData[SearchConstants::SortBy] == SearchConstants::DistanceOrder ? SearchRequestBase::SortTypeByDist : SearchRequestBase::SortTypeByName);
            textReq->setAddrCode(code);
            if (reqData[SearchConstants::IsRoad].toBool()) {
                textReq->setStartAddrIndex(newAddrStIdx);
                textReq->setStartAddrCnt(newAddrCnt);
                textReq->setJibun(text);
            } else {
                textReq->setStartAddrIndex(jibunStIdx);
                textReq->setStartAddrCnt(jibunCnt);
                textReq->setRiCode(jibunRicode);
                textReq->setSanFlag(sanFlag);
                if ( isSan ) {
                    text = QString( "산%1" ).arg( text );
                }

                textReq->setJibun(text);

                T_DEBUG( QString( "code: %1 jibunStIdx: %2 jibunCnt: %3 JibunRicode: %4 sanFlag: %5 keyword: %6 text: %7" )
                         .arg( code ).arg( jibunStIdx ).arg( jibunCnt )
                         .arg( jibunRicode ).arg( sanFlag ).arg( isOnline? keywordServer : keyword)
                         .arg( text ) );
            }

            textReq->setKeyword(keyword);
            textReq->setNewAddr(reqData[SearchConstants::IsRoad].toBool());
            if (isOnline) {
                 textReq->setKeywordServer(keywordServer);
                 textReq->setNewAddrServer(reqData[SearchConstants::RoadSearch].toBool());
            }
            int distance = params[SearchConstants::Distance].toInt();
            int wx , wy;
            mController->currentPosition(wx,wy);
            textReq->setDistance(distance);
            textReq->setX(wx);
            textReq->setY(wy);
            textReq->setOnline(isOnline);
            r = textReq;
        }
        break;
    }
    case SearchRequestBase::SearchNextAddress: {
        qDebug() << "[Perf][IntegratedSearchProxy][SearchNextAddress]";
        SearchNextAddrRequest* addrReq = new SearchNextAddrRequest(SearchRequestBase::SearchNextAddress);
        bool isRoadSearch = reqData[SearchConstants::IsRoad].toBool();
        int textParam = reqData[SearchConstants::TextParam].toInt();

        T_DEBUG( QString("isRoadSearch: %1 textParam: %2").arg( isRoadSearch ).arg( textParam ) );

        addrReq->setNewAddr( isRoadSearch );
        addrReq->setAddressCode( textParam );
        r = addrReq;
        break;
    }

    case SearchRequestBase::SearchBySisulCategory: {
        qDebug() << "[Perf][IntegratedSearchProxy][SearchBySisulCategory]";
        int code = params[SearchConstants::AddrCode].toInt();
        SearchRequest* textReq = new SearchRequest(SearchRequestBase::SearchBySisulCategory);
        textReq->setSortType(reqData[SearchConstants::SortBy] == SearchConstants::DistanceOrder ? SearchRequestBase::SortTypeByDist : SearchRequestBase::SortTypeByName);
        textReq->setCategoryCode(reqData[SearchConstants::TextParam].toInt());
        textReq->setNewAddr(reqData[SearchConstants::RoadSearch].toBool());
        textReq->setAddrCode(code);
        int distance = params[SearchConstants::Distance].toInt();
        int wx, wy;
        mController->currentPosition(wx,wy);
        textReq->setDistance(distance);
        textReq->setX(wx);
        textReq->setY(wy);
        r = textReq;
        break;
    }
    case SearchRequestBase::SearchNextCategory: {
        qDebug() << "[Perf][IntegratedSearchProxy][SearchNextCategory]";
        SearchNextCateRequest* categoryReq = new SearchNextCateRequest(SearchRequestBase::SearchNextCategory);
        /* @param : nCategoryCode / setCategoryCode(int)
         * @return : 다음의 구조체로 결과리스트가 전달 됨 'TS_RESULT_CATE'
        */
        categoryReq->setCategoryCode(reqData[SearchConstants::TextParam].toInt());
        r = categoryReq;
        break;
    }

    case SearchRequestBase::SearchByEvAround: {
        bool isOnline = mIsOnline;
        if (params.contains(SearchConstants::SearchType)) {
            isOnline = params[SearchConstants::SearchType].toBool();
        }
        if (!mServerPolicySuccess) {
            isOnline = false;
        }
         int category = params[SearchConstants::Kind].toInt();
         int distance = params[SearchConstants::Distance].toInt();
         int wx , wy;
         mController->currentPosition(wx,wy);
         SearchRequest* textReq = new SearchRequest(SearchRequestBase::SearchByEvAround);
         //< 카테고리 인덱스 (0:주유소, 1:충전소, 2:주차장, 3:은행, 4:쇼핑, 5:음식점, 6:관광, 7:의료, 8:공공기관, 9: 금융, 10: 전기차 충전소 )
         //< 주변 검색 거리 (0:0~2km, 1:2~5km, 2:5~7km, 3:7~10km)
         // sample) textReq->setDistance(3);  textReq->setCategoryIdx(0); ==> 반경 7~10Km내에서 주유소 검색
         textReq->setSortType(reqData[SearchConstants::SortBy] == SearchConstants::DistanceOrder ?
                     SearchRequestBase::SortTypeByDist : SearchRequestBase::SortTypeByName);
         textReq->setNewAddr(reqData[SearchConstants::RoadSearch].toBool());
         textReq->setCategoryIdx(category);
         textReq->setDistance(distance);
         textReq->setX(wx);
         textReq->setY(wy);
         textReq->setOnline(isOnline);
         r = textReq;
         break;
     }

    case SearchRequestBase::SearchAQI: {
        qDebug() << "[Perf][IntegratedSearchProxy][SearchAQI]";
        if(mIsOnline) {    // disable AQI temporary. 2019.8.9
            (this->*mSearchAqiFntr)(reqData);
        } else {
            SendReply(SearchConstants::ResponseOnlineSearch, 0, CommonConstants::Empty, reqData) ;
        }
        return;
    }

    case SearchRequestBase::SearchAddress: {
        qDebug() << "[Perf][IntegratedSearchProxy][SearchAddress]";
        if(mIsOnline) {    // disable geocode temporary. 2019.8.9
            (this->*mSearchAddressFntr)(reqData);
        } else {
            SendReply(SearchConstants::ResponseOnlineSearch, 0, CommonConstants::Empty, reqData) ;
        }
        return;
    }

    case SearchRequestBase::SearchComplexImage: {
        qDebug() << "[Perf][IntegratedSearchProxy][SearchComplexImage]";
        if(mIsOnline) {
            searchComplexImage(reqData);
        } else {
            SendReply(SearchConstants::ResponseOnlineSearch, 0, CommonConstants::Empty, reqData) ;
        }
        return;
    }

    case SearchRequestBase::RemoveSearchQueue: {
        qDebug() << "[Perf][IntegratedSearchProxy][RemoveSearchQueue]";
        searchHelper->clear();
        return;
    }

    default:
        T_DEBUGV("invalid search type: %d", searchType);
        return;
    }

    int reqIndex = -1;
    int startIndex = -1;
    int count = -1;
    //검색결과 동적 업데이트 조건
    if (params.contains(SearchConstants::StartIndex) && params.contains(CommonConstants::Count)) {
        reqIndex = startIndex = params[SearchConstants::StartIndex].toInt();
        count = params[CommonConstants::Count].toInt();
        switch(searchType) {
            case SearchRequestBase::SearchByPOIid:
            case SearchRequestBase::SearchLowestOilSt:
            case SearchRequestBase::SearchByEvPOIid:
                startIndex = 0;
                count = 1;
                break;
        }
        r->setStartIndex(startIndex);
        r->setCount(count);
    }

    SearchRequestBasePtr searchReq(r);
    searchReq->setExtra(reqData);

    qDebug()<<"IntegratedSearchProxy::requestComman=>[success]type:"<< searchType<<",startIndex:"<<startIndex<<",requestIndex:"<<reqIndex<<",requestCount:"<<count;
    qDebug() << "[Perf][IntegratedSearchProxy][request]";
    SearchReplyBaseInterface* reply = searchHelper->request(searchReq);
    if (reply) {
        connect(reply->qobject(), SIGNAL(finished()), this, SLOT(Response()));
    }
}

void IntegratedSearchProxy::onQueueChanged(int count)
{
    sendNotification( SearchConstants::QueueChanged, count );
}

bool goLocalSearch(SearchReplyBaseInterface* rep)
{
    bool bRet = false;
    QVariantMap reqData = rep->request()->extra();
    if (!reqData.contains("goLocalSearch") ||
        !reqData["goLocalSearch"].toBool()) {
        return bRet;
    }
    switch (rep->request()->searchType()) {
        case SearchRequestBase::SearchBySisulName:
        case SearchRequestBase::SearchBySisulAround:
        case SearchRequestBase::SearchByEvAround:
        case SearchRequestBase::SearchByAddress: {
                SearchReply *reply =  static_cast<SearchReply*>(rep);
                if (!reply->error()) {
                    if (reply->totalCount() > 0) {
                        break;
                    }
                }
                if (reply->request()->startIndex() > 0) {
                    break;
                }
                bRet = true;
            }
            break;
        case SearchRequestBase::SearchByEvPOIid: {
                SearchReplyEvPoiDetail *reply =  static_cast<SearchReplyEvPoiDetail*>(rep);
                if (!reply->error()) {
                    if (reply->totalCount() > 0) {
                        break;
                    }
                }
                if (reply->request()->startIndex() > 0) {
                    break;
                }
                bRet = true;
            }
            break;
        default:
            break;
    }
    return bRet;
}

void IntegratedSearchProxy::Response()
{
    SearchReplyBaseInterface* reply = qobject_cast<SearchReplyBaseInterface*>(sender());
    if (reply) {
        QVariantMap reqData = reply->request()->extra();
        if (goLocalSearch(reply)) {
            reqData["goLocalSearch"] =  false;
            reply->request()->setExtra(reqData);
            return;
        }

        reply->qobject()->deleteLater();

        RequestCommandPtr req = reqData[CommonConstants::RequestCommandPtr].value<RequestCommandPtr>();
        QVariantMap resData;
        resData.insert(SearchConstants::Result, QVariant::fromValue(reply->qobject()));
        req->setResData(resData);
        onSearchFinished( req );
    }
}

void IntegratedSearchProxy::onSearchFinished( const RequestCommandPtr& req)
{
    auto note = t_new_shared<mvc::TNotification>( this, RouteConstants::UpdateRoute,
        QVariant::fromValue( req ) );
    req->notifyObserver( note );
}


void dumpProtocol(QSsl::SslProtocol proto)
{
    qDebug() << "\n== Protocol Info ==";
    switch(proto) {
    case QSsl::SslV3:               qDebug() << "Protocol:\t\tSSL 3";               break;
    case QSsl::SslV2:               qDebug() << "Protocol:\t\tSSL 2";               break;
    case QSsl::TlsV1_0:             qDebug() << "Protocol:\t\tTLS 1.0";             break;
    case QSsl::TlsV1_1:             qDebug() << "Protocol:\t\tTLS 1.1";             break;
    case QSsl::TlsV1_2:             qDebug() << "Protocol:\t\tTLS 1.2";             break;
    case QSsl::AnyProtocol:         qDebug() << "Protocol:\t\tAny Protocol";        break;
    case QSsl::TlsV1SslV3:          qDebug() << "Protocol:\t\tTLS1/SSL3";           break;
    case QSsl::SecureProtocols:     qDebug() << "Protocol:\t\tSecure Protocols";    break;
    default:                        qDebug() << "Protocol:\t\tUnknown";             break;
    }
}

void dumpCertificate( const QSslCertificate &cert )
{
    qDebug() << "\n== Subject Info ==";
    qDebug() << "CommonName:\t\t" << cert.subjectInfo( QSslCertificate::CommonName );
    qDebug() << "Organization:\t\t" << cert.subjectInfo( QSslCertificate::Organization );
    qDebug() << "LocalityName:\t\t" << cert.subjectInfo( QSslCertificate::LocalityName );
    qDebug() << "OrganizationalUnitName:\t" << cert.subjectInfo( QSslCertificate::OrganizationalUnitName );
    qDebug() << "StateOrProvinceName:\t" << cert.subjectInfo( QSslCertificate::StateOrProvinceName );

    QMultiMap<QSsl::AlternativeNameEntryType, QString> altNames = cert.subjectAlternativeNames();
    if ( !altNames.isEmpty() ) {
        qDebug() << "Alternate Subject Names (DNS):";
        foreach (const QString &altName, altNames.values(QSsl::DnsEntry)) {
            qDebug() << altName;
        }

        qDebug() << "Alternate Subject Names (Email):";
        foreach (const QString &altName, altNames.values(QSsl::EmailEntry)) {
            qDebug() << altName;
        }
    }

    qDebug() << "\n== Issuer Info ==";
    qDebug() << "CommonName:\t\t" << cert.issuerInfo( QSslCertificate::CommonName );
    qDebug() << "Organization:\t\t" << cert.issuerInfo( QSslCertificate::Organization );
    qDebug() << "LocalityName:\t\t" << cert.issuerInfo( QSslCertificate::LocalityName );
    qDebug() << "OrganizationalUnitName:\t" << cert.issuerInfo( QSslCertificate::OrganizationalUnitName );
    qDebug() << "StateOrProvinceName:\t" << cert.issuerInfo( QSslCertificate::StateOrProvinceName );

    qDebug() << "\n== Certificate ==";
    qDebug() << "Serial Number:\t\t" << cert.serialNumber(); // This seems buggy
    qDebug() << "Effective Date:\t\t" << cert.effectiveDate().toString();
    qDebug() << "Expiry Date:\t\t" << cert.expiryDate().toString();
    qDebug() << "Pem:\t\t" << cert.toPem();
    qDebug() << "Digest:\t\t" << cert.digest();
    //qDebug() << "Der:\t\t" << cert.toDer();
    //qDebug() << "Text:\t\t" << cert.toText();
}

void dumpCipher( const QSslCipher &cipher )
{
    qDebug() << "\n== Cipher ==";

    qDebug() << "Authentication:\t\t" << cipher.authenticationMethod();
    qDebug() << "Encryption:\t\t" << cipher.encryptionMethod();
    qDebug() << "Key Exchange:\t\t" << cipher.keyExchangeMethod();
    qDebug() << "Cipher Name:\t\t" << cipher.name();
    qDebug() << "Protocol:\t\t" <<  cipher.protocolString();
    qDebug() << "Supported Bits:\t\t" << cipher.supportedBits();
    qDebug() << "Used Bits:\t\t" << cipher.usedBits();
}

void IntegratedSearchProxy::cleanManager(QList<NetworkAccessManagerWrapper*> &managerArr)
{
    int count = managerArr.count();
    T_DEBUG(QString( "###[HTTPS start] Manager Count:%1").arg(count));
    for(int i= 0; i< count; i++) {
        NetworkAccessManagerWrapper *pManger = managerArr.value(i);
        if(pManger) {
            pManger->killTimer();
            pManger->closeRely();
        }
    }
}

void IntegratedSearchProxy::removeOneManager(QList<NetworkAccessManagerWrapper*> &managerArr, NetworkAccessManagerWrapper *pManager)
{
    managerArr.removeOne(pManager);
    int count = managerArr.count();
    T_DEBUG(QString( "###[HTTPS end] Manager Count:%1").arg(count));
}

void IntegratedSearchProxy::searchBySisulNamePrediction(QVariantMap reqData)
{
    QVariantMap params = reqData[SearchConstants::Params].value<QVariantMap>();
    cleanManager(mManagerArrSisul);
    NetworkAccessManagerWrapper *pManager = new NetworkAccessManagerWrapper(this, reqData);
    mManagerArrSisul.append(pManager);

    QSslConfiguration config(QSslConfiguration::defaultConfiguration());
    config.setPeerVerifyMode(QSslSocket::VerifyNone);

    QString textParam = reqData[SearchConstants::TextParam].toString();
    //상용(과금) QUrl url = QString("https://acf.tmap.co.kr:13132/TmapAutoCompleteServer/suggest?q=%1&s=tmap").arg(textParam);
    //상용(비과금) QUrl url = QString("https://acnf.tmap.co.kr:13132/TmapAutoCompleteServer/suggest?q=%1&s=tmap").arg(textParam);
    //개발 QUrl url = QString("http://searchdev.skplanet.co.kr:13131/TmapAutoCompleteServer/suggest?q=%1&s=tmap").arg(textParam);
    QUrl url = QString("%1/TmapAutoCompleteServer/suggest?q=%2&s=tmap&client_code=TMC_V1&referrer_code=autoComplete").arg(TmapHostNames::getHostName(TmapHostNames::HOST_AUTOCOMP_INDEX)).arg(textParam);
    qDebug() << "Prediction url=" <<url.toString();

    QNetworkRequest req;
    req.setUrl(url);
    req.setSslConfiguration(config);
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QNetworkReply *reply = pManager->get(req);
    pManager->setTimer(reply, SearchConstants::NetworkTimeoutAPI);

    QObject::connect(pManager, &QNetworkAccessManager::encrypted, [](QNetworkReply *reply){
        QSsl::SslProtocol prot = reply->sslConfiguration().protocol();
        QSslCertificate cert = reply->sslConfiguration().peerCertificate();
        QSslCipher cipher = reply->sslConfiguration().sessionCipher();
        //dumpProtocol(prot);
        //dumpCertificate(cert);
        //dumpCipher(cipher);
    });

    QObject::connect(pManager, &QNetworkAccessManager::sslErrors, [](QNetworkReply *reply, QList<QSslError> errors) {
          for (auto e : errors)
              qDebug() << "####[SSL error]" << e.errorString();
    });
    QObject::connect(pManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(onResultSearchBySisulNamePrediction(QNetworkReply*)));
    QObject::connect(reply, SIGNAL(readyRead()), this, SLOT(slotReadyRead()));
    QObject::connect(reply, SIGNAL(error(QNetworkReply::NetworkError)),     this, SLOT(slotError(QNetworkReply::NetworkError)));
    QObject::connect(reply, SIGNAL(sslErrors(QList<QSslError>)),    this, SLOT(slotSslErrors(QList<QSslError>)));
}

void IntegratedSearchProxy::searchBySisulNamePredictionViaNddsBundle(QVariantMap reqData)
{
    SearchNotNddsRequest *textReq = new SearchNotNddsRequest(SearchRequestBase::SearchBySisulNamePrediction);
    textReq->setSingleUrl(takeUrlSearchBySisulNamePrediction(reqData));
    textReq->setHostDomain(TmapHostNames::getHostName(TmapHostNames::HOST_AUTOCOMP_INDEX));
    textReq->setOnline(true);
    SearchRequestBase* r = static_cast<SearchRequestBase*>(textReq);
    SearchRequestBasePtr searchReq(r);
    searchReq->setExtra(reqData);
    SearchHelper* searchHelper = SearchHelper::instance();
    SearchReplyBaseInterface* reply = searchHelper->request(searchReq);
    if (reply) {
        connect(reply->qobject(), SIGNAL(finished()), this, SLOT(Response()));
    }
}

QString IntegratedSearchProxy::takeUrlSearchBySisulNamePrediction(QVariantMap reqData)
{
   QVariantMap params = reqData[SearchConstants::Params].value<QVariantMap>();
   QString textParam = reqData[SearchConstants::TextParam].toString();
   //상용(과금) QUrl url = QString("https://acf.tmap.co.kr:13132/TmapAutoCompleteServer/suggest?q=%1&s=tmap").arg(textParam);
   //상용(비과금) QUrl url = QString("https://acnf.tmap.co.kr:13132/TmapAutoCompleteServer/suggest?q=%1&s=tmap").arg(textParam);
   //개발 QUrl url = QString("http://searchdev.skplanet.co.kr:13131/TmapAutoCompleteServer/suggest?q=%1&s=tmap").arg(textParam);
   QUrl url = QString("%1/TmapAutoCompleteServer/suggest?q=%2&s=tmap&client_code=TMC_V1&referrer_code=autoComplete").arg(TmapHostNames::getHostName(TmapHostNames::HOST_AUTOCOMP_INDEX)).arg(textParam);
   qDebug() << "Prediction url=" <<url.toString();

   return url.toString();
}

void IntegratedSearchProxy::slotReadyRead()
{
    qDebug() << "####::slotReadyRead";
}

void IntegratedSearchProxy::slotError(QNetworkReply::NetworkError err)
{
 qDebug() << "####::slotError:"<< err;
}

void IntegratedSearchProxy::slotSslErrors(QList<QSslError> list)
{
    for(int i=0; i< list.count(); i++)
        qDebug() << "####slotSslErrors:"<<list.value(i).errorString();
 }

void IntegratedSearchProxy::onResultSearchBySisulNamePrediction(QNetworkReply *reply)
{
    if(!reply) {
        return;
    }

    int totalCount = 0;
    QVariantList list;

    NetworkAccessManagerWrapper *pManager = qobject_cast<NetworkAccessManagerWrapper*> (reply->manager());
    pManager->killTimer();

    QVariantMap reqData = pManager->getReqData();
    if (reply->error() == QNetworkReply::NoError) {
        QString strReply = (QString)reply->readAll();
        qDebug() << "Response:" << strReply;
        QJsonDocument jsonResponse = QJsonDocument::fromJson(strReply.toUtf8());
        QJsonObject jsonObject = jsonResponse.object();
        QJsonValue value = jsonObject.value(QString("response"));
        QJsonObject item = value.toObject();
        QJsonArray jsonArray = item["suggestions"].toArray();
        int errCode = QNetworkReply::NoError;
        if (!item.contains("suggestions")) {
            errCode =  (QNetworkReply::UnknownServerError + 1);
        }
        totalCount = item["totalCount"].toInt();
        foreach (const QJsonValue & value, jsonArray) {
            QJsonObject item = value.toObject();
            list.append(item["keyword"].toString());
        }
        reqData.insert(CommonConstants::Error, errCode);
    } else {
        qDebug() << "####Failure" <<reply->errorString();
        reqData.insert(CommonConstants::Error, reply->error() );
    }
    removeOneManager(mManagerArrSisul, pManager);
    reply->deleteLater();
    pManager->deleteLater();

    SendReply(SearchConstants::ResponseOnlineSearch , totalCount, list, reqData);
}

void IntegratedSearchProxy::onResultSearchBySisulNamePrediction2(const QVariantMap& reqData, const QByteArray& result)
{
    int totalCount = 0;
    QVariantList list;

    // 서버 검색 실패 시 로컬로 전환하지 못하는 문제로 인해 추가함. 2019.8.23 by hheo
    if (result.size() == 0) {
        qWarning(ui) << "server prediction response failure";
        QVariantMap reqData2 = reqData;
        reqData2.insert(SearchConstants::Max, totalCount);
        reqData2.insert(SearchConstants::ResponseNotificationId, SearchConstants::ResponseOnlineSearch );

        QVariantMap body;
        body.insert( SearchConstants::ReqData, reqData2 );
        body.insert( SearchConstants::Result, QVariant::fromValue(list) );

        sendNotification(SearchConstants::ResponseOnlineSearch, body);
        return ;
    }

    QString strReply = (QString)result;
    QJsonDocument jsonResponse = QJsonDocument::fromJson(strReply.toUtf8());
    QJsonObject jsonObject = jsonResponse.object();
    QJsonValue value = jsonObject.value(QString("response"));
    QJsonObject item = value.toObject();
    QJsonArray jsonArray = item["suggestions"].toArray();
    totalCount = item["totalCount"].toInt();
    foreach (const QJsonValue & value, jsonArray) {
        QJsonObject item = value.toObject();
        list.append(item["keyword"].toString());
    }

    QVariantMap reqData2 = reqData;
    reqData2.insert(SearchConstants::Max, totalCount);
    reqData2.insert(SearchConstants::ResponseNotificationId, SearchConstants::ResponseOnlineSearch );

    QVariantMap body;
    body.insert( SearchConstants::ReqData, reqData2 );
    body.insert( SearchConstants::Result, QVariant::fromValue(list) );

    sendNotification(SearchConstants::ResponseOnlineSearch, body);
}

void IntegratedSearchProxy::searchAQI(QVariantMap reqData)
{
    cleanManager(mManagerArrAQI);
    NetworkAccessManagerWrapper *pManager = new NetworkAccessManagerWrapper(this, reqData);
    mManagerArrAQI.append(pManager);

    int wx, wy;
    double longitude,latitude;
    const int kind = reqData[SearchConstants::Kind].toInt();
    switch(kind) {
        case SearchConstants::CurrentPos: {
                mController->currentPosition(wx,wy);
                mController->getVSMWorldToWGS84(wx, wy, longitude, latitude);
            }
            break;
        case SearchConstants::DestinationPos:
        default: {
                auto searchProxy = Facade::instance()->retrieveProxy<RouteSearchProxy>();
                QPoint pos = searchProxy->viaWorldPos(RouteItem_End);
                wx = pos.x();
                wy = pos.y();
                mController->getVSMWorldToWGS84(wx, wy, longitude, latitude);
            }
            break;
    }

    QSslConfiguration config(QSslConfiguration::defaultConfiguration());
    config.setPeerVerifyMode(QSslSocket::VerifyNone);

    QString textParam = reqData[SearchConstants::TextParam].toString();
    QUrl url(QString("%1/aqi/getAirQuality?appKey=TMAP_RSM_AIVI&lat=%2&lon=%3")
             .arg(TmapHostNames::getHostName(TmapHostNames::HOST_AQI_INDEX)).arg(latitude).arg(longitude));
    qDebug() << "AQI url=" <<url.toString();

    QNetworkRequest req;
    req.setUrl(url);
    req.setSslConfiguration(config);
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json;charset=utf-8");


    QNetworkReply *reply = pManager->get(req);
    pManager->setTimer(reply, SearchConstants::NetworkTimeoutAPI);
    QObject::connect(pManager, &QNetworkAccessManager::encrypted, [](QNetworkReply *reply){
        QSsl::SslProtocol prot = reply->sslConfiguration().protocol();
        QSslCertificate cert = reply->sslConfiguration().peerCertificate();
        QSslCipher cipher = reply->sslConfiguration().sessionCipher();
        //dumpProtocol(prot);
        //dumpCertificate(cert);
        //dumpCipher(cipher);
    });

    QObject::connect(pManager, &QNetworkAccessManager::sslErrors, [](QNetworkReply *reply, QList<QSslError> errors) {
          for (auto e : errors)
              qDebug() << "[SSL error]" << e.errorString();
    });
    QObject::connect(pManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(onResultSearchAQI(QNetworkReply*)));

    QObject::connect(reply, SIGNAL(readyRead()), this, SLOT(slotReadyRead()));
    QObject::connect(reply, SIGNAL(error(QNetworkReply::NetworkError)),     this, SLOT(slotError(QNetworkReply::NetworkError)));
    QObject::connect(reply, SIGNAL(sslErrors(QList<QSslError>)),    this, SLOT(slotSslErrors(QList<QSslError>)));

}

void IntegratedSearchProxy::searchAQIViaNddsBundle(QVariantMap reqData)
{
    SearchNotNddsRequest *textReq = new SearchNotNddsRequest(SearchRequestBase::SearchAQI);
    textReq->setSingleUrl(takeUrlSearchAQI(reqData));
    textReq->setHostDomain(TmapHostNames::getHostName(TmapHostNames::HOST_AQI_INDEX));
    SearchRequestBase* r = static_cast<SearchRequestBase*>(textReq);
    SearchRequestBasePtr searchReq(r);
    searchReq->setExtra(reqData);
    SearchHelper* searchHelper = SearchHelper::instance();
    SearchReplyBaseInterface* reply = searchHelper->request(searchReq);
    if (reply) {
        connect(reply->qobject(), SIGNAL(finished()), this, SLOT(Response()));
    }
}

QString IntegratedSearchProxy::takeUrlSearchAQI(QVariantMap reqData)
{
    int wx, wy;
    double longitude,latitude;
    const int kind = reqData[SearchConstants::Kind].toInt();
    switch(kind) {
        case SearchConstants::CurrentPos: {
                mController->currentPosition(wx,wy);
                mController->getVSMWorldToWGS84(wx, wy, longitude, latitude);
            }
            break;
        case SearchConstants::DestinationPos:
        default: {
                auto searchProxy = Facade::instance()->retrieveProxy<RouteSearchProxy>();
                QPoint pos = searchProxy->viaWorldPos(RouteItem_End);
                wx = pos.x();
                wy = pos.y();
                mController->getVSMWorldToWGS84(wx, wy, longitude, latitude);
            }
            break;
    }

    QString textParam = reqData[SearchConstants::TextParam].toString();
    QString aqiHost = TmapHostNames::getHostName(TmapHostNames::HOST_AQI_INDEX);
    QString aqiPath = QString("%1/tmapv20/getAirQuality?appKey=TMAP_RSM_AIVI&lat=%2&lon=%3");
    if (aqiHost.indexOf("avnext.renault.com") < 0) {
        aqiPath = QString("%1/aqi/getAirQuality?appKey=TMAP_RSM_AIVI&lat=%2&lon=%3");
    }
    QUrl url(aqiPath.arg(aqiHost).arg(latitude).arg(longitude));
    qDebug() << "AQI url=" <<url.toString();

    return url.toString();
}

void IntegratedSearchProxy::onResultSearchAQI(QNetworkReply *reply)
{
    if(!reply)
        return;

    int totalCount = 0;
    QString strReply;
    NetworkAccessManagerWrapper *pManger = qobject_cast<NetworkAccessManagerWrapper*> (reply->manager());
    pManger->killTimer();
    QVariantMap reqData = pManger->getReqData();
    if (reply->error() == QNetworkReply::NoError) {
        reqData.insert(CommonConstants::Error, 0);
        qDebug() << "Response:" << strReply;
        strReply = (QString)reply->readAll();
    } else {
        reqData.insert(CommonConstants::Error, reply->error() );
        strReply = reply->errorString();
    }
    removeOneManager(mManagerArrAQI, pManger);
    reply->deleteLater();
    pManger->deleteLater();

    SendReply(SearchConstants::ResponseOnlineSearch , totalCount, strReply, reqData);
}


void IntegratedSearchProxy::onResultSearchAQI2(const QVariantMap& reqData, const QByteArray& result)
{
    if (result.size() == 0) {
        qWarning(ui) << "search AQI failure";
    }

    QString strReply(result);
    int totalCount = 0;

    QVariantMap reqData2 = reqData;
    reqData2.insert(SearchConstants::Max, 0);
    reqData2.insert(SearchConstants::ResponseNotificationId, SearchConstants::ResponseOnlineSearch );

    QVariantMap body;
    body.insert( SearchConstants::ReqData, reqData2 );
    body.insert( SearchConstants::Result, QVariant::fromValue(strReply) );

    sendNotification(SearchConstants::ResponseOnlineSearch, body);
}


void IntegratedSearchProxy::searchAddress(QVariantMap reqData)
{
    cleanManager(mManagerArrAddr);
    NetworkAccessManagerWrapper *pManager = new NetworkAccessManagerWrapper(this, reqData);
    mManagerArrAddr.append(pManager);

    auto proxy = Facade::instance()->retrieveProxy<SettingProxy>();
    auto setting = proxy->setting( SettingConstants::MapMain, SettingConstants::AddressDisplayMethod );
    auto value = setting->value().toInt();

    QString strAddressType;
    if (value == SettingConstants::AddressDisplayMethodLandNumber)
        strAddressType = "A02";
    else
        strAddressType = "A03";

    double longitude,latitude;
    auto mapProxy = Facade::instance()->retrieveProxy<VsmMapProxy>();
    auto pt = mapProxy->centerPosition(nullptr);
    int wx = pt.x();
    int wy = pt.y();
    mController->getVSMWorldToWGS84(wx, wy, longitude, latitude);

    QSslConfiguration config(QSslConfiguration::defaultConfiguration());
    config.setPeerVerifyMode(QSslSocket::VerifyNone);

    QString textParam = reqData[SearchConstants::TextParam].toString();
    QUrl url(QString("%1/tmap/geo/reversegeocoding?version=1&lat=%2&lon=%3&coordType=WGS84GEO&addressType=%4")
        .arg(TmapHostNames::getHostName(TmapHostNames::HOST_REVERSGEO_INDEX)).arg(latitude).arg(longitude).arg(strAddressType));
    qDebug() << "Address url=" <<url.toString();

    QNetworkRequest req;
    req.setUrl(url);
    req.setSslConfiguration(config);
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json;charset=utf-8");


    QNetworkReply *reply = pManager->get(req);
    pManager->setTimer(reply, SearchConstants::NetworkTimeoutAPI);

    QObject::connect(pManager, &QNetworkAccessManager::encrypted, [](QNetworkReply *reply){
        QSsl::SslProtocol prot = reply->sslConfiguration().protocol();
        QSslCertificate cert = reply->sslConfiguration().peerCertificate();
        QSslCipher cipher = reply->sslConfiguration().sessionCipher();
        //dumpProtocol(prot);
        //dumpCertificate(cert);
        //dumpCipher(cipher);
    });

    QObject::connect(pManager, &QNetworkAccessManager::sslErrors, [](QNetworkReply *reply, QList<QSslError> errors) {
          for (auto e : errors)
              qDebug() << "[SSL error]" << e.errorString();
    });
    QObject::connect(pManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(onResultSearchAddress(QNetworkReply*)));

    QObject::connect(reply, SIGNAL(readyRead()), this, SLOT(slotReadyRead()));
    QObject::connect(reply, SIGNAL(error(QNetworkReply::NetworkError)),     this, SLOT(slotError(QNetworkReply::NetworkError)));
    QObject::connect(reply, SIGNAL(sslErrors(QList<QSslError>)),    this, SLOT(slotSslErrors(QList<QSslError>)));

}

void IntegratedSearchProxy::searchAddressViaNddsBundle(QVariantMap reqData)
{
    SearchNotNddsRequest *textReq = new SearchNotNddsRequest(SearchRequestBase::SearchGeoCoding);
    textReq->setSingleUrl(takeSearchAddress(reqData));
    textReq->setHostDomain(TmapHostNames::getHostName(TmapHostNames::HOST_REVERSGEO_INDEX));
    SearchRequestBase *r = static_cast<SearchRequestBase *>(textReq);
    SearchRequestBasePtr searchReq(r);
    searchReq->setExtra(reqData);
    SearchHelper *searchHelper = SearchHelper::instance();
    SearchReplyBaseInterface *reply = searchHelper->request(searchReq);
    if (reply) {
        connect(reply->qobject(), SIGNAL(finished()), this, SLOT(Response()));
    }
}

QString IntegratedSearchProxy::takeSearchAddress(QVariantMap reqData)
{
    auto proxy = Facade::instance()->retrieveProxy<SettingProxy>();
    auto setting = proxy->setting( SettingConstants::MapMain, SettingConstants::AddressDisplayMethod );
    auto value = setting->value().toInt();

    QString strAddressType;
    if (value == SettingConstants::AddressDisplayMethodLandNumber)
        strAddressType = "A02";
    else
        strAddressType = "A03";

    double longitude,latitude;
    auto mapProxy = Facade::instance()->retrieveProxy<VsmMapProxy>();
    auto pt = mapProxy->centerPosition(nullptr);
    int wx = pt.x();
    int wy = pt.y();
    mController->getVSMWorldToWGS84(wx, wy, longitude, latitude);

    QSslConfiguration config(QSslConfiguration::defaultConfiguration());
    config.setPeerVerifyMode(QSslSocket::VerifyNone);

    QString textParam = reqData[SearchConstants::TextParam].toString();
    QUrl url(QString("%1/tmap/geo/reversegeocoding?version=1&lat=%2&lon=%3&coordType=WGS84GEO&addressType=%4")
        .arg(TmapHostNames::getHostName(TmapHostNames::HOST_REVERSGEO_INDEX)).arg(latitude).arg(longitude).arg(strAddressType));
    qDebug() << "Address url=" <<url.toString();

    return url.toString();
}

void IntegratedSearchProxy::onResultSearchAddress(QNetworkReply *reply)
{
    if(!reply)
        return;

    int totalCount = 0;
    QString strReply;
    NetworkAccessManagerWrapper *pManger = qobject_cast<NetworkAccessManagerWrapper*> (reply->manager());
    pManger->killTimer();
    QVariantMap reqData = pManger->getReqData();
    if (reply->error() == QNetworkReply::NoError) {
        reqData.insert(CommonConstants::Error, 0);
        qDebug() << "Response:" << strReply;
        strReply = (QString)reply->readAll();
    } else {
        reqData.insert(CommonConstants::Error, reply->error() );
        strReply = reply->errorString();
    }
    removeOneManager(mManagerArrAddr, pManger);
    reply->deleteLater();
    pManger->deleteLater();

    SendReply(SearchConstants::ResponseOnlineSearch , totalCount, strReply, reqData);
}


void IntegratedSearchProxy::onResultSearchAddress2(const QVariantMap& reqData, const QByteArray& result)
{
    if (result.size() == 0) {
        qWarning(ui) << "server search address geo coding failure";
//        return;       // 아래 코드를 타지 않으면 서버 검색 실패 시 로컬로 전환하지 못하는 문제가 발생하므로, return을 주석처리함. 2019.8.23 by hheo
    }
    int totalCount = 0;
    QString strReply(result);

    QVariantMap reqData2 = reqData;
    reqData2.insert(SearchConstants::Max, 0);
    reqData2.insert(SearchConstants::ResponseNotificationId, SearchConstants::ResponseOnlineSearch );

    QVariantMap body;
    body.insert( SearchConstants::ReqData, reqData2 );
    body.insert( SearchConstants::Result, QVariant::fromValue(strReply) );

    sendNotification(SearchConstants::ResponseOnlineSearch, body);
}

void IntegratedSearchProxy::SendReply(int notificationId, int resultCount, QVariant result, QVariantMap reqData)
{
    reqData.insert(SearchConstants::Max, QVariant::fromValue(resultCount));
    reqData.insert(SearchConstants::ResponseNotificationId, notificationId );

    QVariantMap resData;
    resData.insert(SearchConstants::Result, QVariant::fromValue(result));
    resData.insert(SearchConstants::ReqData,  QVariant::fromValue(reqData));

    RequestCommandPtr req = reqData[CommonConstants::RequestCommandPtr].value<RequestCommandPtr>();
    req->setReqData(reqData);
    req->setResData(resData);
    onSearchFinished( req );
}

void IntegratedSearchProxy::onCurrentStreetChanged(RGDataPtr value)
{
    VSM_UInt16  usInCityCode = value->stCurrent.usInCityCode;
    sendNotification(SearchConstants::ChangedCurrentStreet, usInCityCode);

}

void IntegratedSearchProxy::searchComplexImage(QVariantMap reqData)
{
    SearchNotNddsRequest *req = new SearchNotNddsRequest(SearchRequestBase::SearchComplexImage);
    req->setSingleUrl(reqData[CommonConstants::Url].toString());
    req->setHostDomain(TmapHostNames::getHostName(TmapHostNames::HOST_COMPLEX_INDEX));
    req->setOnline(true);
    SearchRequestBase *r = static_cast<SearchRequestBase *>(req);
    SearchRequestBasePtr searchReq(r);
    searchReq->setExtra(reqData);
    SearchHelper *searchHelper = SearchHelper::instance();
    SearchReplyBaseInterface *reply = searchHelper->request(searchReq);
    if (reply) {
        connect(reply->qobject(), SIGNAL(finished()), this, SLOT(Response()));
    }
}

}
