#ifndef PCH_H
#define PCH_H

#include "loggingcategories.h"


#ifndef QT_NO_DEBUG_OUTPUT

// vsmmap logging with category
#undef qDebug
#define qDebug QMessageLogger(QT_MESSAGELOG_FILE, QT_MESSAGELOG_LINE, QT_MESSAGELOG_FUNC, SKT::vsmmap().categoryName()).debug
#undef qWarning
#define qWarning QMessageLogger(QT_MESSAGELOG_FILE, QT_MESSAGELOG_LINE, QT_MESSAGELOG_FUNC, SKT::vsmmap().categoryName()).warning
#undef qCritical
#define qCritical QMessageLogger(QT_MESSAGELOG_FILE, QT_MESSAGELOG_LINE, QT_MESSAGELOG_FUNC, SKT::vsmmap().categoryName()).critical
#undef qFatal
#define qFatal QMessageLogger(QT_MESSAGELOG_FILE, QT_MESSAGELOG_LINE, QT_MESSAGELOG_FUNC, SKT::vsmmap().categoryName()).fatal
#endif

#else

#endif
