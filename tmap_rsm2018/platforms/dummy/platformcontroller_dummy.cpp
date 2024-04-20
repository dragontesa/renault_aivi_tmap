#include "platformcontroller_dummy.h"
#include "tmapcontroller.h"


namespace SKT {

PlatformController* TmapController::newPlatformController(TmapController *controller) {
    return new PlatformControllerDummy(controller);
}

PlatformControllerDummy::PlatformControllerDummy(TmapController* controller)
    : m_controller(controller)
{
    // monitor engine init
    connect(m_controller, &TmapController::engineInitialized,
            this, &PlatformControllerDummy::onEngineInitialized);
    connect(m_controller, &TmapController::applicationWindowCreated,
            this, &PlatformControllerDummy::onApplicationWindowCreated);
}

PlatformControllerDummy::~PlatformControllerDummy()
{

}

void PlatformControllerDummy::init()
{
    QMetaObject::invokeMethod(this, "platformInitialized",
                              Qt::QueuedConnection);
}

QString PlatformControllerDummy::deviceId() const
{
    return QString();
}

bool PlatformControllerDummy::remount(const QString &path, bool readOnly)
{
    QMetaObject::invokeMethod(this, "platformRemountStatus",
                              Qt::QueuedConnection,
                              Q_ARG(bool, true));
    return true;
}

bool PlatformControllerDummy::remountCommand(uint type, const QString &srcPath, const QString &destPath)
{
    QMetaObject::invokeMethod(this, "platformRemountStatus",
                              Qt::QueuedConnection,
                              Q_ARG(bool, true));
    return true;
}
bool PlatformControllerDummy::restart()
{
    return false;
}

bool PlatformControllerDummy::callNumber(const QString &number)
{
    return false;
}

bool PlatformControllerDummy::dimmingStatus() const
{
    return false;
}

bool PlatformControllerDummy::networkOnline() const
{
    return false;
}

bool PlatformControllerDummy::appForeground() const
{
    return true;
}

void PlatformControllerDummy::onEngineInitialized(bool success)
{
    emit platformUIRequested();
    emit platformAppForegroundChanged(true);
}

void PlatformControllerDummy::onApplicationWindowCreated()
{

}

void PlatformControllerDummy::setNavigationDataUpdateStatusInfo(int status, float progress) const
{
}
