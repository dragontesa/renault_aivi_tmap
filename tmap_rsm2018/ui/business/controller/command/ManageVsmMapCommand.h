#ifndef MANAGEVSMMAPCOMMAND_H
#define MANAGEVSMMAPCOMMAND_H

#include "SimpleCommand.h"

namespace SKT {

class ManageVsmMapCommand : public SimpleCommand
{
    Q_OBJECT
public:
    Q_INVOKABLE ManageVsmMapCommand(QObject *parent = 0);

    virtual bool execute(const mvc::INotificationPtr &noti);

private:
    void addVia(const QVariant &v);
    void insertVia(const QVariant &v);
    void deleteVia(const QVariant &v);
    void selectViaSlot(const QVariant &);
    void setVia(const QVariant &);
    void setStartSearchCode(const QVariant &);

    void setArrival(const QVariant &v);
    void setDeparture(const QVariant &v);
    void update(QObject* mapView);
    void showCarvata(const QVariant &v);
    void addSearchMarker(const QVariant &v);
    void removeSearchMarker(const QVariant &v);
    void changeViewLevel(const QVariant &v);
    void changeAutoZoom(const QVariant &v);
    void showGasStationInfo(const QVariant &v);
    void hideGasStationInfo(const QVariant &v);
    void removeCalloutPopup(const QVariant &v);
};

}//SKT

#endif // MANAGEVSMMAPCOMMAND_H
