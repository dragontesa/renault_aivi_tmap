#include "TRandomUtils.h"

using namespace SKT;

TRandomUtils* TRandomUtils::instance()
{
    static TRandomUtils Instance;

    return &Instance;
}

int TRandomUtils::randInt( int low, int high )
{
	return qrand() % ((high + 1) - low) + low;
}

TRandomUtils::TRandomUtils()
{
	QTime time = QTime::currentTime();
	qsrand( (uint)time.msec() );
}

TRandomUtils::~TRandomUtils()
{
}
