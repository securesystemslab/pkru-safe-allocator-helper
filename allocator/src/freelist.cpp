// freelist.cpp
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

#include <freelist.hpp>

namespace alloc
{

    freelist::freelist() = default;

    freelist::freelist(void* start, void* end)
    {
        init(start, end);
    }

    freelist::~freelist() = default;

    void freelist::init(void* start, void* end)
    {
        // reserve first page of region for internal use
        auto new_start = static_cast<char*>(start) + utils::default_alignment;

        // make a new node at the start of that page
        list_ary = static_cast<freelist_node*>(start);

        // initialize the arena and list
        arena.init(start, utils::default_alignment);
        init_list_head(new_start, end);
    }

    void freelist::init_list_head(void* start, void* end)
    {
        head        = alloc_list_node();
        head->start = start;
        head->end   = end;
        head->next  = nullptr;
        tail        = head;
    }

    node_ptr freelist::search(void* addr)
    {

        if(head)
        {
            assert(tail && "Freelist corrupted: tail pointer is null, but head is not null");
            assert(tail->next == nullptr &&
                   "Freelist corrupted: tail pointer does not point to the end of the freelist");
        }

        auto temp = head;
        for(; temp != nullptr && !temp->hasAddr(addr); temp = temp->next)
        {
        }

        // temp is either nullptr or has our address
        return temp;
    }

    void* freelist::split_node(void* ptr, size_t size)
    {

        auto addr   = static_cast<char*>(ptr);
        auto target = search(addr);

        // ensure that a target node exists
        if(!target)
        {
            return nullptr;
        }

        auto new_end = addr + size;

        // ensure the node has enough space to satisfy the allocation
        if(new_end > target->end)
        {
            return nullptr;
        }

        if(addr == target->start)
        {
            // shrink the node from the front
            if(new_end < target->end)
            {
                target->start = new_end;
            }
            else if(new_end == target->end)
            {
                assert(head != nullptr && "Freelist corrupted: head pointer is null");
                // remove this node completely
                auto prev = head;
                if(prev != target)
                {
                    // walk the list until prev
                    while(prev->next != target)
                    {
                        prev = prev->next;
                    }
                }
                else
                {
                    prev = nullptr;
                }

                remove_node(prev, target, target->next);
            }
            else
            {
                fprintf(
                  stderr,
                  "Freelist corrupted: block of sufficient size and alignment cannot satisfy map_region request!\n");
                exit(EXIT_FAILURE);
            }
        }
        else if(addr > target->start)
        {
            // carve a chunk out of this node and split it into 2 nodes
            if(new_end < target->end)
            {
                auto temp = alloc_list_node();

                // set its start and end pointers
                temp->start = new_end;
                temp->end   = target->end;

                // update this node's end address
                target->end = addr;

                // insert node into freelist
                insert(temp, target, target->next);
            }
            else if(new_end == target->end)
            {
                // truncate the node from the end
                target->end = addr;
            }
        }
        return addr;
    }

    node_ptr freelist::alloc_list_node() const
    {
        internal_arena* arena_ptr = const_cast<internal_arena*>(&arena);
        auto temp_allocator       = utils::internal_arena_allocator<freelist_node>(arena_ptr);
        return temp_allocator.allocate(1);
    }

    void freelist::dealloc_list_node(freelist_node* node) const
    {
        internal_arena* arena_ptr = const_cast<internal_arena*>(&arena);
        auto temp_allocator       = utils::internal_arena_allocator<freelist_node>(arena_ptr);
        temp_allocator.deallocate(node, 1);
    }

    void freelist::insert(node_ptr new_node, freelist_node* first, node_ptr last)
    {
        if(last == head)
        {
            head = new_node;
        }
        else
        {
            first->next = new_node;        // first gets a new forward link
            assert((first->end <= new_node->start) &&
                   "Freelist insertion failed: the new block starts before preceding block ends");
        }

        if(first == tail)
        {
            tail = new_node;
        }
        else
        {

            assert((new_node->end <= last->start) &&
                   "Freelist insertion failed: the new block ends before next block starts");
        }

        // new_node links to the old nodes
        new_node->next = last;        // can be nullptr
        if(tail == new_node)
        {
            assert(tail->next == nullptr &&
                   "Freelist corrupted: tail pointer does not point to the end of the freelist");
        }
    }

    void freelist::remove(void* addr, size_t size)
    {
        split_node(addr, size);
    }

