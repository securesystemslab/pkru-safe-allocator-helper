// internal_arena.cpp
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

#include <cstddef>
#include <internal_arena.hpp>

namespace alloc
{
    void alloc::internal_arena::init(void* start_addr, size_t len)
    {
        start  = (start_addr);
        length = (len);
        curr   = static_cast<char*>(start);
        end    = (static_cast<char*>(start) + length);
    }

    void* internal_arena::allocate(size_t n, size_t align)
    {
        size_t remaining = static_cast<char*>(end) - curr;
        auto temp        = static_cast<void*>(curr);
        void* offset     = std::align(align, n, temp, remaining);

        if(offset != nullptr)
        {
            curr = static_cast<char*>(offset) + n;
            return offset;
        }
        else
        {
            return ::operator new(n);
        }
    }

    void internal_arena::dealocate(void* p, std::size_t n)
    {
        // currently the internal_arena_allocator takes care of
        // deallocating all requests to deallocate memory.

        // only delete memory from outside the arena
        if(p < start || p > end)
        {
            // delete(p,n);
        }
    }

    bool internal_arena::in_arena(void* p)
    {
        return p >= start && p <= end;
    }

}        // namespace alloc
