#ifndef MANAGESETTINGCOMMAND_H
#define MANAGESETTINGCOMMAND_H

#include "SimpleCommand.h"

namespace SKT {

class ManageSettingCommand : public SimpleCommand
{
    Q_OBJECT
public:
    Q_INVOKABLE ManageSettingCommand( QObject* parent = 0 );

    bool execute( const mvc::INotificationPtr& note );
    void saveSetting(const QVariantMap &data);
    void saveHiddenSetting(const QVariantMap& data);
    void initSettings();
    void initDefaultSettings();    
    void initEtcSettings();
    void resetToFactorySettings();
    void resetAudioFile();
};

}

#endif // MANAGESETTINGCOMMAND_H
