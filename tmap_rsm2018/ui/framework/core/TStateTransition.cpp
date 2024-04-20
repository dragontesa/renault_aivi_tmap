#include "TStateTransition.h"
#include "TState.h"
#include "TStateEvent.h"

using namespace SKT;

TStateTransition::TStateTransition( int qEventType )
	: mQEventType( qEventType ) {
}

TStateTransition::~TStateTransition()
{
}

bool TStateTransition::eventTest(QEvent *e) 
{
	int eventType = mQEventType;

	if ( eventType == QEvent::None ) {
		eventType = TEvent::defaultEventType();
	}

	if (e->type() != eventType) {
		return false;
	}

	auto se = static_cast<TStateEvent*>(e);

	setTargetState( se->targetState() );

	return true;
}

void TStateTransition::onTransition(QEvent *) 
{
}
