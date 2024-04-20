#ifndef NAVIGATIONDATAUPDATESERVICE_H
#define NAVIGATIONDATAUPDATESERVICE_H

#include <QObject>
#include "navigationdataupdateservicetype.h"

class NavigationDataUpdateServiceAdaptor;
class NavigationDataUpdateService : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.bosch.cm.navigation.NavigationDataUpdateService")
public:
    explicit NavigationDataUpdateService(QObject* parent = 0);
    ~NavigationDataUpdateService();

    void registerService();
    void unregisterService();
    void notifyPropertiesChanged(const QString& interfaceName, const QVariantMap &changedProperties, const QStringList &invalidatedProperties) const;
    void setDataUpdateServiceAck(uint ack) { m_navigationDataUpdateServiceAck = ack;}
    void setDataDeleteServiceAck(uint ack) { m_navigationDataDeleteServiceAck = ack;}

public: //property
    NavigationDataUpdateStatusInfo navigationDataUpdateStatusInfo() const;
    NavigationDataDeleteStatusInfo navigationDataDeleteStatusInfo() const;
    void setNavigationDataUpdateStatusInfo(const NavigationDataUpdateStatusInfo &info);
    void setNavigationDataDeleteStatusInfo(const NavigationDataDeleteStatusInfo &info);

public slots: // DBUS Methods
    uint DiagPerformFullUpdateViaUSB();
    uint DiagPerformFullDeleteMapData();

signals: // DBUS Property Changed Signals
     void requestDiagDataUpdateviaUSB();
     void requestDiagDataDeleteMapData();

private:
    NavigationDataUpdateServiceAdaptor* m_adaptor;
    bool m_registered;

    NavigationDataUpdateStatusInfo m_navigationDataUpdateStatusInfo;
    NavigationDataDeleteStatusInfo m_navigationDataDeleteStatusInfo;
    uint m_navigationDataUpdateServiceAck;
    uint m_navigationDataDeleteServiceAck;
};

#endif // NAVIGATIONDATAUPDATESERVICE_H
