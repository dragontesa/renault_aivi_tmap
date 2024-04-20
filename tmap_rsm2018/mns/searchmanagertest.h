#ifndef SEARCHMANAGERTEST_H
#define SEARCHMANAGERTEST_H

#include <QObject>
#include <QSharedPointer>
#define DEBUG_GETNEXTLIST 0

namespace SKT {

class SearchManagerTest
{

public:
    explicit SearchManagerTest();
    ~SearchManagerTest();

    static SearchManagerTest* sharedInstance(void) {
        if (!s_instance) {
            s_instance = (SearchManagerTest*) new SearchManagerTest();
        }
        return s_instance;
    }

    void requestSearch_getNextAddressList(bool bNewAddr);
    void requestSearch_getNextAddressList_3depth(bool bNewAddr);
    void requestSearch_getNextCateList();

private:
    static SearchManagerTest* s_instance;
    Q_DISABLE_COPY(SearchManagerTest)
};


}   //namespace SKT
#endif // SEARCHMANAGERTEST_H
