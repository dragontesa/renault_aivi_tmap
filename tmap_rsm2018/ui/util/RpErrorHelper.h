#ifndef RPERRORHELPER_H
#define RPERRORHELPER_H

#include <QVariant>

namespace SKT
{

class SimpleCommand;
class AbstractMediator;

class RpErrorHelper
{
public:
    static void exec(SimpleCommand* command, int errorCode, bool cancelRoute=false);
    static QVariantMap exec(int errorCode);
};

}

#endif // RPERRORHELPER_H
