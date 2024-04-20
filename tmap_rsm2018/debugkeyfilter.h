#ifndef DEBUGKEYFILTER_H
#define DEBUGKEYFILTER_H

#include <QObject>

namespace SKT {

class TmapController;
class DebugKeyFilter : public QObject
{
    Q_OBJECT
public:
    DebugKeyFilter(TmapController* controller, QObject* parent = nullptr);
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    TmapController* m_controller;
    bool m_enableShift;
    bool m_enableControl;
    bool m_usbMount;
    bool m_isKorean = true;
};

}

#endif // DEBUGKEYFILTER_H
