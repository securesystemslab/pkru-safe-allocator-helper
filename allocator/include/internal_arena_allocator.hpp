// internal_arena_allocator.hpp
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

#ifndef ALLOCATOR_INTERNAL_ARENA_ALLOCATOR_HPP
#define ALLOCATOR_INTERNAL_ARENA_ALLOCATOR_HPP

#include "internal_arena.hpp"

#include <type_traits>

namespace alloc
{
    namespace utils
    {
        template <typename T>
        class internal_arena_allocator
        {
        public:
            template <typename U>
            friend struct internal_arena_allocator;
            using value_type                             = T;
            using pointer                                = T*;
            using propagate_on_container_copy_assignment = std::true_type;
            using propagate_on_container_move_assignment = std::true_type;
            using propagate_on_container_swap            = std::true_type;

            explicit internal_arena_allocator(internal_arena* a) : arena(a) {}

            template <typename U>
            internal_arena_allocator(internal_arena_allocator<U> const& rhs) : arena(rhs.arena)
            {
            }

            pointer allocate(size_t n)
            {
                return static_cast<pointer>(arena->allocate(n * sizeof(T), alignof(T)));
            }

            void deallocate(pointer p, size_t n)
            {

                if(!arena->in_arena(p))
                {
                    ::delete(p);
                }
                // arena->dealocate(p, n* sizeof(T));
            }

            template <typename U>
            bool operator==(internal_arena_allocator<U> const& rhs) const
            {
                return arena == rhs.arena;
            }

            template <typename U>
            bool operator!=(internal_arena_allocator<U> const& rhs) const
            {
                return arena != rhs.arena;
            }

        private:
            internal_arena* arena;
        };        // class internal_arena_allocator
    }             // namespace utils
}        // namespace alloc
#endif        // ALLOCATOR_INTERNAL_ARENA_ALLOCATOR_HPP
