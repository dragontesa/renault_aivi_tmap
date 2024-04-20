#ifndef TFIELDINFO_H
#define TFIELDINFO_H

namespace SKT { namespace orm {

class TMAP_RSM_UI_DLLSHARED_EXPORT TFieldInfo
{
public:
    TFieldInfo();

    ~TFieldInfo();

	QString name() const;

	void setName( const QString& name );

	QString typeName() const;

	void setTypeName( const QString& typeName );

	QVariant::Type type() const;

	void setType( QVariant::Type type );

	bool isIdentity() const;

	void setIdentity( bool identity );

	bool isForeign() const;

	void setForeign( bool foreign );

	bool isNotNull() const;

	void setNotNull( bool notnull);



	QString foreignTable() const;

	void setForeignTable( const QString& table );

	QString foreignColumn() const;

	void setForeignColumn( const QString& column );

	bool isFieldCreatable() const;

private:
	bool mIsIdentity;
	bool mIsForeign;
	bool mIsNotNull;
	QString mName;
	QString mTypeName;
	QVariant::Type mType;
	QString mForeignTable;
	QString mForeignColumn;
};

}}

#endif
