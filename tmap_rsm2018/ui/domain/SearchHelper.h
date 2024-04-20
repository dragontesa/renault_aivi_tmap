#ifndef SEARCHHELPER_H
#define SEARCHHELPER_H

#include "searchmanagerinterface.h"

namespace SKT {

class SearchHelper
{
public:
    static SearchHelper* instance();
    SearchManagerInterface* search();
    void setSearch( SearchManagerInterface* search );
    SearchReplyBaseInterface* request(const SearchRequestBasePtr &req);
    void clear();
    void setOwner(const QObject *owner);

private:
    SearchHelper();

private:
    SearchManagerInterface* mSearch;
    QObject *mOwner;
};

}

#endif // SEARCHHELPER_H
