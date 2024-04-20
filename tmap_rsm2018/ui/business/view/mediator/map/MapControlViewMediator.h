#ifndef MAPCONTROLVIEWMEDIATOR_H
#define MAPCONTROLVIEWMEDIATOR_H

#include "mediator/AbstractViewMediator.h"
#include <QList>
#include <QTimer>

class QObject;
class QVariant;

namespace SKT {

class MapControlViewMediator : public AbstractViewMediator {
    Q_OBJECT

    Q_ENUMS( EventK )
public:
    enum EventK {
        Mediated,
        Activated,
        Deactivated,
        NewDestinationAccepted,
        NewDestinationCanceled,
        ModalChanged,
        MapScaleChanged,
        MapScaleReturnButtonClicked,
        RestoreSimulationViewButtonClicked,
        ResetTimer,
        RegistCSW,
        SelectCSW,
        MoveMap,
        SaveMapViewLevel,
        
        //[#온라인 주소 표출] 2018.09.13 cjlee
        RequestSearch,
    };
    enum MenuK {
        SetAsDestination = 0,
        SetAsWaypoint,
        AddToBookmark
    };
    Q_INVOKABLE MapControlViewMediator(QObject *viewComponent = 0);
    virtual ~MapControlViewMediator();

    QList<int> listNotificationInterests();

protected:
    virtual bool onEvent(int event, const QVariant &data);
    virtual bool handleNotification(const mvc::INotificationPtr &noti);

private:
    void applyRouteSetting(const QVariant &data);
    void close();
    void resetTimer();
    void restoreMapMode();
    void requestRouteTypes();
    void procSoftkey(const QVariant &data);
    void sendByDestinationOld(const QVariant &data);
    void sendByDestination();
    void showConfirmDialog(const QPoint &pt, const QString &address);

    void setupSoftkeyMenu(int lastDriveMode, bool registOwner = true);
    void requestAddress();

private:
    QTimer timer;
    int mLastDriveMode;
    QObject* mRenderer = nullptr;
    QString mMapAreaName;
};

}//SKT

#endif // MAPCONTROLVIEWMEDIATOR_H
