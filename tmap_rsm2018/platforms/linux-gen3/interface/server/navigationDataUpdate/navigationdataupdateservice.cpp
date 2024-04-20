#include "navigationdataupdateservice.h"
#include "navigationdataupdateserviceadaptor.h"
#include "build.h"

static const QString serviceName = QStringLiteral("App.Core.AppHmi_Navigation");
static const QString navigationDataUpdateObjectPath = QStringLiteral("/com/bosch/cm/ai/navigationDataUpdate");
static const QString dBusPropertiesInterface = QStringLiteral("org.freedesktop.DBus.Properties");
static const QString dBusPropertiesChangedSignal = QStringLiteral("PropertiesChanged");
static const QString NavigationDataUpdateServiceInterface = QStringLiteral("org.bosch.cm.navigation.NavigationDataUpdateService");

NavigationDataUpdateService::NavigationDataUpdateService(QObject* parent)
    : QObject(parent)
    , m_adaptor(new NavigationDataUpdateServiceAdaptor(this))
    , m_registered(false)
    , m_navigationDataUpdateServiceAck(NavDataUpdateServiceAck_ACCEPTED)
    , m_navigationDataDeleteServiceAck(NavDataDeleteServiceAck_ACCEPTED)
{
    QDBusConnection connection = QDBusConnection::sessionBus();

    if (!connection.isConnected()) {
        qDebug() << "Failed to connect to DBus";
    } else if (!connection.registerObject(navigationDataUpdateObjectPath, this)) {
        qDebug() << "Failed to register object path";
    }
}

NavigationDataUpdateService::~NavigationDataUpdateService()
{
    unregisterService();
}

void NavigationDataUpdateService::registerService()
{
    if (!m_registered) {
        QDBusConnection connection = QDBusConnection::sessionBus();

        if (!connection.isConnected()) {
            qWarning() << "Failed to connect to DBus";
            return;
        }

        if (connection.interface()->isServiceRegistered(serviceName)) {
            qWarning() << serviceName << " okay there get registered !";
        }
        else {
            if (!connection.registerService(serviceName)) {
                qWarning() << "Failed to register service: " << serviceName << " Already taken?";
                return;
            }
        }
        m_registered = true;
    }
}

void NavigationDataUpdateService::unregisterService()
{
    if (m_registered) {
        QDBusConnection connection = QDBusConnection::sessionBus();
        connection.unregisterService(serviceName);
        m_registered = false;
    }
}

// TODO: sending too frequently?
void NavigationDataUpdateService::notifyPropertiesChanged(const QString& interfaceName, const QVariantMap &changedProperties, const QStringList &invalidatedProperties) const
{
    QDBusConnection connection = QDBusConnection::sessionBus();

    if (!connection.isConnected()) {
        qDebug() << "Failed to connect to DBus";
        return;
    }

    QDBusMessage message = QDBusMessage::createSignal(navigationDataUpdateObjectPath,
                                                      dBusPropertiesInterface,
                                                      dBusPropertiesChangedSignal);

    QList<QVariant> arguments;
    arguments << QVariant(interfaceName) << QVariant(changedProperties) << QVariant(invalidatedProperties);
    message.setArguments(arguments);

    if (!connection.send(message)) {
        qDebug() << "Failed to send DBus property notification signal";
    }
}

/**
 * @brief navigationDataUpdateStatusInfo
 * @return NavigationDataUpdateStatusInfo
 *
 * Attribute to get current NavDataUpdateStatusInfo (status, progressValue, remainingTime).
 *
 */
NavigationDataUpdateStatusInfo NavigationDataUpdateService::navigationDataUpdateStatusInfo() const {
    qDebug() << "called NavigationDataUpdateService::navigationDataUpdateStatusInfo()";
    return m_navigationDataUpdateStatusInfo;
}

/**
 * @brief navigationDataDeleteStatusInfo
 * @return NavigationDataDeleteStatusInfo
 *
 * Attribute to get current NavDataDeleteStatusInfo (status, progressValue, remainingTime).
 *
 */
NavigationDataDeleteStatusInfo NavigationDataUpdateService::navigationDataDeleteStatusInfo() const {
    qDebug() << "called NavigationDataUpdateService::navigationDataDeleteStatusInfo()";
    return m_navigationDataDeleteStatusInfo;
}

