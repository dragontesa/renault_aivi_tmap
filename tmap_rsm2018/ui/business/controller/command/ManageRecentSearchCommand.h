#ifndef RECENTSEARCHCOMMAND_H
#define RECENTSEARCHCOMMAND_H

#include "SimpleCommand.h"

namespace SKT {

class ManageRecentSearchCommand : public SimpleCommand
{
	Q_OBJECT
public:
    Q_INVOKABLE ManageRecentSearchCommand(QObject* parent=0);

    bool execute( const mvc::INotificationPtr& note );

protected:
    void sendNaviPropertyChanged();
};

}
#endif // RECENTSEARCHCOMMAND_H
