/*
 * This file was generated by qdbusxml2cpp version 0.8
 * Command line was: qdbusxml2cpp NavigationExtAgent.xml -i extagenttype.h -p NavigationExtAgent
 *
 * qdbusxml2cpp is Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
 *
 * This is an auto-generated file.
 * Do not edit! All changes made to it will be lost.
 */

#ifndef NAVIGATIONEXTAGENT_H_1502436745
#define NAVIGATIONEXTAGENT_H_1502436745

#include <QObject>
#include "extagenttype.h"

/*
 * Proxy class for interface org.bosch.cm.navigation.NavigationExtAgent
 */
class NavigationExtAgentAdaptor;
class OrgBoschCmNavigationNavigationExtAgentInterface: public QObject
{
    Q_OBJECT
    Q_CLASSINFO("NavigationExtAgent", "org.bosch.cm.navigation.NavigationExtAgent")
public:
    static inline const char *staticInterfaceName()
    { return "org.bosch.cm.navigation.NavigationExtAgent"; }
    void registerService();
    void unregisterService();

public:
//    OrgBoschCmNavigationNavigationExtAgentInterface(const QString &service, const QString &path, const QDBusConnection &connection, QObject *parent = 0);
    OrgBoschCmNavigationNavigationExtAgentInterface(QObject *parent = 0);

    ~OrgBoschCmNavigationNavigationExtAgentInterface();

private:
    NavigationExtAgentAdaptor* m_adaptor;
    bool m_registered;

public Q_SLOTS: // METHODS
Q_SIGNALS: // SIGNALS
    void deadreckoning_info(qulonglong drTimestamp, uint drSystemTime, double drLatitude, double drLongitude, double drHeading, double drSpeed, double drAltitude, double drTurnrateX, double drTurnrateY, double drTurnrateZ, double drAccelerationX, double drAccelerationY, double drAccelerationZ, uchar drInfoSource, ushort drInfoValidity);
    void mounting(MountAngles mountAngles, uchar mountingSource);
    void position(double posLatitude, double posLongitude, uchar posSource, ushort countryCode, uchar countryCodeSource, TimeZoneData timeZone, uchar timeZoneSource);
    void velocity(ushort velocityValue, uchar velocitiySource);
};


namespace org {
  namespace bosch {
    namespace cm {
      namespace navigation {
        typedef ::OrgBoschCmNavigationNavigationExtAgentInterface NavigationExtAgent;
      }
    }
  }
}
#endif
