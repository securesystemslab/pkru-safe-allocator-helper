//
// Created by Paul Kirth on 6/6/18.
//

#include "gtest/gtest.h"
#include <freelist.hpp>
#include <sys/mman.h>

namespace
{
    const size_t init_size = 4096 * 5;

    class FreelistTest : public ::testing::Test
    {
    protected:
        // You can remove any or all of the following functions if its body
        // is empty.

        FreelistTest() {}

        virtual ~FreelistTest()
        {
            // You can do clean-up work that doesn't throw exceptions here.
        }

        // If the constructor and destructor are not enough for setting up
        // and cleaning up each test, you can define the following methods:

        virtual void SetUp()
        {
            // Code here will be called immediately after the constructor (right
            // before each test).
            auto prot   = PROT_READ | PROT_WRITE;
            auto flags  = alloc::utils::default_flags;
            auto fd     = alloc::utils::default_fd;
            auto offset = alloc::utils::default_offset;
            buff        = mmap(nullptr, init_size, prot, flags, fd, offset);
            ASSERT_NE(buff, MAP_FAILED);
        }

        virtual void TearDown()
        {
            // Code here will be called immediately after each test (right
            // before the destructor).
            if(buff)
            {
                munmap(buff, init_size);
                buff = nullptr;
            }

            ASSERT_EQ(buff, nullptr);
        }

        // Objects declared here can be used by all tests in the test case for Foo.
        alloc::freelist l;
        void* buff;
    };

    TEST_F(FreelistTest, InitReservesFirstPage)
    {
        char* start = static_cast<char*>(buff);
        char* end   = start + init_size;

        l.init(start, end);
        auto j = l.search(start);
        ASSERT_EQ(j, nullptr);
        for(auto ptr = start; ptr < start + alloc::utils::default_alignment; ptr++)
        {
            j = l.search(start);
            ASSERT_EQ(j, nullptr);
        }

        j = l.search(start + alloc::utils::default_alignment);
        ASSERT_NE(j, nullptr);
    }

    TEST_F(FreelistTest, Insert) {}

    TEST_F(FreelistTest, Remove) {}

    TEST_F(FreelistTest, RemoveNode) {}

    TEST_F(FreelistTest, Coalesce) {}

    TEST_F(FreelistTest, SearchAssertsCorruptedList) {}

    TEST_F(FreelistTest, SearchFindsValidAddr) {}

    TEST_F(FreelistTest, SearchReturnsNullBadAddr) {}

    TEST_F(FreelistTest, SpliteturnsNullBadAddr) {}

    TEST_F(FreelistTest, SplitReturnsNullTooSmall) {}

    TEST_F(FreelistTest, SplitReturnsNull) {}

    TEST_F(FreelistTest, Request) {}

    TEST_F(FreelistTest, ReturnRegion) {}

    TEST_F(FreelistTest, ReleaseFreelist) {}

    TEST_F(FreelistTest, IsMapped) {}

}        // namespace
