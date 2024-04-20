#include "RouteSummaryHelper.h"

#include "StringHelper.h"
#include "VSM_Navi_RgData.h"
#include "RouteSummaryTurnCodeHelper.h"
#include "RouteSummaryItemListModel.h"

namespace SKT
{

RouteSummaryHelper::RouteSummaryHelper(const std::vector<VSM_stTmapSummaryList>& summaries, const QStringList& addressList, bool isLocal)
{
    mIsLocal = isLocal;

    parseHeader(summaries);

    qDebug() << "parseHeader() : mIsLocal :" << mIsLocal << ", mSkipItemCount :" << mSkipItemCount;

    mModel = parseBody(summaries, addressList);
}

QVariant RouteSummaryHelper::model() const
{
    return mModel;
}

void RouteSummaryHelper::parseHeader(const std::vector<VSM_stTmapSummaryList> &summaries)
{
    // 로컬 탐색시 경로 요약 정보 끝부분에 목적지, 경유지 수만큼 추가정보가 붙게 된다.
    // 예) 경유지가 둘일 때
    // ...
    // [ 36 ] 66602019, 19730243  -> 66602338, 19730267 [ 48,  38m,  9,  14] turn(23) "동소문로;삼양로"
    // [ 37 ] 66602212, 19730417  -> 66602212, 19730417 [ 0 ,  0 m,  0,  0 ] turn(25) "경유지"
    // [ 38 ] 66602212, 19730417  -> 66602212, 19730417 [ 0 ,  0 m,  0,  0 ] turn(26) "경유지"
    // [ 39 ] 66602212, 19730417  -> 66602212, 19730417 [ 0 ,  0 m,  0,  0 ] turn(23) "목적지"
    //

    // 서버 탐색시 경로 요약 정보 끝부분에 목적지에 대한 추가 정보만 붙는다.
    // 예) 경유지가 둘일 때
    // ...
    // [ 27 ] 66602211, 19730415  -> 66602211, 19730415 [ 48,  0 m,  0,  0 ] turn(1 ) "목적지"
    // [ 28 ] 66602211, 19730415  -> 66602211, 19730415 [ 0 ,  0 m,  0,  0 ] turn(23) "목적지"
    if (mIsLocal) {
        mSkipItemCount = 0;
        bool noRg = false;

        // 뒤에서 부터 목적지, 경유지에 해당하는 turnCode의 갯수를 센다.
        auto iter = summaries.crbegin();
        for ( ; iter != summaries.crend(); ++iter) {
            auto summary = *iter;
            auto rsd = summary.ucRsdTurnCode;
            //auto turnCode = RouteSummaryTurnCodeHelper::fromRsdTurnCode(rsd);
            //if (isArrivalOrViaTurnCode(turnCode)) {

            if (RouteSummaryTurnCodeHelper::isNoRg(rsd))
                noRg = true;

            if (RouteSummaryTurnCodeHelper::isSummaryCode(rsd)) {
                ++mSkipItemCount;
            } else {
                break;
            }
        }
        if (!noRg) {
            if (0 < mSkipItemCount) {
                --mSkipItemCount;
            }
            if (mSkipItemCount <= 1) {
                //Q_ASSERT(1 < mSkipItemCount);
                qDebug() << "Q_ASSERT(1 < mSkipItemCount)";
            }
        } else {
            Q_ASSERT(1 <=mSkipItemCount);
        }
    } else {
        mSkipItemCount = 1;
    }
}

QVariant RouteSummaryHelper::parseBodyOld(const std::vector<VSM_stTmapSummaryList> &summaries, const QStringList &addressList)
{
    auto model = t_new_shared<RouteSummaryItemListModel>();
    auto index = 0;
    auto summaryNum = summaries.size();
    auto* codec = StringHelper::instance();
    auto prevTurn = 0;
    QString prevSectionName = addressList[0];
    auto isLast = false;
    auto lastIndex = mIsLocal ? summaryNum-(mSkipItemCount+1) : summaryNum-(mSkipItemCount);
    auto prevX = 0;
    auto prevY = 0;    
    auto prevDistance = 0;

    for (auto summary : summaries) {
        auto sectionName = codec->fromCp949(summary.szSectionName);
        auto rsd = summary.ucRsdTurnCode;
        auto turnCode = RouteSummaryTurnCodeHelper::fromRsdTurnCode(rsd);
        auto sx = summary.startWpt.x;
        auto sy = summary.startWpt.y;
        auto ex = summary.endWpt.x;
        auto ey = summary.endWpt.y;
        auto x = 0;
        auto y = 0;
        auto traffic = summary.ucTrafficCollectCon;
        auto speed = summary.ucTrafficCollectSpeed;
        auto time = summary.uiSectionTime;
        auto distance = mIsLocal ? summary.uiSectionDist : prevDistance;
        prevDistance = summary.uiSectionDist;

        if (index == 0) {
            // 첫번째 아이템은 빈 섹션네임
            Q_ASSERT(23 == turnCode);

            sectionName = QObject::tr("출발지");

            x = sx;
            y = sy;
            prevX = x;
            prevY = y;
            prevTurn = turnCode;
        } else if (index == lastIndex) {
            // 마지막 아이템은 주요도로
            if (mIsLocal) {
                if (ROUTE_SUMMARY_POI_ARRIVAL != turnCode) {
                    // rsd가
                    // TURN_NOT_SER_EXP,
                    // TURN_NOT_SER_NORMAL,
                    // TURN_NOT_SER_SPEC
                    // 인 경우 예외 처리 해야 함. 예) 인천 -> 부산IC 안내 후 경로 상세시
                } else {
                    //Q_ASSERT(sectionName.contains(";"));
                    Q_ASSERT(ROUTE_SUMMARY_POI_ARRIVAL/*23*/ == turnCode);
                }
                prevTurn = turnCode;
            } else {
                Q_ASSERT(sectionName == "목적지");
                prevTurn = ROUTE_SUMMARY_POI_ARRIVAL/*23*/;
                isLast = true;
            }

            sectionName = QObject::tr("도착지");
            prevSectionName = addressList[4];
            if (mIsLocal) {
                x = ex;
                y = ey;
            } else {
                x = sx;
                y = sy;
            }
        } else {
            switch (turnCode) {
            case ROUTE_SUMMARY_POI_VIA1:
            case ROUTE_SUMMARY_POI_VIA2:
            case ROUTE_SUMMARY_POI_VIA3: {
                    auto id = turnCode - ROUTE_SUMMARY_POI_VIA1 + 1;
                    sectionName = QObject::tr("경유지") + QString("%1").arg(id);
                    if (mIsLocal)
                        prevTurn = turnCode;
                }
                break;

            default:
                break;
            }

            // 첫번째, 두번째 아이템이 같은 위치를 가리켜야 하기 때문에(출발지)
            // prevX, prevY에 좌표를 담아 처리.
            // TMCRSM-410.
            // 18.10.16 ymhong.
            if (mIsLocal) {
                x = prevX;;
                y = prevY;;
                prevX = ex;
                prevY = ey;
            } else {
                x = prevX;;
                y = prevY;;
                prevX = sx;
                prevY = sy;
                prevTurn = turnCode;
                isLast = isArrivalTurnCode(turnCode);
            }
        }

#ifdef QT_DEBUG
        printSummary(index, sx, sy, ex, ey, turnCode, traffic, distance, speed, time, codec->fromCp949(summary.szSectionName));
#endif

        if (!isLast || !mIsLocal) {
            model->append(RouteSummaryItemVo{
                              x,
                              y,
                              turnCode,
                              distance,
                              prevSectionName,
                              sectionName,
                              traffic,
                              speed,
                              time
                          });
        }

        ++index;

        prevTurn = turnCode;
        prevSectionName = sectionName;
        if (mIsLocal && isArrivalTurnCode(turnCode))
            isLast = true;
    }

    return QVariant::fromValue(model);
}

// 경로 상세 정보는 서버를 기준으로 정리함.
// 기존 처리 함수는 parseBodyOld로 rename.
// 19.04.09 ymhong
QVariant RouteSummaryHelper::parseBody(const std::vector<VSM_stTmapSummaryList> &summaries, const QStringList &addressList)
{
    auto model = t_new_shared<RouteSummaryItemListModel>();
    auto* codec = StringHelper::instance();
    for (auto address : addressList) {
        mAddresses.append(address);
    }

    auto summarySize = summaries.size();
    RouteSummaryItemVo departureVo;
    RouteSummaryItemVo via1Vo;
    RouteSummaryItemVo via2Vo;
    RouteSummaryItemVo via3Vo;
    auto via1Pos = 0;
    auto via2Pos = 0;
    auto via3Pos = 0;

    for (auto i=0; i<summarySize; i++) {
        auto summary = summaries.at(i);
        auto nextSummary = i<summarySize-1 ? summaries.at(i+1) : summaries.at(i);
        auto sectionName = codec->fromCp949(summary.szSectionName);
        auto nextSectionName = codec->fromCp949(nextSummary.szSectionName);
        auto rsd = summary.ucRsdTurnCode;
        auto turnCode = RouteSummaryTurnCodeHelper::fromRsdTurnCode(rsd);
        auto sx = summary.startWpt.x;
        auto sy = summary.startWpt.y;
        auto traffic = summary.ucTrafficCollectCon;
        auto speed = summary.ucTrafficCollectSpeed;
        auto time = summary.uiSectionTime;
        auto distance = summary.uiSectionDist;
        auto item = RouteSummaryItemVo{
                sx,
                sy,
                turnCode,
                distance,
                sectionName,
                nextSectionName,
                traffic,
                speed,
                time
            };

#ifdef QT_DEBUG
        //printSummary(i, sx, sy, 0, 0, turnCode, traffic, distance, speed, time, codec->fromCp949(summary.szSectionName));
#endif

        switch (turnCode) {
        case ROUTE_SUMMARY_POI_DEPARTURE:
            departureVo = item;
            departureVo.setTurnCode(1);
            item.setFrom(mAddresses[0]);
            break;
        case ROUTE_SUMMARY_POI_VIA1:
            via1Vo = item;
            via1Vo.setFrom("경유지1");
            via1Pos = i + 2;
            item.setFrom(mAddresses[1]);
            item.setTo("");
            break;
        case ROUTE_SUMMARY_POI_VIA2:
            via2Vo = item;
            via2Vo.setFrom("경유지2");
            via2Pos = i + 3;
            item.setFrom(mAddresses[2]);
            item.setTo("");
            break;
        case ROUTE_SUMMARY_POI_VIA3:
            via3Vo = item;
            via3Vo.setFrom("경유지3");
            via3Pos = i + 4;
            item.setFrom(mAddresses[3]);
            item.setTo("");
            break;
        case ROUTE_SUMMARY_POI_ARRIVAL:
            if (5 <= mAddresses.count()) {
                item.setFrom(mAddresses[4]);
                item.setTo("");
            }
            break;
        }
        model->append(item);
    }
    model->insert(1, departureVo);
    if (0 < via1Pos) {
        model->insert(via1Pos, via1Vo);
    }
    if (0 < via2Pos) {
        model->insert(via2Pos, via2Vo);
    }
    if (0 < via3Pos) {
        model->insert(via3Pos, via3Vo);
    }

    return QVariant::fromValue(model);
}

bool RouteSummaryHelper::isArrivalTurnCode(int turnCode) const
{
    return turnCode == ROUTE_SUMMARY_POI_ARRIVAL;
}

bool RouteSummaryHelper::isArrivalOrViaTurnCode(int turnCode) const
{
    switch (turnCode) {
    case ROUTE_SUMMARY_POI_VIA1://23:
    case ROUTE_SUMMARY_POI_VIA2:
    case ROUTE_SUMMARY_POI_VIA3:
    case ROUTE_SUMMARY_POI_ARRIVAL:
        return true;

    default:
        return false;
    }
}

#ifdef QT_DEBUG
void RouteSummaryHelper::printSummary(int index, int sx, int sy, int ex, int ey, int turnCode, int traffic, int distance, int speed, int time,  const QString &sectionName) const
{
    qDebug() << "[" << index << "] "
             << sx << ", " << sy << " -> " << ex << ", " << ey << " "
             << "[" << traffic << ", " << distance << "m, " << time << ", " << speed << "] "
             << "turn(" << turnCode << ") "
             << sectionName;
}
#endif

int RouteSummaryHelper::skipItemsCount() const
{
    return mSkipItemCount;
}

void RouteSummaryHelper::setFooterItemCount(int footerItemCount)
{
    mSkipItemCount = footerItemCount;
}

QString RouteSummaryHelper::routeToString(TeRouteOption e)
{
    switch (e) {
    case E_ROUTEOPTION_OPTIMUM: return "BestPath";
    case E_ROUTEOPTION_EXPRESSWAY: return "ExpresswayPriority";
    case E_ROUTEOPTION_FREEWAY: return "FreeRoad";
    case E_ROUTEOPTION_SHORTESTWAY: return "ShortestDistance";
    case E_ROUTEOPTION_EASY: return "BeginnerPass";
    case E_ROUTEOPTION_SHORTTIME: return "MinimumTime";
    }
    return QString();
}

TeRouteOption RouteSummaryHelper::routeToEnum(const QString &s) {
    if (s == "BestPath") {
        return E_ROUTEOPTION_OPTIMUM;
    } else if (s == "MinimumTime") {
        return E_ROUTEOPTION_SHORTTIME;
    } else if (s == "FreeRoad") {
        return E_ROUTEOPTION_FREEWAY;
    } else if (s == "ShortestDistance") {
        return E_ROUTEOPTION_SHORTESTWAY;
    } else if (s == "ExpresswayPriority") {
        return E_ROUTEOPTION_EXPRESSWAY;
    } else if (s == "BeginnerPass") {
        return E_ROUTEOPTION_EASY;
    } else {
        Q_ASSERT(0 && "Invalid key");
    }
}

QStringList RouteSummaryHelper::routeTypeStrings()
{
    QStringList ret;
    ret += routeToString(E_ROUTEOPTION_OPTIMUM);
    ret += routeToString(E_ROUTEOPTION_SHORTTIME);
    ret += routeToString(E_ROUTEOPTION_FREEWAY);
    ret += routeToString(E_ROUTEOPTION_SHORTESTWAY);
    ret += routeToString(E_ROUTEOPTION_EXPRESSWAY);
    ret += routeToString(E_ROUTEOPTION_EASY);
    return ret;
}

}//SKT
