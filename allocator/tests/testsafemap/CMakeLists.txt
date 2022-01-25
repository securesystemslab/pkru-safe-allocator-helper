include(GoogleTest)

file(GLOB SRCS *.cpp)

include_directories("${gtest_SOURCE_DIR}/include" ${gtest_SOURCE_DIR})
add_executable(testsafemap ${SRCS})

target_link_libraries(testsafemap
        safemap
        gtest
        gmock
        )

gtest_discover_tests(testsafemap)
