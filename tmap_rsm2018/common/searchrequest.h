#ifndef SEARCHREQUEST_H
#define SEARCHREQUEST_H

#include <QObject>
#include <QVariantMap>
#include <QSharedPointer>
#include <QScopedPointer>
#include "MNS_Define.h"
#include "NDDS_Define.h"

namespace SKT {

/// 검색 결과 리스트
typedef struct tagTPoiSearchList{
    int nIdx;										///< 검색 결과 인덱스
    char szName[D_MAXLEN_SISULNAME+1];				///< 명칭
    char szStdName[D_MAXLEN_SISULNAME+1];			///< 대표 명칭
    char szAddrName[D_MAXLEN_ADDRNAME+1];			///< 구주소, 새주소 (새주소 요청시 DB에 새주소 정보가 없으면 구까지만 전달)
    char szPhoneNumber[D_MAXLEN_PHONENUMBER+1];		///< 전화번호
    int	nPoiID;										///< POI ID
    int	nAddrCode;									///< 주소 코드
    unsigned char byRPFlag;							///< RPFlag (경로 탐색시 사용)
    int	nDist;										///< 현위치에서 거리
    int	nDeg;										///< 방위각
    int	nKind;										///< 테마검색 일때 : 이미지 존재 1, 유가정보 일때 : 와이파이 주유소  1, 나머지 검색 : 업종코드
    DS_POINT tp;									///< 위치 좌표
    int	nOPIPoleCate;								///< 주유소종류 - category ID 주유소 코드와 같음
    int nOPIPrice;									///< 가격(유가 정보 일때만 유효) -1:TPEG 수신정보 없음, 0:TPEG 수신 되었으나 가격 정보 없음
    DS_DATETIME sGatherTime;						///< 유가정보 제공일자 (유가 정보 일때만 유효)

    char szOilCompany[16];							///< 주유소브랜드(SK, GS, 기타 등), 단말에서 브랜드 필터링시 사용
    int  nDataKind;                                 ///< 주유소종류 (3: 주유소, 4:충전소, 6: EV충전소)
    bool bHighGasoline;								///< 고급휘발유판매점여부 (0:판매안함, 1:판매)
    bool bLowestOilSt;								///< 최저가점포여부. 출력리스트상 최저가 점포일 경우(최저가:1, 아닌경우:0). SK주유소가 최저가인 경우에만 1로 설정
    unsigned int nOPIGasolinePrice;					///< 휘발유가격(원)
    unsigned int nOPIDieselPrice;					///< 경유가격(원)
    unsigned int nOPILPGPrice;						///< LPG가격(원)
    unsigned int nOPIHiGasolinePrice;				///< 프리미엄 휘발유 가격(원)
    unsigned int nOPIHiLPGPrice;					///< 프리미엄 LPG 가격(원)
    char	szNavseq[7];							///< 입구점번호

    int nPrimaryMax;                ///< 주번 최대값
    int nPrimaryMin;                ///< 주번 최소값
    int nSecondaryMax;              ///< 부번 최대값
    int nSecondaryMin;              ///< 부번 취소값

    //EV
    int	evStationCnt;                               //// 충전기 개수
    DS_POI_EVSTATION_CHARGERINFO  chargerInfo[D_MAXLEN_EVCHARGER];	////충전기리스트
}TPOI_SEARCH_LIST;

/*
    typedef struct tagDPoiParkInfos{
        char szCpName[D_MAXLEN_SISULNAME+1];			///< 제공처명 예)"모두의주차장","파킹박"
        char szParkUrl[D_MAXLEN_URL+1];					///< 모바일 웹 링크 URL
    }DS_POI_PARK_INFOS;

    /// POI 상세정보
    typedef struct tagDSearchPoiDetailInfo{
        int nPoiID;										///< POI ID
        char	szNavseq[7];							///< 입구점번호
        char szName[D_MAXLEN_SISULNAME+1];				///< 명칭
        char szAddrName[D_MAXLEN_ADDRNAME+1];			///< 구주소, 새주소 (새주소 요청시 DB에 새주소 정보가 없으면 구까지만 전달)
        DS_POINT tp;									///< 위치 좌표
        char szPhoneNumber[D_MAXLEN_PHONENUMBER+1];		///< 전화번호
        char szUrl[D_MAXLEN_URL+1];						///< 웹사이트
        bool bHoliday;									///< 연중무휴(true:연중무휴)
        char byPark;									///< 주차 가능 여부(0:불가능, 1:가능, 그외:정보없음)
        char szCateName[D_MAXLEN_SISULNAME+1];			///< 업종 명칭
        char szInfo[D_MAXLEN_DETAILINFO+1];				///< 소개글
        DS_POI_IMAGE_INFOS sPoiImageInfos[5];			///< POI 이미지 정보
        DS_POI_PARK_INFOS sPoiParkInfos[3];				///< 제휴 주차장 정보
    }DS_SEARCH_POI_DETAIL_INFO;

    /// 가맹점 상세정보
    typedef struct tagDSearchPoiJoinInfo{
        int nIdx;										///< 검색 결과 인덱스
        int nPoiID;										///< POI ID
        char szJoinType[16];							///< 가맹점 종류 ("OKCB": OK캐시백, "TMEM": T맴버십, "SSCD": 삼성카드)
        char szName[D_MAXLEN_SISULNAME+1];				///< 명칭
        char szCashRate[8];								///< 현금 적립율
        char szCardRate[8];								///< 카드 적립율
        char szWorkTime[16];							///< 평일 영업시간
        char szWeekendWorkTime[16];						///< 주말 영업시간
        char szUrl[D_MAXLEN_URL+1];						///< 웹사이트
        char byPark;									///< 주차 가능 여부(0:불가능, 1:가능, 그외:정보없음)
        char szBenefit[512];							///< 혜택
        char szUserGuide[512];							///< 이용안내
        char szInformation[512];						///< 업소 소개
        int nItem;										///< 소개글 개수
        char szItemInfos[8][128];						///< 소개글

    }DS_SEARCH_POI_JOIN_INFO;
 */

typedef struct tagTPoiDetailInfo{
    int nPoiID;										///< POI ID
    char szName[D_MAXLEN_SISULNAME+1];				///< 명칭
    char szAddrName[D_MAXLEN_ADDRNAME+1];			///< 구주소, 새주소 (새주소 요청시 DB에 새주소 정보가 없으면 구까지만 전달)
    char szPhoneNumber[D_MAXLEN_PHONENUMBER+1];		///< 전화번호
    char szUrl[D_MAXLEN_URL+1];						///< 웹사이트
    bool bHoliday;									///< 연중무휴(true:연중무휴)
    char byPark;									///< 주차 가능 여부(0:불가능, 1:가능, 그외:정보없음)
    char szCateName[D_MAXLEN_SISULNAME+1];			///< 업종 명칭
    char szInfo[D_MAXLEN_DETAILINFO+1];				///< 소개글
    char szUserGuide[512];							///< 이용안내
    char szInformation[512];						///< 업소 소개
    DS_POINT tp;									///< 위치 좌표
    //-
    char szAddInformation[D_MAXLEN_DETAILINFO+1];	///< 추가 정보 (포맷 "[영업시간]00:00~23:59;[좌석수]50;[주차]주차불가능;[휴무]연중무휴;" )
    DS_DATETIME sGatherTime;						///< 유가정보 제공일자 (유가 정보 일때만 유효)
    int  nDataKind;                                 ///< 주유소종류 (3: 주유소, 4:충전소, 6: EV충전소)
    bool bHighGasoline;								///< 고급휘발유판매점여부 (0:판매안함, 1:판매)
    //x bool bLowestOilSt;								///< 최저가점포여부. 출력리스트상 최저가 점포일 경우(최저가:1, 아닌경우:0). SK주유소가 최저가인 경우에만 1로 설정
    unsigned int nOPIGasolinePrice;					///< 휘발유가격(원)
    unsigned int nOPIDieselPrice;					///< 경유가격(원)
    unsigned int nOPILPGPrice;						///< LPG가격(원)
    unsigned int nOPIHiGasolinePrice;				///< 프리미엄 휘발유 가격(원)
    unsigned int nOPIHiLPGPrice;					///< 프리미엄 LPG 가격(원)
    //-

    char	szNavseq[7];							///< 입구점번호

    char szDayOffInfo[D_MAXLEN_DAYOFFINFO+1];			/// 휴무정보
    char szBusinessHours[D_MAXLEN_BUSINESSHOURS+1];	/// 영업시간
    char szDayOffDate[D_MAXLEN_DAYOFFDATE+1];			/// 휴무일자

}TPOI_DETAIL_INFO;

typedef struct tagTPoiEvDetailInfo{
    int nPoiID;									//< POI ID
    //서버검색에서 사용
    char szName[D_MAXLEN_SISULNAME+1];          // 충전소명
    char szAddrName[D_MAXLEN_ADDRNAME+1];		// 주소
    char szRoadAddr[D_MAXLEN_ROADADDR+1];       // 도로명주소
    char szAddrDetail[D_MAXLEN_ADDRDETAIL+1];   // 상세주소
    char szMngName[D_MAXLEN_MNGNAME+1];         // 운영기관
    char szMngId[D_MAXLEN_MNGID+1];             // 기관관리 ID
    char szPhoneNumber[D_MAXLEN_PHONENUMBER+1]; // 연락처
    bool bFreeYn;								// 주차비무료여부
    bool bReservYn;								// 예약가능여부
    char szUseTime[D_MAXLEN_USETIME+1];			// 이용가능시간
    bool bPayYn;                                // 유료여부
    char szFee[D_MAXLEN_FEE+1];                 // 단가
    char szUpdateDt[D_MAXLEN_USETIME+1];		// 충전상태수집일시(YYYYMMDD HH24:MI:SS)
    DS_POINT tp;								// 좌표
    int	evStationCnt;                           // 충전기 개수
    DS_POI_EVSTATION_CHARGERINFO chargerInfo[D_MAXLEN_EVCHARGER];	//충전기리스트

    //로컬검색에서 사용
    char szNavseq[7];							//< 입구점번호
    bool	bSlowChage; //완속
    //DC차데모,AC3상, DC콤보 - 급속종류
    bool	bDCDemo;	//DC차데모
    bool	bAC3;		//AC3상
    bool	bDCCombo;	//DC콤보

    //New로컬검색에서 사용
    char szPoidAddr[36];
    char szPoidRoadAddr[36];
}TPOI_EvDETAIL_INFO;

typedef struct tagTPoiStationInfo{
    int nPoiID;										///< POI ID
    char szName[D_MAXLEN_SISULNAME+1];				///< 명칭
    DS_POINT tp;									///< 위치 좌표
    int  nDataKind;                                 ///< 주유소종류 (3: 주유소, 4:충전소)
    unsigned int nOPIGasolinePrice;					///< 휘발유가격(원)
    unsigned int nOPIDieselPrice;					///< 경유가격(원)
    unsigned int nOPILPGPrice;						///< LPG가격(원)
    unsigned int nOPIHiGasolinePrice;				///< 프리미엄 휘발유 가격(원)
    unsigned int nOPIHiLPGPrice;					///< 프리미엄 LPG 가격(원)
}TPOI_STATION_INFO;

typedef struct tagTMyHomeOfficeFavoriteInfo {
    DS_MYFAVORITE baseInfo;
    char szHomePoiName[D_MAXLEN_SISULNAME+1];              ///< 명칭
    char szHomeAddress[D_MAXLEN_ADDRNAME+1];           ///< 구주소, 새주소 (새주소 요청시 DB에 새주소 정보가 없으면 구까지만 전달)    
    char szOfficePoiName[D_MAXLEN_SISULNAME+1];              ///< 명칭
    char szOfficeAddress[D_MAXLEN_ADDRNAME+1];           ///< 구주소, 새주소 (새주소 요청시 DB에 새주소 정보가 없으면 구까지만 전달)    
} TPOI_MYHOMEOFFICE_INFO;


typedef struct tagTFavoriteInfo {
    DS_FAVORITE_LIST baseInfo;
    char szPoiName[D_MAXLEN_SISULNAME+1];              ///< 명칭
    char szAddress[D_MAXLEN_ADDRNAME+1];           ///< 구주소, 새주소 (새주소 요청시 DB에 새주소 정보가 없으면 구까지만 전달)        
} TPOI_FAVORITE_INFO;

typedef struct tagTRecentDestinationInfo {
    DS_SEARCH_RECENT_DESTNATION baseInfo;
    char szPoiName[D_MAXLEN_SISULNAME+1];              ///< 명칭
    char szAddress[D_MAXLEN_ADDRNAME+1];           ///< 구주소, 새주소 (새주소 요청시 DB에 새주소 정보가 없으면 구까지만 전달)            
} TPOI_RECENTDESTINATION_INFO;

typedef struct tagTSendToCarInfo {
    DS_SENDTOCARINFO baseInfo;
    char szPoiName[D_MAXLEN_SISULNAME+1];              ///< 명칭
    char szAddress[D_MAXLEN_ADDRNAME+1];           ///< 구주소, 새주소 (새주소 요청시 DB에 새주소 정보가 없으면 구까지만 전달)
} TPOI_SENDTOCAR_INFO;

typedef struct tagTUploadItemBase
{
    int id;
    int bytes;
} TPOI_UPLOAD_ITEMBASE;

typedef struct tagTUploadRecentDestination
{
    TPOI_UPLOAD_ITEMBASE item;
    DS_SEARCH_UPLOAD_RECENT_DESTNATION recentdest;

} TPOI_UPLOAD_RECENTDESTINATION;

typedef struct tagTRemoveRecentDestination
{
    TPOI_UPLOAD_ITEMBASE item;
    DS_SEARCH_REMOVE_RECENT_DESTNATION recentdest;

} TPOI_REMOVE_RECENTDESTINATION;

typedef struct tagTUploadFavorite
{
    TPOI_UPLOAD_ITEMBASE item;
    DS_UPLOAD_POIFAVORITE favorite;

} TPOI_UPLOAD_FAVORITE;

typedef struct tagTUploadHomeOffice
{
    TPOI_UPLOAD_ITEMBASE item;
    DS_UPLOAD_POIMYFAVORITE homeoffice;
} TPOI_UPLOAD_HOMEOFFICE;

typedef struct tagTRegistFavorite
{
    TPOI_UPLOAD_ITEMBASE item;
    DS_REGIST_FAVORITE_LIST favorite;
} TPOI_REGIST_FAVORITE;

typedef struct tagTRemoveFavorite
{
    TPOI_UPLOAD_ITEMBASE item;
    DS_REMOVE_POIFAVORITE favorite;

} TPOI_REMOVE_FAVORITE;

typedef struct tagTModifyFavorite
{
    TPOI_UPLOAD_ITEMBASE item;
    DS_MODIFY_POIFAVORITE favorite;

} TPOI_MODIFY_FAVORITE;

typedef struct tagTUploadMySetting
{
    TPOI_UPLOAD_ITEMBASE item;
} TPOI_UPLOAD_MYSETTING;


typedef struct tagTSisulNamePrediction
{
    static const int BYTES = (D_MAXLEN_SISULNAME + 80) * 200;   // (D_MAXLEN_SISULNAME + 80) * 200 (80 is json key's length + extra size)
    char rawdata[BYTES];
} TSisulNamePrediction;

typedef struct tagTGeoCoding
{
    static const int BYTES = 2 * 1024;  // 2kb
    char rawdata[BYTES];
} TGeoCoding;

typedef struct tagTComplexImage
{
    static const int BYTES = 256 * 1024;
    char rawdata[BYTES]; // image size < 256kb
    int size;   // recevied image size
} TComplexImage;

typedef struct tagTAqi
{
    static const int BYTES = 64 * 1024; // 64kb
    char rawdata[BYTES];
} TAqi;

class SearchRequestBase
{
public:
    enum SearchType {
        SearchBySisulName = 0,
        SearchByAddress,
        SearchByAddressVr,
        SearchNextAddress,
        SearchByServerAddress,
        SearchBySisulAround,
        SearchByEvAround,
        SearchBySisulCategory,
        SearchNextCategory,
        SearchBySisulNamePrediction,
        SearchByPOIid,                  // get detail POI info
        SearchByEvPOIid,                // 전기차충전소
        SearchRecentDestination,
        SearchFavorites,
        SearchMyHomeOffice,
        SearchMySetting,
        SearchRegisterSendToCar,
        SearchFindSendToCar,
        SearchRemoveSendToCar,
        SearchLowestOilSt,
        SearchGeoCoding,
        SearchComplexImage,
        SearchAQI,
        SearchAddress,
        RemoveSearchQueue,               // 검색 요청 Queue 삭제
        UploadAllRecentDestination,
        UploadAllFavorite,
        UploadAllFavoritesAndResetHomeOffice,
        UploadAllFavoritesAndRegistHomeOffice,
        UploadMySetting,
        RegistNewFavorite,
        RegistMultiFavorites,
        ModifyFavorite,
        ModifyMultiFavorites,
        RemoveMultiFavorites,
        RemoveMultiRecentDestinations,
    };

