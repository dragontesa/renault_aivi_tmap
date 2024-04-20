#include "TProxy.h"

using namespace SKT;
using namespace SKT::mvc;

TProxy::TProxy( const QVariant& data )
    : mProxyName( "Undefined" )
{
}

TProxy::~TProxy()
{
}

QString TProxy::getProxyName()
{
    return mProxyName;
}

void TProxy::setProxyName( const QString& name )
{
    mProxyName = name;
}

void TProxy::setData( const QVariant& data )
{
    mData = data;
}

QVariant TProxy::getData()
{
    return mData;
}

void TProxy::onBeforeRegister()
{
}

void TProxy::onRegister()
{
}

void TProxy::onRemove()
{
}
