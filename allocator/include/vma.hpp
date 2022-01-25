// vma.hpp
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

#ifndef ALLOCATOR_VMA_HPP
#define ALLOCATOR_VMA_HPP

#include "freelist.hpp"
#include "mpk.h"

#include <cerrno>
#include <cstring>
#include <sys/mman.h>

namespace alloc
{
    class vma
    {
    public:
        vma() noexcept;
        ~vma() noexcept;
        void* map_region(void* addr, size_t length, int prot, int flags, int fd, ptrdiff_t offset) noexcept;
        int unmap_region(void* addr, size_t length) noexcept;
        int get_pkey() noexcept;
        bool is_safe_addr(void* addr) noexcept;
        void print_mem() noexcept;

    private:
        void* region_start;
        void* region_end;
        ptrdiff_t size;
        freelist list;
        int pkey;
    };

}        // namespace alloc

#endif        // ALLOCATOR_VMA_HPP
