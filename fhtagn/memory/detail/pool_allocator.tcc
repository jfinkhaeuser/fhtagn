/**
 * $Id: template.h 197 2008-11-02 12:02:37Z unwesen $
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
#ifndef FHTAGN_MEMORY_DETAIL_BLOCK_ALLOCATOR_TCC
#define FHTAGN_MEMORY_DETAIL_BLOCK_ALLOCATOR_TCC

#ifndef __cplusplus
#error You are trying to include a C++ only header file
#endif

#include <limits>
#include <stdexcept>

namespace fhtagn {
namespace memory {

/*****************************************************************************
 * pool_allocation_policy_base
 **/
template <
  typename memory_poolT
>
typename pool_allocation_policy_base<memory_poolT>::memory_pool_ptr
pool_allocation_policy_base<memory_poolT>::get_global_memory_pool()
{
  return sm_global_pool;
}



template <
  typename memory_poolT
>
bool
pool_allocation_policy_base<memory_poolT>::set_global_memory_pool(
    typename pool_allocation_policy_base<memory_poolT>::memory_pool_ptr new_pool)
{
  if (sm_global_pool && sm_global_pool->in_use()) {
    return false;
  }

  sm_global_pool = new_pool;
  return true;
}


/*****************************************************************************
 * pool_allocation_policy
 **/
template <
  typename T,
  typename memory_poolT
>
pool_allocation_policy<T, memory_poolT>::pool_allocation_policy()
{
  initialize_pool();
}



template <
  typename T,
  typename memory_poolT
>
pool_allocation_policy<T, memory_poolT>::~pool_allocation_policy()
{
}



template <
  typename T,
  typename memory_poolT
>
pool_allocation_policy<T, memory_poolT>::pool_allocation_policy(
    pool_allocation_policy<T, memory_poolT> const &)
{
  initialize_pool();
}



template <
  typename T,
  typename memory_poolT
>
template <
  typename U
>
pool_allocation_policy<T, memory_poolT>::pool_allocation_policy(
    pool_allocation_policy<U> const &)
{
  initialize_pool();
}



template <
  typename T,
  typename memory_poolT
>
template <
  typename U,
  typename other_poolT
>
pool_allocation_policy<T, memory_poolT>::pool_allocation_policy(
    pool_allocation_policy<U, other_poolT> const &)
{
  initialize_pool();
}



template <
  typename T,
  typename memory_poolT
>
void
pool_allocation_policy<T, memory_poolT>::initialize_pool()
{
  if (m_pool && m_pool->in_use()) {
    return;
  }

  if (sm_type_pool) {
    m_pool = sm_type_pool;
    return;
  }

  if (!pool_allocation_policy_base<memory_poolT>::sm_global_pool) {
    throw std::logic_error("Constructing a pool_allocation_policy<T> without "
        "either a global pool or per-type pool set.");
  }

  m_pool = pool_allocation_policy_base<memory_poolT>::sm_global_pool;
}



template <
  typename T,
  typename memory_poolT
>
typename pool_allocation_policy<T, memory_poolT>::memory_pool_ptr
pool_allocation_policy<T, memory_poolT>::get_memory_pool()
{
  return sm_type_pool;
}



template <
  typename T,
  typename memory_poolT
>
bool
pool_allocation_policy<T, memory_poolT>::set_memory_pool(
    typename pool_allocation_policy<T, memory_poolT>::memory_pool_ptr new_pool)
{
  if (sm_type_pool && sm_type_pool->in_use()) {
    return false;
  }

  sm_type_pool = new_pool;
  return true;
}



template <
  typename T,
  typename memory_poolT
>
typename pool_allocation_policy<T, memory_poolT>::pointer
pool_allocation_policy<T, memory_poolT>::allocate(size_type count,
    typename std::allocator<void>::const_pointer /* = 0 */)
{
  return static_cast<T *>(m_pool->alloc(count * sizeof(T)));
}



template <
  typename T,
  typename memory_poolT
>
void
pool_allocation_policy<T, memory_poolT>::deallocate(pointer p, size_type)
{
  m_pool->free(p);
}


template <
  typename T,
  typename memory_poolT
>
typename pool_allocation_policy<T, memory_poolT>::size_type
pool_allocation_policy<T, memory_poolT>::max_size() const
{
  return std::numeric_limits<size_type>::max();
}




template <
  typename T,
  typename memory_poolT
>
inline bool operator==(
    pool_allocation_policy<T, memory_poolT> const & rhs,
    pool_allocation_policy<T, memory_poolT> const & lhs)
{
  // Same memory_poolT, so we can if it's also the same instance.
  return (rhs.get_memory_pool() == lhs.get_memory_pool());
}



template <
  typename T1,
  typename T2,
  typename memory_poolT
>
inline bool operator==(
    pool_allocation_policy<T1, memory_poolT> const & rhs,
    pool_allocation_policy<T2, memory_poolT> const & lhs)
{
  // Same memory_poolT, so we can if it's also the same instance.
  return (rhs.get_memory_pool() == lhs.get_memory_pool());
}



template <
  typename T1,
  typename memory_poolT1,
  typename T2,
  typename memory_poolT2
>
inline bool operator==(
    pool_allocation_policy<T1, memory_poolT1> const &,
    pool_allocation_policy<T2, memory_poolT2> const &)
{
  // Different memory pool type - no, won't work.
  return false;
}



template <
  typename T,
  typename memory_poolT,
  typename other_allocatorT
>
inline bool operator==(
    pool_allocation_policy<T, memory_poolT> const &,
    other_allocatorT const &)
{
  // No. In theory, it might actually be possible, but let's not risk it.
  return false;
}



}} // namespace fhtagn::memory

#endif // guard