#pragma once
/**
 *  \file      use_boost_header_only.hpp
 *  \brief     TODO
 *  \author    Fang Pengfei (CM/ESW21-CN)<Pengfei.Fang@cn.bosch.com>
 *  \version   1.0
 *  \date      Jan 19, 2018
 *  \copyright (C) 2018 BOSCH Automotive Products (Suzhou) Co. Ltd.
 */
#ifndef USE_BOOST_HEADER_ONLY_HPP_
#define USE_BOOST_HEADER_ONLY_HPP_

#define BOOST_ALL_NO_LIB
#define BOOST_ERROR_CODE_HEADER_ONLY
#define BOOST_SYSTEM_NO_DEPRECATED
#define BOOST_CHRONO_HEADER_ONLY
#define BOOST_CHRONO_DONT_PROVIDE_HYBRID_ERROR_HANDLING
#define BOOST_THREAD_DONT_PROVIDE_INTERRUPTIONS

#include <boost/cerrno.hpp>
#include <boost/cstdint.hpp>
#include <boost/cstdlib.hpp>

#include <boost/align.hpp>
#include <boost/atomic.hpp>
#include <boost/chrono.hpp>
#include <boost/circular_buffer.hpp>
#include <boost/make_shared.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/unordered_map.hpp>
#include <boost/utility.hpp>
#include <boost/weak_ptr.hpp>

#include <boost/thread/condition_variable.hpp>
#include <boost/thread/locks.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/once.hpp>
#include <boost/thread/recursive_mutex.hpp>
#include <boost/thread/shared_mutex.hpp>

#endif /* USE_BOOST_HEADER_ONLY_HPP_ */
