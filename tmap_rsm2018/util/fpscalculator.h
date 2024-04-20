#ifndef FPSCALCULATOR_H
#define FPSCALCULATOR_H

#include <QObject>
#include <QDateTime>
#include <QDebug>
#include <QVector>

namespace SKT {

class FpsCalculator : public QObject
{
    Q_OBJECT
public:
    FpsCalculator(QObject *parent = NULL)
        : QObject(parent),
          m_currentFPS(0),
          m_cacheCount(0) {}

public slots:
    void recalculateFPS() {
        qint64 currentTime = QDateTime::currentMSecsSinceEpoch();
        m_times.push_back(currentTime);

        while (m_times[0] < currentTime - 1000) {
            m_times.pop_front();
        }

        int currentCount = m_times.length();
        m_currentFPS = (currentCount + m_cacheCount) / 2;

        if (currentCount != m_cacheCount) {
            // fps changed
            emit fpsChanged(m_currentFPS);
        }

        m_cacheCount = currentCount;
    }

    int fps() const {
        return m_currentFPS;
    }

signals:
    void fpsChanged(int fps);

private:
    int m_currentFPS;
    int m_cacheCount;
    QVector<qint64> m_times;
};

}

#endif // FPSCALCULATOR_H
