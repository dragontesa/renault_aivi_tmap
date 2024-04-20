#ifndef CSWHISTORY_H
#define CSWHISTORY_H

#include <QString>
#include <QMap>

namespace SKT
{

struct CswHistory
{
    int section;
    int sequence;
    int extra;
    QString objectName;
};

class CswHistoryManager
{
public:
    void insert(const QString &viewName, const CswHistory &history);
    bool hasValue(const QString &viewName) const;
    CswHistory value(const QString &viewName) const;
    void erase(const QString &viewName);
    void clear();

private:
    QMap<QString, CswHistory> mHistoryMap;
};

}//SKT

#endif // CSWHISTORY_H
