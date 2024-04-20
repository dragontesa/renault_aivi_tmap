#include "DatabaseMigration.h"

namespace SKT {
    using namespace orm;
DatabaseMigration::DatabaseMigration(QSqlDatabase conDB):
		mConDB(conDB)
{
}

DatabaseMigration::~DatabaseMigration()
{

}

void DatabaseMigration::migrateDatabase(QSqlDatabase dst)
{

}

void DatabaseMigration::migrateTable(
		const IDriver& driver,
		const QMetaObject& dto
)
{
	MigrateWorker* worker = new MigrateWorker(
			MigrationJobType::TB2TB,
			mConDB,
			driver,
			dto);

	worker->start();
}

DatabaseMigration::MigrateTableJob::MigrateTableJob(QSqlDatabase conDB, const IDriver& driver, const QMetaObject& dto):
   mConDB(conDB),
   mDriver(driver),
   mDto(dto)
 {
 }

 QString DatabaseMigration::MigrateTableJob::getNewTableName() const
 {
    QString tableName;
    const QMetaObject& dto = mDto;
    for ( int i=0; i< dto.classInfoCount(); ++i ) {
        QMetaClassInfo mci = dto.classInfo(i);
        QString name(mci.name());
        QString value(mci.value());
        if ( name == QStringLiteral("table") ) {
            QStringList keyval = value.split('=');
            if ( keyval.size() > 1 ) {
                tableName = keyval[1];
            }
        }
    }
    return tableName;
}

QStringList DatabaseMigration::MigrateTableJob::getNewColumnNameList() const
{
    QStringList colNameList;
    const QMetaObject& dto = mDto;
    for ( int i=0; i< dto.classInfoCount(); ++i ) {
        QMetaClassInfo mci = dto.classInfo(i);
        QString name(mci.name());
        QString value(mci.value());
        QMap<QString, QString> attrMap;
        if (name == QStringLiteral("column")) {
            QStringList attrs = value.split(',');
            for ( auto attr: attrs ) {
                QStringList keyval = attr.split('=');
                if ( keyval.size() > 1 ) {
                    attrMap[keyval[0]] = keyval[1];
                }
            }
            colNameList.append(attrMap[QStringLiteral("name")]);
        }
    }
    return colNameList;
}

QList<DatabaseMigration::ColumnInfo> DatabaseMigration::MigrateTableJob::getNewTableColumnList() const
{
    QList<ColumnInfo> colinfoList;
    const QMetaObject& dto = mDto;
    for ( int i=0; i< dto.classInfoCount(); ++i ) {
        QMetaClassInfo mci = dto.classInfo(i);
        QString name(mci.name());
        QString value(mci.value());
        QMap<QString, QString> attrMap;
        if (name == QStringLiteral("column")) {
            QStringList attrs = value.split(',');
            for ( auto attr: attrs ) {
                QStringList keyval = attr.split('=');
                if ( keyval.size() > 1 ) {
                    attrMap[keyval[0]] = keyval[1];
                }
            }
            ColumnInfo ci;
            ci.name = attrMap[QStringLiteral("name")];
            ci.type = attrMap[QStringLiteral("type")].toUpper();

            auto notNull = attrMap[QStringLiteral("not_null")];
            if ( !notNull.isEmpty() ) {
                ci.notNull = notNull.toLower() == QStringLiteral("true");
            }
            auto pk = attrMap[QStringLiteral("pk")];
            if ( !pk.isEmpty() ) {
                ci.pk = pk.toLower() == QStringLiteral("true");
            }
            auto fk = attrMap[QStringLiteral("fk")];
            auto reftbl = attrMap[QStringLiteral("reftable")];
            auto refcol = attrMap[QStringLiteral("refcol")];
            if ( !fk.isEmpty() &&
              !reftbl.isEmpty() &&
              !refcol.isEmpty()) {
                ci.fk = fk;
                ci.refTable = reftbl;
                ci.refCol = refcol;
            }
            
        auto ai = attrMap[QStringLiteral("ai")];
        if ( !ai.isEmpty() ) {
            ci.ai = pk.toLower() == QStringLiteral("true");
        }
        auto unique = attrMap[QStringLiteral("unique")];
        if ( !unique.isEmpty() ) {
            ci.unique = unique.toLower() == QStringLiteral("true");
        }

        auto ondel = attrMap[QStringLiteral("ondelete")];
        if ( !ondel.isEmpty()) {
            ci.onDelete = ondel; // you can use two behaves like "RESTRICT" and "CASCADE"
        }

        auto onupdate = attrMap[QStringLiteral("onupdate")];
        if ( !onupdate.isEmpty() ){
            ci.onUpdate = onupdate;  // you can use two behaves like "RESTRICT" and "CASCADE"
        }

        auto onconflict = attrMap[QStringLiteral("onconflict")];
        if ( !onconflict.isEmpty()) {
            ci.onConflict = onconflict; // you can use like "REPLACE" or "ROLLBACK" 
        }

        colinfoList.append( ci );
        }
    }
    return colinfoList;
}

QList<DatabaseMigration::IndexInfo> 
DatabaseMigration::MigrateTableJob::getNewIndexList() const
{
    QList<DatabaseMigration::IndexInfo> indexList;
    const QMetaObject& dto = mDto;
    for ( int i = 0; i < dto.classInfoCount(); i++ ) {
     QMetaClassInfo mci = dto.classInfo( i );
     QString name = mci.name();
     QString value = mci.value();

    QMap<QString,QString> attrMap;
     auto attrs = value.split( QStringLiteral(",") );
     for ( auto attr : attrs ) {
         auto keyVal = attr.split( QStringLiteral("=") );
         if ( keyVal.count() > 1 ) {
             attrMap[keyVal[0]] = keyVal[1];
         }
     }


     if ( name == QStringLiteral("index") ) {
         auto index = value.indexOf( QStringLiteral("columns") );
         if ( index > -1 ) {
             IndexInfo iInfo;
             iInfo.name = attrMap[QStringLiteral("name")];
             iInfo.columns = attrMap[QStringLiteral("columns")];
             iInfo.columns.replace( QChar('|'), QChar(',') );
             indexList.append( iInfo );
         }
     }
     else if ( name == QStringLiteral("unique_index") ) {
         auto index = value.indexOf( QStringLiteral("columns") );
         if ( index > -1 ) {
             IndexInfo uiInfo;
             uiInfo.name = attrMap[QStringLiteral("name")];
             uiInfo.columns = attrMap[QStringLiteral("columns")];
             uiInfo.columns.replace( QChar('|'), QChar(',') );
             uiInfo.unique = true;

             indexList.append( uiInfo );
         }
     }

 }
        
    return indexList;
}



QString DatabaseMigration::MigrateTableJob::buildTableSchema()
{
	QString sql;
	const QMetaObject& dto = mDto;

	QString tableName = getNewTableName();
	QList<ColumnInfo> columnInfoList = getNewTableColumnList();

	sql.append( QString( QStringLiteral("CREATE TABLE %1 (") ).arg( tableName ) );
	sql.append( QStringLiteral("\n") );

	for ( int i = 0, count = columnInfoList.count(); i < count; i++  ) {
		auto ci = columnInfoList[i];
		QString col;
		col.append( QStringLiteral("\t") );
		col.append( ci.name );
		col.append( QStringLiteral("\t") );
		col.append( ci.type );
		col.append( QStringLiteral("\t") );
		if ( ci.pk ) {
			col.append( QStringLiteral("PRIMARY KEY") );
			col.append( QStringLiteral("\t") );
		}
		if ( !ci.fk.isEmpty() ) {
			col.append( QStringLiteral("REFERENCES %1(%2)").arg(ci.refTable).arg(ci.refCol));
		}
		if ( ci.ai ) {
			col.append( QStringLiteral("AUTOINCREMENT") );
			col.append( QStringLiteral("\t") );
		}
		if ( ci.notNull ) {
			col.append( QStringLiteral("NOT NULL") );
			col.append( QStringLiteral("\t") );
		}
		if ( ci.unique ) {
			col.append( QStringLiteral("UNIQUE") );
		}
		if ( !ci.onDelete.isEmpty() ) {
			col.append( QStringLiteral("ON DELETE %1").arg(ci.onDelete));
		}
		if ( !ci.onDelete.isEmpty() ) {
			col.append( QStringLiteral("ON UPDATE %1").arg(ci.onUpdate));
		}

		if ( !ci.onConflict.isEmpty() ) {
			col.append( QStringLiteral("ON CONFLICT %1").arg(ci.onConflict));
		}


		sql.append( col );

		if ( i != count - 1 ) {
			sql.append( QStringLiteral(",") );
		}
		sql.append( QStringLiteral("\n") );
	}

	sql.append( QStringLiteral(")") );


	return sql;
}

