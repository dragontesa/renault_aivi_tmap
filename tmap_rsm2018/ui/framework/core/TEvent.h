#pragma once

namespace SKT
{

class TMAP_RSM_UI_DLLSHARED_EXPORT TEvent : public QEvent
{
public:
	TEvent( int type );

	TEvent( const TContext& context, int type );

	virtual ~TEvent();

	static int defaultEventType();

	TContext& context();

private:
	TContext mContext;
};

}
