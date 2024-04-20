#ifndef FILEGPSPROVIDER_H
#define FILEGPSPROVIDER_H

#include <QString>
#include "positionprovider.h"
#include "rgdatapool.h"
#include "logfile.h"

namespace SKT {
/**
 * @brief The ClientStatus enum
 */
enum FileType {
    GNSS = 0,
    DR
};


class FileGpsReader;
class FileGpsProvider : public PositionProvider
{
public:
    FileGpsProvider(const QString &logName);
    virtual ~FileGpsProvider();

public slots:
    void startUpdates() Q_DECL_OVERRIDE;
    void stopUpdates() Q_DECL_OVERRIDE;
    void sendExtraCommand(const QString &cmd) Q_DECL_OVERRIDE;
#ifdef PLATFORM_QNX
    void UpdateLocationData(int nMsg, void* lpParam) override {

    }
#endif
    void changeGpsLogUsage(bool enabled) Q_DECL_OVERRIDE;
    void applyGpsTimeBias(int bias) Q_DECL_OVERRIDE;
    void onRouteGuidanceUpdated(SKT::RGDataPtr rgdata) Q_DECL_OVERRIDE;

    TMAP_TMC_QC__MMFB_OVERRIDE

private:
    TMAP_TMC_QC_MMFB_DECLARE  // mmfb usage, by lckxx 2018.4.18
    QString m_logName;
    FileGpsReader* m_reader;
    LogFile *m_pLogFileMMFB;
    double m_mmfbTime;
    int m_mmfbX, m_mmfbY;
};

}

#endif // FILEGPSPROVIDER_H