    enum SortType {
        SortTypeNone = 0,
        SortTypeByName,
        SortTypeByDist,
        SortTypeByPrice,
        SortTypeByServerAccuracy,     // only supported when using server-search
        SortTypeByServerDist,         // only supported when using server-search
    };

    explicit SearchRequestBase(SearchType type)
        : m_type(type)
        , m_sortType(SortTypeNone)
        , m_x(0)
        , m_y(0)
        , m_startIndex(0)
        , m_count(0)
        , m_newAddrServer(false)
        , m_newAddrLocal(false)
        , m_distance(3)
        , m_riCode(0)
        , m_isOnline(false)
        , m_isLineChanged(false)
        {

        }

    virtual ~SearchRequestBase() { }

    void setSortType(SortType type) { m_sortType = type; }
    void setX(int x) { m_x = x; }
    void setY(int y) { m_y = y; }
    void setExtra(const QVariantMap &extra) { m_extra = extra; }
    void setStartIndex(int index) { m_startIndex = index; }
    void setCount(int count) { m_count = count; }
    void setNewAddr(bool newAddr) { m_newAddrServer = newAddr; m_newAddrLocal = newAddr; }
    void setNewAddrServer(bool newAddr) { m_newAddrServer = newAddr; }
    void setNewAddrLocal(bool newAddr) { m_newAddrLocal = newAddr; }
    void setDistance(int dist) { m_distance = dist; }
    void setRiCode(int riCode) { m_riCode = riCode; }
    void setOnline(bool isOnline) { m_isOnline = isOnline; }
    void setLineChanged(bool isLineChanged) { m_isLineChanged = isLineChanged; }

