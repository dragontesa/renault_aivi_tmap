#ifndef LOGGINGUTILS_H
#define LOGGINGUTILS_H

namespace SKT {

#ifndef QT_NO_DEBUG_OUTPUT

extern QMessageLogger::CategoryFunction sys;
extern QMessageLogger::CategoryFunction ui;

#else

#define ui
#define sys

#endif

}

#endif // LOGGINGUTILS_H
