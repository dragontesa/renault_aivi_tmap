#include "TEvent.h"

using namespace SKT;

TEvent::TEvent( int type )
	: QEvent( QEvent::Type( type == QEvent::None ? TEvent::defaultEventType() : type ) )
{
}

TEvent::TEvent( const TContext& context, int type )
	: mContext( context ),
	QEvent( QEvent::Type( type == QEvent::None ? TEvent::defaultEventType() : type ) )
{
}

TEvent::~TEvent()
{
}

int TEvent::defaultEventType() 
{
	static int TYPE = QEvent::None;

	if ( TYPE == QEvent::None ) {
		TYPE = QEvent::registerEventType();
	}

	return TYPE;
}

TContext& TEvent::context()
{
	return mContext;
}
