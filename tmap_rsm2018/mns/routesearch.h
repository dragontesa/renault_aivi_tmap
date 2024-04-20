#ifndef LOCAL_SEARCH_H
#define LOCAL_SEARCH_H

#include <QObject>
#include "MNS_Interface.h"
#include "MNS_Define.h"

namespace SKT {

class CRouteSearch : public QObject
{
    Q_OBJECT

public:
    // not thread safe
    static CRouteSearch* s_instance;
    static CRouteSearch* sharedInstance(void) {
        if (!s_instance) {
            s_instance = new CRouteSearch();
        }
        return s_instance;
    }

    void init();
    void destroy();
    void route();
    void stopRoute();
    void getNextAddrList();
    void getNextCateList();
    void MySearchTest(int type);
    QString toUnicode(const char* cstr);

public slots:

signals:
    void RSinit(bool success);
    void SearchRPEnd(bool success);

private:
    CRouteSearch();
    ~CRouteSearch();
    Q_DISABLE_COPY(CRouteSearch)

private:
    //static CRouteSearch* s_instance;

private:
    bool m_init;

private:
    // mns callbacks
    static int callbackSearchStatus(int nMsg, long lParam1, long lParam2, void *lpParam);
    static int callbackNddsStatus(int nMsg, long lParam1, long lParam2, void *lpParam);
};

}

#endif // LOCAL_SEARCH_H
