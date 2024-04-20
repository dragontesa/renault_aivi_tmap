#include "TFacade.h"
#include "TModel.h"
#include "TView.h"
#include "TController.h"
#include "TNotification.h"

using namespace SKT;
using namespace SKT::mvc;

TFacade::TFacade()
{
}

TFacade::~TFacade()
{
}

void TFacade::setCore( IModel* model, IView* view, IController* controller )
{
    mModel = model;
    mView = view;
    mController = controller;
}

void TFacade::registerCommand( int noteId, const QMetaObject* commandClassRef )
{
    mController->registerCommand( noteId, commandClassRef );
}

void TFacade::removeCommand( int noteId )
{
    mController->removeCommand( noteId );
}

bool TFacade::hasCommand( int noteId )
{
    return mController->hasCommand(noteId);
}

 void TFacade::registerProxy ( const IProxyPtr& proxy )
 {
     mModel->registerProxy ( proxy );
 }

 IProxyPtr TFacade::retrieveProxy ( const QString& proxyName )
 {
     return mModel->retrieveProxy ( proxyName );
 }

IProxyPtr TFacade::removeProxy ( const QString& proxyName )
{
    IProxyPtr proxy;
    if ( mModel != nullptr ) proxy = mModel->removeProxy ( proxyName );
    return proxy;
}

bool TFacade::hasProxy( const QString& proxyName )
{
    return mModel->hasProxy( proxyName );
}

void TFacade::registerMediator( const IMediatorPtr& mediator )
{
    if ( mView != nullptr ) mView->registerMediator( mediator );
}

IMediatorPtr TFacade::retrieveMediator( const QString& mediatorName )
{
    return mView->retrieveMediator( mediatorName );
}

IMediatorPtr TFacade::removeMediator( const QString& mediatorName )
{
    IMediatorPtr mediator;
    if ( mView != nullptr ) mediator = mView->removeMediator( mediatorName );
    return mediator;
}

bool TFacade::hasMediator( const QString& mediatorName )
{
    return mView->hasMediator( mediatorName );
}

void TFacade::notifyObservers ( const INotificationPtr& note )
{
    if ( mView != nullptr ) mView->notifyObservers( note );
}
