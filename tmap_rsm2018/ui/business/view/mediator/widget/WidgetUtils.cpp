#include "WidgetUtils.h"
#include "mediator/rg/RgUtils.h"
#include "ResourceResolver.h"
#include "RgConstants.h"
#include "ViewRegistry.h"

using namespace SKT;

QString WidgetUtils::tbtFirstImage( WidgetConstants::Size size, int code )
{
	auto img = RgUtils::tbtFirstImage( code );
	if ( !img.isEmpty() )  {
        switch ( size ) {
        case WidgetConstants::SizeXXXL: {
                img = ResourceResolver::instance()->imageUrl( QStringLiteral("tbt"), img );
            }
            break;
        case WidgetConstants::SizeXXL:
        case WidgetConstants::SizeXL:
        case WidgetConstants::SizeLarge:
        case WidgetConstants::SizeSmall:
        case WidgetConstants::SizeMedium: {
                img = ResourceResolver::instance()->imageUrl( QStringLiteral("widgettbt"), QStringLiteral("wid_") + img );
            }
            break;
        case WidgetConstants::SizeNone:
            break;
        }
	}
	
	return img;
}

QString WidgetUtils::tbtSecondImage( WidgetConstants::Size size, int code )
{
    auto img = RgUtils::tbtSecondImage( code );
    if ( !img.isEmpty() )  {
        switch ( size ) {
        case WidgetConstants::SizeXXXL: {
                img = ResourceResolver::instance()->imageUrl( QStringLiteral("tbt"), img );
            }
            break;
        case WidgetConstants::SizeXXL:
        case WidgetConstants::SizeXL:        
        case WidgetConstants::SizeSmall: {
                img = ResourceResolver::instance()->imageUrl( QStringLiteral("widgettbt"), QStringLiteral("wid_") + img );
            }
            break;
        case WidgetConstants::SizeLarge:
        case WidgetConstants::SizeMedium: {
                img = ResourceResolver::instance()->imageUrl( QStringLiteral("widgettbt"), QStringLiteral("wid_m_") + img );
            }
            break;
        case WidgetConstants::SizeNone:
            break;
        }
    }

    return img;
}

QStringList WidgetUtils::tbtFirstDistanceNumberImages( WidgetConstants::Size size, const QString& number )
{	
    QStringList ret;
	
    for(int i = 0; i < number.length(); i++){
        auto ch = number.at(i);        
		auto img = RgUtils::tbtFirstDistanceNumberImage(ch);
        switch ( size ) {
        case WidgetConstants::SizeXXXL: {
                img = ResourceResolver::instance()->imageUrl( QStringLiteral("tbt"), img );
            }
            break;
        case WidgetConstants::SizeXXL:
        case WidgetConstants::SizeXL:
        case WidgetConstants::SizeLarge:
        case WidgetConstants::SizeMedium:
        case WidgetConstants::SizeSmall: {
                img = ResourceResolver::instance()->imageUrl( QStringLiteral("widgettbt"), QStringLiteral("wid_") + img );
            }
            break;
        case WidgetConstants::SizeNone:
            break;
        }

        ret.append( img );
		if ( ch == QChar('k') ) {
			i++;
		}
	}	
	
    return ret;
}

QStringList WidgetUtils::tbtSecondDistanceNumberImages( WidgetConstants::Size size, const QString& number )
{
    QStringList ret;

    for(int i = 0; i < number.length(); i++){
        auto ch = number.at(i);
        auto img = RgUtils::tbtSecondDistanceNumberImage(ch);
        switch ( size ) {
        case WidgetConstants::SizeXXXL: {
                img = ResourceResolver::instance()->imageUrl( QStringLiteral("tbt"), img );
            }
            break;
        case WidgetConstants::SizeXXL:
        case WidgetConstants::SizeXL:
        case WidgetConstants::SizeLarge:
        case WidgetConstants::SizeSmall: {
                img = ResourceResolver::instance()->imageUrl( QStringLiteral("widgettbt"), QStringLiteral("wid_") + img );
            }
            break;
        case WidgetConstants::SizeMedium: {
                img = ResourceResolver::instance()->imageUrl( QStringLiteral("widgettbt"), QStringLiteral("wid_m_") + img );
            }
            break;
        case WidgetConstants::SizeNone:
            break;
        }

        ret.append( img );
        if ( ch == QChar('k') ) {
            i++;
        }
    }

    return ret;
}

