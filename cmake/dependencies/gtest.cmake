# @author hoxnox <hoxnox@gmail.com>
# @date 20170322 16:08:27
#
# Settings:
#     WITH_SYSTEM_GTEST - don't build openssl, try to find it
#     GTEST_ROOT        - where to find openssl
#
# After including this file you'll have the following targets with it's
# properties:
#     gtest
#         INTERFACE_INCLUDE_DIRECTORIES - include dirs
#         INTERFACE_LINK_LIBRARIES      - dependencies (may include pthread)
#     gtest::gtest
#         IMPORTED_LOCATION             - gtest library
#     gtest::main
#         IMPORTED_LOCATION             - gtest_main library

if (NOT TARGET dependencies::gtest)

	include(dependencies/common)

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

	option(WITH_SYSTEM_GTEST "Don't build gtest with the project" OFF)

	add_library(dependencies::gtest UNKNOWN IMPORTED)

	add_library(dependencies::gtest::main UNKNOWN IMPORTED)
	add_dependencies(dependencies::gtest::main dependencies::gtest)

	add_library(dependencies::gtest::gtest UNKNOWN IMPORTED)
	add_dependencies(dependencies::gtest::gtest dependencies::gtest)


	if (WITH_SYSTEM_GTEST)

		if(NOT GTEST_ROOT)
			set(GTEST_ROOT $ENV{GTEST_ROOT})
		endif()
		find_package(GTest REQUIRED)

	else()

		sources_url(GTEST
			"google/googletest/googletest-release-1.8.0.tar.gz"
			"https://github.com/google/googletest/archive/release-1.8.0.tar.gz")
		ExternalProject_Add(dependencies_gtest
			URL ${GTEST_URL}
			DOWNLOAD_NAME googletest-release-1.8.0.tar.gz
			URL_HASH SHA256=58a6f4277ca2bc8565222b3bbd58a177609e9c488e8a72649359ba51450db7d8 
			#PREFIX "${CMAKE_CURRENT_BINARY_DIR}/gtest"
			CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
				-DBUILD_SHARED_LIBS=False
				-DCMAKE_CXX_COMPILER:STRING='${CMAKE_CXX_COMPILER}'
				-DCMAKE_CXX_FLAGS:STRING='${CMAKE_CXX_FLAGS}'
			INSTALL_DIR "${STAGING_DIR}"
			BUILD_IN_SOURCE 1
			LOG_DOWNLOAD 1
			LOG_UPDATE 1
			LOG_CONFIGURE 1
			LOG_BUILD 1
			LOG_TEST 1
			LOG_INSTALL 1
		)
		add_dependencies(dependencies::gtest dependencies_gtest)
		set(GTEST_INCLUDE_DIRS "${STAGING_DIR}/include")
		set(GTEST_LIBRARIES "${STAGING_DIR}/lib/libgtest.a")
		set(GTEST_MAIN_LIBRARIES "${STAGING_DIR}/lib/libgtest_main.a")

	endif()

	set_property(TARGET dependencies::gtest APPEND
		PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${GTEST_INCLUDE_DIRS})
	set_property(TARGET dependencies::gtest::gtest APPEND
		PROPERTY IMPORTED_LOCATION ${GTEST_LIBRARIES})
	set_property(TARGET dependencies::gtest::main APPEND
		PROPERTY IMPORTED_LOCATION ${GTEST_MAIN_LIBRARIES})

	find_package(Threads)
	set_property(TARGET dependencies::gtest APPEND
		PROPERTY INTERFACE_LINK_LIBRARIES ${CMAKE_THREAD_LIBS_INIT})

endif()
