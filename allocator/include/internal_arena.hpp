// internal_arena.hpp
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

#ifndef ALLOCATOR_INTERNAL_ALLOC_HPP
#define ALLOCATOR_INTERNAL_ALLOC_HPP

#include <cstddef>
#include <memory>

namespace alloc
{
    /**
     * A bump arena allocator to use when bootstrapping other allocators, i.e. jemalloc
     */
    class internal_arena
    {
    public:
        /**
         * default Constructor
         */
        internal_arena() noexcept = default;

        /**
         * initializes the allocator using the region pointed to by pStart
         * @param start_addr start
         * @param len
         */
        void init(void* start_addr, size_t len);

        /**
         * Allocates a region of n bytes with alignment align,
         * allocation requests exceeding the range of the region
         * are backed by the default allocator through operator new()
         * @param n size of allocation
         * @param align alignment
         * @return pointer to the start of the allocated region
         */
        void* allocate(size_t n, size_t align);

        /**
         * dealocates memory from the region. Memory from the backed region is
         * leaked, while allocations backed by the default allocator have
         * delete called on them.
         * @param p pointer to the region to deallocate
         * @param n size of the allocation to deallocate
         */
        void dealocate(void* p, std::size_t n);

        /**
         * Checks if a pointer value is within the arena
         * @param p the pointer to check
         * @return true if the pointer is within the arena, false otherwise
         */
        bool in_arena(void* p);

    private:
        void* start;          // pointer to start of arena
        void* end;            // pointer to end of arena
        size_t length;        // length of backed region
        char* curr;           // pointer to the top of the arena
    };
}        // namespace alloc

#endif        // ALLOCATOR_INTERNAL_ALLOC_HPP