    SearchType searchType() const { return m_type; }
    SortType sortType() const { return m_sortType; }
    int x() const { return m_x; }
    int y() const { return m_y; }
    QVariantMap extra() const { return m_extra; }
    int startIndex() const { return m_startIndex; }
    int count() const { return m_count; }
    bool newAddr() const { return getOnline()? m_newAddrServer : m_newAddrLocal; }
    int distance() const { return m_distance; }
    int riCode() const { return m_riCode; }
    bool getOnline() const { return m_isOnline;}
    bool getLineChanged() const { return m_isLineChanged;}

protected:
    SearchType m_type;
    SortType m_sortType;
    int m_x;
    int m_y;
    QVariantMap m_extra;
    int m_startIndex;
    int m_count;
    bool m_newAddrServer;
    bool m_newAddrLocal;
    int m_distance;
    int m_riCode;
    bool m_isOnline;
    bool m_isLineChanged;
};

class SearchRequest : public SearchRequestBase
{
public:
    explicit SearchRequest(SearchType type)
        : SearchRequestBase(type)
        , m_bPrediction(false)
        , m_cSanFlag('1')
        , m_startAddrIndex(0)
        , m_startAddrCnt(0)
        , m_nAddrCode(0)
        , m_categoryIdx(0)
        , m_categoryCode(0)
        , m_POIid(0)
        , m_startAptIndex(0)
        , m_startAptCnt(0)
        , m_keywordServer("")
        , m_keywordLocal("")
    {

    }