QString WidgetUtils::sdiSignImage( WidgetConstants::Size size, const QString& img )
{
    QString ret;

    if ( size != WidgetConstants::SizeXXXL ) {
        ret = ResourceResolver::instance()->imageUrl( QStringLiteral("widgetsdi"), QStringLiteral("wid_") + img );
    }
    else {
        ret = ResourceResolver::instance()->imageUrl( "sdi", img );
    }

    return ret;
}

QStringList WidgetUtils::sdiSignNumberImages( WidgetConstants::Size size, const QString& number )
{
    QStringList ret;

    QString img;
    for(int i = 0; i < number.length(); i++){
        auto ch = number.at(i);
        img = RgUtils::sdiSignNumberImage(ch);
        if ( size != WidgetConstants::SizeXXXL ) {
            img = ResourceResolver::instance()->imageUrl( QStringLiteral("widgetsdi"), QStringLiteral("wid_") + img );
        }
        else {
            img = ResourceResolver::instance()->imageUrl( QStringLiteral("sdi"), img );
        }
        ret.append( img );
    }

    return ret;
}

QStringList WidgetUtils::sdiSignSmallNumberImages( WidgetConstants::Size size, const QString& number )
{
    QStringList ret;

    QString img;
    for(int i = 0; i < number.length(); i++){
        auto ch = number.at(i);
        img = RgUtils::sdiSignNumberSmallImage(ch);
        if ( size != WidgetConstants::SizeXXXL ) {
            img = ResourceResolver::instance()->imageUrl( QStringLiteral("widgetsdi"), QStringLiteral("wid_") + img );
        }
        else {
            img = ResourceResolver::instance()->imageUrl( QStringLiteral("sdi"), img );
        }
        ret.append( img );
    }

    return ret;
}

QStringList WidgetUtils::sdiRemainDistanceNumberImages( WidgetConstants::Size size, const QString& number )
{
    QStringList ret;

    for(int i = 0; i < number.length(); i++){
        auto ch = number.at(i);
        auto img = RgUtils::sdiDistanceNumberImage(ch);
        if ( size != WidgetConstants::SizeXXXL ) {
            img = ResourceResolver::instance()->imageUrl( QStringLiteral("widgetsdi"), QStringLiteral("wid_") + img );
        }
        else {
            img = ResourceResolver::instance()->imageUrl( QStringLiteral("sdi"), img );
        }
        ret.append( img );
        if ( ch == QChar('k') ) {
            i++;
        }
    }

    return ret;
}

QStringList WidgetUtils::sdiCurrentSpeedNumberImages( WidgetConstants::Size size, const QString& number, bool alert )
{
    QStringList ret;

    for(int i = 0; i < number.length(); i++){
        auto ch = number.at(i);
        auto img = RgUtils::sdiCurrentSpeedNumberImage(ch, alert);
        if ( size != WidgetConstants::SizeXXXL ) {
            if ( size == WidgetConstants::SizeLarge ) {
                img = ResourceResolver::instance()->imageUrl( QStringLiteral("sdi"), img );
            }
            else {
                img = ResourceResolver::instance()->imageUrl( QStringLiteral("widgetsdi"), QStringLiteral("wid_") + img );
            }
        }
        else {
            img = ResourceResolver::instance()->imageUrl( QStringLiteral("sdi"), img );
        }
        ret.append( img );
        if ( ch == QChar('k') ) {
            i++;
        }
    }

    return ret;
}

QStringList WidgetUtils::sdiSectionSpeedAverageNumberImages( WidgetConstants::Size size, RgConstants::SdiSectionSpeedNumberType numberType, const QString& number )
{
    QStringList ret;

    for(int i = 0; i < number.length(); i++){
        auto ch = number.at(i);
        auto img = RgUtils::sdiSectionSpeedAverageNumberImage(numberType, ch);
        if ( size != WidgetConstants::SizeXXXL ) {
            img = ResourceResolver::instance()->imageUrl( QStringLiteral("widgetsdi"), QStringLiteral("wid_") + img );
        }
        else {
            img = ResourceResolver::instance()->imageUrl( QStringLiteral("sdi"), img );
        }
        ret.append( img );
        if ( ch == QChar('k') ) {
            i++;
        }
    }

    if ( size != WidgetConstants::SizeLarge ) {
        ret.append( ResourceResolver::instance()->imageUrl( QStringLiteral("widgetsdi"),
            QStringLiteral("wid_") + RgUtils::sdiSpeedSectionAverageNumberKmhImage( numberType ) ) );
    }

    return ret;
}

