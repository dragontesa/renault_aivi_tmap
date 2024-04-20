#ifndef PLATFORMCONTROLLER_DUMMY_H
#define PLATFORMCONTROLLER_DUMMY_H

#include "platformcontroller.h"

namespace SKT {

class TmapController;
class PlatformControllerDummy : public PlatformController
{
    Q_OBJECT
public:
    explicit PlatformControllerDummy(TmapController* controller);
    virtual ~PlatformControllerDummy();

    void init() override;
    QString deviceId() const override;
    bool remount(const QString &path, bool readOnly) override;
    bool remountCommand(uint type, const QString &srcPath, const QString &destPath) override;
    bool restart() override;
    bool callNumber(const QString &number) override;
    bool dimmingStatus() const override;
    bool networkOnline() const override;
    bool appForeground() const override;
    void setNavigationDataUpdateStatusInfo(int status, float progress) const override;

private slots:
    void onEngineInitialized(bool success);
    void onApplicationWindowCreated();

private:
    TmapController* m_controller;
};

}

#endif // PLATFORMCONTROLLER_DUMMY_H
