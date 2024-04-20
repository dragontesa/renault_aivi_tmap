#include "RouteManager.h"

#ifndef Q_OS_WIN32
#include "../vsm/map.h"
#include "../vsm/addressmanager.h"
#endif

#include "Constants.h"
#include "Facade.h"
#include "MarkerManager.h"

#include "VSM_Map.h"
#include "../platforms/linux-gen3/interface/server/navigation/navigationservicetype.h"

namespace SKT {

RouteManager* RouteManager::sInstance = nullptr;

#define VIA_SAVE_HEADER    "version=1"

//-----------------------------------------------------------------------------
//  RouteManager Class
//-----------------------------------------------------------------------------
RouteManager *RouteManager::sharedInstance()
{
    return sInstance;
}

RouteManager::RouteManager() : addressManager(0)
{
    RouteManager::sInstance = this;
}

void RouteManager::initialize(QObject *addr)
{
    clear();

    addressManager = addr;

    Q_ASSERT(addressManager != nullptr);

    connect(addressManager, SIGNAL(replied(int,int,QString)), this, SIGNAL(updated(int,int,QString)));
}

void RouteManager::clear()
{
    for (int i=0; i<RouteItem_Num; i++) {
        mItems[i].reset();
        mBackupItems[i].reset();
    }
}

void RouteManager::attachMapView(QObject* mapView)
{
    if ( mapView != nullptr ) {
        mMarkerManager.attachMapView(mapView);
    }
}

void RouteManager::detachMapView(QObject* mapView)
{
    mMarkerManager.detachMapView(mapView);
}

bool RouteManager::hasVia() const
{
    int count = 0;
    for (int i=RouteItem_Via1; i<=RouteItem_End; i++) {
        if (isValidPosition(mItems[i].wx, mItems[i].wy)) {
            ++count;
        }
    }

    // 도착 지점 포함 유효한 경유지가 한개 이면 경유지가 없는 것으로 간주한다.
    // 목적지 없이 경유지가 한개인 경우 목적지로 간주.
    return 1 < count;
}

bool RouteManager::hasVia(int index) const
{
    if (RouteItem_Start <= index && index <= RouteItem_End) {
        return isValidPosition(mItems[index].wx, mItems[index].wy);
    }
    return false;
}

bool RouteManager::hasEmptyVia() const
{
    for (int i=RouteItem_Via1; i<=RouteItem_End; i++) {
        if (!isValidPosition(mItems[i].wx, mItems[i].wy)) {
            return true;
        }
    }

    return false;
}

int RouteManager::addVia(const RouteManager::RouteItem &item)
{
    if (!isValidPosition(item.wx, item.wy))
        return -1;

    for (int i=RouteItem_Via1; i<=RouteItem_End; i++) {
        if (!isValidPosition(mItems[i].wx, mItems[i].wy)) {
            setItem(static_cast<RouteItemType>(i), item);
            return i;
        }
    }

    return -1;
}

void RouteManager::backup()
{
    for (int i=RouteItem_Start; i<=RouteItem_End; i++) {
        mBackupItems[i] = mItems[i];
#ifdef QT_DEBUG
        auto item = mBackupItems[i];
                if (item.wx && item.wy) {
            qDebug() << QString("[%1]").arg(i) << item.name << item.wx << item.wy;
        }
#endif
    }
}

void RouteManager::restore()
{
    for (int i=RouteItem_Start; i<=RouteItem_End; i++) {
        mItems[i] = mBackupItems[i];
    }
}

void RouteManager::restoreFromStringList(const QStringList &sl, bool skipDeparture)
{
    if (1 < sl.count()) {
        auto version = sl.at(0);
        if (version == VIA_SAVE_HEADER) {
            auto count = sl.count() - 1;
            if (count % 8 == 0) {
                auto num = count / 8;

                int index;
                int base;
                RouteItem item;
                for (int i=0; i<num; i++) {
                    base = 1 + i*8;
                    index = sl.at(base + 0).toInt();

                    auto isDeparture = index == RouteItem_Start;
                    if (skipDeparture && isDeparture)
                        continue;

                    item.poiId = sl.at(base + 1).toInt();
                    item.wx = sl.at(base + 2).toInt();
                    item.wy = sl.at(base + 3).toInt();
                    item.name = sl.at(base + 4);
                    item.address = sl.at(base + 5);
                    item.navSeq = sl.at(base + 6).toInt();
                    item.rpFlag = sl.at(base + 7).toInt();
                    if (!item.isNull()) {
                        item.status = INSERTED;
                    }

                    setItem(RouteItemType(index), item);
                }
            }
        } else {
            qDebug() << "The version of the saved via information is incorrect.";
        }
    }
}

int RouteManager::moveDown(int index)
{
    int another;
    if (index < RouteItem_End) {
        another = index+1;
    } else {
        another = static_cast<int>(RouteItem_Via1);
    }
    swap(index, another);

    return another;
}

int RouteManager::moveUp(int index)
{
    int another;
    if (index > RouteItem_Via1) {
        another = index-1;
    } else {
        another = static_cast<int>(RouteItem_End);
    }
    swap(index, another);

    return another;
}

bool RouteManager::remove(int index)
{
    if (!mItems[index].toPoint().isNull()) {
        mItems[index].wx = mItems[index].wy = 0;
        mItems[index].name.clear();
        mItems[index].status = DELETED;

        return true;
    }

    return false;
}

RouteManager::RouteItem RouteManager::getItem(RouteItemType type) const
{
    return mItems[type];
}

void RouteManager::setItem(RouteItemType type, const RouteManager::RouteItem &item)
{
    auto pos0 = mItems[type].toPoint();
    auto pos1 = item.toPoint();
    auto samePosition = pos0 == pos1;

    mItems[type] = item;

    if (!isValidPosition(item.wx, item.wy)) {
        mItems[type].name.clear();
    } else {
        if (mItems[type].name.isEmpty()) {
            // 독도인 경우 주소만 전달.
            mItems[type].name = mItems[type].address;
        }

        if (mItems[type].name.isEmpty() || !samePosition) {
            // 전달된 아이템의 주소가 없는 경우에만 요청한다.
            if (item.address.isEmpty() && item.name.isEmpty()) {
                RouteRequestAddress request(this);
                request(item.wx, item.wy);
            }
        }
    }
}

bool RouteManager::insertItem(RouteItemType type, const RouteManager::RouteItem &item, bool overwriteIfItemsFull)
{
    if ( !overwriteIfItemsFull && !hasEmptyVia() ) {
        // overwrite가 false이고 빈 경유지슬롯이 없다면 실패
        return false;
    }

    auto i = static_cast<int>(type);
    auto copy = item;
    for ( ; i <= RouteItem_End; i++ ) {
        auto temp = mItems[i];
        if (!temp.isNull()) {
            temp.status = MOVED;
        }
        mItems[i] = copy;
        copy = temp;

        if (copy.isNull()) {
            break;
        }
    }
    if (item.address.isEmpty() && item.name.isEmpty()) {
        RouteRequestAddress request(this);
        request(item.wx, item.wy);
    }
    return true;
}

RouteManager::RouteItem RouteManager::targetItem() const
{
    for (int i=RouteItem_End; i>RouteItem_Start; --i) {
        auto poi = mItems[i].name;
        if (poi.length() > 0) {
            Q_ASSERT(mItems[i].wx != 0);
            Q_ASSERT(mItems[i].wy != 0);
            return mItems[i];
        }
    }
    return RouteItem {};
}

void RouteManager::addViaMarker(QObject* mapView, int id, const QPoint &pos)
{
    mMarkerManager.setVia( mapView, id, pos );
}

void RouteManager::removeViaMarker(QObject* mapView, int id)
{
    mMarkerManager.removeVia( mapView, id );
}

void RouteManager::clearViaMarker(QObject* mapView)
{
    mMarkerManager.clear(mapView);
}

void RouteManager::requestAddress(int wx, int wy)
{
    QMetaObject::invokeMethod(addressManager, "request", Q_ARG(int, wx), Q_ARG(int, wy));
}

bool RouteManager::isValidPosition(int wx, int wy) const
{
    return wx != 0 && wy != 0;
}

void RouteManager::swap(int index0, int index1)
{
    mItems[index0].status = mItems[index0].isNull() ? DELETED : MOVED;
    mItems[index1].status = mItems[index1].isNull() ? DELETED : MOVED;

    std::swap<RouteItem>(mItems[index0], mItems[index1]);
}

void RouteManager::relocation()
{
    std::vector<RouteItem> items;
    for (auto item : mItems) {
        if (isValidPosition(item.wx, item.wy)) {
            items.push_back(item);
        }
    }
    for (int i=0; i<RouteItem_Num; i++)
        mItems[i].reset();

    for (int i=0; i<items.size(); i++) {
        if (i != items.size()-1) {
            setItem(RouteItemType(i), items[i]);
        } else {
            setItem(RouteItem_End, items[i]);
        }
    }
}

bool RouteManager::canInsert(int slot) const
{
    bool hasEmpty = false;
    for (auto i=slot; i<=RouteItem_End; i++) {
        auto item = mItems[i];
        if (item.isNull()) {
            hasEmpty = true;
            break;
        }
    }
    return hasEmpty;
}

QString RouteManager::startName() const
{
    return mItems[RouteItem_Start].name;
}

QString RouteManager::endName() const
{
    return mItems[RouteItem_End].name;
}

QString RouteManager::viaName(int index) const
{
    return mItems[RouteItem_Via1+index].name;
}

QString RouteManager::name(RouteItemType type) const
{
    return mItems[type].name;
}

QString RouteManager::targetAddress() const
{
    for (int i=RouteItem_End; i>RouteItem_Start; --i) {
        auto poi = mItems[i].name;
        if (poi.length() > 0) {
            Q_ASSERT(mItems[i].wx != 0);
            Q_ASSERT(mItems[i].wy != 0);
            return poi;
        }
    }
    return QString();
}

QPoint RouteManager::worldPos(RouteItemType type) const
{
    return mItems[type].toPoint();
}

QPoint RouteManager::targetWorldPos() const
{
    for (int i=RouteItem_End; i>RouteItem_Start; --i) {
        auto wx = mItems[i].wx;
        auto wy = mItems[i].wy;
        if (wx != 0 && wy != 0) {
            //Q_ASSERT(mItems[i].name.length() > 0);

            return QPoint(wx, wy);
        }
    }
    return QPoint();
}

void RouteManager::updateAddress(int wx, int wy, QString address)
{
    int index = -1;
    for (int i=0; i<RouteItem_Num; i++) {
        if (mItems[i].wx == wx && mItems[i].wy == wy && mItems[i].name.isEmpty()) {
            auto len = address.length();

            if (0 < len) {
                mItems[i].name = address;
                index = i;
            } else {
#if (0)
                // 주소 얻는 api가 빈 문자열을 돌려주는 케이스가 있음.
                // 그리고 주소가 없는 지역의 경우 미리 경유지 추가를 차단 하므로
                // 더이상 아래와 같은 처리는 필요가 없음.
                // 18.11.06 ymhong.

                // 주소를 알 수 없는 지역(예: 바다 한가운데)인 경우
                // 경유지에서 제거를 위해 좌표를 초기화(0,0) 한다.
                mItems[i].wx = 0;
                mItems[i].wy = 0;

                emit removeMarker(i);
#endif
            }
            break;
        }
    }
    if (0 <= index)
        emit updated(index);
}

void RouteManager::routeRequested(TS_POINT* points)
{
    Q_UNUSED(points);
}

void RouteManager::addSearchMarker(QObject* mapView, int id, int focusIdx, const QString& text, int x, int y)
{    
    mMarkerManager.addSearchMarker(mapView, id, focusIdx, text, x, y);
}

void RouteManager::removeSearchMarker(QObject* mapView, int id, int x, int y)
{
    mMarkerManager.removeSearchMarker(mapView, id, x, y);
}

QStringList RouteManager::getViaStringList() const
{
    QStringList s;
    s += VIA_SAVE_HEADER;
    for (int i=0; i<RouteItem_Num; i++) {
        auto x = mItems[i].wx;
        auto y = mItems[i].wy;
        auto name = mItems[i].name;
        auto address = mItems[i].address;
        auto navSeq = mItems[i].navSeq;
        auto rpFlag = mItems[i].rpFlag;
        auto poiId = mItems[i].poiId;
        if (x && y) {
            s += QString::number(i);
            s += QString::number(poiId);
            s += QString::number(x);
            s += QString::number(y);
            s += name;
            s += address;
            s += QString::number(navSeq);
            s += QString::number(rpFlag);
        }
    }
    return s;
}

bool RouteManager::checkVia(int type, const QPoint &currentPos)
{
    for (auto i = (int)RouteItem_Start; i <= (int)RouteItem_End; i++) {
        if (i == type)
            continue;
        QPoint pos = mItems[i].toPoint();
        if (!pos.isNull()) {
            VSM_Int32 distance;
            auto ok = VSMMap_GetDistanceWorld(pos.x(), pos.y(), currentPos.x(), currentPos.y(), distance);
            if (ok) {
                if (distance < 50) {
                    return false;
                }
            }
        }
    }
    return true;
}

CheckViaResult RouteManager::checkDepartureAndViaDistance(const QPoint &currentPos)
{
    int result = 0;
    const int departureAndVia = 0x1;
    const int departureAndArrival = 0x2;

    QPoint startPos = mItems[RouteItem_Start].toPoint();
    if (startPos.isNull()) {
        startPos = currentPos;
    }

    for (auto i = (int)RouteItem_Via1; i <= (int)RouteItem_End; i++) {
        QPoint pos = mItems[i].toPoint();
        if (!pos.isNull()) {
            VSM_Int32 distance;
            auto ok = VSMMap_GetDistanceWorld(startPos.x(), startPos.y(), pos.x(), pos.y(), distance);
            if (ok) {
                if (distance < 50) {
                    if (i != RouteItem_End)
                        result |= departureAndVia;
                    else
                        result |= departureAndArrival;
                }
            }
        }
    }
    if (result) {
        switch (result) {
        case departureAndVia:
            return CheckViaResultFail_DepartuerAndVia;
        case departureAndArrival:
            return CheckViaResultFail_DepartureAndArrival;
        case departureAndArrival | departureAndVia:
            return CheckViaResultFail_All;
        }
    } else {
        if (!checkViaAndArrivalDistance())
            return CheckViaResultFail_ViaAndArrival;
        else if (!checkViaDistance())
            return CheckViaResultFail_Via;

        return CheckViaResultOk;
    }
}

bool RouteManager::checkViaAndArrivalDistance() const
{
    QPoint arrival = mItems[RouteItem_End].toPoint();
    if (!arrival.isNull()) {
        for (auto i = (int)RouteItem_Via1; i <= (int)RouteItem_Via3; i++) {
            QPoint pos = mItems[i].toPoint();
            if (!pos.isNull()) {
                VSM_Int32 distance;
                auto result = VSMMap_GetDistanceWorld(arrival.x(), arrival.y(), pos.x(), pos.y(), distance);
                if (result) {
                    if (distance < 50) {
                        return false;
                    }
                }
            }
        }
    }
    return true;
}

// 두 좌표가 근접(50미터 이내)한지 검사하는 함수
static bool checkDistanceIsNear(const QPoint& p0, const QPoint& p1)
{
    if (!p0.isNull() && !p1.isNull()) {
        VSM_Int32 distance;
        auto result = VSMMap_GetDistanceWorld(p0.x(), p0.y(), p1.x(), p1.y(), distance);
        if (result) {
            if (distance < 50) {
                return true;
            }
        }
    }
    return false;
}

bool RouteManager::checkViaDistance() const
{
    QPoint via1 = mItems[RouteItem_Via1].toPoint();
    QPoint via2 = mItems[RouteItem_Via2].toPoint();
    QPoint via3 = mItems[RouteItem_Via3].toPoint();

    if (checkDistanceIsNear(via1, via2) ||
        checkDistanceIsNear(via2, via3) ||
        checkDistanceIsNear(via1, via3))
        return false;

    return true;
}

//-----------------------------------------------------------------------------
//  RouteRequestAddress Class
//-----------------------------------------------------------------------------
RouteRequestAddress::RouteRequestAddress(QObject *receiver) : mReceiver(receiver)
{
}

void RouteRequestAddress::operator ()(int wx, int wy)
{
    QObject *sender = RouteManager::sharedInstance();
    if (mReceiver == NULL)
        mReceiver = sender;

    QObject::connect(sender, SIGNAL(updated(int,int,QString)), mReceiver, SLOT(updateAddress(int,int,QString)), Qt::UniqueConnection);

    RouteManager::sharedInstance()->requestAddress(wx, wy);
}

bool RouteManager::RouteItem::operator==(const RouteManager::RouteItem &other) const
{
    return wx == other.wx &&
           wy == other.wy &&
           name == other.name &&
           navSeq == other.navSeq &&
           rpFlag == other.rpFlag &&
           searchFlag == other.searchFlag &&
           poiId == other.poiId;
}

RouteManager::RouteItem &RouteManager::RouteItem::operator=(const RouteManager::RouteItem &other)
{
    if ( this != &other ) {
        wx = other.wx;
        wy = other.wy;
        name = other.name;
        address = other.address;
        navSeq = other.navSeq;
        rpFlag = other.rpFlag;
        poiId = other.poiId;
        poleCate = other.poiId;
        dirty = false;
        status = other.status;
        searchFlag = other.searchFlag;
    }
    return *this;
}

void RouteManager::RouteItem::setTsRoutePos(TS_ROUTE_POS *p)
{
    p->nPoiID = poiId;
    p->tp.x = wx;
    p->tp.y = wy;
    p->byRPFlag = rpFlag;
    auto *codec = QTextCodec::codecForName("EUC-KR");
    if (!name.isEmpty()) {
        auto euckr = codec->fromUnicode(name);
        qstrncpy(p->szName, euckr.data(), TD_POSNAME_LEN);
    } else {
        auto euckr = codec->fromUnicode(address);
        qstrncpy(p->szName, euckr.data(), TD_POSNAME_LEN);
    }
}

void RouteManager::RouteItem::set(TS_ROUTE_POS &pos)
{
    poiId = pos.nPoiID;
    wx = pos.tp.x;
    wy = pos.tp.y;
    rpFlag = pos.byRPFlag;
    //name = QString::fromStdString(pos.szName);
}

bool RouteManager::RouteItem::isNull() const
{
    return wx == 0 && wy == 0;
}

void strcpyNavSeqFromInt(char *szNavSeq, int navSeq)
{
    auto str = QString::number(navSeq);
    strncpy(szNavSeq, str.toStdString().c_str(), 6);
}

}//SKT