QStringList WidgetUtils::sdiSectionSpeedTimeImages( WidgetConstants::Size size, RgConstants::SdiSectionSpeedNumberType numberType, const QString& number )
{
    QStringList ret;

    for(int i = 0; i < number.length(); i++){
        auto ch = number.at(i);
        auto img = RgUtils::sdiSectionSpeedTimeImage(numberType, ch);
        if ( size != WidgetConstants::SizeXXXL ) {
            img = ResourceResolver::instance()->imageUrl( QStringLiteral("widgetsdi"), QStringLiteral("wid_") + img );
        }
        else {
            img = ResourceResolver::instance()->imageUrl( QStringLiteral("sdi"), img );
        }
        ret.append( img );
    }

    return ret;
}

QStringList WidgetUtils::laneImages( WidgetConstants::Size size, int laneIndex, const QStringList& images )
{
    QStringList ret;

    for ( auto img : images ){        
        if ( size != WidgetConstants::SizeXXXL ) {
            img = ResourceResolver::instance()->imageUrl( QStringLiteral("widgetlane"), QStringLiteral("wid_") + img );
        }
        else {
            img = ResourceResolver::instance()->imageUrl( QStringLiteral("lane"), img );
        }
        ret.append( img );
    }

    return ret;
}

QStringList WidgetUtils::laneDistanceNumberImages( WidgetConstants::Size size, int laneIndex, const QString& number )
{
    QStringList ret;

    for(int i = 0; i < number.length(); i++){
        auto ch = number.at(i);
        auto img = RgUtils::laneDistanceNumberImage( laneIndex, ch );
        if ( size != WidgetConstants::SizeXXXL ) {
            // TODO wid_laneguide2_??? => wid_laneguid2_???
            if ( img.indexOf( QStringLiteral("laneguid2") ) > -1 ) {
                img.replace( QStringLiteral("laneguid2"), QStringLiteral("laneguide2") );
            }
            img = ResourceResolver::instance()->imageUrl( QStringLiteral("widgetlane"), QStringLiteral("wid_") + img );
        }
        else {
            img = ResourceResolver::instance()->imageUrl( QStringLiteral("lane"), img );
        }

        ret.append( img );
        if ( ch == QChar('k') ) {
            i++;
        }
    }

    return ret;
}

bool WidgetUtils::handleTbtUpdate( WidgetConstants::Size size, QObject* view, const mvc::INotificationPtr& note )
{
    auto m = note->getBody().toMap();
    auto item = m["item"].toInt();
    switch ( item ) {
    case RgConstants::TbtReset:
        {
            QQmlProperty::write( view, QStringLiteral("tbtFirstImageSource"), QString() );
            QQmlProperty::write( view, QStringLiteral("tbtFirstDistNumberImageSources"), QStringList() );
            QQmlProperty::write( view, QStringLiteral("tbtFirstPointName"), QString() );
            QQmlProperty::write( view, QStringLiteral("tbtSecondImageSource"), QString() );
            QQmlProperty::write( view, QStringLiteral("tbtSecondDistNumberImageSources"), QString() );
            QQmlProperty::write( view, QStringLiteral("tbtReset"), false );
            QQmlProperty::write( view, QStringLiteral("tbtReset"), true );
            QQmlProperty::write( view, QStringLiteral("tbtReset"), false );
        }
        break;
    case RgConstants::TbtFirstTurnType:
        {
            auto value = m["value"].toInt();
            auto img = WidgetUtils::tbtFirstImage( size, value );
            if ( !img.isEmpty() ) {
                QQmlProperty::write( view, QStringLiteral("tbtFirstImageSource"), img );
            }
        }
        break;
    case RgConstants::TbtFirstDistance:
        {
            auto value = m["value"].toString();
            auto images = WidgetUtils::tbtFirstDistanceNumberImages( size, value );
            QQmlProperty::write( view, QStringLiteral("tbtFirstDistNumberImageSources"), images );

        }
        break;
    case RgConstants::TbtFirstPointName:
        {
            auto value = m["value"].toString();
            QQmlProperty::write( view, QStringLiteral("tbtFirstPointName"), value );

        }
        break;
    case RgConstants::TbtSecondTurnType:
        {
            auto value = m["value"].toInt();
            auto img = WidgetUtils::tbtSecondImage( size, value );
            QQmlProperty::write( view, QStringLiteral("tbtSecondImageSource"), img );
        }
        break;
    case RgConstants::TbtSecondDistance:
        {
            auto value = m["value"].toString();
            auto images = WidgetUtils::tbtSecondDistanceNumberImages( size, value );
            QQmlProperty::write( view, QStringLiteral("tbtSecondDistNumberImageSources"), images );
        }
        break;
    }

    return true;
}

