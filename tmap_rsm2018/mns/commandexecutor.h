#ifndef COMMANDEXECUTOR_H
#define COMMANDEXECUTOR_H
#include "rpnsearch.h"

#include <QThread>
#include <QSemaphore>
#include <QMutex>
#include <QQueue>
#include <QMap>

#include <map>
#include <string>

namespace SKT {

struct temppp{
    QString command;
    int commandIndex[10];
};

class CommandExecutor : public QThread
{
public:
    CommandExecutor(RPnSearch * client);
    ~CommandExecutor();

    void exeuteCommand(RPRequest *req);
    void insertCommand(RPRequest *req);

protected:
    void run() Q_DECL_OVERRIDE;

private:
    static QString done;
    static QString notSupported;
    static QString notImplemneted;

    typedef QMap<QString,void*> voidMap;
    voidMap *exeMap;

    RPnSearch *client;
    QQueue<RPRequest*> commandQueue;
    QSemaphore waitForCommand;
    QMutex lockQueue;
    QMutex lockMethod;

    voidMap* findSubMap(voidMap *parentMap, QStringList &list,int pos);
    void registerCommand();

    void exeMethod(RPRequest *req);
    void execute(RPRequest* req);
    bool callCommandMethod(int index, RPRequest* req);

    void deleteMap(voidMap* map);
};

}

#endif // COMMANDEXECUTOR_H
