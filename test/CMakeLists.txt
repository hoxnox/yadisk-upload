# @author $username$ <$usermail$>
# @date $date$
# yadisk-upload cmake tests build script

find_package(Threads)
list(APPEND LIBRARIES ${CMAKE_THREAD_LIBS_INIT})

function(add_gtests gtname sources)
    foreach(source ${sources})
        file(READ "${source}" contents)
        string(REGEX MATCHALL "TEST_?F?\\(([A-Za-z_0-9 ,]+)\\)"
               found_tests ${contents})
        foreach(hit ${found_tests})
            string(REGEX REPLACE
                   ".*\\( *([A-Za-z_0-9]+), *([A-Za-z_0-9]+) *\\).*" "\\1.\\2"
                   test_name ${hit})
            add_test("${gtname}-${test_name}"
                     test_${gtname}_${PROJECT_NAME}
                     --gtest_filter=${test_name})
        endforeach()
    endforeach()
endfunction()

if (NOT USE_SYSTEM_GTEST)
    include(ExternalProject)
    if(NOT GTEST_SRC)
        set(GTEST_SRC $ENV{GTEST})
    endif()
    if(NOT GTEST_SRC)
        set(GTEST_SRC https://github.com/google/googletest/archive/release-1.7.0.tar.gz)
    endif()
    ExternalProject_Add(
        gtestlib
        URL ${GTEST_SRC}
        PREFIX "${CMAKE_CURRENT_BINARY_DIR}/gtest"
        CMAKE_ARGS -DCMAKE_PREFIX_PATH=<INSTALL_DIR>
            -DBUILD_SHARED_LIBS=False
            -DCMAKE_CXX_COMPILER:STRING='${CMAKE_CXX_COMPILER}'
            -DCMAKE_CXX_FLAGS:STRING='${CMAKE_CXX_FLAGS}'
        INSTALL_COMMAND ""
        BUILD_IN_SOURCE 1
        LOG_DOWNLOAD 1
        LOG_UPDATE 1
        LOG_CONFIGURE 1
        LOG_BUILD 1
        LOG_TEST 1
        LOG_INSTALL 1
    )
    ExternalProject_Get_Property(gtestlib BINARY_DIR)
    ExternalProject_Get_Property(gtestlib SOURCE_DIR)
    set(GTEST_INCLUDE_DIRS ${SOURCE_DIR}/include)
    set(GTEST_LIBRARIES ${BINARY_DIR}/libgtest.a)
else ()
    find_package(GTest REQUIRED)
endif()
include_directories(${GTEST_INCLUDE_DIRS})
list(APPEND LIBRARIES ${GTEST_LIBRARIES})

set(TEST_UNIT_SRC
        test.cpp)
add_executable(test_unit_${PROJECT_NAME} ${TEST_UNIT_SRC})
add_dependencies(test_unit_${PROJECT_NAME} gtestlib)
target_link_libraries(test_unit_${PROJECT_NAME} ${LIBRARIES})
add_gtests("unit" "${TEST_UNIT_SRC}")

