#pragma once
/**
 *  \file      EventWaiter.h
 *  \brief     TODO
 *  \author    Fang Pengfei (CM/ESW21-CN)<Pengfei.Fang@cn.bosch.com>
 *  \version   1.0
 *  \date      Jan 19, 2018
 *  \copyright (C) 2018 BOSCH Automotive Products (Suzhou) Co. Ltd.
 */
#ifndef EVENTWAITER_H_
#define EVENTWAITER_H_

#include "use_boost_header_only.hpp"

class EventWaiter : boost::noncopyable {
public:
    EventWaiter() : _signaled(false) {}
    ~EventWaiter() {}

    void Wait() {
        boost::mutex::scoped_lock lock(_mutex);
        while (!_signaled) {
            _cond.wait(lock);
        }
        _signaled = false;
    }

    void WakeupOne() {
        boost::mutex::scoped_lock lock(_mutex);
        _signaled = true;
        _cond.notify_one();
    }

    void WakeupAll() {
        boost::mutex::scoped_lock lock(_mutex);
        _signaled = true;
        _cond.notify_all();
    }

private:
    boost::mutex _mutex;
    boost::condition_variable _cond;
    boost::atomic_bool _signaled;
};

#endif /* EVENTWAITER_H_ */
