#include "TStopwatch.h"

using namespace SKT;

TStopwatch::TStopwatch()
	: mElapsed( 0 )
{	
	mTime.start();	
}

TStopwatch::TStopwatch( double* elapsed )
	: mElapsed( elapsed )
{	
	mTime.start();	
}

TStopwatch::~TStopwatch()
{
	*mElapsed = (double)mTime.elapsed() / 1000;
}

double TStopwatch::elapsed() const
{
	return mTime.elapsed() / 1000;
}
