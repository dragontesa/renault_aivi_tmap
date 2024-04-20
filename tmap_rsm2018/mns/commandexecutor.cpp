#include "commandexecutor.h"
#include <QVariantMap>
#include <QDebug>
namespace SKT {
QString CommandExecutor::done = "Done";
QString CommandExecutor::notSupported = "지원 되지 않는 기능입니다.";
QString CommandExecutor::notImplemneted = "미구현 입니다.";

enum {
    CreatePositionInScreenCenter = 1,
    CreatePositionToSelectedPOI,
    SetPositionToDestination,
    SetPositionToVia,
    SetPositionToVia1,
    SetPositionToVia2,
    SetPositionToVia3,
    ClearAllPosition,
    RequestRoute,
    RequestRoute2,
    RequestRoute3,
    RequestRouteForOptimum,
    RequestRouteForMimumTime,
    RequestRouteForFreePee,
    RequestRouteForMimumDistance,
    RequestRouteForHighway,
    RequestRouteForBiginer,
    WhatIsDeparture,
    WhatIsVia1,
    WhatIsVia2,
    WhatIsVia3,
    WhatIsDestination,
    WhatIsAllVia,
    ClearVia1,
    ClearVia2,
    ClearVia3,
    ClearDestination,
    Via1Down,
    Via2Down,
    Via2Up,
    Via3Down,
    Via3Up,
    DestinationDown,
    DestinationUp,
    RequestReRouteByUSER,
    RequestReRouteByAuto,
    SearchByName,
    SearchByName_SortByDistance,
    SearchByNewAddress,
    SearchByOldAddress,
    getNextAddressList,
    getOldNextAddressList,
};

temppp commandMap[] = {
//    {"생성",{CreatePositionInScreenCenter,0}},
//    {"추가",{CreatePositionInScreenCenter,0}},
//    {"만들기",{CreatePositionInScreenCenter,0}},
//    {"추가 만들기",{CreatePositionInScreenCenter,SetPositionToDestination,0}},
    {"화면 위치를 목적지로 추가",{CreatePositionInScreenCenter,SetPositionToDestination,0}},
    {"화면 위치로 경로 요청",{CreatePositionInScreenCenter,SetPositionToDestination,RequestRoute,0}},
    {"화면에서 위치 생성",{CreatePositionInScreenCenter,0,}},
    {"화면 위치 생성",{CreatePositionInScreenCenter,0,}},
    {"화면 중심점으로 위치 생성",{CreatePositionInScreenCenter,0,}},
    {"보내는 검색 객체로 위치 생성",{CreatePositionToSelectedPOI,0,}},
    {"선택된 결과로 위치 생성",{CreatePositionToSelectedPOI,0,}},
    {"위치를 목적지로 설정",{SetPositionToDestination,0,}},
    {"위치를 목적지로 추가",{SetPositionToDestination,0,}},
    {"위치를 경유지로 추가",{SetPositionToVia,0,}},
    {"위치를 경유지1로 추가",{SetPositionToVia1,0,}},
    {"위치를 경유지2로 추가",{SetPositionToVia2,0,}},
    {"위치를 경유지3으로 추가",{SetPositionToVia3,0,}},
    {"모든 지점을 지움",{ClearAllPosition,0,}},
    {"경로 요청",{RequestRoute,RequestRoute2,RequestRoute3,0,}},
    {"경로 모두 요청",{RequestRoute,RequestRoute2,RequestRoute3,0,}},
//    {"첫 번째 경로 요청",{RequestRoute,0,}},
//    {"두 번째 경로 요청",{RequestRoute2,0,}},
//    {"세 번째 경로 요청",{RequestRoute3,0,}},
    {"경로 탐색",{RequestRoute,0,}},
    {"최적 경로 요청",{RequestRouteForOptimum,0,}},
    {"최소시간 경로 요청",{RequestRouteForMimumTime,0,}},
    {"무료도로 경로 요청",{RequestRouteForFreePee,0,}},
    {"최단거리 경로 요청",{RequestRouteForMimumDistance,0,}},
    {"고속도로우선 경로 요청",{RequestRouteForHighway,0,}},
    {"초보자 경로 요청",{RequestRouteForBiginer,0,}},
    {"출발지를 알려줘",{WhatIsDeparture,0,}},
    {"경유지1을 알려줘",{WhatIsVia1,0,}},
    {"경유지2를 알려줘",{WhatIsVia2,0,}},
    {"경유지3을 알려줘",{WhatIsVia3,0,}},
    {"도착지를 알려줘",{WhatIsDestination,0,}},
    {"목적지를 알려줘",{WhatIsDestination,0,}},
    {"모든 경로점들을 알려줘",{WhatIsAllVia,0,}},
    {"경유지1을 삭제",{ClearVia1,0,}},
    {"경유지2를 삭제",{ClearVia2,0,}},
    {"경유지3을 삭제",{ClearVia3,0,}},
    {"목적지를 삭제",{ClearDestination,0,}},
    {"경유지1을 아래로",{Via1Down,0,}},
    {"경유지2를 아래로",{Via2Down,0,}},
    {"경유지2를 위로",{Via2Up,0,}},
    {"경유지3을 아래로",{Via3Down,0,}},
    {"경유지3을 위로",{Via3Up,0,}},
    {"목적지를 아래로",{DestinationDown,0,}},
    {"목적지를 위로",{DestinationUp,0,}},
    {"경로 수동 재탐색",{RequestReRouteByUSER,0,}},
    {"경로 자동 재탐색",{RequestReRouteByAuto,0,}},
    {"이름으로 검색",{SearchByName,0,}},
    {"거리순 이름으로 검색",{SearchByName_SortByDistance,0,}},
    {"신주소로 검색",{SearchByNewAddress,0,}},
    {"구주소로 검색",{SearchByOldAddress,0,}},
    {"하위 신주소로 검색",{getNextAddressList,0,}},
    {"하위 구주소로 검색",{getOldNextAddressList,0,}},

};

CommandExecutor::CommandExecutor(RPnSearch * client){
    this->client = client;
    exeMap = new voidMap();
    registerCommand();
}

CommandExecutor::~CommandExecutor(){
    commandQueue.clear();
    deleteMap(exeMap);
}

void CommandExecutor::exeuteCommand(RPRequest *req){
    exeMethod(req);
}

void CommandExecutor::insertCommand(RPRequest *req){
    lockQueue.lock();
    commandQueue.enqueue(req);
    lockQueue.unlock();
    waitForCommand.release();
}

CommandExecutor::voidMap* CommandExecutor::findSubMap(CommandExecutor::voidMap *parentMap, QStringList &list,int pos){


    if((*parentMap)[list[pos]] != 0){
        voidMap *thisMap = (voidMap *)((*parentMap)[list[pos]]);
        if(pos+1 == list.size())
            return thisMap;
        return findSubMap(thisMap,list,pos+1);
    } else {
        voidMap *thisMap = new voidMap();
        (*parentMap)[list[pos]] = (void *)thisMap;
        if(pos+1 < list.size()){
            return findSubMap(thisMap,list,pos+1);
        }else{
            return thisMap;
        }
    }
}

void CommandExecutor::registerCommand(){
    int size = sizeof(commandMap) / sizeof(temppp);
    for(int c = 0 ; c < size ; c++){
        QString command = commandMap[c].command;
        QStringList commandItems = command.split(" ");

        voidMap *regMap = findSubMap(exeMap,commandItems,0);
        QString *mIstr = new QString();
        mIstr->append(QString::number(commandMap[c].commandIndex[0]));
        int mSize = sizeof(commandMap[c].commandIndex)/sizeof(int);
        for(int cc = 1 ; cc < mSize ; cc++)
        {
            mIstr->append(",");
            mIstr->append(QString::number(commandMap[c].commandIndex[cc]));
        }
        (*regMap)["methodIndex"] = (void*)mIstr;
    }
}

void CommandExecutor::deleteMap(voidMap* map){
    QList<QString> keys = map->keys();
    for(int c= 0 ; c < keys.size() ; c++){
        if(keys[c].compare("methodIndex")==0){
            QString * mid =(QString*)(*map)["methodIndex"];
            if(mid != 0)
                delete mid;
            continue;
        }
        voidMap *cmap = (voidMap *)((*map)[keys[c]]);
        deleteMap(cmap);
    }
    delete map;
}


// sangbong.woo - s


bool CommandExecutor::callCommandMethod(int index, RPRequest *req){
    switch(index){
    case CreatePositionInScreenCenter:
        client->makeMapPosition(RPnSearch::RpPositionScreen);
        break;
    case CreatePositionToSelectedPOI:
//        client->makeMapPosition(RPnSearch::RpPositionSelectedPOI,lparam);
        break;
    case SetPositionToDestination:
        client->applyMapPosition(RPnSearch::RpRouteDestination);
        break;
    case SetPositionToVia:
        client->applyMapPosition();
        break;
    case SetPositionToVia1:
        client->applyMapPosition(RPnSearch::RpRouteVia1);
        break;
    case SetPositionToVia2:
        client->applyMapPosition(RPnSearch::RpRouteVia2);
        break;
    case SetPositionToVia3:
        client->applyMapPosition(RPnSearch::RpRouteVia3);
        break;
    case ClearAllPosition:
        client->clearMapPosition(RPnSearch::RpRouteNONE);
        break;
    case RequestRoute:{
        client->setVia(RPnSearch::RpRouteDeparture,req->getVia(1));
        client->setVia(RPnSearch::RpRouteVia1,req->getVia(2));
        client->setVia(RPnSearch::RpRouteVia2,req->getVia(3));
        client->setVia(RPnSearch::RpRouteVia3,req->getVia(4));
        client->setVia(RPnSearch::RpRouteDestination,req->getVia(5));

        QRouteOptionListPtr rpOptions = req->getRPOptions();
        QStringListPtr tvasFileName = req->getTvasFileName();
        if(rpOptions.isNull() == true || tvasFileName.isNull() == true) { // will delete
            client->requestRoute(RPnSearch::RpOptionOPTIMUM); // will delete
        }
        else{
            for(int c = 0 ; c < rpOptions->length() && c < tvasFileName->length() ; c++){
                client->requestRoute(static_cast<RPnSearch::RpOption>(rpOptions->at(c)),tvasFileName->at(c));
            }
        }
    }
        break;
    case RequestRoute2:
//        client->requestRoute(RPnSearch::RpOptionFREEWAY,2); // 추후 설정 값으로 동작 되어야함.
        break;
    case RequestRoute3:
//        client->requestRoute(RPnSearch::RpOptionSHORTESTWAY,3); // 추후 설정 값으로 동작 되어야함.
        break;
    case RequestRouteForOptimum:
        client->requestRoute(RPnSearch::RpOptionOPTIMUM);
        break;
    case RequestRouteForMimumTime:
        notSupported;
        return false;
        break;
    case RequestRouteForFreePee:
        client->requestRoute(RPnSearch::RpOptionFREEWAY);
        break;
    case RequestRouteForMimumDistance:
        client->requestRoute(RPnSearch::RpOptionSHORTESTWAY);
        break;
    case RequestRouteForHighway:
        client->requestRoute(RPnSearch::RpOptionEXPRESSWAY);
        break;
    case RequestRouteForBiginer:
        notSupported;
        return false;
        break;
    case WhatIsDeparture:{
        QVariantMap &resData = *(req->getRes());
        RoutePosPtr posPtr = client->getVia(RPnSearch::RpRouteDeparture);
        resData.insert("Via1",QVariant::fromValue(posPtr));
    }
        break;
    case WhatIsVia1:{
        QVariantMap &resData = *(req->getRes());
        RoutePosPtr posPtr = client->getVia(RPnSearch::RpRouteVia1);
        resData.insert("Via2",QVariant::fromValue(posPtr));
    }
        break;
    case WhatIsVia2:{
        QVariantMap &resData = *(req->getRes());
        RoutePosPtr posPtr = client->getVia(RPnSearch::RpRouteVia2);
        resData.insert("Via3",QVariant::fromValue(posPtr));
    }
        break;
    case WhatIsVia3:{
        QVariantMap &resData = *(req->getRes());
        RoutePosPtr posPtr = client->getVia(RPnSearch::RpRouteVia3);
        resData.insert("Via4",QVariant::fromValue(posPtr));
    }
        break;
    case WhatIsDestination:{
        QVariantMap &resData = *(req->getRes());
        RoutePosPtr posPtr = client->getVia(RPnSearch::RpRouteDestination);
        resData.insert("Via5",QVariant::fromValue(posPtr));
    }
        break;
    case WhatIsAllVia:{
        QVariantMap &resData = *(req->getRes());
        RoutePosPtr posPtr1 = client->getVia(RPnSearch::RpRouteDestination);
        RoutePosPtr posPtr2 = client->getVia(RPnSearch::RpRouteVia1);
        RoutePosPtr posPtr3 = client->getVia(RPnSearch::RpRouteVia1);
        RoutePosPtr posPtr4 = client->getVia(RPnSearch::RpRouteVia1);
        RoutePosPtr posPtr5 = client->getVia(RPnSearch::RpRouteDestination);
        resData.insert("Via1",QVariant::fromValue(posPtr1));
        resData.insert("Via2",QVariant::fromValue(posPtr2));
        resData.insert("Via3",QVariant::fromValue(posPtr3));
        resData.insert("Via4",QVariant::fromValue(posPtr4));
        resData.insert("Via5",QVariant::fromValue(posPtr5));
    }
        break;
    case ClearVia1:
        client->clearMapPosition(RPnSearch::RpRouteVia1);
        break;
    case ClearVia2:
        client->clearMapPosition(RPnSearch::RpRouteVia2);
        break;
    case ClearVia3:
        client->clearMapPosition(RPnSearch::RpRouteVia3);
        break;
    case ClearDestination:
        client->clearMapPosition(RPnSearch::RpRouteDestination);
        break;
    case Via1Down:
        client->swapMapPosition(RPnSearch::RpRouteVia1,RPnSearch::RpRouteVia2);
        break;
    case Via2Down:
        client->swapMapPosition(RPnSearch::RpRouteVia2,RPnSearch::RpRouteVia3);
        break;
    case Via2Up:
        client->swapMapPosition(RPnSearch::RpRouteVia2,RPnSearch::RpRouteVia1);
        break;
    case Via3Down:
        client->swapMapPosition(RPnSearch::RpRouteVia3,RPnSearch::RpRouteDestination);
        break;
    case Via3Up:
        client->swapMapPosition(RPnSearch::RpRouteVia3,RPnSearch::RpRouteVia2);
        break;
    case DestinationDown:
        client->swapMapPosition(RPnSearch::RpRouteDestination,RPnSearch::RpRouteVia3);
        break;
    case DestinationUp:
        client->swapMapPosition(RPnSearch::RpRouteDestination,RPnSearch::RpRouteVia3);
        break;
    case RequestReRouteByUSER:
        notImplemneted;
        return false;
        break;
    case RequestReRouteByAuto:
        // TODO: implement!
        client->requestReroute();
        break;
    default:
        notImplemneted;
        return false;
    }
    return true;
}

void CommandExecutor::execute(RPRequest *req){
    QVariantMap &reqData = *(req->getReq());
    QString command = reqData["Command"].toString();
    QVariantMap* resData = new QVariantMap();
    resData->insert("Answer",done);
    reqData.insert("Response",QVariant::fromValue(QVariantMapPtr(resData)));

    QStringList commandItems = command.split(" ");
    voidMap *regMap = findSubMap(exeMap,commandItems,0);
    QString *methodIndex = (QString*) (*regMap)["methodIndex"];
    if(methodIndex == 0){
        // it is new command!!
        // we will need to register commandMethod of new command
        // so need to Loging... to Server
        (*resData)["Answer"] = notSupported;
        return;
    }
    QStringList methodIndexList = methodIndex->split(",");
    for(int c = 0 ; c < methodIndexList.size() ; c++){
        int mIndex = methodIndexList[c].toInt();
        if(mIndex == 0)
            break;
        bool retValue = callCommandMethod(mIndex, req);
        if(retValue == false)
            return;
    }
}
// sangbong.woo - e

void CommandExecutor::run() {
    forever {
        waitForCommand.acquire();
        if (commandQueue.empty()) {
            continue;
        }
        lockQueue.lock();
        RPRequest *req = commandQueue.dequeue();
        lockQueue.unlock();

        exeMethod(req);

//        emit req->finished();
        QMetaObject::invokeMethod(req, "finished", Qt::QueuedConnection);
    }
}

void CommandExecutor::exeMethod(RPRequest *req){
    lockMethod.lock();
    execute(req);
    lockMethod.unlock();
}

}

