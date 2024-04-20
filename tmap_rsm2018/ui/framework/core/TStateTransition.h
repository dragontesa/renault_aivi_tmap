#pragma once

namespace SKT {

class TMAP_RSM_UI_DLLSHARED_EXPORT TStateTransition : public QAbstractTransition
{
public:
	TStateTransition( int qEventType = QEvent::None  );

	~TStateTransition();

	virtual bool eventTest(QEvent *e);

	virtual void onTransition(QEvent *);

private:
	int mQEventType;
};

}
