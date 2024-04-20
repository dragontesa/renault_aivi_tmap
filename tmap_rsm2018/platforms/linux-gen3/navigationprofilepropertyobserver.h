#ifndef NAVIGATIONPROFILEPROPERTYOBSERVER_H
#define NAVIGATIONPROFILEPROPERTYOBSERVER_H
#include <QObject>
#include "navigationexttype.h"

namespace SKT {

class NavigationProfilePropertyObserver :public QObject
{
    Q_OBJECT
    Q_PROPERTY(char     m_activeProfileStatus   READ activeProfile      NOTIFY activeProfileChanged)
    Q_PROPERTY(copyInfo m_copyProfile           READ copyProfile        NOTIFY copyProfileChanged)
    Q_PROPERTY(uchar    m_createProfile         READ createProfile      NOTIFY createProfileChanged)
    Q_PROPERTY(uint     m_dataChanged           READ dataChanged        NOTIFY profileDataChanged)
    Q_PROPERTY(uchar    m_deleteProfile         READ deleteProfile      NOTIFY deleteProfileChanged)
    Q_PROPERTY(uchar    m_profileImage          READ profileImage       NOTIFY profileImageChanged)
    Q_PROPERTY(QString  m_profileName           READ profileName        NOTIFY profileNameChanged)
    Q_PROPERTY(uint     m_status                READ status             NOTIFY statusChanged)

public:
    explicit NavigationProfilePropertyObserver(QObject *parent = 0);
    char activeProfile()        const { return m_activeProfileStatus; }
    copyInfo copyProfile()      const { return m_copyProfile; }
    uchar createProfile()       const { return m_createProfile; }
    uint dataChanged()          const { return m_dataChanged; }
    uchar deleteProfile()       const { return m_deleteProfile; }
    uchar profileImage()        const { return m_profileImage; }
    QString profileName()       const { return m_profileName; }
    uint status()               const { return m_status; }

signals:
    void activeProfileChanged(uchar activeProfileStatus);
    void copyProfileChanged(qint32 srcId, qint32 dstId);
    void createProfileChanged(uchar createProfileStatus);
    void profileDataChanged(uint dataChangedStatus);
    void deleteProfileChanged(uchar deleteProfileStatus);
    void externalIdentifierChanged(uint externalIdentifierStatus);
    void profileImageChanged(uchar profileImageStatus);
    void profileNameChanged(QString profileNameStatus);
    void statusChanged(uint profileStatus);

private slots:
    void onPropertyChanged(const QString &propertyName, const QVariant &value);
private:
        uchar       m_activeProfileStatus;
        copyInfo    m_copyProfile;
        uchar       m_createProfile;
        uint        m_dataChanged;
        uchar       m_deleteProfile;
        // uint        m_externalIdentifier; // not support
        uchar       m_profileImage;
        QString     m_profileName;
        uint        m_status;
};
}
#endif // NAVIGATIONPROFILEPROPERTYOBSERVER_H
