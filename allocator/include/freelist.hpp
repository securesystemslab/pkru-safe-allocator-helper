// freelist.hpp
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

#ifndef ALLOCATOR_FREELIST_HPP
#define ALLOCATOR_FREELIST_HPP

#include "freelist_node.hpp"
#include "internal_arena.hpp"
#include "internal_arena_allocator.hpp"
#include "utilities.hpp"

#include <cassert>

namespace alloc
{

    class freelist
    {

    public:
        static void fake_deleter(void* ptr) {}

        freelist(void* start, void* end);

        freelist();

        ~freelist();

        void* split_node(void* ptr, size_t size);

        void insert(node_ptr new_node, freelist_node* first, node_ptr last);

        void remove(void* addr, size_t size);

        void remove_node(node_ptr first, freelist_node* target, node_ptr last);

        void coalesce();

        node_ptr search(void* addr);

        void* request(void* addr, size_t size, size_t align);

        void* request(size_t size)
        {
            return request(nullptr, size, utils::default_alignment);
        }

        void return_region(void* addr, size_t size);

        void init(void* start, void* end);

        void release_freelist();
        ptrdiff_t mem_available();

        bool is_mapped_node(node_ptr ptr) const;

    private:
        node_ptr head;
        node_ptr tail;
        node_ptr list_ary;
        internal_arena arena;

        void* find_first_block(size_t align, size_t new_size);
        void* aligned_alloc(size_t align, size_t new_size, freelist_node& curr);
        node_ptr alloc_list_node() const;
        void dealloc_list_node(freelist_node* node) const;
        void init_list_head(void* start, void* end);
    };

}        // end namespace alloc

#endif        // ALLOCATOR_FREELIST_HPP
