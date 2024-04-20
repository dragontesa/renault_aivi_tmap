#include "tmap_rsm_ui.h"

#include <QtCore/qglobal.h>

#include "tmap_rsm_ui_export.h"

#include <QtCore>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQmlProperty>
#include <QQmlComponent>

#include "TCore.h"
#include "TMvc.h"
#include "Facade.h"
#include "ResourceResolver.h"
#include "DatabaseHelper.h"
#include "QmlAppHelper.h"
#include "RpnSearchHelper.h"
#include "RouteManager.h"
#include "SearchHelper.h"
#include "CrossImageProvider.h"
#include "ViewRegistry.h"
#include "PlatformHelper.h"

namespace SKT {

void TMAP_RSM_UI_DLLSHARED_EXPORT RsmUi_Initialize( QQmlApplicationEngine* qmlAppEngine,
    QMap<QString,QMessageLogger::CategoryFunction> logCats,
    void (*logHandler)(int,const char*),
    const QVariantMap& context
)
{    
#ifndef QT_NO_DEBUG_OUTPUT
    ui = logCats["ui"];
    sys = logCats["sys"];
#endif

//    qInfo(ui) << "test qInfo";
//    qDebug(ui) << "test qDebug";
//    qWarning(ui) << "test qWarning";
//    qCritical(ui) << "test qCritical";
    //qFatal( "qFatal" );

    enum LOG_LEVEL {
        LOG_INFO = 0,
        LOG_DEBUG,
        LOG_WARNING,
        LOG_CRITICAL,
        LOG_FATAL
    };

    logHandler( LOG_INFO, "test LOG_INFO" );
    logHandler( LOG_DEBUG, "test LOG_DEBUG" );
    logHandler( LOG_WARNING, "test LOG_WARNING" );
    logHandler( LOG_CRITICAL, "test LOG_CRITICAL" );
    //logHandler( LOG_FATAL, "test LOG_FATAL" );

    T_DEBUG( "Initialize DebugLogger" );

    auto isEngineInit = context["isEngineInit"].toBool();
    auto dbDirPath = context["dbDirPath"].toString();
    auto crossImagePath = context["crossImagePath"].toString();
    // rpn should not be ran on engine init failure by lckxx 2017.10.18, artf261697
    auto controller = context["controller"].value<QObject*>();

    PlatformHelper::instance()->initialize(controller, qmlAppEngine);

    if (isEngineInit)
    {
    	auto rpnSearch = context["rpnSearch"].value<QObject*>();
    	auto addressManager = context["addressManager"].value<QObject*>();
    	auto searchManager = context["searchManager"].value<QObject*>();

        RpnSearchHelper::instance()->setRpnSearch( qobject_cast<RPManagerInterface*>(rpnSearch) );
    	DatabaseHelper::instance()->initialize( dbDirPath );
    	CrossImageProvider::instance()->initialize( crossImagePath );
    	QmlAppHelper::instance()->initialize( qmlAppEngine, context );
        SearchHelper::instance()->setSearch( qobject_cast<SearchManagerInterface*>(searchManager) );

    	SKT::RouteManager *routeManager = RouteManager::sharedInstance();
        routeManager->initialize(addressManager);
    }
    else
    {
    	DatabaseHelper::instance()->initialize( dbDirPath );
    	CrossImageProvider::instance()->initialize( crossImagePath );
    	QmlAppHelper::instance()->initialize( qmlAppEngine, context );
    }
}

void TMAP_RSM_UI_DLLSHARED_EXPORT RsmUi_Terminate()
{
    DatabaseHelper::instance()->terminate();
    Facade::instance()->terminate();
}

void TMAP_RSM_UI_DLLSHARED_EXPORT RsmUi_OpenGuideWindow(QQmlApplicationEngine* qmlAppEngine,
		const QVariantMap& context)
{
//	GuideWindow::instance()->initialize(qmlAppEngine, context);
}

}