    virtual ~SearchRequest() { }

    void setPrediction(bool flag) { m_bPrediction = flag; }
    void setKeyword(const QString &keyword) { m_keywordServer = keyword;m_keywordLocal = keyword; }
    void setKeywordLocal(const QString &keyword) { m_keywordLocal = keyword; }
    void setKeywordServer(const QString &keyword) { m_keywordServer = keyword; }
    void setJibun(const QString &jibun) { m_jibun = jibun; }
    void setSanFlag(unsigned char c) { m_cSanFlag = c; }
    void setStartAddrIndex(int index) { m_startAddrIndex = index; }
    void setStartAddrCnt(int cnt) { m_startAddrCnt = cnt; }
    void setAddrCode(int addrCode) { m_nAddrCode = addrCode; }
    void setCategoryIdx(int index) { m_categoryIdx = index; }
    void setCategoryCode(int cateCode) { m_categoryCode = cateCode; }
    void setPoiId(int index) { m_POIid = index; }
    void setNavSeq(const QString &navSeq) { m_navSeq = navSeq;}
    void setStartAptIndex(int index) { m_startAptIndex = index; }
    void setStartAptCnt(int cnt) { m_startAptCnt = cnt; }
    void setAddressVrParams( const QVariantMap& params ) { m_addressVrParams = params; }
    QVariantMap addressVrParams() { return m_addressVrParams; }

