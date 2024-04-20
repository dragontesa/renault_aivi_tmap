#ifndef LASTROUTEDTO_H
#define LASTROUTEDTO_H

namespace SKT {
T_SHAREABLE( LastRouteDto );

class LastRouteDto : public QObject
{
    Q_OBJECT
    Q_CLASSINFO( "table", "name=last_route" )
    Q_CLASSINFO( "column", "name=id,type=integer,pk=true,ai=true,not_null=true" )
    Q_CLASSINFO( "column", "name=isNormallyTerminated,type=integer,default=0" )
    Q_CLASSINFO( "column", "name=selectedRouteIndex,type=integer" )
    Q_CLASSINFO( "column", "name=selectedRouteOption,type=integer" )
    Q_CLASSINFO( "column", "name=mapLevel,type=integer" )
    Q_CLASSINFO( "column", "name=tpX,type=integer" )
    Q_CLASSINFO( "column", "name=tpY,type=integer" )
    Q_CLASSINFO( "column", "name=tpTime,type=text" )
    Q_CLASSINFO( "column", "name=sessionId,type=text" )
    Q_CLASSINFO( "column", "name=address,type=text" )
    Q_CLASSINFO( "column", "name=via,type=text" )

//    Q_CLASSINFO( "index", "name=last_route_index,columns=sessionId" )

    Q_PROPERTY(QVariant isNormallyTerminated READ isNormallyTerminated WRITE setIsNormallyTerninated)
    Q_PROPERTY(QVariant selectedRouteIndex READ selectedRouteIndex WRITE setSelectedRouteIndex)
    Q_PROPERTY(QVariant selectedRouteOption READ selectedRouteOption WRITE setSelectedRouteOption)
    Q_PROPERTY(QVariant mapLevel READ mapLevel WRITE setMapLevel)
    Q_PROPERTY(QVariant tpX READ tpX WRITE setTpX)
    Q_PROPERTY(QVariant tpY READ tpY WRITE setTpY)
    Q_PROPERTY(QVariant tpTime READ tpTime WRITE setTpTime)
    Q_PROPERTY(QVariant sessionId READ sessionId WRITE setSessionId)
    Q_PROPERTY(QVariant address READ address WRITE setAddress)
    Q_PROPERTY(QVariant via READ via WRITE setVia)
public:
    explicit LastRouteDto(QObject* parent=0);

    inline QVariant isNormallyTerminated() const { return mIsNormallyTerminated; }
    inline void  setIsNormallyTerninated(const QVariant& value) { mIsNormallyTerminated = value; }

    inline QVariant selectedRouteIndex() const { return mSelectedRouteIndex; }
    inline void setSelectedRouteIndex(const QVariant& value) { mSelectedRouteIndex = value;}

    inline QVariant selectedRouteOption() const { return mSelectedRouteOption; }
    inline void setSelectedRouteOption(const QVariant& value) { mSelectedRouteOption = value; }

    inline QVariant mapLevel() const { return mMapLevel; }
    inline void setMapLevel(const QVariant& value) { mMapLevel = value; }

    inline QVariant tpX() const { return mTpX; }
    inline void setTpX(const QVariant& value) { mTpX = value;}

    inline QVariant tpY() const { return mTpY; }
    inline void setTpY(const QVariant& value) { mTpY = value;}

    inline QVariant tpTime() const { return mTpTime; }
    inline void setTpTime(const QVariant& value) { mTpTime = value; }

    inline QVariant sessionId() const { return mSessionId; }
    inline void setSessionId(const QVariant& value) { mSessionId = value;}

    inline QVariant address() const { return mAddress;}
    inline void setAddress(const QVariant& value) { mAddress = value; }

    inline QVariant via() const { return mVia; }
    inline void setVia(const QVariant& value) { mVia = value; }

private:
    QVariant mAddress;
    QVariant mIsNormallyTerminated;
    QVariant mSelectedRouteIndex;
    QVariant mSelectedRouteOption;
    QVariant mVia;
    QVariant mMapLevel;
    QVariant mTpX;
    QVariant mTpY;
    QVariant mTpTime;
    QVariant mSessionId;
};

}
#endif // LASTROUTEDTO_H
