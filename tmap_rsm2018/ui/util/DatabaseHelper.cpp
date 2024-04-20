#include "DatabaseHelper.h"
#include "TTableInfo.h"

using namespace SKT;
using namespace SKT::orm;

static const char* TM_USER_DB_ENTRIMAP[6] = {
        "tmap.db",
        "tmap_user1.db",
        "tmap_user2.db",
        "tmap_user3.db",
        "tmap_user4.db",
        "tmap_user5.db"
};

static const char* TmapUserTmpForCloud = "tmap_user_tmp_for_cloud.db";

class SqliteDriver : public orm::IDriver {
public:
    SqliteDriver()
    {

    }

    static SqliteDriver* instance()
    {
        static SqliteDriver Instance;

        return &Instance;
    }

    TTableInfo* buildTableInfo( const QMetaObject* mo )
    {
        TTableInfo* ret = new TTableInfo;

        QString name;
        QString value;
        QMap<QString,QString> attrMap;
        for ( int i = 0; i < mo->classInfoCount(); i++ ) {
            QMetaClassInfo mci = mo->classInfo( i );
            name = mci.name();
            value = mci.value();

            attrMap.clear();
            auto attrs = value.split( QStringLiteral(",") );
            for ( auto attr : attrs ) {
                auto keyVal = attr.trimmed().split( QStringLiteral("=") );
                attrMap[keyVal[0].trimmed()] = keyVal[1].trimmed();
            }

            if ( name.toUpper() == QStringLiteral("table").toUpper() ) {
                ret->setTableName( attrMap[QStringLiteral("name")] );
            }
            else if ( name.toUpper() == QStringLiteral("column").toUpper() ) {
                QVariant::Type varType;

                auto colName = attrMap[QStringLiteral("name")];
                auto colType = attrMap[QStringLiteral("type")].toUpper();

                if ( colType == QStringLiteral("integer").toUpper() ) {
                    varType = QVariant::Int;
                }
                else if ( colType == QStringLiteral("text").toUpper() ) {
                    varType = QVariant::String;
                }
                else if ( colType == QStringLiteral("blob").toUpper() ) {
                    varType = QVariant::ByteArray;
                }
                else if ( colType == QStringLiteral("real").toUpper() ) {
                    varType = QVariant::Double;
                }
                else if ( colType == QStringLiteral("numeric").toUpper() ) {
                }

                bool notNull = false;
                bool pk = false;
                bool ai = false;
                bool unique = false;

                auto attrVal = attrMap[QStringLiteral("not_null")];
                if ( !attrVal.isEmpty() ) {
                    notNull = attrVal.toLower() == QStringLiteral("true") ? true : false;
                }
                attrVal = attrMap[QStringLiteral("pk")];
                if ( !attrVal.isEmpty() ) {
                    pk = attrVal.toLower() == QStringLiteral("true") ? true : false;
                }
                attrVal = attrMap[QStringLiteral("ai")];
                if ( !attrVal.isEmpty() ) {
                    ai = attrVal.toLower() == QStringLiteral("true") ? true : false;
                }
                attrVal = attrMap[QStringLiteral("unique")];
                if ( !attrVal.isEmpty() ) {
                    unique = attrVal.toLower() == QStringLiteral("true") ? true : false;
                }

                auto fi = new TFieldInfo;
                fi->setName( colName );
                fi->setTypeName( colType );
                fi->setType( varType );
                fi->setIdentity( pk );

                ret->addFieldInfo( fi );
            }
        }

        return ret;
    }

