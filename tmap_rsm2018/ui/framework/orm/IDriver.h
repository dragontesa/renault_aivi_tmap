#ifndef IDRIVER
#define IDRIVER

namespace SKT { namespace orm {

class TMAP_RSM_UI_DLLSHARED_EXPORT IDriver
{
public:
    virtual ~IDriver() {}

    virtual TTableInfo* buildTableInfo( const QMetaObject* mo ) = 0;

    virtual void createTable( const QSqlDatabase& db, const QMetaObject* mo ) = 0;

    virtual QString tableSchemaInfo( QSqlDatabase db, const QString& tableNamem) const = 0;

    virtual QString indexSchemaInfo( QSqlDatabase db, const QString& indexName) const = 0;

    virtual QStringList tableColumnList( QSqlDatabase db, const QString& tableName) const = 0;


};

}}

#endif // IDRIVER

