#ifndef RPNSEARCHHELPER_H
#define RPNSEARCHHELPER_H

#include "rpmanagerinterface.h"

namespace SKT {

class RpnSearchHelper
{
public:
    static RpnSearchHelper* instance();

    RPManagerInterface* rpnSearch();

    void setRpnSearch( RPManagerInterface* rpnSearch );

    bool isSync() const;

    RPRequestHandle* createRequest(RPRequestPtr req);
    void requestCommand(RPRequestHandle* req);

private:
    RpnSearchHelper();

    RPManagerInterface* mRpnSearch;
};

}

#endif // RPNSEARCHHELPER_H
