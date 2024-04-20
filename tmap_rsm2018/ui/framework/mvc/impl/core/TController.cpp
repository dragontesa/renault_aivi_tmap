#include "TController.h"
#include "TView.h"
#include "TObserver.h"

using namespace SKT;
using namespace SKT::mvc;

TController::TController()
    : mView( nullptr )
{
}

TController::~TController() {
}

void TController::setView( IView* view )
{
    mView = view;
}

bool TController::executeCommand( const INotificationPtr& note )
{
    auto commandClassRef = mCommandMap[ note->getId() ];
    if ( commandClassRef == nullptr ) return false;

    auto instance = dynamic_cast<ICommand*>(commandClassRef->newInstance(nullptr));
    auto commandPtr = ICommandPtr( instance );
    note->setDestination( qSharedPointerDynamicCast<QObject>(commandPtr) );
    return commandPtr->execute( note );
}

void TController::registerCommand( int noteId, const QMetaObject* commandClassRef )
{
    if ( mCommandMap[ noteId ] == nullptr ) {
        IObserver::NotifyMethod notifyMethod = std::bind(&TController::executeCommand, this, std::placeholders::_1);
        // 아래 observer가 소멸시 TController가 소멸되지 않도록 shareSelf
        auto observer = new TObserver( notifyMethod, shareSelf() );
        observer->setObserverName( commandClassRef->className() );
        mView->registerObserver( noteId, IObserverPtr( observer ) );
    }
    mCommandMap[ noteId ] = commandClassRef;
}

bool TController::hasCommand( int noteId )
{
    return mCommandMap[ noteId ] != nullptr;
}

void TController::removeCommand( int noteId )
{
    if ( hasCommand( noteId ) ) {
        mView->removeObserver( noteId, shareSelf() );
        mCommandMap[ noteId ] = nullptr;
    }
}

