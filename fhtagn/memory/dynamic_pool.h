/**
 * $Id$
 *
 * Copyright (C) 2009 the authors.
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
#ifndef FHTAGN_MEMORY_DYNAMIC_POOL_H
#define FHTAGN_MEMORY_DYNAMIC_POOL_H

#ifndef __cplusplus
#error You are trying to include a C++ only header file
#endif

#include <fhtagn/fhtagn.h>

#include <map>

#include <boost/shared_ptr.hpp>

#include <fhtagn/memory/memory_pool.h>
#include <fhtagn/memory/utility.h>
#include <fhtagn/threads/lock_policy.h>

namespace fhtagn {
namespace memory {

/**
 * The dynamic_pool class is a thin wrapper around such MemoryPool objects that
 * manage memory blocks of a fixed size, such as fixed_pool or block_pool.
 *
 * It maintains a defers to a list of such pools. If allocation from one of them
 * fails, an additional such pool is created, and memory is allocated from there
 * instead.
 *
 * If after freeing a pointer, the pool it was from becomes empty, the pool
 * is destroyed again.
 *
 * Pools are created with a fixed size, passed to dynamic_pool as a template
 * parameter. One obvious result of that is that dynamic_pool can never allocate
 * more data than it's memory block size.
 *
 * XXX There's a caveat when using dynamic_pool with an underlying block_pool,
 *     as reallocations or allocations with the wrong size will usually fail,
 *     which would cause dynamic_pool to create a new block_pool. The only way
 *     to work with the combination of the two is to ensure the block_pool
 *     always receives requests of a valid size, which means passing valid
 *     sizes to the dynamic_pool as well.
 **/
template <
  typename poolT,
  int MEMORY_BLOCK_SIZE,
  typename mutexT = fhtagn::threads::fake_mutex
>
class dynamic_pool
{
public:
  /**
   * Convenience typedefs
   **/
  typedef mutexT            mutex_t;
  typedef poolT             pool_t;

  /**
   * API - see memory_pool.h for details
   **/
  inline void * alloc(std::size_t size);
  inline void * realloc(void * ptr, std::size_t new_size);
  inline void free(void * ptr);
  inline bool in_use() const;
  inline std::size_t alloc_size(void * ptr) const;

private:

  typedef boost::shared_ptr<pool_t>       pool_ptr;
  typedef std::pair<void *, void *>       pool_key_t;
  typedef std::map<pool_key_t, pool_ptr>  pool_map_t;

  /**
   * For a given pointer, returns the pool that should contain it.
   **/
  inline typename pool_map_t::iterator find_pool(void * ptr);
  inline typename pool_map_t::const_iterator find_pool(void * ptr) const;

  /**
   * Lock-free version of alloc, used internally.
   **/
  inline void * lockfree_alloc(std::size_t size);

  pool_map_t      m_pool_map;
  mutable mutex_t m_mutex;
};


}} // namespace fhtagn::memory

#include <fhtagn/memory/detail/dynamic_pool.tcc>

#endif // guard