#ifndef TMACROCOMMAND_H
#define TMACROCOMMAND_H

namespace SKT { namespace mvc {

class TMAP_RSM_UI_DLLSHARED_EXPORT TMacroCommand
    : public ICommand
{
public:
    TMacroCommand();

    virtual ~TMacroCommand();

public:
    void addSubCommand( const QMetaObject* commandClassRef );

    bool execute( const INotificationPtr& note );

private:
    QList<const QMetaObject*> mSubCommands;
};

}}

#endif // TMACROCOMMAND_H

