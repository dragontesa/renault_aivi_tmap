#ifndef TSELECTBUILDER_H
#define TSELECTBUILDER_H

#include "TStatementBuilder.h"

namespace SKT { namespace orm {

class TMAP_RSM_UI_DLLSHARED_EXPORT TSelectBuilder : public TStatementBuilder
{
public:
	TSelectBuilder();

	~TSelectBuilder();

	TSelectBuilder& table( const QString& table );

	TSelectBuilder& column( const QString& column );	

	TSelectBuilder& column( const TSelectBuilderPtr& sb, const QString column, const QString& asName );	

	TSelectBuilder& removeColumns();

	TSelectBuilder& top( int top );

	TSelectBuilder& limitN(int n);

	TSelectBuilder& limitOffset(int offset);

	TSelectBuilder& where( const QString& where );	

	TSelectBuilder& orderBy( const QString& orderBy );

	QString build() const;

	QString buildAsColumnSubQuery( const QString& column ) const;
	
private:
	void onComposeWith( const TTableInfo* tableInfo );

	QString mTable;
	QStringList mColumns;
	QVariant mTop;
	QString mWhere;
	QString mOrderBy;
	QVariant mLimitN; // for sqlite, mysql, postgre sql
	QVariant mLimitOffset;
};

}}

#endif
