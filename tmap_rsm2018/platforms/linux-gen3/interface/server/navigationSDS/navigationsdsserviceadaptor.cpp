/*
 * This file was generated by qdbusxml2cpp version 0.8
 * Command line was: qdbusxml2cpp -a navigationsdsserviceadaptor -i navigationsdsservicetype.h -i ../server/navigation/navigationservicetype.h NavigationSDSService.xml
 *
 * qdbusxml2cpp is Copyright (C) 2016 The Qt Company Ltd.
 *
 * This is an auto-generated file.
 * Do not edit! All changes made to it will be lost.
 */

#include "navigationsdsserviceadaptor.h"
#include <QtCore/QMetaObject>
#include <QtCore/QByteArray>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariant>
#include "navigationsdsservice.h"
/*
 * Implementation of adaptor class NavigationSDSServiceAdaptor
 */

NavigationSDSServiceAdaptor::NavigationSDSServiceAdaptor(NavigationSDSService *parent)
    : QDBusAbstractAdaptor(parent),
    m_sdsService(parent)
{
    // adaptor must have the service for itself.
    if (parent == nullptr) {
        m_sdsService = new NavigationSDSService();
    }

    // constructor
    setAutoRelaySignals(true);
}

NavigationSDSServiceAdaptor::~NavigationSDSServiceAdaptor()
{
    // destructor
}

NSPositionInfo NavigationSDSServiceAdaptor::positionInfo() const
{
    // get the value of property positionInfo
    return m_sdsService->positionInfo();
}

NSPositionStatusInfo NavigationSDSServiceAdaptor::positionStatusInfo() const
{
    // get the value of property positionStatusInfo
    return m_sdsService->positionStatusInfo();
}

NSSDSAddressWithOptions NavigationSDSServiceAdaptor::sdsAddressWithOptions() const
{
    // get the value of property sdsAddressWithOptions
    return m_sdsService->sdsAddressWithOptions();
}

uint NavigationSDSServiceAdaptor::getMaxWaypointCount()
{
    // handle method call org.bosch.cm.navigation.NavigationSDSService.getMaxWaypointCount
    uint maxWayPointCount;
    QMetaObject::invokeMethod(parent(), "getMaxWaypointCount", Q_RETURN_ARG(uint, maxWayPointCount));
    return maxWayPointCount;
}

void NavigationSDSServiceAdaptor::sdsCheckAddress(NSSDSAddress requestedAddress)
{
//    qDebug() << "NavigationSDSServiceAdaptor:::sdsCheckAddress. count:" << requestedAddress.size();
    // handle method call org.bosch.cm.navigation.NavigationSDSService.sdsCheckAddress
    m_sdsService->sdsCheckAddress(requestedAddress);
}

QString NavigationSDSServiceAdaptor::sdsGetHouseNumberPattern(QString &maxHouseNumber, QStringList &patterns)
{
    // handle method call org.bosch.cm.navigation.NavigationSDSService.sdsGetHouseNumberPattern
    return m_sdsService->sdsGetHouseNumberPattern(maxHouseNumber, patterns);
}

NSRefinementWithIdInfos NavigationSDSServiceAdaptor::sdsGetRefinementList(uint sdsAddressElementType)
{
    // handle method call org.bosch.cm.navigation.NavigationSDSService.sdsGetRefinementList
    return m_sdsService->sdsGetRefinementList(sdsAddressElementType);
}

void NavigationSDSServiceAdaptor::selectSdsRefinement(uint id)
{
//    qDebug() << "NavigationSDSServiceAdaptor:::selectSdsRefinement. id:" << id;
    // handle method call org.bosch.cm.navigation.NavigationSDSService.selectSdsRefinement
    m_sdsService->selectSdsRefinement(id);
}

void NavigationSDSServiceAdaptor::setLocationWithSdsInput()
{
//    qDebug() << "NavigationSDSServiceAdaptor:::setLocationWithSdsInput.";
    // handle method call org.bosch.cm.navigation.NavigationSDSService.setLocationWithSdsInput
    m_sdsService->setLocationWithSdsInput();
}

