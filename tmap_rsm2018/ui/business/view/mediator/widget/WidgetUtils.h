#ifndef WIDGETUTILS_H
#define WIDGETUTILS_H

#include "WidgetConstants.h"
#include "RgConstants.h"

namespace SKT {

class WidgetUtils
{
public:
    static QString tbtFirstImage( WidgetConstants::Size size, int code );
    static QString tbtSecondImage( WidgetConstants::Size size, int code );
    static QStringList tbtFirstDistanceNumberImages( WidgetConstants::Size size, const QString& number );
    static QStringList tbtSecondDistanceNumberImages( WidgetConstants::Size size, const QString& number );

    static QString sdiSignImage( WidgetConstants::Size size, const QString& img );
    static QStringList sdiSignNumberImages( WidgetConstants::Size size, const QString& number );
    static QStringList sdiSignSmallNumberImages( WidgetConstants::Size size, const QString& number );
    static QStringList sdiRemainDistanceNumberImages( WidgetConstants::Size size, const QString& number );
    static QStringList sdiCurrentSpeedNumberImages( WidgetConstants::Size size, const QString& number, bool alert );
    static QStringList sdiSectionSpeedAverageNumberImages( WidgetConstants::Size size, RgConstants::SdiSectionSpeedNumberType numberType, const QString& number );
    static QStringList sdiSectionSpeedTimeImages( WidgetConstants::Size size, RgConstants::SdiSectionSpeedNumberType numberType, const QString& number );

    static QStringList laneImages( WidgetConstants::Size size, int laneIndex, const QStringList& images );
    static QStringList laneDistanceNumberImages( WidgetConstants::Size size, int laneIndex, const QString& number );

    static bool handleTbtUpdate( WidgetConstants::Size size, QObject* view, const mvc::INotificationPtr& note );
    static bool handleSdiUpdate( WidgetConstants::Size size, QObject* view, const mvc::INotificationPtr& note );
    static bool handleLaneUpdate( WidgetConstants::Size size, QObject* view, const mvc::INotificationPtr& note );

    static QString viewUrl( UIConstants::GadgetType type );
    static QSize viewSize( UIConstants::GadgetType type );
    static QString viewObjectName( UIConstants::GadgetType type );

    static bool isNaviGadget( UIConstants::GadgetType type );
};

};

#endif // WIDGETUTILS_H
