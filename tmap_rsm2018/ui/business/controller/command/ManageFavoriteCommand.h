#ifndef FAVORITECOMMAND_H
#define FAVORITECOMMAND_H
#include "SimpleCommand.h"
#include "FavoriteDto.h"

namespace SKT
{

class ManageFavoriteCommand : public SimpleCommand
{
	Q_OBJECT
public:
    Q_INVOKABLE ManageFavoriteCommand(QObject* parent=0);

	bool execute( const mvc::INotificationPtr& note );

private:
    void requestFavoiteList(const QVariantMap &m, bool forProperty);
    void requestFavoiteListForProperty();
    void initNaviProperty(const mvc::ICommandPtr& command, const FavoriteDtoPtr& favorite);
    void resetHomeLocationProperty();
    void resetWorkLocationProperty();
};

}

#endif // FAVORITECOMMAND_H
