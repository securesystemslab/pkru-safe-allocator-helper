// safemap.h
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

#ifndef ALLOCATOR_SAFEMAP_H
#define ALLOCATOR_SAFEMAP_H

#include <stddef.h>

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * Maps a set of pages from a reserved pool, with a pkey set. Uses the same interface as mmap() syscall
     * @param addr The requested start address of a region. If null the first address that satisfies alignment
     * requirments
     * @param length The size of the requested region in bytes. Should be aligned to page boundaries
     * @param prot Requested page protections (see mmap(2))
     * @param flags Flags
     * @param fd Backing File descriptor (unsupported -- only use MAP_ANONYMOUS)
     * @param offset Offset into mmaped file (unsupported -- only use 0)
     * @return A pointer to the mapped region on success, or MAP_FAILED in case of error
     */
    void* map_region(void* addr, size_t length, int prot, int flags, int fd, ptrdiff_t offset);

    /**
     * Unmaps the region, returning it to the pool of available pages. Does not return to the OS. Same as munmap()
     * syscall
     * @param addr Start address of the region. Must be page aligned. (see munmap())
     * @param length Size of the region to unmap in bytes. (see munmap())
     * @return 0 on success or -1 on failure (see munmap())
     */
    int unmap_region(void* addr, size_t length);

    /**
     * Get the value of the pkey used for the trusted region/vma
     * @return The value of the pkey used when mapping trusted pages
     */
    int vma_pkey();

    /**
     * Checks if the pointer belongs to the trusted region. Does not check for liveness, only if the pointer belongs to
     * correct address range
     * @param addr The pointer to check
     * @return true if the pointer's address falls within the reserved region, otherwise false
     */
    bool is_safe_address(void* addr);

    void inc_gate_count();

    static void __attribute__((constructor)) register_term_handler();

#ifdef __cplusplus
};
#endif

#endif        // ALLOCATOR_SAFEMAP_H
