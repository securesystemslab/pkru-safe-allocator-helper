// safemap.cpp
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

#include "safemap.h"

#include "vma.hpp"

#include <atomic>
#include <csignal>
#include <iostream>
#include <mutex>
#include <signal.h>

static alloc::vma global_vma;
std::mutex vma_lock;
std::atomic<uint64_t> gate_count(0);
__sighandler_t prevSigTermAction = nullptr;

void segTermHandler(int signum)
{
    if(signum != SIGTERM && signum != SIGINT)
    {
        signal(signum, SIG_DFL);
        raise(signum);
        return;
    }
    {
        std::lock_guard<std::mutex> map_guard(vma_lock);
        std::cout << "[call-gates]  Passed: " << gate_count << "\n";
        global_vma.print_mem();
    }

    // Resume program exit.
    if (!prevSigTermAction) {
        signal(signum, SIG_DFL);
    } else {
        signal(signum, prevSigTermAction);
    }
    raise(signum);
}

extern "C"
{

    void* map_region(void* addr, size_t length, int prot, int flags, int fd, ptrdiff_t offset)
    {
        std::lock_guard<std::mutex> map_guard(vma_lock);
        return global_vma.map_region(addr, length, prot, flags, fd, offset);
    }

    int unmap_region(void* addr, size_t length)
    {
        std::lock_guard<std::mutex> map_guard(vma_lock);
        return global_vma.unmap_region(addr, length);
    }

    int vma_pkey()
    {
        return global_vma.get_pkey();
    }

    bool is_safe_address(void* addr)
    {
        return global_vma.is_safe_addr(addr);
    }

    void inc_gate_count()
    {
        gate_count++;
    }

    static void __attribute__((constructor)) register_term_handler()
    {
        prevSigTermAction = signal(SIGTERM, segTermHandler);
    }
}
