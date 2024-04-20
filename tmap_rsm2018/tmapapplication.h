#ifndef TMAPAPPLICATION_H
#define TMAPAPPLICATION_H

#include <QGuiApplication>

namespace SKT {

class Logger;
class TmapController;
class TmapApplication : public QGuiApplication
{
    Q_OBJECT
public:
    explicit TmapApplication(int &argc, char **argv);
    virtual ~TmapApplication();

    TmapController* controller() { return m_controller; }

private:
    Q_DISABLE_COPY(TmapApplication)

    TmapController* m_controller;
    Logger* m_logger;
};

}

#endif // TMAPAPPLICATION_H
