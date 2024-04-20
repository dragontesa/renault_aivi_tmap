#ifndef TDELETEBUILDER_H
#define TDELETEBUILDER_H

#include "TStatementBuilder.h"

namespace SKT { namespace orm {

class TMAP_RSM_UI_DLLSHARED_EXPORT TDeleteBuilder : public TStatementBuilder
{
public:
	TDeleteBuilder();

	~TDeleteBuilder();

	TDeleteBuilder& table( const QString& table );

	TDeleteBuilder& column( const QString& column );

	TDeleteBuilder& where( const QString& where );

	QString build() const;

private:
	void onComposeWith( const TTableInfo* tableInfo );

	QString mTable;
	QString mWhere;
};

}}

#endif
