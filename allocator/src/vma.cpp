// vma.cpp
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

#include <vma.hpp>
#include <iostream>

namespace alloc
{
    vma::vma() noexcept
    {
        using namespace utils;

        int prot         = default_prot;
        int flags        = default_flags;
        int fd           = default_fd;
        ptrdiff_t offset = default_offset;
        auto size_flag   = default_size + utils::default_alignment;

        region_start = mmap(nullptr, size_flag, prot, flags, fd, offset);
        if(region_start == MAP_FAILED)
        {
            fprintf(stderr, "mmap size_of_segment=%ld *fd=%d failed %s\n", size_flag, fd, strerror(errno));
            exit(EXIT_FAILURE);
        }

        region_end            = static_cast<char*>(region_start) + size_flag;
        size                  = size_flag;
        auto new_region_start = static_cast<char*>(region_start) + utils::default_alignment;

        // give read write protection to the leading page in the safe zone;
        pkey = pkey_alloc(0, 0);                                            // allocate pkey from OS
        pkey_mprotect(region_start, size_flag, PROT_NONE, pkey);        // protect entire region w/ pkey
        pkey_mprotect(region_start, utils::default_alignment, PROT_READ | PROT_WRITE, pkey);        // enable read/write

        // initialize the freelist with the adjusted region
        list.init(region_start, region_end);

        // don't give  the underlying request our page for internal use
        // (advances the region start pointer past our reserved 1 page allocation)
        region_start = new_region_start;
    }

    vma::~vma() noexcept
    {
        pkey_set(pkey, 0x0);
        auto avail = list.mem_available();
        auto mem = size - avail;
        std::cout << "[pkalloc]  Used "<< mem/4096 <<" of "<< size/4096 <<" pages\n"; 
        list.release_freelist();
    }

    void* vma::map_region(void* addr, size_t length, int prot, int flags, int fd, ptrdiff_t offset) noexcept
    {
        auto pages = list.request(addr, length, utils::default_alignment);
        if(pages == nullptr || pages == MAP_FAILED)
        {
            return MAP_FAILED;
        }

        auto err = pkey_mprotect(pages, length, prot, pkey);
        if(err == -1)
        {
            list.return_region(addr, length);
            return MAP_FAILED;
        }
        return pages;
    }

    int vma::unmap_region(void* addr, size_t length) noexcept
    {
        using namespace utils;
        auto res = mmap(addr, length, PROT_NONE, default_flags | MAP_FIXED, default_fd, default_offset);
        if(res == MAP_FAILED)
        {
            return -1;
        }

        auto err =
          pkey_mprotect(addr, length, PROT_NONE, pkey);        // remove permissions before returning to freelist
        list.return_region(addr, length);                      // reinsert region into freelist;
        return err;
    }

    int vma::get_pkey() noexcept
    {
        // public API for getting the pkey used in our defense
        return pkey;
    }

    bool vma::is_safe_addr(void* addr) noexcept
    {
        return (addr >= region_start) && (addr <= region_end);
    }
    
    void vma::print_mem() noexcept
    {
        pkey_set(pkey, 0x0);
        auto avail = list.mem_available();
        auto mem = size - avail;
        std::cout << "[pkalloc]  Used "<< mem/4096 <<" of "<< size/4096 <<" pages\n";
    }
}        // namespace alloc