    void createTable( const QSqlDatabase& db, const QMetaObject* mo )
    {
        QString sql;

        struct ColumnInfo {
            QString name;
            QString type;
            QString fk;
            QString refTable;
            QString refCol;
            QString onDelete;
            QString onUpdate;
            QString onConflict;
            bool notNull = false;
            bool pk = false;
            bool ai = false;
            bool unique = false;
        };

        QString tableName;
        QList<ColumnInfo> columnInfoList;

        struct UniqueIndexInfo {
            QString name;
            QString columns;
        };

        QList<UniqueIndexInfo> uniqueIndexList;

        typedef UniqueIndexInfo IndexInfo;
        QList<IndexInfo> indexList;

        QString name;
        QString value;
        QMap<QString,QString> attrMap;
        for ( int i = 0; i < mo->classInfoCount(); i++ ) {
            QMetaClassInfo mci = mo->classInfo( i );
            name = mci.name();
            value = mci.value();

            attrMap.clear();
            auto attrs = value.split( QStringLiteral(",") );
            for ( auto attr : attrs ) {
                auto keyVal = attr.split( QStringLiteral("=") );
                if ( keyVal.count() > 1 ) {
                    attrMap[keyVal[0]] = keyVal[1];
                }
            }

            if ( name == QStringLiteral("table") ) {
                tableName = attrMap[QStringLiteral("name")];
            }
            else if ( name == QStringLiteral("column") ) {
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

                columnInfoList.append( ci );
            }
            else if ( name == QStringLiteral("unique_index") ) {
                auto index = value.indexOf( QStringLiteral("columns") );
                if ( index > -1 ) {
                    UniqueIndexInfo uiInfo;
                    uiInfo.name = attrMap[QStringLiteral("name")];

                    uiInfo.columns = attrMap[QStringLiteral("columns")];
                    uiInfo.columns.replace( QChar('|'), QChar(',') );

                    uniqueIndexList.append( uiInfo );
                }
            }
            else if ( name == QStringLiteral("index") ) {
                auto index = value.indexOf( QStringLiteral("columns") );
                if ( index > -1 ) {
                    IndexInfo iInfo;
                    iInfo.name = attrMap[QStringLiteral("name")];

                    iInfo.columns = attrMap[QStringLiteral("columns")];
                    iInfo.columns.replace( QChar('|'), QChar(',') );

                    indexList.append( iInfo );
                }
            }

        }

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

        sql.append( QStringLiteral(");") );


        bool create = false;
        QSqlQuery qry( db );
        qry.prepare( QString( QStringLiteral("SELECT * FROM sqlite_master WHERE name='%1'") ).arg( tableName ) );
        qry.exec();
        if ( !qry.next() ) {
            create = true;
        }

        //T_DEBUG( sql );

        if ( create ) {
            T_DEBUG(sql);

            db.exec( sql );

            //T_DEBUG( tableName );

            for ( auto index : uniqueIndexList ) {
                auto sql = QString( QStringLiteral("CREATE UNIQUE INDEX IF NOT EXISTS %1 ON %2 (%3)") ).
                    arg( index.name ).
                    arg( tableName ).
                    arg( index.columns );

                //T_DEBUG( sql );

                db.exec( sql );
            }


            for ( auto index : indexList ) {
                auto sql = QString( QStringLiteral("CREATE INDEX IF NOT EXISTS %1 ON %2 (%3)") ).
                    arg( index.name ).
                    arg( tableName ).
                    arg( index.columns );

                //T_DEBUG( sql );

                db.exec( sql );
            }
        }
    }

    QString tableSchemaInfo(QSqlDatabase db, const QString& tableName) const {
        QString schema;
        QString sql(QStringLiteral("SELECT sql FROM sqlite_master WHERE type='table' AND tbl_name='%1'").arg(tableName));

        QSqlQuery q(db);
        if (!q.exec(sql)) {
            T_DEBUG("failed to query table schema info:"+q.lastError().text());
            return QString();
        }
        else {
            if (q.next()) {
                schema = q.value(0).toString();
            }
        }

        return schema;
    }

    QString indexSchemaInfo( QSqlDatabase db, const QString& indexName) const {
        QString schema;
        QString sql(QStringLiteral("SELECT sql FROM sqlite_master WEHRE type='index' AND name='%1'").arg(indexName));

        QSqlQuery q(db);
        if (!q.exec(sql)) {
            T_DEBUG("failed to query table schema info:"+q.lastError().text());
            return QString();
        }
        else {
            if (q.next()) {
                schema = q.value(0).toString();
            }
        }

        return schema;
    }

