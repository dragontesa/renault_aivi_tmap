#include "CswHistory.h"

namespace SKT
{

void CswHistoryManager::insert(const QString &viewName, const CswHistory &history)
{
    mHistoryMap.insert(viewName, history);
}

bool CswHistoryManager::hasValue(const QString &viewName) const
{
    return mHistoryMap.contains(viewName);
}

CswHistory CswHistoryManager::value(const QString &viewName) const
{
    CswHistory history;
    history.section = -1;
    history.sequence = -1;
    if (hasValue(viewName)) {
        history = mHistoryMap.value(viewName);
    }
    return history;
}

void CswHistoryManager::erase(const QString &viewName)
{
    auto iter = mHistoryMap.find(viewName);
    if (iter != mHistoryMap.end()) {
        mHistoryMap.erase(iter);
    }
}

void CswHistoryManager::clear()
{
    mHistoryMap.clear();
}

}
