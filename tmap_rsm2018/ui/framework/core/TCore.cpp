#include "TCore.h"

using namespace SKT;

bool isValidObject( QObject* obj )
{
    bool ret = true;

    if ( obj == 0 ) {
        ret = false;
    }
    else if ( dynamic_cast<QObject*>( obj ) == 0 ) {
        ret = false;
    }

    return ret;
}

QTextStream& qStdOut()
{
    static QTextStream ts( stdout );
    return ts;
}
