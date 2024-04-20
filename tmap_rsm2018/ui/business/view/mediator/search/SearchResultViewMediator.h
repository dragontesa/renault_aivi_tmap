#ifndef SEARCHRESULTVIEWMEDIATOR_H
#define SEARCHRESULTVIEWMEDIATOR_H

#include "mediator/AbstractViewMediator.h"
#include "VSM_Enum.h"
#include <QMutex>

namespace SKT {


class SearchResultViewMediator : public AbstractViewMediator
{
    Q_OBJECT
    Q_ENUMS( EventK )
    Q_ENUMS( OrderK )
    Q_ENUMS( HitObjectK )
    Q_ENUMS( DistanceK )
public:
    enum EventK {
        Activated,
        ChangeMenu,
        DeActivated,
        CreateView,
        CloseView,
        ShowView,
        HideView,
        ShowKeyboard,
        HideKeyboard,
        SelectZone1Title,
        SelectZone1TextInput,
        FocusZone1TextInput,
        RequestSearch,
        RequestAtThePOI,
        RequestAddVia,
        ViewSizeChanged,
        RequestAddMarker,
        RequestRemoveMarker,
        UpdateView,
        ModalChanged,
        ResetCsw,
        RegistCsw,
        RequestSetting,
        SaveSetting,
        RequestAddressType,
        RequestGetGasStationInfo,
        RequestGetEStationInfo,
        RequestDetailInfo,
        RequestRouteGuidanceState,
        NoResult,
        QueryOnlineStatus,
        ShowToast,
        RequestEvDetailInfo,
        SelectSoftkeyIndexMenuMode,
        ShowProgressIndicator,
        HideProgressIndicator,
        RequestSort,
        RemoveSearchQueue,
        RequestLang,
        RequestViewList,
        UpdateSearchResultCount,
        RequestCacheMerge,
        QueryInDriveMode,
    };

    enum OrderK {
        DistanceOrder,
        PriceOrder,
        AlphabeticalOrder,
        NameOrder
    };

    enum HitObjectK {
         HitNone       = VSM_HITCALLBACK_TYPE_NONE,
         HitPoi        = VSM_HITCALLBACK_TYPE_POI,
         HitMarker     = VSM_HITCALLBACK_TYPE_MARKER,
         HitOil        = VSM_HITCALLBACK_TYPE_OIL,
         HitTraffic    = VSM_HITCALLBACK_TYPE_TRAFFIC,
         HitEvPoi      = VSM_HITCALLBACK_TYPE_EV_POI,
         HitUserDefine = VSM_HITCALLBACK_TYPE_USER_DEFINE
    };


    //[주의]검색 API에 정해져 있는 값으로 변경
    enum DistanceK {
        Distance2Km,
        Distance5Km,
        Distance7Km,
        Distance10Km,
    };


    Q_INVOKABLE SearchResultViewMediator( QObject* viewComponent = nullptr );
    ~SearchResultViewMediator();

    QList<int> listNotificationInterests();

    QVariantMap runSort(QVariantMap& data);
    void finishedSort();

protected:
    bool onEvent( int event, const QVariant& data );
    bool handleNotification( const mvc::INotificationPtr& note );

private:
    bool sendByDestination(const QVariant &data);
    bool sendAddSearchMarker(const QVariant &data);
    bool sendRemoveSearchMarker(const QVariant &data);
    void closeAllViewWithoutCaller(const QString &caller);
    void closeAllView();
    void showConfirmDialog(const QString &address);
    void changeMenu( const QVariant& data );
    void updateSearchResultCount(const QString& text, int count);
    void responsedFreeTextSearchResult(const QVariantMap &data, const QVariantList &values);    
    int getIndexByFuel(int fuel);
    void getInfoChargingEvSt(QVariantList &result);

    bool invokeResultLowest(const QVariantMap &data);
    bool invokeResultDetailInfo(const QVariantMap &data);
    bool invokeResultEvDetailInfo(const QVariantMap &data);
    bool invokeResult(const QVariantMap &data);

    void queryInDriveMode();

    bool isVisible() const;
    bool isActive() const;

    //[임시:업종검색 필터]
    void setIndustryCodeMap();
    QVariantMap getIndustryPoiId(const int &startListIndex, const QString &code);
    bool isIndustryPoiID(const QVariantMap &poiIdMap, const int &poiId);
    QString getDistanceText(const int &distance);

    int getDistance(const int &x, const int &y);

private:
    QFutureWatcher<QVariantMap> *mWatcher;
    bool mIsRg;
    bool mIsRoadName;
    int mSearchResultCount;
    QVariant mDestData;
    bool mIsGadget = false;
    bool mUpdateSearchResultText = false;

    //[임시:업종검색 필터]
    QVariantMap mIndustryCodeMap;
    QVariantMap mIndustryPoiIdMap;

};
}

#endif // SEARCHRESULTVIEWMEDIATOR_H

