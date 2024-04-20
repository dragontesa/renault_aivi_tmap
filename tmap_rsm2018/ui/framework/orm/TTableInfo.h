#ifndef TTABLEINFO_H
#define TTABLEINFO_H

#include "TFieldInfo.h"

namespace SKT { namespace orm {

class TMAP_RSM_UI_DLLSHARED_EXPORT TTableInfo
{
public:
	TTableInfo();

	~TTableInfo();

	QString tableName() const;

	void setTableName( const QString& tableName );

	const TFieldInfo* idField() const;

	const QList<TFieldInfoPtr>& fieldInfos() const;

	void addFieldInfo( TFieldInfo* fieldInfo );

	const TFieldInfo* getFieldInfoByColumnName( const QString& columnName ) const;	

	// 입력으로 들어 온 foreign 필드와 현재 테이블의 id 필드가 일치하는가
	bool isForeignIdField( const TFieldInfo* foreignField ) const;

private:
	QString mTableName;
	QList<TFieldInfoPtr> mFieldInfos;
	QMap<QString,const TFieldInfo*> mFieldNameMap;
	const TFieldInfo* mIdField;
};

}}

#endif
