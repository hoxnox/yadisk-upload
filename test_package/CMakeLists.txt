PROJECT(SnappyTest)
cmake_minimum_required(VERSION 2.8.12)
set(CMAKE_CXX_STANDARD 11)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

find_package(Threads REQUIRED)

ADD_EXECUTABLE(test test.cpp)
TARGET_LINK_LIBRARIES(test ${CONAN_LIBS} ${CMAKE_THREAD_LIBS_INIT})
