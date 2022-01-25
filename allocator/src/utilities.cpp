// utilities.cpp
//
// Copyright 2018 Paul Kirth
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.

#include "utilities.hpp"

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <sys/mman.h>

namespace alloc
{
    namespace utils
    {
        extern const size_t min_alignment          = 4096;              // page size
        extern const size_t default_alignment      = 1UL << 12U;        // also page size (4096)
        extern const int default_prot              = PROT_NONE;        // pages don't have permissions until we map them
        extern const int default_fd                = -1;               // required fd for MAP_ANONYMOUS
        extern const std::ptrdiff_t default_offset = 0;                // no offset allowed w/o backing file
        extern const size_t default_size           = 1UL << 46U;        // by default map half the address space
        extern const int default_flags = MAP_PRIVATE | MAP_ANONYMOUS | MAP_NORESERVE;        // don't back w/ swap

        size_t get_aligned_size(size_t size, size_t align)
        {
            auto alignment    = std::max(align, min_alignment);
            auto mask         = alignment - 1;        // create a mask for the maximal alignment
            auto aligned_bits = size & mask;          // mask the aligned bits
            auto ret          = size;
            assert(((alignment & (min_alignment - 1)) == 0) && "Requested alignment does not meet minimum alignment");

            if(aligned_bits != 0)
            {
                ret = (size & ~mask) + alignment;
            }

            return ret;
        }

        void* get_aligned(void* ptr, size_t alignment)
        {
            auto addr     = reinterpret_cast<uintptr_t>(ptr);
            uintptr_t ret = get_aligned_size(addr, alignment);

            return reinterpret_cast<void*>(ret);
        }
    }        // namespace utils
}        // namespace alloc