bool WidgetUtils::handleSdiUpdate( WidgetConstants::Size size, QObject* view, const mvc::INotificationPtr& note )
{
    auto m = note->getBody().toMap();
    auto item = m["item"].toInt();
    switch ( item ) {
    case RgConstants::SdiReset:
        {
            QQmlProperty::write( view, QStringLiteral("sdiSignType"), RgConstants::SdiCircledNumber );
            QQmlProperty::write( view, QStringLiteral("sdiSignImageSource"), QString() );
            QQmlProperty::write( view, QStringLiteral("sdiSignNumberImageSources"), QStringList() );
            QQmlProperty::write( view, QStringLiteral("sdiSectionSpeedVisible"), false );
            QQmlProperty::write( view, QStringLiteral("sdiSectionSpeedAverageVisible"), false );
            QQmlProperty::write( view, QStringLiteral("sdiSectionSpeedTimeVisible"), false );
            QQmlProperty::write( view, QStringLiteral("sdiSignVisible"), false );
            QQmlProperty::write( view, QStringLiteral("sdiRemainDistVisible"), false );
            QQmlProperty::write( view, QStringLiteral("sdiRemainDistNumberImageSources"), QStringList() );
            QQmlProperty::write( view, QStringLiteral("sdiSectionSpeedTime"), 0 );
            //[TMCRSM-212 leejaeho] 화면을 계속 reset하여 필요한 현재속도가 노출되지 않음. 주석처리함
//            QQmlProperty::write( view, QStringLiteral("sdiCurrentSpeedNumberImageSources"), QStringList() );
//            QQmlProperty::write( view, QStringLiteral("sdiReset"), false );
//            QQmlProperty::write( view, QStringLiteral("sdiReset"), true );
//            QQmlProperty::write( view, QStringLiteral("sdiReset"), false );
        }
        break;
    case RgConstants::SdiSignType:
        {
            auto value = m["value"].toInt();
            QQmlProperty::write( view, QStringLiteral("sdiSignType"), value );
        }
        break;
    case RgConstants::SdiSignImage:
        {
            auto value = m["value"].toString();
            if ( !value.isEmpty() ) {
                auto image = WidgetUtils::sdiSignImage( size, value );
                QQmlProperty::write( view, QStringLiteral("sdiSignImageSource"), image );
            }
        }
        break;
    case RgConstants::SdiSignNumber:
        {
            auto value = m["value"].toString();
            auto images = WidgetUtils::sdiSignNumberImages( size, value );
            QQmlProperty::write( view, QStringLiteral("sdiSignNumberImageSources"), images );

            qDebug() << "[Gadget][SDI][SdiSignNumber]value: " << value << " images: " << images.length();
            for ( int i = 0; i < images.length(); i++ ) {
                auto image = images[i];
                qDebug() << "[Gadget][SDI][SdiSignNumber[" << i << "]" << image;
            }
        }
        break;
    case RgConstants::SdiSignSmallNumber:
        {
            auto value = m["value"].toString();
            auto images = WidgetUtils::sdiSignSmallNumberImages( size, value );
            QQmlProperty::write( view, QStringLiteral("sdiSignNumberImageSources"), images );

            qDebug() << "[Gadget][SDI][SdiSignSmallNumber]value: " << value << " images: " << images.length();
            for ( int i = 0; i < images.length(); i++ ) {
                auto image = images[i];
                qDebug() << "[Gadget][SDI][SdiSignSmallNumber[" << i << "]" << image;
            }
        }
        break;
    case RgConstants::SdiSignVisible:
        {
            auto value = m["value"].toBool();
            QQmlProperty::write( view, QStringLiteral("sdiSignVisible"), value );
        }
        break;
    case RgConstants::SdiAlert:
        {
            auto value = m["value"].toBool();
        }
        break;
    case RgConstants::SdiDistanceVisible:
        {
            auto value = m["value"].toBool();
            QQmlProperty::write( view, QStringLiteral("sdiRemainDistVisible"), value );
        }
        break;
    case RgConstants::SdiDistance:
        {
            auto value = m["value"].toString();
            auto images = WidgetUtils::sdiRemainDistanceNumberImages( size, value );
            QQmlProperty::write( view, QStringLiteral("sdiRemainDistNumberImageSources"), images );
        }
        break;
    case RgConstants::SdiCurrentSpeed:
        {
            auto value = m["value"].toString();
            auto alert = m["alert"].toBool();
            auto images = WidgetUtils::sdiCurrentSpeedNumberImages( size, value, alert );
            QQmlProperty::write( view, QStringLiteral("sdiCurrentSpeedNumberImageSources"), images );
        }
        break;
    case RgConstants::SdiSectionSpeedAverageNumber:
        {
            auto value = m["value"].toString();
            auto numberType = (RgConstants::SdiSectionSpeedNumberType)m["numberType"].toInt();
            auto images = WidgetUtils::sdiSectionSpeedAverageNumberImages( size, numberType, value );

            qDebug() << "[Gadget][SDI][SectionSpeedAverageNumber]numberType: " << numberType << " images: " << images.length();
            for ( int i = 0; i < images.length(); i++ ) {
                auto image = images[i];
                qDebug() << "[Gadget][SDI][SectionSpeedAverageNumber[" << i << "]" << image;
            }

            QQmlProperty::write( view, QStringLiteral("sdiSectionSpeedAverageNumberImageSources"), images );
        }
        break;
    case RgConstants::SdiSectionSpeedVisible:
        {
            auto value = m["value"].toBool();

            QQmlProperty::write( view, QStringLiteral("sdiSectionSpeedVisible"), value );
        }
        break;
    case RgConstants::SdiSectionSpeedTime:
        {
            auto value = m["value"].toString();
            auto numberType = (RgConstants::SdiSectionSpeedNumberType)m["numberType"].toInt();
            auto images = WidgetUtils::sdiSectionSpeedTimeImages( size, numberType, value );

//            qDebug() << "[Gadget][SDI][SdiSectionSpeedTime]numberType: " << numberType << " images: " << images.length();
//            for ( int i = 0; i < images.length(); i++ ) {
//                auto image = images[i];
//                qDebug() << "[Gadget][SDI][SdiSectionSpeedTime[" << i << "]" << image;
//            }

            QQmlProperty::write( view, QStringLiteral("sdiSectionSpeedTimeImageSources"), images );
        }
        break;
    case RgConstants::SdiSectionSpeedToggleAverageAndTime:
        {
            auto value = m["value"].toBool();

            QQmlProperty::write( view, QStringLiteral("sdiSectionSpeedToggleAverageAndTime"), value );
        }
        break;
    case RgConstants::SdiSectionSpeedAverageVisible:
        {
            auto value = m["value"].toBool();

            QQmlProperty::write( view, QStringLiteral("sdiSectionSpeedAverageVisible"), value );
        }
        break;
    case RgConstants::SdiSectionSpeedTimeVisible:
        {
            auto value = m["value"].toBool();

            QQmlProperty::write( view, QStringLiteral("sdiSectionSpeedTimeVisible"), value );
        }
        break;
    }

    return true;
}

