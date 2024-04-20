#pragma once

#include "TEvent.h"

namespace SKT {

class TState;

class TMAP_RSM_UI_DLLSHARED_EXPORT TStateEvent : public TEvent
{
public:
	TStateEvent( TState* targetState, int qEventType = QEvent::None );

	TStateEvent( TState* targetState, const TContext& context, int qEventType = QEvent::None );

	virtual ~TStateEvent();

	TState* targetState();

private:
	TState* mTargetState = nullptr;
};

}
