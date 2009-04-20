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
#ifndef FHTAGN_MEMORY_MEMORY_POOL_H
#define FHTAGN_MEMORY_MEMORY_POOL_H

#ifndef __cplusplus
#error You are trying to include a C++ only header file
#endif

#include <fhtagn/fhtagn.h>

#include <boost/concept_check.hpp>

namespace fhtagn {
namespace memory {

/**
 * Memory pools are conceptually little more than a block of memory out of which
 * one can allocate and free a sequence of bytes.
 *
 * The interesting part is that this interface can hide any level of complexity,
 * from a simply heap abstraction to some form of memory pool management.
 *
 * This file defines both a concept check for memory pools, and a heap_pool
 * class that merely wraps the heap. See heap_pool for documentation on member
 * functions.
 **/
namespace concepts {

template <
  typename poolT
>
struct MemoryPoolConcept
{
  void constraints()
  {
    p = pool.alloc(s);
    p = pool.realloc(p, s);
    pool.free(p);

    const_constraints();
  }

  void const_constraints() const
  {
    bool b = pool.in_use();
    boost::ignore_unused_variable_warning(b);

    fhtagn::size_t s2 = pool.alloc_size(p);
    boost::ignore_unused_variable_warning(s2);
  }

  fhtagn::size_t s;
  void * p;
  poolT & pool;
};

} // namespace concepts


/**
 * The heap_pool class purposely wraps C-style malloc/realloc/free functions,
 * as reallocation is not supported by C++ new/delete operators.
 *
 * Of course, reallocation of an object would not make sense and/or introduce
 * new problems of it's own, so the realloc function may well be considered
 * pointless for pure C++ use-cases.
 *
 * It's still a useful function to provide for other use-cases.
 **/
struct heap_pool
{
  /**
   * Allocates size bytes, and returns a pointer to it. If allocation failed,
   * NULL is returned instead.
   *
   * A size of zero will always cause NULL to be returned.
   **/
  inline void * alloc(fhtagn::size_t size);

  /**
   * Reallocates the given pointer to a new size, and returns a new pointer to
   * the resized memory area, or NULL if reallocation failed. Note that the
   * pointer handed to realloc may be invalidated, whereas the returned pointer
   * (if non-NULL) will always be valid.
   *
   * If NULL is returned, the pointer parameter is not invalidated.
   *
   * A size of zero will always cause NULL to be returned.
   **/
  inline void * realloc(void * ptr, fhtagn::size_t new_size);

  /**
   * Frees the memory pointed to by the pointer parameter. If the pointer
   * parameter is NULL, this function becomes a no-op.
   **/
  inline void free(void * ptr);

  /**
   * Returns true if the pool has memory allocated to objects, false
   * otherwise. This is a guard that can help prevent a MemoryPool from being
   * destroyed while objects allocated from it still exist.
   *
   * This implementation always returns true, which could prevent heap_pool
   * objects from being destroyed - make sure you understand this in your
   * calling code.
   **/
  inline bool in_use() const;

  /**
   * Returns the size of the memory block backing the pointer. In most cases,
   * this will be at least the same size as the size requested when allocating
   * the pointer.
   *
   * The exception to this is this heap_pool class - on many operating systems,
   * this is not information the OS divulges. In those cases, and in case the
   * pointer isnt' handled by the pool at all, a zero size is returned.
   **/
  inline fhtagn::size_t alloc_size(void * ptr) const;
};


}} // namespace fhtagn::memory

#include <fhtagn/memory/detail/memory_pool.tcc>

#endif // guard