    void freelist::remove_node(node_ptr first, freelist_node* target, node_ptr last)
    {
        if(!first && !last)
        {
            head = nullptr;
            tail = nullptr;
        }
        else
        {
            assert((tail && (tail->next == nullptr)) &&
                   "Freelist corrupted: tail pointer does not point to the end of the freelist");
            if(target == head)
            {
                head = last;
            }

            if(target == tail)
            {
                tail = first;
                assert(last == nullptr && "Freelist corrupted: removal target was tail, but last was not nullptr");
            }

            if(first)
                first->next = last;

            assert(tail->next == nullptr &&
                   "Freelist corrupted: tail pointer does not point to the end of the freelist");
        }        // else

        dealloc_list_node(target);
    }

    void freelist::coalesce()
    {
        auto cur = head;
        while(cur != tail)
        {
            // coalesce nodes, and throw away the duplicates
            while(cur->next && (cur->next->start == cur->end))
            {
                cur->end = cur->next->end;

                remove_node(cur, cur->next, cur->next->next);
            }        // while

            // advance the cur pointer
            if(cur->next)
            {
                cur = cur->next;
            }
        }        // while
    }

    void freelist::return_region(void* addr, size_t size)
    {

        // walk freelist until we find the slot for this memory
        char* begin     = static_cast<char*>(addr);
        char* end       = begin + size;
        auto new_node   = alloc_list_node();
        new_node->start = begin;
        new_node->end   = end;

        if(!head && !tail)
        {
            // create new node with this region and set it to head and tail
            insert(new_node, head, tail);
        }
        else
        {

            assert(head && "Freelist corrupted: tail pointer is null, but head is not null");
            auto curr_ptr = head;
            auto next_ptr = curr_ptr->next;

            if(head && (end <= head->start))
            {
                // insert node before this node .. can only happen at head?
                insert(new_node, nullptr, head);
            }
            else if(tail && (begin >= tail->end))
            {
                // insert node after tail
                insert(new_node, tail, nullptr);
            }
            else
            {
                bool inserted = false;
                // loop over freelist
                while(curr_ptr && next_ptr)
                {
                    if((begin >= curr_ptr->end) && (end <= next_ptr->start))
                    {
                        // insert node
                        insert(new_node, curr_ptr, next_ptr);
                        inserted = true;
                        break;
                    }

                    // advance pointers
                    curr_ptr = next_ptr;
                    next_ptr = next_ptr->next;
                }        // while
                assert(inserted && "Error inserting into freelist, no suitable block for insertion");
            }        // else
        }            // else

        // we added to the list, so coalesce all nodes
        coalesce();
    }

    void* freelist::request(void* addr, size_t size, size_t align)
    {
        // no zero sized allocations
        if(size == 0)
            return nullptr;

        // calculate the new size with requested alignment
        auto unaligned  = size % utils::min_alignment;
        size_t new_size = unaligned ? size + (utils::min_alignment - unaligned) : size;

        if(!addr)
        {
            // search for a suitable block
            return find_first_block(align, new_size);
        }
        else
        {
            // if the request if for a fixed mapping, try to satisfy it
            auto next_aligned = utils::get_aligned(addr, align);
            return split_node(next_aligned, new_size);
        }
    }

    void* freelist::find_first_block(size_t align, size_t new_size)
    {
        auto curr = head;

        // ensure that curr was a valid pointer
        while(curr)
        {
            if(curr->size() >= new_size)
            {
                return aligned_alloc(align, new_size, *curr);
            }
            curr = curr->next;
        }
        return nullptr;
    }

    void* freelist::aligned_alloc(size_t align, size_t new_size, freelist_node& curr)
    {
        auto next_aligned = utils::get_aligned(curr.start, align);
        return split_node(next_aligned, new_size);
    }

    void freelist::release_freelist()
    {
        while(head)
        {
            auto temp = head;
            head      = head->next;

            bool mapped = is_mapped_node(temp);

            // delete the list, ignoring any mapped nodes
            if(!mapped)
            {
                delete(temp);
            }
        }
    }

    ptrdiff_t freelist::mem_available()
    {
        auto tmp = head;
        ptrdiff_t sum = 0;
        while(tmp)
        {
            sum += tmp->size();
            tmp = tmp->next;
        }
        return sum;
    }

    bool freelist::is_mapped_node(node_ptr ptr) const
    {
        // list array is guaranteed to be the start of a page
        uintptr_t mapped  = reinterpret_cast<uintptr_t>(list_ary);
        uintptr_t ptr_val = reinterpret_cast<uintptr_t>(ptr);
        return mapped == (ptr_val & mapped);
    }

}        // end namespace alloc