bool WidgetUtils::handleLaneUpdate( WidgetConstants::Size size, QObject* view, const mvc::INotificationPtr& note )
{
    auto m = note->getBody().toMap();
    auto item = m["item"].toInt();
    switch ( item ) {
    case RgConstants::LaneReset:
        {
            QQmlProperty::write( view, QStringLiteral("laneFirstImageSources"), QStringList() );
            QQmlProperty::write( view, QStringLiteral("laneSecondImageSources"), QStringList() );
            QQmlProperty::write( view, QStringLiteral("laneFirstDistNumberImageSources"), QStringList() );
            QQmlProperty::write( view, QStringLiteral("laneSecondDistNumberImageSources"), QStringList() );
            QQmlProperty::write( view, QStringLiteral("laneFirstVisible"), false );
            QQmlProperty::write( view, QStringLiteral("laneSecondVisible"), false );
            QQmlProperty::write( view, QStringLiteral("laneReset"), false );
            QQmlProperty::write( view, QStringLiteral("laneReset"), true );
            QQmlProperty::write( view, QStringLiteral("laneReset"), false );
        }
        break;
    case RgConstants::LaneImageList:
        {
            auto laneIndex = m["laneIndex"].toInt();
            auto value = m["value"].toList();

            QStringList imgList = WidgetUtils::laneImages( size, laneIndex, value[0].toStringList() );
            QStringList coverImgList = WidgetUtils::laneImages( size, laneIndex, value[1].toStringList() );
            value = QVariantList{{
                QVariant::fromValue(imgList),
                QVariant::fromValue(coverImgList)
            }};

            if ( laneIndex == 0 ) {
                QQmlProperty::write( view, QStringLiteral("laneFirstImageSources"), value );
            }
            else {
                QQmlProperty::write( view, QStringLiteral("laneSecondImageSources"), value );
            }
        }
        break;
    case RgConstants::LaneDistance:
        {
            auto laneIndex = m["laneIndex"].toInt();
            auto value = m["value"].toString();

            if ( laneIndex == 0 ) {
                auto images = WidgetUtils::laneDistanceNumberImages( size, laneIndex, value );
                QQmlProperty::write( view, QStringLiteral("laneFirstDistNumberImageSources"), images );
            }
            else {
                auto images = WidgetUtils::laneDistanceNumberImages( size, laneIndex, value );
                QQmlProperty::write( view, QStringLiteral("laneSecondDistNumberImageSources"), images );
            }
        }
        break;
    case RgConstants::LaneVisible:
        {
            auto laneIndex = m["laneIndex"].toInt();
            auto value = m["value"].toBool();
            if ( laneIndex == 0 ) {
                QQmlProperty::write( view, QStringLiteral("laneFirstVisible"), value );
            }
            else {
                QQmlProperty::write( view, QStringLiteral("laneSecondVisible"), value );
            }
        }
        break;
    }

    return true;
}

