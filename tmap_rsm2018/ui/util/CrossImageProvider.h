#ifndef CROSSIMAGEPROVIDER_H
#define CROSSIMAGEPROVIDER_H

#include <QByteArray>
#include <QSqlDatabase>

namespace SKT {

class CrossImageProvider
{
public:
    static CrossImageProvider* instance();

    void initialize(const QString &dbPath);

    QByteArray getImage(quint32 id, bool day = true);

private:
    CrossImageProvider();
    ~CrossImageProvider();

    QSqlDatabase database(bool day);

    QString mDbPath;
};

}

#endif // CROSSIMAGEPROVIDER_H
