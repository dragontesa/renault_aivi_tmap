#pragma once
/**
 *  \file      ThreadSync.h
 *  \brief     TODO
 *  \author    Fang Pengfei (CM/ESW21-CN)<Pengfei.Fang@cn.bosch.com>
 *  \version   1.0
 *  \date      Jan 19, 2018
 *  \copyright (C) 2018 BOSCH Automotive Products (Suzhou) Co. Ltd.
 */
#ifndef THREADSYNC_H_
#define THREADSYNC_H_

#include "use_boost_header_only.hpp"

class ThreadSync : boost::noncopyable {
public:
    ThreadSync() : _thread_ready(false), _event_reached(false), _event_need_return(false), _event_return_reached(false) {}
    ~ThreadSync() {}

    void WaitForThreadReady() {
        boost::mutex::scoped_lock lock(_mutex);
        while (!_thread_ready) {
            _cond.wait(lock);
        }
    }

    void SignalThreadReady() {
        boost::mutex::scoped_lock lock(_mutex);
        _thread_ready = true;
        _cond.notify_all();
    }

    void WaitForEvent() {
        boost::mutex::scoped_lock lock(_mutex);
        while (!_event_reached) {
            _cond.wait(lock);
        }
        _event_reached = false;
    }

    void SignalEvent(bool need_return = false) {
        _event_need_return = need_return;
        boost::mutex::scoped_lock lock(_mutex);
        _event_reached = true;
        _cond.notify_one();
    }

    void WaitForEventReturn() {
        boost::mutex::scoped_lock lock(_mutex);
        while (!_event_return_reached) {
            _cond.wait(lock);
        }
        _event_return_reached = false;
    }

    void SignalEventReturn() {
        boost::mutex::scoped_lock lock(_mutex);
        _event_return_reached = true;
        _event_need_return = false;
        _cond.notify_one();
    }

    bool IsThreadReady() { return _thread_ready; }
    bool NeedReturn() { return _event_need_return; }

private:
    boost::mutex _mutex;
    boost::condition_variable _cond;
    bool _thread_ready;
    bool _event_reached;
    bool _event_need_return;
    bool _event_return_reached;
};

#endif /* THREADSYNC_H_ */
