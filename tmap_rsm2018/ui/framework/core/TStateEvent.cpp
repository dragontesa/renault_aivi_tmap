#include "TStateEvent.h"

using namespace SKT;

TStateEvent::TStateEvent( TState* targetState, int qEventType )
	: TEvent( qEventType ),
	mTargetState( targetState ) 
{
}

TStateEvent::TStateEvent( TState* targetState, const TContext& context, int qEventType )
	: TEvent( context, qEventType ),
	mTargetState( targetState ) 
{
}

TStateEvent::~TStateEvent() 
{
}

TState* TStateEvent::targetState() 
{
	return mTargetState;
}