 QStringList DatabaseMigration::MigrateTableJob::buildIndexSchema()
 {
       QStringList schemaList;
       const QMetaObject& dto = mDto;

       QString tableName = getNewTableName();
       QList<DatabaseMigration::IndexInfo> indexList = getNewIndexList();


        for ( auto index : indexList ) {
        	QString sql;
            if (index.unique)
               sql = QString( QStringLiteral("CREATE UNIQUE INDEX IF NOT EXISTS %1 ON %2 (%3)") ).
               arg( index.name ).
               arg( tableName ).
               arg( index.columns );
           else
               sql = QString( QStringLiteral("CREATE INDEX IF NOT EXISTS %1 ON %2 (%3)") ).
               arg( index.name ).
               arg( tableName ).
               arg( index.columns );
                
           schemaList.append(sql);
       }

    return schemaList;
}

void DatabaseMigration::MigrateDbJob::runJob(
	QSqlDatabase src, QSqlDatabase dst
	)
{
	// TODO:
}


void DatabaseMigration::MigrateTableJob::runJob()
{
    QSqlDatabase db(mConDB);
    const IDriver& driver(mDriver);

    QString dstTblName = getNewTableName();
    QString newSchema = buildTableSchema();
    QString oldSchema = driver.tableSchemaInfo(db,dstTblName);
    QString newColDefs = newSchema.mid(newSchema.indexOf('(')).trimmed();
    QString oldColDefs = oldSchema.mid(oldSchema.indexOf('(')).trimmed();
    bool isNeed =  (newColDefs != oldColDefs);
    if (!isNeed) return;


    // enumerate column 
    QStringList newColList, oldColList, compatibleList;
    newColList = getNewColumnNameList();
    oldColList = driver.tableColumnList(db,dstTblName);

    for (auto n1: newColList) {
        for (auto n2: oldColList) {
            if (n1 == n2 && n1 != "id") {
                compatibleList.append(n1);
                break;
            }
        }
    }

    // create new schema's table
    QString sql1(QStringLiteral("CREATE TABLE IF NOT EXISTS new_%1 %2").arg(dstTblName).arg(newColDefs));
    QSqlQuery qry(db);
    if (!qry.exec(sql1)) {
        T_DEBUG("failed to create migration table:"+qry.lastError().text());
        T_DEBUG(sql1);
        return;
    }

    // copy data from original table if has compatible type
    if (compatibleList.size() > 0) {
    	QString cols = compatibleList.join(',');
    	QString sql(QStringLiteral("INSERT INTO new_%1 (%2) SELECT %3 FROM %4").arg(dstTblName).arg(cols).arg(cols).arg(dstTblName));

    	if (!qry.exec(sql)) {
    		T_DEBUG("failed to copy data from old table:"+qry.lastError().text());
            T_DEBUG(sql);

            // rollback
            qry.exec(QStringLiteral("DROP TABLE new_%1").arg(dstTblName));
            return;
        }

    }

    // drop old table
    QString sql4(QStringLiteral("DROP TABLE %1").arg(dstTblName));
    if (!qry.exec(sql4)) {
    	T_DEBUG("failed to drop old table:"+qry.lastError().text());
        T_DEBUG(sql4);
        return;
    }

    // rename new table's name to original's one 
    QString sql5(QStringLiteral("ALTER TABLE new_%1 RENAME TO %2").arg(dstTblName).arg(dstTblName));
    if (!qry.exec(sql5)) {
        T_DEBUG("failed to drop old table:"+qry.lastError().text());
        T_DEBUG(sql5);
        return;
    }

    qry.finish();
}

DatabaseMigration::MigrateWorker::MigrateWorker(
  MigrationJobType jobType,
  QSqlDatabase db,
  const IDriver& driver,
  const QMetaObject& dto) :
mJobType(jobType),
mWorkDB(db),
mWorkDriver(driver),
mWorkDto(dto)
{
	connect(this,SIGNAL(finished()),this,SLOT(deleteLater()));
}


void DatabaseMigration::MigrateWorker::run()
{
	switch (mJobType) {
		case MigrationJobType::TB2TB:
		{
			DatabaseMigration::MigrateTableJob job(mWorkDB,mWorkDriver,mWorkDto);
			job.runJob();
		}
		break;
		case MigrationJobType::DB2DB: {
            DatabaseMigration::MigrateDbJob job;
            // job.runJob();
        }

        break;
        default : break;
    }
}

}
