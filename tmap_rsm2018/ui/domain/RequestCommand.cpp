#include "RequestCommand.h"

namespace SKT {

RequestCommand::RequestCommand(
  const mvc::IObserver::NotifyMethod& notifyMethod,
  const QObjectPtr& notifyContext,
  const QVariantMap reqCommand)
  : TObserver( notifyMethod, notifyContext ){
    reqData = reqCommand;
}
RequestCommand::~RequestCommand(){

}

QVariantMap RequestCommand::getReqData(){
    return reqData;
}

QVariantMap RequestCommand::getResData(){
    return resData;
}

void RequestCommand::setResData(const QVariantMap &data){
    resData = data;
}

void RequestCommand::setReqData(const QVariantMap &data){
    reqData = data;
}


}
