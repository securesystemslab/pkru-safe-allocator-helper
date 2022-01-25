//
// Created by Paul Kirth on 6/6/18.
//

#include "gtest/gtest.h"
#include <vma.hpp>

namespace
{

    static alloc::vma v;

    struct foo
    {
        void* ptr;
        size_t size;
    };

    void* alloc_pages(size_t size)
    {
        auto prot   = PROT_READ | PROT_WRITE;
        auto flags  = alloc::utils::default_flags;
        auto fd     = alloc::utils::default_fd;
        auto offset = alloc::utils::default_offset;
        return v.map_region(nullptr, size, prot, flags, fd, offset);
    }

    class VmaTest : public ::testing::Test
    {
    protected:
        // You can remove any or all of the following functions if its body
        // is empty.

        VmaTest()
        {
            // You can do set-up work for each test here.
        }

        virtual ~VmaTest()
        {
            // You can do clean-up work that doesn't throw exceptions here.
        }

        // If the constructor and destructor are not enough for setting up
        // and cleaning up each test, you can define the following methods:

        virtual void SetUp()
        {
            // Code here will be called immediately after the constructor (right
            // before each test).
        }

        virtual void TearDown()
        {
            // Code here will be called immediately after each test (right
            // before the destructor).
        }

        // Objects declared here can be used by all tests in the test case for Foo.
    };

    // Tests that the Foo::Bar() method does Abc.
    TEST_F(VmaTest, MethodMapRegionCannotAllocateExcessAddressRange)
    {
        // the first page of the region is reserved for internal use
        auto j = alloc_pages(alloc::utils::default_size + alloc::utils::default_alignment);
        EXPECT_EQ(j, MAP_FAILED);
    }

    TEST_F(VmaTest, MethodMapRegionCanAllocateFullAddressRange)
    {
        auto size = (alloc::utils::default_size);
        auto j    = alloc_pages(size);
        EXPECT_NE(j, MAP_FAILED);
        auto res = v.unmap_region(j, size);
        EXPECT_EQ(res, 0);
    }

    TEST_F(VmaTest, MethodMapRegionCanAllocateInLoop)
    {
        for(auto size = 1UL << 12; size < alloc::utils::default_size; size = size << 1)
        {
            auto j = alloc_pages(size);
            EXPECT_NE(j, MAP_FAILED);
            auto res = v.unmap_region(j, size);
            EXPECT_EQ(res, 0);
        }

        const auto len = 100;
        std::vector<foo> blocks(len);

        for(int i = 0; i < len; i++)
        {
            auto seed = rand() % 16;
            auto size = 4096 << seed;
            auto j    = alloc_pages(size);
            EXPECT_NE(j, MAP_FAILED);
            blocks[i].ptr  = j;
            blocks[i].size = size;
        }

        for(auto item : blocks)
        {
            auto res = v.unmap_region(item.ptr, item.size);
            EXPECT_EQ(res, 0);
        }
    }

    TEST_F(VmaTest, MethodMapRegionSubPage)
    {
        auto j = alloc_pages(16);
        EXPECT_NE(j, MAP_FAILED);
    }

    TEST_F(VmaTest, MethodMapRegionZeroLengthFails)
    {
        auto j = alloc_pages(0);
        EXPECT_EQ(j, MAP_FAILED);
    }

}        // namespace
