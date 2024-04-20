#ifndef MANAGEPREFERENCECOMMAND_H
#define MANAGEPREFERENCECOMMAND_H

#include "SimpleCommand.h"
#include "PreferenceVo.h"

namespace SKT {

class ManagePreferenceCommand : public SimpleCommand
{
    Q_OBJECT
public:
    Q_INVOKABLE ManagePreferenceCommand( QObject* parent = 0 );

    ~ManagePreferenceCommand();

    bool execute( const mvc::INotificationPtr& note );

private:
    void setRgPreference(const QString &section, const QString &key, const QVariant &value, QMap<QString,PreferenceVo> &ret);
};

}

#endif // MANAGEPREFERENCECOMMAND_H
