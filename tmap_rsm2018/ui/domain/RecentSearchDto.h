#ifndef RECENTSEARCHDTO_H
#define RECENTSEARCHDTO_H

namespace SKT {

T_SHAREABLE( RecentSearchDto );

class RecentSearchDto : public QObject
{
    Q_OBJECT
	Q_CLASSINFO( "table", "name=recent_search" )
    Q_CLASSINFO( "column", "name=id,type=integer,pk=true,ai=true,not_null=true" )
	Q_CLASSINFO( "column", "name=keyword,type=text" )
	Q_CLASSINFO( "column", "name=accessTime,type=text" )
    Q_CLASSINFO( "index",  "name=recent_search_index,columns=keyword|accessTime" )

public:
    explicit RecentSearchDto(QObject *parent = 0);
    Q_PROPERTY(QVariant id READ id WRITE setId)
    Q_PROPERTY(QVariant keyword READ keyword WRITE setKeyword)
	Q_PROPERTY(QVariant accessTime READ accessTime WRITE setAccessTime)

	inline QVariant id() const { return mId; }
    inline void setId(const QVariant& value) { mId = value; }

    inline QVariant keyword() const { return mKeyword; }
    inline void setKeyword(const QVariant& value) { mKeyword = value; }

    inline QVariant accessTime() const { return mAccessTime; }
    inline void setAccessTime(const QVariant& value) { mAccessTime = value; }


    private:
        QVariant mId;
        QVariant mKeyword;
        QVariant mAccessTime;
};

}
#endif // RECENTSEARCHDTO_H
