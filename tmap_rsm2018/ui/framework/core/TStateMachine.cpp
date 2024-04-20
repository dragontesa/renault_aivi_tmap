#include "TStateMachine.h"
#include "TState.h"
#include "TStateEvent.h"

using namespace SKT;

TStateMachine::TStateMachine() {
}

TStateMachine::~TStateMachine() {
}

TState* TStateMachine::currentState() const 
{
	return mCurrentState;
}

void TStateMachine::setCurrentState( TState* state ) 
{
	mCurrentState = state;
}

TState* TStateMachine::state( const QString& id ) const 
{
	return mStates[id];
}

void TStateMachine::addState( const QString& id, TState* state ) 
{
	mStates[id] = state;
	QStateMachine::addState( state );
}

void TStateMachine::changeState( const QString& targetStateId )
{
	QStateMachine::postEvent( new TStateEvent( state( targetStateId ) ) );
}

void TStateMachine::changeState( const QString& targetStateId, const TContext& context )
{
	QStateMachine::postEvent( new TStateEvent( state( targetStateId ), context ) );
}
