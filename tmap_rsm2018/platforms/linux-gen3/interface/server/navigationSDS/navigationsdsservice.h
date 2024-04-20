#ifndef NAVIGATIONSDSSERVICE_H
#define NAVIGATIONSDSSERVICE_H

#include <QObject>
#include "navigationsdsservicetype.h"

class NavigationSDSServiceAdaptor;
class AddressSearchResultCache;
class NavigationSDSService : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.bosch.cm.navigation.NavigationSDSService")
public:
    explicit NavigationSDSService(QObject* parent = 0);
    ~NavigationSDSService();

    void registerService();
    void unregisterService();
    void notifyPropertiesChanged(const QString& interfaceName, const QVariantMap &changedProperties, const QStringList &invalidatedProperties) const;

public: //property
    NSSDSAddressWithOptions sdsAddressWithOptions() const;
    void setSdsAddressWithOptions(const NSSDSAddressWithOptions &value);

    NSPositionStatusInfo positionStatusInfo() const;
    void setPositionStatusInfo(const NSPositionStatusInfo &info);

    NSPositionInfo positionInfo() const;
    void setPositionInfo(const NSPositionInfo &info);

public slots: // DBUS Methods
    uint getMaxWaypointCount() const;
    QString sdsGetHouseNumberPattern(QString &maxHouseNumber, QStringList &patterns) const;
    NSRefinementWithIdInfos sdsGetRefinementList(uint sdsAddressElementType) const;
    void sdsCheckAddress(const NSSDSAddress& requestedAddress);
    void handleAddressSearchResult(const QVariant& results);
    void selectSdsRefinement(uint id);
    void setLocationWithSdsInput();

signals: // DBUS Property Changed Signals
    void doneSdsCheckAddressResults();

    void notifySdsSearchAddress(const NSSDSAddress& requestedAddress);
    void doneSdsSearchAddress();
    void notifySetLocationWithDetails(const NSPOILocation& location);

private:
    void setLocation(const NSPOILocation& location);

    NavigationSDSServiceAdaptor* m_adaptor;
    bool m_registered;

    NSPositionStatusInfo m_positionStatusInfo;
    NSPositionInfo m_positionInfo;
    NSSDSAddressWithOptions m_sdsAddressWithOptions;
    uint m_maxWaypointCount;
    QStringList m_patterns;

    AddressSearchResultCache* m_addressSearchResultCache;
    NSPOILocation m_currentSetLocation;
};

#endif // NAVIGATIONSDSSERVICE_H
