#pragma once
/**
 *  \file      Singleton.h
 *  \brief     TODO
 *  \author    Fang Pengfei (CM/ESW21-CN)<Pengfei.Fang@cn.bosch.com>
 *  \version   1.0
 *  \date      Jan 25, 2018
 *  \copyright (C) 2018 BOSCH Automotive Products (Suzhou) Co. Ltd.
 */

#ifndef SINGLETON_H_
#define SINGLETON_H_
#include <boost/noncopyable.hpp>

#define GCC_VERSION (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)
/* Test for GCC > 4.3.0 */
#if GCC_VERSION > 40300
// Meyers singleton, thread-safe after C++11 or g++ > 4.3.0 or after Microsoft VS2015
template <typename classType> class Singleton : boost::noncopyable {
public:
    static classType &instance() {
        static classType tmp;
        return tmp;
    }
};
#else
#error "Does't support GCC version lower than 4.3.0!"
#endif

#endif /* SINGLETON_H_ */
