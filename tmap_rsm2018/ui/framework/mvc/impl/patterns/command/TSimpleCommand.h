#ifndef TSIMPLECOMMAND_H
#define TSIMPLECOMMAND_H

namespace SKT { namespace mvc {

class TMAP_RSM_UI_DLLSHARED_EXPORT TSimpleCommand
    : public ICommand
{
public:
    TSimpleCommand();

    virtual ~TSimpleCommand();

    virtual bool execute( const INotificationPtr& note );
};

}}

#endif // TSIMPLECOMMAND_H

