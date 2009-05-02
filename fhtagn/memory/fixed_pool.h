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
#ifndef FHTAGN_MEMORY_FIXED_POOL_H
#define FHTAGN_MEMORY_FIXED_POOL_H

#ifndef __cplusplus
#error You are trying to include a C++ only header file
#endif

#include <fhtagn/fhtagn.h>

#include <fhtagn/memory/utility.h>
#include <fhtagn/memory/memory_pool.h>
#include <fhtagn/memory/common.h>
#include <fhtagn/threads/lock_policy.h>

namespace fhtagn {
namespace memory {

/**
 * The fixed_pool class implements a MemoryPool that allocates space from a
 * fixed-sized chunk of preallocated memory. fixed_pool does not allocate this
 * chunk of memory itself, nor frees it (by default, that can be changed via
 * the adoption_policyT template parameter), but merely subdivides it.
 *
 * As a result, fixed_pool works equally well with a heap and stack memory;
 * it's the perfect pool for allocation a reasonably small amount of stack or
 * heap at the start of your program, to allocate memory from for fatal error
 * handling.
 *
 * Either way, you must ensure yourself that the memory handled by fixed_pool
 * lives at least as long as the pool itself, and the pool itself should not
 * be destroyed as long as in_use() returns true.
 *
 * And to mix things up a little, there's also the possibility to influence
 * the alignment of memory allocated from the pool. Alignment is handled via
 * a block size; essentially memory will internally always be allocated in
 * multiples of this block size, and aligned on such a block boundary.
 *
 * In most cases, you don't need to change the block alignment, but if you feel
 * the need to squeeze the last bytes out of things, feel free to do so.
 **/
template <
  typename mutexT = fhtagn::threads::fake_mutex,
  typename block_alignmentT = block_alignment<>,
  template <typename> class adoption_policyT = fhtagn::memory::ignore_array_policy
>
class fixed_pool : public adoption_policyT<char>
{
public:
  /**
   * Convenience typedefs
   **/
  typedef mutexT            mutex_t;
  typedef block_alignmentT  block_alignment_t;

  /**
   * The constructor accepts a pointer to a block of memory of the given size.
   * Note that the class does not take ownership of this memory; you must ensure
   * yourself that the memory lives at least as long as this fixed_pool
   * instance.
   **/
  inline fixed_pool(void * memblock, fhtagn::size_t size);

  /**
   * API - see memory_pool.h for details
   **/
  inline void * alloc(fhtagn::size_t size);
  inline void * realloc(void * ptr, fhtagn::size_t new_size);
  inline void free(void * ptr);
  inline bool in_use() const;
  inline fhtagn::size_t alloc_size(void * ptr) const;

private:

  /**
   * Helper structure, that is the header of each free or allocated segment in
   * the fixed_pool.
   **/
  struct segment
  {
    enum {
      FREE      = 0,
      ALLOCATED = ~char(0)
    };

    enum {
      LAST_SEGMENT = fhtagn::size_t(0)
    };

    segment(fhtagn::size_t _size)
      : size(_size - sizeof(segment))
      , status(FREE)
    {
      marker = LAST_SEGMENT;
    }

    inline fhtagn::size_t full_size() const
    {
      return size + sizeof(segment);
    }

    static inline fhtagn::size_t header_size()
    {
      return block_alignmentT::adjust_size(sizeof(segment));
    }

    fhtagn::size_t  size;
    char            status;
    union {
      segment *       next;
      fhtagn::size_t  marker;
    };
  };

  /**
   * Finds and allocates a free segment of the given size, splitting larger
   * segments if necessary.
   **/
  inline segment * allocate_segment(fhtagn::size_t size);

  /**
   * Finds the segment in which ptr resides, or throws if the ptr can't be
   * found.
   **/
  inline segment * find_segment_for(void * ptr) const;

  /**
   * Merges contiguous free segments.
   **/
  inline void defragment_free_list();

  void *          m_memblock;
  fhtagn::size_t  m_size;

  segment *       m_start;

  mutable mutex_t m_mutex;
};


}} // namespace fhtagn::memory

#include <fhtagn/memory/detail/fixed_pool.tcc>

#endif // guard
