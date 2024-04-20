#ifndef TMAP_RSM_UI_H
#define TMAP_RSM_UI_H

#include <QQmlApplicationEngine>
#include "tmap_rsm_ui_export.h"

namespace SKT {

void TMAP_RSM_UI_DLLSHARED_EXPORT RsmUi_Initialize(
        QQmlApplicationEngine* qmlAppEngine,
        QMap<QString,QMessageLogger::CategoryFunction> logCats,
        //QMessageLogger::CategoryFunction loggingCategory,
        void (*logHandler)(int,const char*),
        // context
        //      resDirPath tmap_rsm_ui.rcc directory path
        //      dbDirPath database directory path
        //      gpsService
        //      simulator
        const QVariantMap& context
        );

void TMAP_RSM_UI_DLLSHARED_EXPORT RsmUi_Terminate();

void TMAP_RSM_UI_DLLSHARED_EXPORT RsmUi_OpenGuideWindow(QQmlApplicationEngine* qmlAppEngine, const QVariantMap& context);

}

#endif // TMAP_RSM_UI_H

