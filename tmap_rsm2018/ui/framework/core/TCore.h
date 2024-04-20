#pragma once

#include <tuple>
#include <functional>

// base type

namespace SKT {

typedef int t_bool;
typedef QMap<QString,QString> TMapString;
typedef QList<QSharedPointer<QObject>> TSharedObjectList;

};

// unique name

#define T_CONCAT( a, b ) T_CONCAT2( a, b )
#define T_CONCAT2( a, b ) T_CONCAT3( ~, a ## b )
#define T_CONCAT3( p, res ) res
#define T_UNIQUE_NAME( base ) T_CONCAT( base, __COUNTER__ )

// shared ponter

#define T_SHAREABLE( T ) class T; typedef QSharedPointer<T> T##Ptr; typedef QWeakPointer<T> T##WeakPtr;

T_SHAREABLE( QObject )

namespace SKT {

template<class T>
using t_shared = QSharedPointer<T>;
template<class T>
using t_weak = QWeakPointer<T>;
template<class T>
using t_scoped = QScopedPointer<T>;

// shared cast
template <class X, class T>
inline X* t_shared_cast(const QSharedPointer<T> &src)
{
    register X *ptr = static_cast<X *>(src.data());
    return ptr;
}

// scoped cast
template <class X, class T>
inline X* t_scoped_cast(const QScopedPointer<T> &src)
{
    register X *ptr = static_cast<X *>(src.data());
    return ptr;
}

template<typename T>
class TShareable {
public:
    virtual ~TShareable() {
    }

    static void preventDeletion( T* ) {
    }

    t_shared<T> shareSelf() {
        return t_shared<T>((T*)this, preventDeletion );
    }

    template <typename... Args>
    static t_shared<T> newShare(Args... args) {
        return t_shared<T>( new T( args... ) );
    }
};

template <class T, typename... Args>
inline t_shared<T> t_new_shared(Args... args) {
    return t_shared<T>(new T(args...));
}

};

// thread

#define T_USING_SYNCHRONIZED QMutex __mutex__;
#define T_SYNCHRONIZED if ( SKT::TMutexLocker lock = &__mutex__ )
#define T_SYNCHRONIZED_WITH( Mutex ) if ( SKT::TMutexLocker lock = Mutex )

// string

#define T_CSTR toLocal8Bit().constData
#define T_Q2A( a, q ) \
    QString a##_es_ = q; \
    QByteArray a##_bytes_es_ = a##_es_.toLocal8Bit(); \
    const char* a = a##_bytes_es_.constData();
#define T_SPRINTF QString().sprintf
#define T_TRIM( s ) QByteArray( s ).trimmed().constData()
#define T_TRIM_LIST( s ) QStringList( s ).replaceInStrings( QRegExp( "^\\s*" ), "" )
#define T_ATOI( s ) QByteArray( s ).trimmed().toInt()
#define T_ATOF( s ) QByteArray( s ).trimmed().toDouble()

// exception

namespace SKT {

class TRuntimeException
{
public:
    TRuntimeException( const QString& msg )
        : mMsg( msg ) {
    }

    virtual ~TRuntimeException() throw() {
    }

    virtual QString what() const throw() {
        return mMsg;
    }

private:
    QString mMsg;
};

}

//#define T_THROW(msg) \
//    throw TRuntimeException( msg );
//#define T_THROWV(msg) \
//    throw TRuntimeException( QString().sprintf( "[%s]%s", __FUNCTION__, QString().sprintf( __VA_ARGS__ ).T_CSTR() ).T_CSTR() );

// debug

#ifdef _DEBUG
#define T_BREAK() \
    __asm { \
    int 3 \
}
#define T_BREAK_ON( cond ) \
    if ( cond ) { \
    __asm { \
    int 3 \
} \
    }
#else
#define T_BREAK()
#define T_BREAK_ON( a, b )
#endif

// todo

#define T_QUOTE(x) #x
#define T_QUOTE2(x) T_QUOTE(x)
#define __FILE__LINE__ __FILE__ "(" T_QUOTE2(__LINE__) "): "

/*
#define T_NOTE( x ) __pragma( message( #x ) )
#define T_TODO( x )  __pragma( message( __FILE__LINE__"[TODO] " #x ) )
#define T_FIXME( x )  __pragma( message( __FILE__LINE__"[FIXME] " #x ) )
*/

// signal

#define T_SIGNAL_MEM_FUNC( type, method, ... ) (void(##type##::*)(__VA_ARGS__))&##type##::##method
#define T_SLOT_MEM_FUNC( type, method, ... ) (void(##type##::*)(__VA_ARGS__))&##type##::##method

// object

namespace SKT
{

bool isValidObject( QObject* obj );

template<typename Base, typename T>
inline bool t_instanceof(const T* ptr) {
    //return std::is_base_of<Base, T>::value;
    return dynamic_cast<const Base*>(ptr) != nullptr;
}

};


// misc

namespace SKT
{

QTextStream& qStdOut();

}

#include "TContext.h"
#include "TLogger.h"
#include "TMutex.h"
#include "TTypeSafeEnum.h"
