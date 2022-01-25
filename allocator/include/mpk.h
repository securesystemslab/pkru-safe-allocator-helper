// mpk.h
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

// This file supplies a set of APIs similar or identical to those found in glibc-2.27
// We mimic these for future compatibility with standard libraries.

#ifndef ALLOCATOR_MPK_H
#define ALLOCATOR_MPK_H

#include <stddef.h>

extern "C"
{

    /**
     * Wrapper for RDPKRU instruction
     * @return value of pkru register
     *
     */
    unsigned int pkru_pkey_read();

    /**
     * Wrapper for WRPKRU instruction
     * @param pkru new PKRU value
     */
    void pkru_pkey_write(unsigned int pkru);

    /**
     * Gets the protection bits for key from PKRU register
     *
     * @param key The protection key to check
     * @return the protection bits for key
     */
    int pkru_pkey_get(int key);

    /**
     * Sets the protection bits for key in the PKRU register
     *
     * @param pkey  the protection key to set the bits for
     * @param rights the Read/Write bits to set
     * @return -1 error, 0 success
     */
    int pkru_pkey_set(int key, unsigned int rights);

    /***
     * Set the protection bits in the PTE for Addr, acording to pkey
     * @param addr the address to protect w/ pkey
     * @param len  the size of the region to set mpk protection
     * @param prot The normal OS page permissions
     * @param pkey The mpk protection key to assign
     * @return 0 success, -1 error
     */
    int pkru_pkey_mprotect(void* addr, size_t len, int prot, int pkey);

    /**
     * Allocate a new protection key
     * @return the new protection key on success, -1 on failure
     */
    int pkru_pkey_alloc();

    /**
     * Release the protection key
     * @param pkey the protection key to release
     * @return 0 on success, -1 on failure
     */
    int pkru_pkey_free(unsigned long pkey);
};

#endif        // ALLOCATOR_MPK_H
