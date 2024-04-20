#ifndef FAVORITEMEDIATOR_H
#define FAVORITEMEDIATOR_H

#include "../AbstractMediator.h"

namespace SKT
{

class FavoriteMediator : public AbstractMediator
{
    Q_OBJECT

public:
    FavoriteMediator();
    virtual ~FavoriteMediator();

    enum AdditionType {
        None = -1,
        IntergratedSearch = 0,  // 통합검색
        RecentDestination,      // 최근목적지
        AroundSearch,         // 주변검색
        AddressSearch,          // 주소검색
        IndustrySearch,         // 업종검색
        MapSearch               // 지도검색
    };

    QList<int> listNotificationInterests();
    bool handleNotification(const mvc::INotificationPtr &);

private:
    AdditionType mAdditionType;
};

}//SKT

#endif // FAVORITEMEDIATOR_H
