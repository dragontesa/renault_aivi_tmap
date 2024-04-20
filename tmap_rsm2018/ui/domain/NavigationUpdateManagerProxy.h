#ifndef NAVIGATIONUPDATEMANAGERPROXY_H
#define NAVIGATIONUPDATEMANAGERPROXY_H

#include "AbstractProxy.h"
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QRunnable>
#include "../common/updatemanagerinterface.h"
#include "../common/navigationproperty.h"
#include "../common/navigationcontroller.h"
#include "VSM_Navi.h"

namespace SKT {

T_SHAREABLE( NavigationUpdateManagerProxy );
T_SHAREABLE( RequestCommand );


class NavigationUpdateManagerProxy : public AbstractProxy
{
    Q_OBJECT
public:

    NavigationUpdateManagerProxy(QObject* controller, QObject* updateManager, QObject* property);

    void applyCommand(const RequestCommandPtr& req);
    void getCommand(const RequestCommandPtr& req);
    void requestCommand(const RequestCommandPtr& req);
    void requestToViewList(int viewId);
    void sendReplyToViewList(const QList<int> data, const int findId);
    //NavigationDataUpdateStatus와 맞춰야함
    enum State {
        StateIdle = 0,
        StateStart,
        StateCopy,
        StateDone,
        StateError,
    };

signals:
    void responseToViewList(QList<int> data);

private slots:
    void onUpdatePackageMounted(bool locked);
    void onUpdatePackageUnmounted();
    void onUpdatePackageProgress(quint64 storedSize, quint64 totalSize, quint32 count, quint32 totalCount);
    void onUpdatePackageStored();
    void onUpdatePackageError(SKT::UpdateManagerInterface::ErrorType errorType);
    void onPackageStatusChanged(SKT::UpdateManagerInterface::PackageStatus packageStatus);
    void onUpdateApplied();


private:
    NavigationController* mController;
    QObject* mUpdateManager;
    QObject* mProperty;
    bool mMounted;
    int mStatus;
    float mProgress;
    bool mIsRg;
};

}

#endif // NAVIGATIONUPDATEMANAGERPROXY_H

