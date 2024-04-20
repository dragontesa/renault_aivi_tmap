#include "TState.h"
#include "TStateTransition.h"
#include "TStateMachine.h"

using namespace SKT;

TState::TState( QState* parent ) 
	: QState( parent ) 
{
	addTransition( new TStateTransition() );
}

TState::~TState() 
{
}

void TState::onEntry( QEvent* event ) 
{
	auto tsm = dynamic_cast<TStateMachine*>( parentState() );
	if ( tsm != nullptr ) {
		tsm->setCurrentState( this );
	}

	onEntered( dynamic_cast<TEvent*>( event ) );
}

void TState::onExit( QEvent* event )
{
}

void TState::onEntered( TEvent* event ) 
{
	emit entered( event );
}
