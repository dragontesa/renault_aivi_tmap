#pragma once

#include "TEvent.h"

namespace SKT {

class TMAP_RSM_UI_DLLSHARED_EXPORT TState : public QState
{
	Q_OBJECT
public: 
	TState( QState* parent = 0 );

	virtual ~TState();

	void onEntry( QEvent* event );

	void onExit( QEvent* event );

	virtual void onEntered( TEvent* event );

signals:
	void entered( TEvent* event );
};

}
