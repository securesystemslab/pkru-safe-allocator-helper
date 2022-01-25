// freelist_node.hpp
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

#ifndef ALLOCATOR_FREELIST_NODE_HPP
#define ALLOCATOR_FREELIST_NODE_HPP

#include <cstddef>

namespace alloc
{
    class freelist_node
    {
    public:
        using node_ptr = freelist_node*;
        node_ptr next;
        void* start;
        void* end;

        freelist_node()  = default;
        ~freelist_node() = default;
        bool hasAddr(void* addr);
        ptrdiff_t size();

    private:
    };

    using node_ptr = freelist_node::node_ptr;
}        // namespace alloc
#endif        // ALLOCATOR_FREELIST_NODE_HPP
