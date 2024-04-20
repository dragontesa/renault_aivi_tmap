#ifndef MAPSEARCHVIEWMEDIATOR_H
#define MAPSEARCHVIEWMEDIATOR_H

#include "mediator/AbstractViewMediator.h"

namespace SKT {

class MapSearchViewMediator : public AbstractViewMediator
{
    Q_OBJECT
    Q_ENUMS( EventK )
    Q_ENUMS( MenuK )

public:
    enum EventK {
        Mediated = 0,
        Activated,
        Deactivated,
        MapScaleChanged,
        MapScaleReturnButtonClicked,
        RequestMapScale,
        RequestSearch,
        RegistCsw,
        SelectCsw,
        MoveMap,
        MoveMapByPos,
        RetranslateUi,
        SelectSoftkeyIndexMenu,
    };
    enum MenuK {
        SetAsDeparture = 0,
        SetAsWaypoint,
        SetAsArrival,
        AddToBookmark,
        RequestRoute
    };

    Q_INVOKABLE MapSearchViewMediator(QObject *viewComponent = nullptr);

    QList<int> listNotificationInterests();

protected:
    bool onEvent(int event, const QVariant& data) Q_DECL_OVERRIDE;
    bool handleNotification(const mvc::INotificationPtr& note) Q_DECL_OVERRIDE;

private:
    void closeView();
    void procSoftkey(const QVariant &data);
    void requestRoute(const QPoint &pos, const QString &address);
    void requestAddress();
    void requestMapPosition();
    void showChangeDestinationDialog(const QPoint &pos, const QString &address);

    void addVia(const QString& address);
    void addToBookmark(const QString& address);
    void enableMapMove(bool enable);
    QPoint mapCenterPos() const;

private:
    QObject* mRenderer = nullptr;
    QString mMapAreaName;
    bool mIsRg = false;
    QPoint mDestPos;
    int mSelectedMenu = 0;
};

} //SKT

#endif // MAPSEARCHVIEWMEDIATOR_H
