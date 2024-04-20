#pragma once

namespace SKT {

class TMAP_RSM_UI_DLLSHARED_EXPORT TMutexLocker
{
public:
	TMutexLocker( QMutex* mutex );

	~TMutexLocker();

	operator bool();

private:
	QMutex* mMutex;
};

}
