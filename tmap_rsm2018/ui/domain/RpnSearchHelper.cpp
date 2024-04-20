#include "RpnSearchHelper.h"

using namespace SKT;

RpnSearchHelper::RpnSearchHelper()
    : mRpnSearch( nullptr )
{

}

RpnSearchHelper* RpnSearchHelper::instance()
{
    static RpnSearchHelper Instance;

    return &Instance;
}

RPManagerInterface* RpnSearchHelper::rpnSearch()
{
    return mRpnSearch;
}

void RpnSearchHelper::setRpnSearch( RPManagerInterface* rpnSearch )
{
    mRpnSearch = rpnSearch;
}

bool RpnSearchHelper::isSync() const
{
    return mRpnSearch->isSync();
}

RPRequestHandle *RpnSearchHelper::createRequest(RPRequestPtr req)
{
    return mRpnSearch->createRequest(req);
}

// Text Command Type
void RpnSearchHelper::requestCommand(RPRequestHandle* req) // will deprecate
{
    mRpnSearch->requestCommand(req);
}
