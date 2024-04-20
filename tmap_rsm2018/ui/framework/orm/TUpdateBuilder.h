#ifndef TUPDATEBUILDER_H
#define TUPDATEBUILDER_H

#include "TStatementBuilder.h"

namespace SKT { namespace orm {

class TMAP_RSM_UI_DLLSHARED_EXPORT TUpdateBuilder : public TStatementBuilder
{
public:
	TUpdateBuilder();

	~TUpdateBuilder();

	TUpdateBuilder& table( const QString& table );

	TUpdateBuilder& column( const QString& column );

	TUpdateBuilder& where( const QString& where );

	QString build() const;

private:
	void onComposeWith( const TTableInfo* tableInfo );

	void onComposeWith( const QObject& dto );

	QString mTable;
	QStringList mColumns;
	QString mWhere;
};

}}

#endif
