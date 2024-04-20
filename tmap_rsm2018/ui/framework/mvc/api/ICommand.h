#ifndef ICOMMAND_H
#define ICOMMAND_H

namespace SKT { namespace mvc {

class TMAP_RSM_UI_DLLSHARED_EXPORT ICommand
    : public virtual INotifier
{
public:
    virtual ~ICommand() {}

    virtual bool execute( const INotificationPtr& note ) = 0;
};

}}

#endif // ICOMMAND_H