    virtual QStringList tableColumnList( QSqlDatabase db, const QString& tableName) const {
        QStringList colList;
        QSqlQuery q(db);
        QString sql(QStringLiteral("PRAGMA table_info(%1)").arg(tableName));
        if (!q.exec(sql)) {
            T_DEBUG("failed to enumerate table column:"+q.lastError().text());
            T_DEBUG(sql);
            return QStringList();
        }

        while (q.next()) {
            colList.append(q.value(1).toString());
        }

        return colList;
    }
};

using namespace SKT;

DatabaseHelper::DatabaseHelper():
        mCurrentUserProfileId(0) // default profile id is guest(0),
       ,mUsingCloudByOtp(false)
{

}

DatabaseHelper::~DatabaseHelper()
{
    disconnectForCloud();
}

DatabaseHelper* DatabaseHelper::instance()
{
    static DatabaseHelper* Instance = new DatabaseHelper;

    return Instance;
}

void DatabaseHelper::initialize( const QString& dirPath )
{
    mDirPath = dirPath;

    // sqlite
    orm::TOrm::instance()->setDriver( SqliteDriver::instance() );
}

void DatabaseHelper::terminate()
{
    for ( auto connName : mConnectionList )
    {
        QSqlDatabase::removeDatabase(connName);
    }

    auto connNames = QSqlDatabase::connectionNames();
    for ( auto connName : connNames ) {
        if (  connName.indexOf( QStringLiteral("crossimage_") ) > -1 ) {
            QSqlDatabase::removeDatabase( connName );
        }
    }

    delete this;
}

/** this version see current activated user's db and put the connection.
 * Note! most stuff on ui views should use it to connect every DB.
 * @brief DatabaseHelper::connection
 * @return
 */
QSqlDatabase DatabaseHelper::connection()
{
    if (mUsingCloudByOtp)
        return connection(QString(TmapUserTmpForCloud));
    return connection(QString(TM_USER_DB_ENTRIMAP[mCurrentUserProfileId%6]));
}


/** This is used to handle user profile by Profile Manager
 * @brief DatabaseHelper::connection
 * @param userProfileId
 * @return
 */
QSqlDatabase DatabaseHelper::connection(int userProfileId)
{
    return connection(QString(TM_USER_DB_ENTRIMAP[userProfileId%6]));
}

/** get the connection current user's local db
 *
 */
QSqlDatabase DatabaseHelper::connectionOfCurrentUser()
{
    return connection(QString(TM_USER_DB_ENTRIMAP[mCurrentUserProfileId%6]));
}

QList<QSqlDatabase> DatabaseHelper::connectableConnections()
{
    QList<QSqlDatabase> cons;
    // DB 순서 중요!, 로컬 DB가 우선되면 서버 동기화때 로컬에서 중복체크가 되서 동기화 DB에 저장이 안된다
    if (mUsingCloudByOtp)
        cons << connection(QString(TmapUserTmpForCloud));
    else // 2020.2.6 동기화 사용자의 데이터는 절대 로컬 유저와 공유되면 안된다!! TMCRSM-3025
        cons << connection(QString(TM_USER_DB_ENTRIMAP[mCurrentUserProfileId%6]));

    return cons;
}

QSqlDatabase DatabaseHelper::connection(const QString& dbFileName)
{
    if (dbFileName.isEmpty()) return QSqlDatabase(); // return empty db

    QSqlDatabase conn;
    auto connName = QString( QStringLiteral("%1_%2") ).arg(dbFileName).arg(reinterpret_cast<std::uintptr_t>(QThread::currentThreadId()) );

    if ( !QSqlDatabase::contains( connName ) ) {
        conn = QSqlDatabase::addDatabase( QStringLiteral("QSQLITE"), connName );
        conn.setDatabaseName( mDirPath + QChar('/') + dbFileName);
        T_DEBUG( QString( QStringLiteral("Created database connection '%1'") ).arg( connName ) );
    }
    else {
        conn = QSqlDatabase::database( connName, false );
    }

    if ( !conn.isOpen() ) {
        T_DEBUG( QString( "Open database '%1'" ).arg( connName ) );
        conn.open();
    }
    if ( !conn.isOpen() ) {
        auto lastError = conn.lastError().text();
        T_DEBUG( QString( "<B>Could not open database(%1) Error:%2</B>" ).arg( connName ).arg( lastError ) );
    }

    if (! mConnectionList.contains(connName))
        mConnectionList.append(connName);
    return conn;
}

