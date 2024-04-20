/* ***************************************************************************************
* FILE:          Macros.h
* SW-COMPONENT:  HMI-BASE
*  DESCRIPTION:  Macros.h is part of HMI-Base util Library
*    COPYRIGHT:  (c) 2015-2016 Robert Bosch Car Multimedia GmbH
*
* The reproduction, distribution and utilization of this file as well as the
* communication of its contents to others without express authorization is
* prohibited. Offenders will be held liable for the payment of damages.
* All rights reserved in the event of the grant of a patent, utility model or design.
*
*************************************************************************************** */

#ifndef __HMIBASE_MACROS_H
#define __HMIBASE_MACROS_H

#include "hmibase/util/Assert.h"
#include "hmibase/util/StringUtils.h"

// lint suppress macros
/**
* allow same function name with different signature in derived class especially for onCourierMessage, as it is concept to do so
*/
//lint -esym(1411, *onCourierMessage) "Member with different signature hides virtual member 'Symbol' ..."
#define HMIBASE_SUPPRESS_LINT_1511_MEMBER_HIDES_NONVIRTUAL_MEMBER(_member_)  /*lint -esym(1511,*_member_)*/

#if(__cplusplus >= 201103L)
// C++11
#else
//#define override  // to mark overridden methods
#endif

#define HMIBASE_UNCOPYABLE(type)   type(const type&); \
                                   type& operator=(const type& rhs);


#ifndef HMI_APP_ASSERT
#ifdef WIN32
#define HMI_APP_ASSERT(condition) _ASSERT(condition)
#elif VARIANT_S_FTR_ENABLE_QT_SUPPORT /* fr83hi:todo */
#define HMI_APP_ASSERT(condition) if ((condition) == false) {  \
        fprintf(stderr," *** FATAL Assertion failed: %s, file %s, line %d", #condition, __FILE__, __LINE__);  \
        }
#else
#define HMI_APP_ASSERT(condition) if ((condition) == false) { hmibase::util::Assert((#condition),__FILE__, __LINE__,false); }
#endif
#endif

#ifndef HMI_APP_ASSERT_ALWAYS
#ifdef WIN32
#define HMI_APP_ASSERT_ALWAYS() _ASSERT(false)
#elif VARIANT_S_FTR_ENABLE_QT_SUPPORT /* fr83hi:todo */
#define HMI_APP_ASSERT_ALWAYS() HMI_APP_ASSERT(false)
#else
#define HMI_APP_ASSERT_ALWAYS()  hmibase::util::Assert(("HMI_ASSERT_ALWAYS"),__FILE__, __LINE__,false);
#endif
#endif

#ifndef HMI_APP_ALERT_MSG
#ifdef WIN32
#define HMI_APP_ALERT_MSG(msg) printf(#msg);
#elif VARIANT_S_FTR_ENABLE_QT_SUPPORT /* fr83hi:todo */
#define HMI_APP_ALERT_MSG(msg) HMI_APP_ASSERT(msg)
#else
#define HMI_APP_ALERT_MSG(msg)  hmibase::util::Assert((#msg),__FILE__, __LINE__,false);
#endif
#endif

#ifndef HMI_APP_ABORT
#ifdef WIN32
#define HMI_APP_ABORT _ASSERT(false)
#elif VARIANT_S_FTR_ENABLE_QT_SUPPORT /* fr83hi:todo */
#define HMI_APP_ABORT(condition) if ((condition) == false) {  \
        fprintf(stderr," *** FATAL Assertion failed: %s, file %s, line %d", #condition, __FILE__, __LINE__);  \
        assert(0); \
        ::std::abort(); \
        }
#else
#define HMI_APP_ABORT(condition)  if((condition) == false) { hmibase::util::Assert((#condition),__FILE__, __LINE__,true); }
#endif
#endif

#ifndef HMI_APP_ABORT_ALWAYS
#ifdef WIN32
#define HMI_APP_ABORT_ALWAYS _ASSERT(false)
#elif VARIANT_S_FTR_ENABLE_QT_SUPPORT /* fr83hi:todo */
#define HMI_APP_ABORT_ALWAYS HMI_APP_ABORT(false)
#else
#define HMI_APP_ABORT_ALWAYS() hmibase::util::Assert("HMI ABORT",__FILE__, __LINE__,true);
#endif
#endif

#ifndef TABSIZE
#define TABSIZE(a)         (sizeof(a)/sizeof(a[0]))
#endif

