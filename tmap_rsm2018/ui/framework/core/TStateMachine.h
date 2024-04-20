#pragma once

namespace SKT {

class TState;

class TMAP_RSM_UI_DLLSHARED_EXPORT TStateMachine : public QStateMachine
{
public:
	TStateMachine();

	virtual ~TStateMachine();

	TState* currentState() const;

	void setCurrentState( TState* state );

	TState* state( const QString& id ) const;

	void addState( const QString& id, TState* state );

	void changeState( const QString& targetStateId );

	void changeState( const QString& targetStateId, const TContext& context );

	void setDebug( int debug ) {
		mDebug = debug;
	}

private:
	TState* mCurrentState = nullptr;
	QMap<QString,TState*> mStates;
	int mDebug = 0;
};

}