    bool isPrediction() const { return m_bPrediction; }
    QString keyword() const { return getOnline()? m_keywordServer: m_keywordLocal; }
    QString jibun() const { return m_jibun; }
    unsigned char sanFlag() const { return m_cSanFlag; }
    int startAddrIndex() const { return m_startAddrIndex; }
    int startAddrCnt() const { return m_startAddrCnt; }
    int addrCode() const { return m_nAddrCode; }
    int categoryIdx() const { return m_categoryIdx; }
    int categoryCode() const { return m_categoryCode; }
    int PoiId() const { return m_POIid; }
    int startAptIndex() const { return m_startAptIndex; }
    int startAptCnt() const { return m_startAptCnt; }

    void setOiltype(DeOilType oiltype) { m_oilType = oiltype; }
    void setAddressName(const QString &keyword) { m_addressName = keyword; }
    DeOilType oilType() const { return m_oilType; }
    QString addressName() const { return m_addressName; }
    QString navSeq() const { return m_navSeq; }

private:
    // SearchBySisulName

    // SearchBySisulPrediction
    bool m_bPrediction;

    // SearchByAddress
    QString m_keywordServer;
    QString m_keywordLocal;
    QString m_jibun;
    unsigned char m_cSanFlag;
    int m_startAddrIndex;
    int m_startAddrCnt;
    int m_nAddrCode;

    // SearchBySisulAround
    int m_categoryIdx;

    // SearchBySisulCategory
    int m_categoryCode;

    // SearchByServerSisulName
    QString m_addressName;
    DeOilType m_oilType;

    // SearchByPOIid
    int m_POIid;

    int m_startAptIndex;
    int m_startAptCnt;

    QString m_navSeq;
    QVariantMap m_addressVrParams;
};

class SearchNextAddrRequest : public SearchRequestBase
{
public:
    explicit SearchNextAddrRequest(SearchType type)
        : SearchRequestBase(type)
        , m_addressCode(0)
    {

    }

    virtual ~SearchNextAddrRequest() { }

    void setAddressCode(int addressCode) { m_addressCode = addressCode; }

    int addressCode() const { return m_addressCode; }

protected:
    int m_addressCode;
};

class SearchNextCateRequest : public SearchRequestBase
{
public:
    explicit SearchNextCateRequest(SearchType type)
        : SearchRequestBase(type)
        , m_categoryCode(0)
    {

    }

    virtual ~SearchNextCateRequest() { }