#ifndef PARAM_UNUSED
#define PARAM_UNUSED(a)    (void) a
#endif

#ifndef PARAM_UNUSED2
#define PARAM_UNUSED2(a,b)    (void)a; (void)b
#endif

#if defined(_MSC_VER) && (_MSC_VER >= 1400 )
#define SNPRINTF(_buffer, _buffersize_, args, ...)	hmibase::util::utilsnprintf(_buffer, _buffersize_, args, ## __VA_ARGS__)
#define snprintf SNPRINTF
#elif defined(_MSC_VER) && (_MSC_VER >= 1200 )
#define SNPRINTF _snprintf
#else
#define SNPRINTF snprintf
#endif


// #pragma message(Reminder "Fix this problem!")

#ifndef _REMINDER
# define _REMINDERSTR1(x) #x
# define _REMINDERSTR2(x) _REMINDERSTR1(x)
# define _REMINDER(description) message("  " __FILE__ "(" _REMINDERSTR2(__LINE__) "): |Reminder| " #description)
# define _TODO(description)     message("  " __FILE__ "(" _REMINDERSTR2(__LINE__) "): |TODO| " #description)
#endif


#define HMIBASE_DEPRECATED_HFILE(msg, file_descriptor) \
    namespace file_descriptor { \
        DEPRECATED1(msg, static int header_file()); \
        static int header_file() { return 0; } \
        namespace { int val = header_file(); } \
    }

#if defined __GNUC__ && (__GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR__ > 0))
#define DEPRECATED1(msg, func)  func	__attribute__ ((deprecated))
#define DEPRECATED2						__attribute__((deprecated))
#define DEPRECATED_CONST1(msg, constant) __attribute__ ((deprecated)) constant
#elif defined(__MINGW32__) || defined(__ghs__)
#define DEPRECATED1(msg, func)  func	__attribute__ ((deprecated))
#define DEPRECATED2						__attribute__((deprecated))
#define DEPRECATED_CONST1(msg, constant) __attribute__ ((deprecated)) constant
#elif defined(_MSC_VER)
#define DEPRECATED1(msg, func)	__declspec(deprecated("DEPRECATED!\n >>>" msg)) func
#define DEPRECATED2				__declspec(deprecated)
#define DEPRECATED_CONST1(msg, constant) __declspec(deprecated("DEPRECATED!\n >>>" msg)) constant
#else
#pragma message("WARNING: You need to implement DEPRECATED for this compiler")
#define DEPRECATED1(msg, func)	func
#define DEPRECATED2
#define DEPRECATED_CONST1(msg, constant) constant
#endif

// 1: DEPRECATED1( "Please use new func2 inctead of func", void func(in a ) )
// 2: DEPRECATED2  void func(in a )

// Macro to mark a function as deprecated
#define HMIBASE_DEPRECATED(msg,func) DEPRECATED1(msg,func)
#define HMIBASE_DEPRECATED_CONST(msg,constant) DEPRECATED_CONST1(msg,constant);

#define HMIBASE_DEPRECATED_TYPE_ALIAS(oldType, newType) HMIBASE_DEPRECATED("use new type", typedef newType oldType);
#define HMIBASE_DEPRECATED_CONST_ALIAS(oldValue, newValue, newType) HMIBASE_DEPRECATED_CONST("use new value", const newType oldValue = newValue);
#define HMIBASE_DEPRECATED_NAMESPACE_ALIAS(oldName, newName) namespace oldName = newName;

/* -----------------------------------------------------------------------------------

--------- sample1: --------
  HMIBASE_DEPRECATED("Use func2() instead of func1()!",
      void func1(int a, int b)
  );

--------- sample2: --------

#ifdef HMIBASE_DEPRECATED
  typedef InPlaceEffect2D* InPlaceEffect2DPointer;
#else
#error Remove this!
#enif

--------- sample3: --------

   HMIBASE_DEPRECATED("Use SimplePropertyEx<SharedPointer<T>> instead!",
         const T* Get() const
    );

--------- sample4: -- eine header datei ist toto ------

#if !defined(__FILEGUARD_HH)
#define __FILEGUARD_HH
#include "blabla.h"
HMIBASE_DEPRECATED_HFILE("Classes in this file are deprecated. Use instead ...", __FILEGUARD_HH)
#endif

--------- sample5: --------

    template <typename T> class HMIBASE_DEPRECATED("Use ClassNew() instead.",
         ClassOld: public Search<T> {}
    ) ;

*/

#endif // __HMIMACROS_H
