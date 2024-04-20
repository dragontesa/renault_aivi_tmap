#include "SearchHelper.h"

using namespace SKT;

SearchHelper::SearchHelper()
    : mSearch( nullptr ), mOwner( nullptr)
{

}

SearchHelper* SearchHelper::instance()
{
    static SearchHelper Instance;

    return &Instance;
}

SearchManagerInterface* SearchHelper::search()
{
    return mSearch;
}

void SearchHelper::setSearch( SearchManagerInterface* search )
{
    mSearch = search;
    setOwner(mOwner);
}

SearchReplyBaseInterface* SearchHelper::request(const SearchRequestBasePtr &req)
{
    SearchReplyBaseInterface* reply = 0;

    if (mSearch) {
        reply = mSearch->request(req);
    }

    return reply;
}

void SearchHelper::clear()
{
    if (mSearch) {
       mSearch->clear();
    }
}

void SearchHelper::setOwner(const QObject *owner)
{
    if (!owner)
        return;
    mOwner =  const_cast<QObject*>(owner);
    if (mSearch) {
        mSearch->setOwner(owner);
    }
}

