#ifndef CSWMEDIATOR_H
#define CSWMEDIATOR_H

#include "mediator/AbstractMediator.h"
#include "util/CswManager.h"
#include "util/CswHistory.h"

// CSW simplification 정책에 의해 내비앱에서 CSW 기능을 사용하지 않음
// 19.06.11 ymhong
#define DISABLE_CSW_IN_NAVI

namespace SKT
{

class CSWMediator : public AbstractMediator
{
    Q_OBJECT

public:
    CSWMediator(QObject *);
    virtual ~CSWMediator();

    virtual QList<int> listNotificationInterests() Q_DECL_OVERRIDE;
    virtual bool handleNotification(const mvc::INotificationPtr &) Q_DECL_OVERRIDE;

private:
    void procCSW(int key);
    void sendDebugInfo();
    void selectCswFromHistory(const QString &viewName);

private:
    bool mBlockSoftkey = false;
    bool mCswGoBackPressed = false;
    QString mViewName;
#ifndef DISABLE_CSW_IN_NAVI
    CswManager mCswManager;
    CswHistoryManager mHistoryManager;
#endif
};

}//SKT

#endif // CSWMEDIATOR_H