void DatabaseHelper::disconnect(const QString& dbName)
{
    QString connName =  QString( QStringLiteral("%1_%2") ).arg(dbName).arg( reinterpret_cast<std::uintptr_t>(QThread::currentThreadId()) );
    {
        QSqlDatabase conn;
        if ( !QSqlDatabase::contains( connName ) ) {
            conn = QSqlDatabase::addDatabase( QStringLiteral("QSQLITE"), connName );
            conn.setDatabaseName( mDirPath + QChar('/') + dbName);
            // TODO SET TIMEOUT
            T_DEBUG( QString( QStringLiteral("Created database connection '%1'") ).arg( connName ) );
        }
        else {
            conn = QSqlDatabase::database( connName, false );
        }

        if ( conn.isOpen()) conn.close();
    }

    QSqlDatabase::removeDatabase(connName);
}

void DatabaseHelper::disconnect(int userProfileId)
{
    QString dbName = QString(TM_USER_DB_ENTRIMAP[userProfileId%6]);
    disconnect(dbName);
}

/** this puts connection for the tmap user who gets authorized by opt
 *  the db lives just until the authenticated session has expired
 *  and all user db on local get shadowed at that time.
 * @brief DatabaseHelper::connectionForCloud
 * @return
 */
QSqlDatabase DatabaseHelper::connectionForCloud()
{
    mUsingCloudByOtp = true;
    return connection(QString(TmapUserTmpForCloud));
}

void DatabaseHelper::disconnectForCloud()
{
    mUsingCloudByOtp = false;
    mIsSessionConnected = false;
    QString dbName(TmapUserTmpForCloud);
    disconnect(dbName);
    // delete tmp db
    QString tmpPath = mDirPath + QChar('/') + dbName;
    if (!QFile::remove(tmpPath)) {
        T_DEBUG(QStringLiteral("failure to delete the use db file :%1").arg(tmpPath));
    }
}

void DatabaseHelper::setCurrentUserProfileId(int profileId)
{
     mCurrentUserProfileId = profileId;
        // hold current user id on offline
     QString lastFilePath(getLastUserFilePath());
    QFile fl(lastFilePath);

    if (!fl.open(QIODevice::WriteOnly)) {
        T_DEBUG( QStringLiteral("failed to set last user:").arg(profileId));
        return;
    }

    QTextStream ts(&fl);
    ts << profileId;
}

QString DatabaseHelper::getUserDBFilePath(int profileId) const
{
    return mDirPath + QChar('/') + QStringLiteral("%1").arg(TM_USER_DB_ENTRIMAP[profileId%6]);
}


IDriver& DatabaseHelper::getDriver() const
{
    return *SqliteDriver::instance();
}

bool DatabaseHelper::isCloudDb() const
{
    return mUsingCloudByOtp || mIsSessionConnected;
}

void DatabaseHelper::setCloudDb()
{
    mIsSessionConnected = true;
}


void DatabaseHelper::deleteAllUserDB()
{
    QDir dbentry(mDirPath);
    if (!dbentry.exists()) {
        T_DEBUG( QStringLiteral("failed delete all db, not found such dir:").arg(mDirPath));
        return;
    }


    // disconnect every profile db
    for (int i=0;i<6;i++)
         disconnect(i);

    QStringList filters;
    filters<<"*.db";
    auto files = dbentry.entryInfoList(filters, QDir::Files|QDir::Readable);
    for (auto fi : files) {
        QFile::setPermissions( fi.filePath(), fi.permissions()|QFileDevice::WriteOwner );
        if (!QFile::remove(fi.filePath()) ) {
            T_DEBUG(QStringLiteral("failure to delete the use db file :%1").arg(fi.filePath()));
            return;
        }
    }
}
