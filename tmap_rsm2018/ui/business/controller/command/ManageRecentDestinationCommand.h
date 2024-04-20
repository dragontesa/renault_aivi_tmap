#ifndef RECENTDESTINATIONCOMMAND_H
#define RECENTDESTINATIONCOMMAND_H
#include "SimpleCommand.h"

namespace SKT
{

class ManageRecentDestinationCommand : public SimpleCommand
{
	Q_OBJECT
public:
    Q_INVOKABLE ManageRecentDestinationCommand(QObject* parent=0);

    bool execute( const mvc::INotificationPtr& note );
protected:
	void sendNaviPropertyChanged();
	void initNaviProperty(const mvc::ICommandPtr& command);
};

}
#endif // RECENTDESTINATIONCOMMAND_H