    void setCategoryCode(int categoryCode) { m_categoryCode = categoryCode; }

    int categoryCode() const { return m_categoryCode; }

protected:
    int m_categoryCode;
};

class SearchNotNddsRequest: public SearchRequestBase
{
public:
    SearchNotNddsRequest(SearchType type)
        : SearchRequestBase(type)
    {
    }
    virtual ~SearchNotNddsRequest() {}
    void setSingleUrl(const QString& url) { m_SingleURL = url; }
    void setHostDomain(const QString& domain) { m_HostDomain = domain;}
    QString singleUrl() const { return m_SingleURL;}
    QString hostDomain() const { return m_HostDomain;}

private:
    QString m_SingleURL;
    QString m_HostDomain;
};

typedef QSharedPointer<SearchRequestBase> SearchRequestBasePtr;

class SearchReplyBaseInterface
{
public:
    virtual ~SearchReplyBaseInterface() {}
    virtual QObject* qobject() = 0;
    virtual SearchRequestBasePtr request() = 0;
    virtual bool error() const = 0;
    virtual int errorCause() const = 0;

signals:
    // just hints
    void finished();
    void queueChanged(int count);
};

class SearchReplyInterface
{
public:
    virtual ~SearchReplyInterface() {}

    virtual const TPOI_SEARCH_LIST* results() const = 0;
    virtual int resultCount() const = 0;
    virtual int totalCount() const = 0;
};

class SearchPoiReplyInterface
{
public:
    virtual ~SearchPoiReplyInterface() {}

    virtual const TPOI_DETAIL_INFO* results() const = 0;
    virtual int resultCount() const = 0;
    virtual int totalCount() const = 0;
};

class SearchEvPoiReplyInterface
{
public:
    virtual ~SearchEvPoiReplyInterface() {}

    virtual const TPOI_EvDETAIL_INFO* results() const = 0;
    virtual int resultCount() const = 0;
    virtual int totalCount() const = 0;
};

class SearchPoiStationReplyInterface
{
public:
    virtual ~SearchPoiStationReplyInterface() {}

    virtual const TPOI_STATION_INFO* results() const = 0;
    virtual int resultCount() const = 0;
    virtual int totalCount() const = 0;
};

class SearchNextAddrReplyInterface
{
public:
    virtual ~SearchNextAddrReplyInterface() {}

    virtual const TS_RESULT_ADDR* results() const = 0;
    virtual int resultCount() const = 0;
    virtual int totalCount() const = 0;
};

class SearchNextCateReplyInterface
{
public:
    virtual ~SearchNextCateReplyInterface() {}

    virtual const TS_RESULT_CATE* results() const = 0;
    virtual int resultCount() const = 0;
    virtual int totalCount() const = 0;
};


class SearchFavoriteReplyInterface
{
public:
    virtual ~SearchFavoriteReplyInterface() {}
    virtual const TPOI_FAVORITE_INFO* results() const = 0;
    virtual int resultCount() const = 0;
    virtual int totalCount() const = 0;
};


class SearchMyHomeOfficeReplyInterface
{
public:
    virtual ~SearchMyHomeOfficeReplyInterface() {}
    virtual const TPOI_MYHOMEOFFICE_INFO* results() const = 0;
};

class SearchRecentDestinationReplyInterface
{
public:
    virtual ~SearchRecentDestinationReplyInterface() {}
    virtual const TPOI_RECENTDESTINATION_INFO* results() const = 0;
    virtual int resultCount() const = 0;
    virtual int totalCount() const = 0;
};

class SearchSendToCarReplyInterface
{
public:
    virtual ~SearchSendToCarReplyInterface() {}
    virtual const TPOI_SENDTOCAR_INFO* results() const = 0;
    virtual int resultCount() const = 0;
    virtual int totalCount() const = 0;
};

class SearchSisulNanmePredictionReplyInterface
{
    public:
    virtual const TSisulNamePrediction* results() const = 0;
    virtual int resultCount() const = 0;
    virtual int totalCount() const = 0;
};

class SearchGeoCodingReplyInterface
{
    public:
    virtual const TGeoCoding* results() const = 0;
};

class SearchComplexImageReplyInterface
{
    public:
    virtual const TComplexImage* results() const = 0;
};


class SearchAqiReplyInterface
{
    public:
    virtual const TAqi* results() const = 0;
};


// Upload Request Interfaces
class UploadRequestBase : public SearchRequestBase
{
public:
   explicit UploadRequestBase( SearchType type, int cnt=0):
        SearchRequestBase(type) { m_count = cnt; }

      void setUploadCount(int cnt) { m_count = cnt; }
      int uploadCount() const { return m_count; }
      virtual int size() const = 0;
      virtual const TPOI_UPLOAD_ITEMBASE* itemData() const = 0;
};

typedef QSharedPointer<UploadRequestBase> UploadRequestBasePtr;

class UploadAllRecentDestinationRequest: public UploadRequestBase
{
public: 
    UploadAllRecentDestinationRequest(int cnt=0) :
    UploadRequestBase(SearchRequestBase::UploadAllRecentDestination, cnt)
    {
    }

