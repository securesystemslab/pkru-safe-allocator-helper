# CMakeLists.txt
# 
# Copyright 2018 Paul Kirth
# 
# Permission is hereby granted, free of charge, to any person obtaining a
# copy of this software and associated documentation files (the "Software"),
# to deal in the Software without restriction, including without limitation
# the rights to use, copy, modify, merge, publish, distribute, sublicense,
# and/or sell copies of the Software, and to permit persons to whom the
# Software is furnished to do so, subject to the following conditions:
# 
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
# FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
# IN THE SOFTWARE.

cmake_minimum_required(VERSION 3.9)
#include_directories(PUBLIC ${AllocatorProject}/allocator/include)
#add_library(safemap safemap.cpp utilities.cpp)
add_library(safemap safemap.cpp utilities.cpp mpk.cpp vma.cpp freelist.cpp freelist_node.cpp internal_arena.cpp)
target_include_directories(safemap PUBLIC
        $<BUILD_INTERFACE:${AllocatorProject}/allocator/include>
        $<INSTALL_INTERFACE:include>)
#set_target_properties(safemap PROPERTIES PUBLIC_HEADER "safemap.h")

install(TARGETS safemap
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib)
install(DIRECTORY ../include/ DESTINATION include)
#install(TARGETS safemap DESTINATION lib)
#install(FILES ${AllocatorProject}/allocator/include/safemap.h DESTINATION include)

