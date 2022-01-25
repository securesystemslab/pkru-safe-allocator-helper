// utilities.hpp
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

//
// Created by paul on 2/1/18.
//

#ifndef ALLOCATOR_UTILITIES_HPP
#define ALLOCATOR_UTILITIES_HPP

#include <cstddef>

namespace alloc
{
    namespace utils
    {
        extern const size_t min_alignment;            /// the minimum alignement: page size (4096)
        extern const size_t default_alignment;        /// the default alignment: page size (4096)
        extern const int default_prot;                /// the default protections: PROT_NONE
        extern const int default_flags;               /// default flags: MAP_PRIVATE | MAP_ANONYMOUS | MAP_NORESERVE
        extern const int default_fd;                  /// default file descriptor: -1
        extern const ptrdiff_t default_offset;        /// default file offset: 0
        extern const size_t default_size;             /// default size of protected region

        /**
         * Calculate the aligned size
         * @param size size of request
         * @param align requested alignment
         * @return the size rounded up for alignment requirements
         */
        size_t get_aligned_size(size_t size, size_t align);

        /**
         * Request an aligned pointer
         * @param ptr requested address
         * @param alignment required alignment
         * @return A pointer with the required alignment
         */
        void* get_aligned(void* ptr, size_t alignment);
    }        // namespace utils
}        // namespace alloc

#endif        // ALLOCATOR_UTILITIES_HPP
