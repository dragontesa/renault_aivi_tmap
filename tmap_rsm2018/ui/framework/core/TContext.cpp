#include "TContext.h"

using namespace SKT;

TContext::TContext()
{
}

TContext::~TContext()
{
}

TContext::TContext( const TContext& other )
{
	mValues = other.mValues;
}

TContext& TContext::operator=( const TContext& other )
{
	return *this;
}
