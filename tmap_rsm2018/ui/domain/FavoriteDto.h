#ifndef FAVORITEDTO_H
#define FAVORITEDTO_H

namespace SKT {

T_SHAREABLE( FavoriteDto );

class FavoriteDto : public QObject
{
	Q_OBJECT
	Q_CLASSINFO( "table", "name=favorites" )
    Q_CLASSINFO( "column", "name=id,type=integer,pk=true,ai=true,not_null=true" )
	Q_CLASSINFO( "column", "name=dest,type=text" )
	Q_CLASSINFO( "column", "name=address,type=text" )
	Q_CLASSINFO( "column", "name=alias, type=text" )
	Q_CLASSINFO( "column", "name=mapX, type=integer" )
	Q_CLASSINFO( "column", "name=mapY, type=integer" )
	Q_CLASSINFO( "column", "name=destType, type=integer, default=0" ) // 0: unkown, 1: office, 2: home
	Q_CLASSINFO( "column", "name=poiID, type=integer" ) // POI ID
	Q_CLASSINFO( "column", "name=navSeq,type=integer" )
    Q_CLASSINFO( "column", "name=rpFlag,type=integer" )
    Q_CLASSINFO( "column", "name=insTime,type=text") // YYYYMMDDHH24MISS
    Q_CLASSINFO( "column", "name=opiPoleCate,type=integer")
    Q_CLASSINFO( "column", "name=opiPrice,type=integer" )

    Q_CLASSINFO( "index", "name=favorites_index,columns=dest|mapX|mapY" )

	Q_PROPERTY(QVariant id READ id WRITE setId)
	Q_PROPERTY(QVariant dest READ dest WRITE setDest)
	Q_PROPERTY(QVariant address READ address WRITE setAddress)
	Q_PROPERTY(QVariant alias READ alias WRITE setAlias)
	Q_PROPERTY(QVariant mapX READ mapX WRITE setMapX)
	Q_PROPERTY(QVariant mapY READ mapY WRITE setMapY)
	Q_PROPERTY(QVariant destType READ destType WRITE setDestType)
	Q_PROPERTY(QVariant poiID READ poiID WRITE setPoiID)
	Q_PROPERTY(QVariant navSeq READ navSeq WRITE setNavSeq)
	Q_PROPERTY(QVariant rpFlag READ rpFlag WRITE setRpFlag)
	Q_PROPERTY(QVariant insTime READ insTime WRITE setInsTime)
	Q_PROPERTY(QVariant opiPoleCate READ opiPoleCate WRITE setOpiPoleCate)
    Q_PROPERTY(QVariant opiPrice READ opiPrice WRITE setOpiPrice)

public:
    explicit FavoriteDto(QObject* parent=0);

    inline QVariant id() const { return mId; }
    inline void setId(const QVariant& value) { mId = value; }

	inline QVariant dest() const { return mDest; }
	inline void setDest(const QVariant& value) { mDest = value; }

	inline QVariant address() const { return mAddress; }
	inline void setAddress(const QVariant& value) { mAddress = value; }

	inline QVariant alias() const { return mAlias; }
	inline void setAlias(const QVariant& value) { mAlias = value; }

	inline QVariant mapX() const { return mMapX; }
	inline void setMapX(const QVariant& value) { mMapX = value; }

	inline QVariant mapY() const { return mMapY; }
	inline void setMapY(const QVariant& value) { mMapY = value; }

	inline QVariant destType() const { return mDestType; }
	inline void setDestType(const QVariant& value) { mDestType = value; }

	inline QVariant poiID() const { return mPoiID; }
	inline void setPoiID(const QVariant& value) { mPoiID = value; }

	inline QVariant navSeq() const { return mNavSeq;}
	inline void setNavSeq(const QVariant& value) { mNavSeq = value; }

	inline QVariant rpFlag() const { return mRpFlag; }
	inline void setRpFlag(const QVariant& value) { mRpFlag = value; }

	inline QVariant insTime() const { return mInsTime; }
	inline void setInsTime(const QVariant& value) { mInsTime = value; }

	inline QVariant opiPoleCate() const { return mOpiPoleCate; }
    inline void     setOpiPoleCate(const QVariant& value) { mOpiPoleCate = value; }

    inline QVariant opiPrice() const { return mOpiPrice; }
    inline void     setOpiPrice(const QVariant& value) { mOpiPrice = value; }

private:
    QVariant mId;
    QVariant mDest;
    QVariant mAddress;
    QVariant mAlias;
    QVariant mMapX;
    QVariant mMapY;
    QVariant mDestType; // 0: unknown, 1: office, 2: home
    QVariant mPoiID;
    QVariant mNavSeq;
    QVariant mRpFlag;
    QVariant mInsTime;
    QVariant mOpiPoleCate;
    QVariant mOpiPrice;
};

}
#endif // FAVORITEDTO_H