void NavigationDataUpdateService::setNavigationDataUpdateStatusInfo(const NavigationDataUpdateStatusInfo &info) {
    qDebug() << "called NavigationDataUpdateService::setNavigationDataUpdateStatusInfo()";
    if (info != m_navigationDataUpdateStatusInfo) {
        m_navigationDataUpdateStatusInfo = info;
        if (info.m_status == NavDataUpdateState_USB_REMOVED                        ||
            info.m_status == NavDataUpdateState_FAILED_NO_UPDATES_FOUND            ||
            info.m_status == NavDataUpdateState_AVAILABLE_UPDATES_ABORTED_BY_ERROR ||
            info.m_status == NavDataUpdateState_FAILED_INVALID_USB_DATA            ||
            info.m_status == NavDataUpdateState_FAILED_OTHERS                      ||
            info.m_status == NavDataUpdateState_UPDATE_SOURCE_ERROR)
            m_navigationDataUpdateServiceAck = NavDataUpdateServiceAck_DECLINED_CANNOT_PROCESS;
        else if (info.m_status == NavDataUpdateState_AVAILABLE_UPDATES_IN_PROGRESS ||
                 info.m_status == NavDataUpdateState_INSTALLATION_IN_PROGRESS      ||
                 info.m_status == NavDataUpdateState_INSTALLATION_STARTED
        ) 
            m_navigationDataUpdateServiceAck = NavDataUpdateServiceAck_DECLINED_REQUEST_ACTIVE;
        else
            m_navigationDataUpdateServiceAck = NavDataUpdateServiceAck_ACCEPTED;

        QVariantMap changedProperties;
        changedProperties[QStringLiteral("navigationDataUpdateStatusInfo")] = QVariant::fromValue(m_navigationDataUpdateStatusInfo);
        notifyPropertiesChanged(NavigationDataUpdateServiceInterface, changedProperties, QStringList());
    }
}

void NavigationDataUpdateService::setNavigationDataDeleteStatusInfo(const NavigationDataDeleteStatusInfo &info) {
    qDebug() << "called NavigationDataDeleteService::setNavigationDataDeleteStatusInfo()";
    if (info != m_navigationDataDeleteStatusInfo) {
        qDebug() << "diagdata, update status: " << info.m_status;
        m_navigationDataDeleteStatusInfo = info;
        if (info.m_status == NavDataDeleteStatus_MAPDATA_DELETE_IN_PROGRESS    ||
            info.m_status == NavDataDeleteStatus_MAPDATA_DELETE_STARTED
                )
            m_navigationDataDeleteServiceAck = NavDataDeleteServiceAck_DECLINED_REQUEST_ACTIVE;
        else if (info.m_status  == NavDataDeleteCannotStatus_NoMapdata) {
             m_navigationDataUpdateServiceAck = NavDataDeleteServiceAck_DECLINED_NO_MAP_DATA;
             return; // no update info due to this status is internal
        }
        else if (info.m_status  == NavDataDeleteCannotStatus_FilePermissionDenied   ||
                 info.m_status  == NavDataDeleteCannotStatus_BusyWorking            ||
                 info.m_status  == NavDataDeleteCannotStatus_Others                 ) {
            m_navigationDataDeleteServiceAck  = NavDataDeleteServiceAck_DECLINED_CANNOT_PROCESS;
            return;
        }
        else {
            m_navigationDataDeleteServiceAck  = NavDataDeleteServiceAck_ACCEPTED;
        }

        QVariantMap changedProperties;
        changedProperties[QStringLiteral("mapDataDeleteStatusInfo")] = QVariant::fromValue(m_navigationDataDeleteStatusInfo);
        notifyPropertiesChanged(NavigationDataUpdateServiceInterface, changedProperties, QStringList());
    }
}

/**
 * @brief NavigationDataUpdateService::DiagPerformFullUpdateViaUSB
 * @return uint
 *
 * Method to request full update via USB in Diagnostic mode.
 *
 */
uint NavigationDataUpdateService::DiagPerformFullUpdateViaUSB() {

    qDebug() << "DiagPerformFullUpdateViaUSB:" << m_navigationDataUpdateServiceAck;
    // if (m_navigationDataUpdateServiceAck == NavDataUpdateServiceAck_ACCEPTED )  // DONOT, update must be invoked no matter what
        emit requestDiagDataUpdateviaUSB(); // start map update via usb on diagnosis mode
    return m_navigationDataUpdateServiceAck;
}

/**
 * @brief NavigationDataUpdateService::DiagPerformFullUpdateViaUSB
 * @return uint
 *
 * Method to request full update via USB in Diagnostic mode.
 *
 */
uint NavigationDataUpdateService::DiagPerformFullDeleteMapData() {

    qDebug() << "DiagPerformFullDeleteMapData:" << m_navigationDataDeleteServiceAck;
    if (m_navigationDataDeleteServiceAck == NavDataDeleteServiceAck_ACCEPTED )
        emit requestDiagDataDeleteMapData(); // start map update via usb on diagnosis mode
    return m_navigationDataDeleteServiceAck;
}
