#ifndef RING_H
#define RING_H

#include <QQuickItem>

namespace SKT
{

class Ring : public QQuickItem
{
    Q_OBJECT

    Q_PROPERTY(quint32 segment READ segment WRITE setSegment NOTIFY segmentChanged)
    Q_PROPERTY(qreal innerRadius READ innerRadius WRITE setInnerRadius NOTIFY innerRadiusChanged)
    Q_PROPERTY(qreal outerRadius READ outerRadius WRITE setOuterRadius NOTIFY outerRadiusChanged)
    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)
    Q_PROPERTY(qreal beginAngle READ beginAngle WRITE setBeginAngle NOTIFY beginAngleChanged)
    Q_PROPERTY(qreal endAngle READ endAngle WRITE setEndAngle NOTIFY endAngleChanged)

public:
    Ring(QQuickItem *parent = NULL);

    virtual QSGNode *updatePaintNode(QSGNode *, UpdatePaintNodeData *) Q_DECL_OVERRIDE;
    virtual QRectF boundingRect() const Q_DECL_OVERRIDE;

    quint32 segment() const { return mSegment; }
    qreal innerRadius() const { return mInnerRadius; }
    qreal outerRadius() const { return mOuterRadius; }
    QColor color() const { return mColor; }
    qreal beginAngle() const { return mBeginAngle; }
    qreal endAngle() const { return mEndAngle; }

    void setSegment(quint32 segment) {
        if (mSegment != segment) {
            mSegment = segment;
            emit segmentChanged();
            update();
        }
    }
    void setInnerRadius(qreal radius) {
        if (mInnerRadius != radius) {
            mInnerRadius = radius;
            emit innerRadiusChanged();
            update();
        }
    }
    void setOuterRadius(qreal radius) {
        if (mOuterRadius != radius) {
            mOuterRadius = radius;
            emit outerRadiusChanged();
            update();
        }
    }
    void setColor(const QColor &color) {
        if (mColor != color) {
            mColor = color;
            emit colorChanged();
            update();
        }
    }
    void setBeginAngle(qreal angle) {
        if (mBeginAngle != angle) {
            mBeginAngle = angle;
            emit beginAngleChanged(angle);
            update();
        }
    }
    void setEndAngle(qreal angle) {
        if (mEndAngle != angle) {
            mEndAngle = angle;
            emit endAngleChanged(angle);
            update();
        }
    }

Q_SIGNALS:
    void segmentChanged();
    void innerRadiusChanged();
    void outerRadiusChanged();
    void colorChanged();
    void beginAngleChanged(qreal);
    void endAngleChanged(qreal);

private:
    quint32 mSegment;
    qreal mInnerRadius;
    qreal mOuterRadius;
    qreal mBeginAngle;
    qreal mEndAngle;
    QColor mColor;
};

}//SKT

#endif // RING_H
