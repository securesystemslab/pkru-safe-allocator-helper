// main.cpp
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
#include "utilities.hpp"

#include <csignal>
#include <cassert>
#include <chrono>
#include <cstdio>
#include <cstring>
#include <sys/mman.h>
#include <thread>
#include <vector>

//const int num_threads = (1 << 13);
 const int num_threads = 0;

void write_all_mem(void* start, size_t len, char pattern)
{
    memset(start, pattern, len);
}

bool check_all_mem(void* start, size_t len, char pattern)
{
    char* it  = static_cast<char*>(start);
    char* end = it + len;
    for(; it != end; it++)
    {
        if(*it != pattern)
        {
            return false;
        }
    }
    return true;
}

int alloc_test(int thread_id)
{
    size_t rand            = std::max(std::rand() % 100, 1);
    size_t size_of_segment = 4096 * rand;
    // size_t size_of_segment = (1UL<<46)/num_threads;
    // auto size_of_segment = 1UL << 59;
    auto prot   = PROT_READ | PROT_WRITE;
    auto flags  = alloc::utils::default_flags;
    auto fd     = alloc::utils::default_fd;
    auto offset = alloc::utils::default_offset;

    char* t = static_cast<char*>(map_region(nullptr, size_of_segment, prot, flags, fd, offset));

    if(t == MAP_FAILED)
    {
        fprintf(stderr, "map_region() size_of_segment=%lu *fd=%d thread=%d failed\n",
                static_cast<long>(size_of_segment), fd, thread_id);
        exit(EXIT_FAILURE);
    };

    printf("Region of size %lu was allocated with address %p from thread %d\n", size_of_segment, t, thread_id);

    assert(t != nullptr);

    char pattern = 'x';
    //pkey_set(1, 0x2);
    write_all_mem(t, size_of_segment, pattern);
    //pkey_set(1, 0x0);
    //assert(check_all_mem(t, size_of_segment, pattern));

    //pkey_set(1, 0x0);
    //unmap_region(t, size_of_segment);
    raise(SIGTERM);
    return 0;
}

int main()
{
    std::vector<std::thread> threads;
    threads.reserve(num_threads);
    alloc_test(0);
    for(size_t i = 1; i <= num_threads; i++)
    {
        threads.emplace_back(std::thread(alloc_test, i));
    }

    for(auto& item : threads)
    {
        item.join();
    }

    return EXIT_SUCCESS;
}