      virtual ~UploadAllRecentDestinationRequest() {}

      virtual int size() const override { return sizeof(TPOI_UPLOAD_RECENTDESTINATION) * uploadCount(); }
      virtual const TPOI_UPLOAD_ITEMBASE* itemData() const override { return reinterpret_cast<TPOI_UPLOAD_ITEMBASE*>(m_data.data()); }
      void setItemData(TPOI_UPLOAD_RECENTDESTINATION* d ) { m_data.reset(d); }


  private:
    QScopedArrayPointer<TPOI_UPLOAD_RECENTDESTINATION> m_data;

    friend class ServerSearch;
};


class UploadAllFavoriteRequest: public UploadRequestBase
{
public: 
    explicit UploadAllFavoriteRequest(int cnt=0, const QString option=QStringLiteral("FI") ) :
      UploadRequestBase(SearchRequestBase::UploadAllFavorite, cnt),
      m_UploadOption(option)
      {
        memset(&m_homeoffice,0,sizeof(TPOI_UPLOAD_HOMEOFFICE));
        if (option.isEmpty())
            m_UploadOption = QStringLiteral("FI");
      }

      virtual int size() const override { return sizeof(TPOI_UPLOAD_FAVORITE) * uploadCount(); }
      virtual const TPOI_UPLOAD_ITEMBASE* itemData() const override { return reinterpret_cast<TPOI_UPLOAD_ITEMBASE*>(m_data.data()); }
      void setItemData(TPOI_UPLOAD_FAVORITE* d ) { m_data.reset(d); }
      TPOI_UPLOAD_HOMEOFFICE* homeOffice() { return &m_homeoffice; }
      void setHomeOffice(const TPOI_UPLOAD_HOMEOFFICE* d) {
         m_homeoffice.homeoffice.nHomePoiID   = d->homeoffice.nHomePoiID;
         m_homeoffice.homeoffice.Hometp       = d->homeoffice.Hometp;
         m_homeoffice.homeoffice.byHomeRpFlag = d->homeoffice.byHomeRpFlag;
         m_homeoffice.homeoffice.nOfficePoiID  = d->homeoffice.nOfficePoiID;
         m_homeoffice.homeoffice.Officetp     = d->homeoffice.Officetp;
         m_homeoffice.homeoffice.byOfficeRpFlag = d->homeoffice.byOfficeRpFlag;
         strcpy(m_homeoffice.homeoffice.szHomeCustName, d->homeoffice.szHomeCustName);
         strcpy(m_homeoffice.homeoffice.szHomeNavSeq,   d->homeoffice.szHomeNavSeq);
         strcpy(m_homeoffice.homeoffice.szHomeAddInfo,  d->homeoffice.szHomeAddInfo);
         strcpy(m_homeoffice.homeoffice.szOfficeCustName, d->homeoffice.szOfficeCustName);
         strcpy(m_homeoffice.homeoffice.szOfficeNavSeq, d->homeoffice.szOfficeNavSeq);
         strcpy(m_homeoffice.homeoffice.szOfficeAddInfo, d->homeoffice.szOfficeAddInfo);
      }

  private:
    QScopedArrayPointer<TPOI_UPLOAD_FAVORITE> m_data;
    TPOI_UPLOAD_HOMEOFFICE m_homeoffice;
    QString m_UploadOption; //  "FI":즐겨찾기 업로드 / "FI_MC":POI즐겨찾기업로드 +집으로/회사로 초기화 / "FI_MS":POI즐겨찾기업로드 +집으로/회사로 설정 / NULL인 경우 기본 FI
    friend class ServerSearch;
};


class RegistMultiFavoritesRequest: public UploadRequestBase
{
public:
    explicit RegistMultiFavoritesRequest( int cnt=0 ) : 
    UploadRequestBase( SearchRequestBase::RegistMultiFavorites, cnt ) {}
    virtual int size() const override { return sizeof(TPOI_REGIST_FAVORITE) * uploadCount(); }
    virtual const TPOI_UPLOAD_ITEMBASE* itemData() const override { return reinterpret_cast<TPOI_UPLOAD_ITEMBASE*>(m_data.data()); }
    void setItemData(TPOI_REGIST_FAVORITE* d ) { m_data.reset(d); }

private:
    QScopedArrayPointer<TPOI_REGIST_FAVORITE> m_data;

