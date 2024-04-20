#include "navigationprofilepropertyobserver.h"
#include <QDebug>

namespace SKT {
static const char* PropName_ActiveProfile      = "activeProfile";
static const char* PropName_CopyProfile        = "copyProfile";
static const char* PropName_CreateProfile      = "createProfile";
static const char* PropName_DataChanged        = "dataChanged";
static const char* PropName_DeleteProfile      = "deleteProfile";
// static const char* PropName_ExternalIdentifier = "externalIdentifier"; // not support
static const char* PropName_ProfileImage       = "profileImage";
static const char* PropName_ProfileName        = "profileName";
static const char* PropName_Status             = "status";

NavigationProfilePropertyObserver::NavigationProfilePropertyObserver(QObject *parent)
    :QObject(parent)
    , m_activeProfileStatus(0)
    , m_createProfile(0)
    , m_dataChanged(0)
    , m_deleteProfile(0)
    , m_profileImage(0)
    , m_profileName("")
    , m_status(0)
{
    if (parent) {
        connect(parent, SIGNAL(propertyChanged(QString,QVariant)),
                this, SLOT(onPropertyChanged(QString,QVariant)));

        // // set initial value
        // m_activeProfileStatus   = qvariant_cast<uchar>(parent->property(PropName_ActiveProfile));
        // m_copyProfile           = qvariant_cast<copyInfo>(parent->property(PropName_CopyProfile));
        // m_createProfile         = qvariant_cast<uchar>(parent->property(PropName_CreateProfile));
        // m_dataChanged           = qvariant_cast<uint>(parent->property(PropName_DataChanged));
        // m_deleteProfile         = qvariant_cast<uchar>(parent->property(PropName_DeleteProfile));
        // m_profileImage          = qvariant_cast<uchar>(parent->property(PropName_ProfileImage));
        // m_profileName           = qvariant_cast<QString>(parent->property(PropName_ProfileName));
        // m_status                = qvariant_cast<uint>(parent->property(PropName_Status));
    }
}
void NavigationProfilePropertyObserver::onPropertyChanged(const QString &propertyName, const QVariant &value) {
#if 0 // Too verbose
    qDebug() << Q_FUNC_INFO << "name:" << propertyName << "value:" << value;
#endif
    if (propertyName == QLatin1String(PropName_ActiveProfile)) {
        uchar activeProfile = qvariant_cast<uchar>(value);
        if (m_activeProfileStatus != activeProfile) {
            m_activeProfileStatus = activeProfile;
            emit activeProfileChanged(m_activeProfileStatus);
        }
    } else if (propertyName == QLatin1String(PropName_CopyProfile)) {
        copyInfo ci = qvariant_cast<copyInfo>(value);
        if (m_copyProfile != ci) {
            m_copyProfile = ci;
            emit copyProfileChanged(ci.m_source, ci.m_destination);
        }
    } else if (propertyName == QLatin1String(PropName_CreateProfile)) {
        uchar createProfile = qvariant_cast<uchar>(value);
        if (m_createProfile != createProfile) {
            m_createProfile = createProfile;
            emit createProfileChanged(m_createProfile);
        }
    } else if (propertyName == QLatin1String(PropName_DataChanged)) {
        uint dataChanged = qvariant_cast<uint>(value);
        if (m_dataChanged != dataChanged) {
            m_dataChanged = dataChanged;
            emit profileDataChanged(m_dataChanged);
        }
    } else if (propertyName == QLatin1String(PropName_DeleteProfile)) {
        uchar deleteProfile = qvariant_cast<uchar>(value);
        if (m_deleteProfile != deleteProfile) {
            m_deleteProfile = deleteProfile;
            emit deleteProfileChanged(m_deleteProfile);
        }
    } else if (propertyName == QLatin1String(PropName_ProfileImage)) {
        uchar profileImage = qvariant_cast<uchar>(value);
        if (m_profileImage != profileImage) {
            m_profileImage = profileImage;
            emit profileImageChanged(m_profileImage);
        }
    } else if (propertyName == QLatin1String(PropName_ProfileName)) {
        QString profileName = qvariant_cast<QString>(value);
        if (m_profileName != profileName) {
            m_profileName = profileName;
            emit profileNameChanged(m_profileName);
        }
    } else if (propertyName == QLatin1String(PropName_Status)) {
        uint status = qvariant_cast<uint>(value);
        if (m_status != status) {
            m_status = status;
            emit statusChanged(m_status);
        }
    }
}
}
