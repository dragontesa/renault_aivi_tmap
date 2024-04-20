#ifndef BOOKMARKVIEWMEDIATOR_H
#define BOOKMARKVIEWMEDIATOR_H

#include "BookmarkPageViewMediator.h"

namespace SKT {

class BookmarkViewMediator : public BookmarkPageViewMediator
{
    Q_OBJECT
    Q_ENUMS( EventK )
public:
    enum EventK {
    };
    Q_INVOKABLE BookmarkViewMediator( QObject* viewComponent = nullptr );
};

}

#endif // BOOKMARKVIEWMEDIATOR_H