    friend class ServerSearch;

};


class ModifyFavoriteRequest: public UploadRequestBase
{
public:
    ModifyFavoriteRequest() : 
    UploadRequestBase( SearchRequestBase::ModifyFavorite, 1) {}
    virtual int size() const override { return sizeof(TPOI_MODIFY_FAVORITE); }
    virtual const TPOI_UPLOAD_ITEMBASE* itemData() const override { return reinterpret_cast<TPOI_UPLOAD_ITEMBASE*>(m_data.data()); }
    void setItemData(TPOI_MODIFY_FAVORITE* d) { m_data.reset(d); }

private:
    QScopedPointer<TPOI_MODIFY_FAVORITE> m_data;

    friend class ServerSearch;    
};


class RemoveMultiFavoritesRequest: public UploadRequestBase
{
public:
    explicit RemoveMultiFavoritesRequest( int cnt=0 ) : 
       UploadRequestBase( SearchRequestBase::RemoveMultiFavorites, cnt) {}
       virtual int size() const override { return sizeof(TPOI_REMOVE_FAVORITE) * uploadCount(); }
       virtual const TPOI_UPLOAD_ITEMBASE* itemData() const override { return reinterpret_cast<TPOI_UPLOAD_ITEMBASE*>(m_data.data()); }
       void setItemData(TPOI_REMOVE_FAVORITE* d) { m_data.reset(d); }

private:
    QScopedArrayPointer<TPOI_REMOVE_FAVORITE> m_data;

    friend class ServerSearch;        
};


class RemoveMultiRecentDestinationsRequest: public UploadRequestBase
{
public:
    RemoveMultiRecentDestinationsRequest( int cnt=0):
    UploadRequestBase(RemoveMultiRecentDestinations, cnt) {}
    virtual int size() const override { return sizeof(TPOI_REMOVE_RECENTDESTINATION) * uploadCount(); }
    virtual const TPOI_UPLOAD_ITEMBASE* itemData() const override { return reinterpret_cast<TPOI_UPLOAD_ITEMBASE*>(m_data.data()); }
    void setItemData(TPOI_REMOVE_RECENTDESTINATION* d) { m_data.reset(d); }

private:
    QScopedArrayPointer<TPOI_REMOVE_RECENTDESTINATION> m_data;

    friend class ServerSearch;        
};

class UploadReplyBaseInterface
{
public:
    virtual ~UploadReplyBaseInterface() {}
    virtual int uploadedCount() const = 0;
};

}

Q_DECLARE_INTERFACE(SKT::SearchReplyBaseInterface, "com.skt.tmap.search.SearchReplyBaseInterface/1.0")
Q_DECLARE_INTERFACE(SKT::SearchReplyInterface, "com.skt.tmap.search.SearchReplyInterface/1.0")
Q_DECLARE_INTERFACE(SKT::SearchPoiReplyInterface, "com.skt.tmap.search.SearchPoiReplyInterface/1.0")

Q_DECLARE_INTERFACE(SKT::SearchEvPoiReplyInterface, "com.skt.tmap.search.SearchEvPoiReplyInterface/1.0")
Q_DECLARE_INTERFACE(SKT::SearchPoiStationReplyInterface, "com.skt.tmap.search.SearchPoiStationReplyInterface/1.0")
Q_DECLARE_INTERFACE(SKT::SearchNextAddrReplyInterface, "com.skt.tmap.search.SearchNextAddrReplyInterface/1.0")
Q_DECLARE_INTERFACE(SKT::SearchNextCateReplyInterface, "com.skt.tmap.search.SearchNextCateReplyInterface/1.0")
Q_DECLARE_INTERFACE(SKT::SearchFavoriteReplyInterface, "com.skt.tmap.search.SearchFavoriteReplyInterface/1.0")
Q_DECLARE_INTERFACE(SKT::SearchMyHomeOfficeReplyInterface, "com.skt.tmap.search.SearchMyHomeOfficeReplyInterface/1.0")
Q_DECLARE_INTERFACE(SKT::SearchRecentDestinationReplyInterface, "com.skt.tmap.search.SearchRecentDestinationReplyInterface/1.0")
Q_DECLARE_INTERFACE(SKT::SearchSendToCarReplyInterface, "com.skt.tmap.search.SearchSendToCarReplyInterface/1.0")
Q_DECLARE_INTERFACE(SKT::SearchSisulNanmePredictionReplyInterface, "com.skt.tmap.search.SearchSisulNanmePredictionReplyInterface/1.0")
Q_DECLARE_INTERFACE(SKT::SearchGeoCodingReplyInterface, "com.skt.tmap.search.SearchGeoCodingReplyInterface/1.0")
Q_DECLARE_INTERFACE(SKT::SearchComplexImageReplyInterface, "com.skt.tmap.search.SearchComplexImageReplyInterface/1.0")
Q_DECLARE_INTERFACE(SKT::SearchAqiReplyInterface, "com.skt.tmap.search.SearchAqiReplyInterface/1.0")
Q_DECLARE_INTERFACE(SKT::UploadReplyBaseInterface, "com.skt.tmap.search.UploadReplyBaseInterface/1.0")

#endif // SEARCHREQUEST_H
