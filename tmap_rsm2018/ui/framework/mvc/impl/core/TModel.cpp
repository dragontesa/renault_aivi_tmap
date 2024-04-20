#include "TModel.h"

using namespace SKT;
using namespace SKT::mvc;

TModel::TModel()
{
}

TModel::~TModel()
{

}

void TModel::registerProxy( const IProxyPtr& proxy )
{    
    proxy->onBeforeRegister();
    mProxyMap[ proxy->getProxyName() ] = proxy;
    proxy->onRegister();
}

IProxyPtr TModel::retrieveProxy( const QString& proxyName )
{
    IProxyPtr ret;

    if ( mProxyMap.contains( proxyName ) ) {
        ret = mProxyMap[ proxyName ];
    }

    return ret;
}

bool TModel::hasProxy( const QString& proxyName )
{
    bool ret = false;

    if ( mProxyMap.contains( proxyName ) ) {
        ret = mProxyMap[ proxyName ] != nullptr;
    }

    return ret;
}

IProxyPtr TModel::removeProxy( const QString& proxyName )
{
    IProxyPtr ret;

    if ( mProxyMap.contains( proxyName ) ) {
        ret = mProxyMap[ proxyName ];
        if ( !ret.isNull() ) {
            ret->onRemove();
        }
        mProxyMap.remove( proxyName );
    }

    return ret;
}