QString WidgetUtils::viewUrl( UIConstants::GadgetType type )
{
    QString ret;

    switch ( type ) {
    case UIConstants::GadgetXXXL:
        ret = ViewRegistry::url(ViewRegistry::WidgetXxxlView);
        break;
    case UIConstants::GadgetXXL:
        ret = ViewRegistry::url(ViewRegistry::WidgetXxlView);
        break;
    case UIConstants::GadgetXL:
        ret = ViewRegistry::url(ViewRegistry::WidgetXlView);
        break;
    case UIConstants::GadgetLarge:
        ret = ViewRegistry::url(ViewRegistry::WidgetLargeView);
        break;
    case UIConstants::GadgetMedium:
        ret = ViewRegistry::url(ViewRegistry::WidgetMediumView);
        break;
    case UIConstants::GadgetSmall:
        ret = ViewRegistry::url(ViewRegistry::WidgetSmallView);
        break;
    }

    return ret;
}

QString WidgetUtils::viewObjectName( UIConstants::GadgetType type )
{
    QString ret;

    switch ( type ) {
    case UIConstants::GadgetXXXL:
        ret = ViewRegistry::objectName(ViewRegistry::WidgetXxxlView);
        break;
    case UIConstants::GadgetXXL:
        ret = ViewRegistry::objectName(ViewRegistry::WidgetXxlView);
        break;
    case UIConstants::GadgetXL:
        ret = ViewRegistry::objectName(ViewRegistry::WidgetXlView);
        break;
    case UIConstants::GadgetLarge:
        ret = ViewRegistry::objectName(ViewRegistry::WidgetLargeView);
        break;
    case UIConstants::GadgetMedium:
        ret = ViewRegistry::objectName(ViewRegistry::WidgetMediumView);
        break;
    case UIConstants::GadgetSmall:
        ret = ViewRegistry::objectName(ViewRegistry::WidgetSmallView);
        break;
    }

    return ret;
}

QSize WidgetUtils::viewSize( UIConstants::GadgetType type )
{
    QSize ret;

    switch ( type ) {
    case UIConstants::GadgetXXXL:
        ret = QSize(598, 788);
        break;
    case UIConstants::GadgetXXL:
        ret = QSize(800, 390);
        break;
    case UIConstants::GadgetXL:
        ret = QSize(598, 390);
        break;
    case UIConstants::GadgetLarge:
        ret = QSize(396, 390);
        break;
    case UIConstants::GadgetMedium:
        ret = QSize(396, 191);
        break;
    case UIConstants::GadgetSmall:
        ret = QSize(194, 191);
        break;
    }

    return ret;
}

bool WidgetUtils::isNaviGadget( UIConstants::GadgetType type )
{
    bool ret = false;

    switch ( type ) {
    case UIConstants::GadgetXXXL:
    case UIConstants::GadgetXXL:
    case UIConstants::GadgetXL:
    case UIConstants::GadgetLarge:
    case UIConstants::GadgetMedium:
    case UIConstants::GadgetSmall:
        ret = true;
        break;
    }

    return ret;
}
