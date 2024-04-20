#pragma once

namespace SKT
{

class TMAP_RSM_UI_DLLSHARED_EXPORT TStopwatch
{
public:
	TStopwatch();

	TStopwatch( double* elapsed );

	~TStopwatch();

	operator bool() const {
		return true;
	}

	double elapsed() const;

private:
	double* mElapsed;
	QTime mTime;
};

};

#define T_SCOPED_STOPWATCH( name ) \
	double name = 0; \
    if ( SKT::TStopwatch T_UNIQUE_NAME( name ) = &name )

#define T_NAMED_SCOPED_STOPWATCH( name, valName ) \
	double valName = 0; \
    if ( SKT::TStopwatch name = &valName )
