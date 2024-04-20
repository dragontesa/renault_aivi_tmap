#include "TMutex.h"

using namespace SKT;

TMutexLocker::TMutexLocker( QMutex* mutex ) 
	: mMutex( mutex ) 
{
	mMutex->lock();
}

TMutexLocker::~TMutexLocker() 
{
	mMutex->unlock();
}

TMutexLocker::operator bool() 
{
	return true;
}

