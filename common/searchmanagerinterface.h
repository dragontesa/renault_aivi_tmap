#ifndef SEARCHMANAGERINTERFACE_H
#define SEARCHMANAGERINTERFACE_H

#include "searchrequest.h"

namespace SKT {

class SearchManagerInterface
{
public:
    virtual ~SearchManagerInterface() {}
    virtual SearchReplyBaseInterface* request(const SearchRequestBasePtr &req) = 0;
    virtual void clear() = 0;
    virtual void setOwner(const QObject *owner) = 0;
};

}

Q_DECLARE_INTERFACE(SKT::SearchManagerInterface, "com.skt.tmap.search.SearchManagerInterface/1.0")


#endif // SEARCHMANAGERINTERFACE_H
