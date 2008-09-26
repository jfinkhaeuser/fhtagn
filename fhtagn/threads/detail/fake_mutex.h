/**
 * $Id$
 *
 * Copyright (C) 2008 the authors.
 *
 * Author: Jens Finkhaeuser <unwesen@users.sourceforge.net>
 *
 * This file is part of the Fhtagn! C++ Library, and may be distributed under
 * the following license terms:
 *
 * Boost Software License - Version 1.0 - August 17th, 2003
 *
 * Permission is hereby granted, free of charge, to any person or organization
 * obtaining a copy of the software and accompanying documentation covered by
 * this license (the "Software") to use, reproduce, display, distribute,
 * execute, and transmit the Software, and to prepare derivative works of the
 * Software, and to permit third-parties to whom the Software is furnished to
 * do so, all subject to the following:
 *
 * The copyright notices in the Software and this entire statement, including
 * the above license grant, this restriction and the following disclaimer,
 * must be included in all copies of the Software, in whole or in part, and
 * all derivative works of the Software, unless such copies or derivative
 * works are solely in the form of machine-executable object code generated by
 * a source language processor.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
 * SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
 * FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 **/
#ifndef FHTAGN_THREADS_DETAIL_FAKE_MUTEX_H
#define FHTAGN_THREADS_DETAIL_FAKE_MUTEX_H

#ifndef __cplusplus
#error You are trying to include a C++ only header file
#endif


#include <boost/noncopyable.hpp>
#include <boost/thread/xtime.hpp>

namespace fhtagn {
namespace threads {

/**
 * Provides an interface like boost's mutexes, but does nothing at all. Since it
 * does nothing at all, it might as well provide all the lock types.
 *
 * Similar to recursive_mutex behaviour in a single thread.
 **/
class fake_mutex
    : private boost::noncopyable
{
public:
    typedef boost::unique_lock<fake_mutex> scoped_lock;
    typedef scoped_lock scoped_try_lock;
    typedef scoped_lock scoped_timed_lock;

    /*************************************************************************
     * Lockable implementation
     */
    void lock()
    {
    }


    bool try_lock()
    {
        return true;
    }

    void unlock()
    {
    }


    /*************************************************************************
     * TimedLockable implementation
     */
    bool timed_lock(boost::system_time const & abs_time)
    {
        return true;
    }


    template <typename time_durationT>
    bool timed_lock(time_durationT const & rel_time)
    {
        return true;
    }


    /*************************************************************************
     * SharedLockable implementation
     */
    void lock_shared()
    {
    }


    bool try_lock_shared()
    {
        return true;
    }


    void unlock_shared()
    {
    }


    bool timed_lock_shared(boost::system_time const & abs_time)
    {
        return true;
    }


    /*************************************************************************
     * UpgradeLockable implementation
     */
    void lock_upgrade()
    {
    }


    void unlock_upgrade()
    {
    }


    void unlock_upgrade_and_lock()
    {
    }


    void unlock_and_lock_upgrade()
    {
    }


    void unlock_upgrade_and_lock_shared()
    {
    }

};

}} // namespace fhtagn::threads

#endif // guard
