#ifndef TMVC_H
#define TMVC_H

namespace SKT { namespace mvc {

T_SHAREABLE( INotification );
T_SHAREABLE( INotifier );
T_SHAREABLE( ICommand );
T_SHAREABLE( IController );
T_SHAREABLE( IMediator );
T_SHAREABLE( IProxy );
T_SHAREABLE( IModel );
T_SHAREABLE( IObserver );
T_SHAREABLE( IView );
T_SHAREABLE( IFacade );
T_SHAREABLE( TModel );
T_SHAREABLE( TView );
T_SHAREABLE( TController );
T_SHAREABLE( TMacroCommand );
T_SHAREABLE( TSimpleCommand );
T_SHAREABLE( TFacade );
T_SHAREABLE( TMediator );
T_SHAREABLE( TNotification );
T_SHAREABLE( TNotifier );
T_SHAREABLE( TObserver );
T_SHAREABLE( TProxy );

}}

#include "INotification.h"
#include "INotifier.h"
#include "ICommand.h"
#include "IController.h"
#include "IMediator.h"
#include "IProxy.h"
#include "IModel.h"
#include "IObserver.h"
#include "IView.h"
#include "IFacade.h"

#include "TModel.h"
#include "TView.h"
#include "TController.h"
#include "TMacroCommand.h"
#include "TSimpleCommand.h"
#include "TFacade.h"
#include "TMediator.h"
#include "TNotification.h"
#include "TObserver.h"
#include "TProxy.h"

#endif // TMVC_H
