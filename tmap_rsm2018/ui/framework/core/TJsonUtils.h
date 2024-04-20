#ifndef TJSONUTILS_H
#define TJSONUTILS_H

namespace SKT {

class TJsonUtils
{
public:
    TJsonUtils();

    static QString toJsonString( const QVariantMap& varMap );
};

}

#endif // TJSONUTILS_H
