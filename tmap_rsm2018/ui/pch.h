#ifndef PCH_H
#define PCH_H

#include <QtCore>
#include <QtCore/qglobal.h>
#include <QObject>
#include <QWindow>
#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQmlProperty>
#include <QAbstractListModel>
#include <QSettings>
#include <QHash>
#include <QByteArray>
#include <QtSql>
#include <QJsonDocument>
#include <QJsonObject>
#include <QtSql/QtSql>
#include <QQmlEngine>
#include <QQmlComponent>
#include <QQuickItem>
#include <QQuickWindow>
#include <QCoreApplication>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QGuiApplication>
#include <QLoggingCategory>
#include <QMutex>

#include "tmap_rsm_ui_export.h"

#include "TCore.h"
#include "TMvc.h"
#include "TOrm.h"
#include "Facade.h"
#include "DebugLogger.h"
#include "LoggingUtils.h"

#ifndef QT_NO_DEBUG_OUTPUT

#undef qDebug
#define qDebug QMessageLogger(QT_MESSAGELOG_FILE, QT_MESSAGELOG_LINE, QT_MESSAGELOG_FUNC, ui().categoryName()).debug
#undef qInfo
#define qInfo QMessageLogger(QT_MESSAGELOG_FILE, QT_MESSAGELOG_LINE, QT_MESSAGELOG_FUNC, ui().categoryName()).info
#undef qWarning
#define qWarning QMessageLogger(QT_MESSAGELOG_FILE, QT_MESSAGELOG_LINE, QT_MESSAGELOG_FUNC, ui().categoryName()).warning
#undef qCritical
#define qCritical QMessageLogger(QT_MESSAGELOG_FILE, QT_MESSAGELOG_LINE, QT_MESSAGELOG_FUNC, ui().categoryName()).critical
#undef qFatal
#define qFatal QMessageLogger(QT_MESSAGELOG_FILE, QT_MESSAGELOG_LINE, QT_MESSAGELOG_FUNC, ui().categoryName()).fatal

#else

#endif

#define T_PERF_LOG QT_NO_QDEBUG_MACRO
//#define T_PERF_LOG QMessageLogger(QT_MESSAGELOG_FILE, QT_MESSAGELOG_LINE, QT_MESSAGELOG_FUNC).debug

#endif // PCH_H
