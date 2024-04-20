#ifndef RECENTDESTINATIONDTO_H
#define RECENTDESTINATIONDTO_H

namespace SKT {

T_SHAREABLE( RecentDestinationDto );

class RecentDestinationDto : public QObject
{
    Q_OBJECT
	Q_CLASSINFO( "table", "name=recent_dest" )
    Q_CLASSINFO( "column", "name=id,type=integer,pk=true,ai=true,not_null=true" )
    Q_CLASSINFO( "column", "name=dest,type=text" )
	Q_CLASSINFO( "column", "name=address,type=text" )
	Q_CLASSINFO( "column", "name=mapX,type=integer" )
	Q_CLASSINFO( "column", "name=mapY,type=integer" )
	Q_CLASSINFO( "column", "name=destType,type=integer,default=0" ) // 0: unkown, 1: home, 2: office,
	Q_CLASSINFO( "column", "name=hit,type=integer,default=0" )
    Q_CLASSINFO( "column", "name=poiID,type=integer" )
    Q_CLASSINFO( "column", "name=navSeq,type=integer" )
    Q_CLASSINFO( "column", "name=rpFlag,type=integer" )
    Q_CLASSINFO( "column", "name=opiPoleCate,type=integer")
    Q_CLASSINFO( "column", "name=opiPrice,type=integer" )

	// TODO: POI ID must be kept
#if 0
	Q_CLASSINFO( "column", "name=poiID, type=integer" ); // POI ID
#endif
	Q_CLASSINFO( "index", "name=recent_dest_index,columns=dest|mapX|mapY" )

	Q_PROPERTY(QVariant id READ id WRITE setId)
	Q_PROPERTY(QVariant dest READ dest WRITE setDest)
	Q_PROPERTY(QVariant address READ address WRITE setAddress)
	Q_PROPERTY(QVariant mapX READ mapX WRITE setMapX)
	Q_PROPERTY(QVariant mapY READ mapY WRITE setMapY)
	Q_PROPERTY(QVariant destType READ destType WRITE setDestType)
	Q_PROPERTY(QVariant hit READ hit WRITE setHit)
    Q_PROPERTY(QVariant poiID READ poiID WRITE setPoiID)
    Q_PROPERTY(QVariant navSeq READ navSeq WRITE setNavSeq)
    Q_PROPERTY(QVariant rpFlag READ rpFlag WRITE setRpFlag)
    Q_PROPERTY(QVariant opiPoleCate READ opiPoleCate WRITE setOpiPoleCate)
    Q_PROPERTY(QVariant opiPrice READ opiPrice WRITE setOpiPrice)
	// TODO: POI ID must be kept
#if 0
	Q_PROPERTY(QVariant poiID READ poiID WRITE setPoiID)
#endif

public:
    explicit RecentDestinationDto(QObject *parent = 0);

    inline QVariant id() const { return mId; }
    inline void setId(const QVariant& value) { mId = value; }

    inline QVariant dest() const { return mDest; }
    inline void setDest(const QVariant& value) { mDest = value;}

    inline QVariant address() const { return mAddress; }
    inline void setAddress(const QVariant& value) { mAddress = value;}

    inline QVariant mapX() const { return mMapX; }
    inline void setMapX(const QVariant& value) { mMapX = value;}

    inline QVariant mapY() const { return mMapY;}
    inline void setMapY(const QVariant& value) { mMapY = value;}

    inline QVariant destType() const { return mDestType; }
    inline void setDestType(const QVariant& value) { mDestType = value; }

    QVariant hit() const { return mHit; }
    void setHit(const QVariant& value) { mHit = value; }

    inline QVariant poiID() const { return mPoiID; }
    inline void setPoiID(const QVariant& value) { mPoiID = value; }

    inline QVariant navSeq() const { return mNavSeq; }
    inline void setNavSeq(const QVariant& value) { mNavSeq = value;}

    inline QVariant rpFlag() const { return mRpFlag; }
    inline void     setRpFlag(const QVariant& value) { mRpFlag = value; }

    inline QVariant opiPoleCate() const { return mOpiPoleCate; }
    inline void     setOpiPoleCate(const QVariant& value) { mOpiPoleCate = value; }

    inline QVariant opiPrice() const { return mOpiPrice; }
    inline void     setOpiPrice(const QVariant& value) { mOpiPrice = value; }

private:
    QVariant mId;
    QVariant mDest;
    QVariant mAddress;
    QVariant mMapX;
    QVariant mMapY;
    QVariant mDestType; // 0: unknown, 1: office, 2: home
    QVariant mHit;
    QVariant mPoiID;
    QVariant mNavSeq;
    QVariant mRpFlag;
    QVariant mOpiPoleCate;
    QVariant mOpiPrice;
#if 0
    QVariant mPoiID;
#endif
};

}
#endif // RECENTDESTINATIONDTO_H
