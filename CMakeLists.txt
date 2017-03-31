# Copyright 2017 hoxnox <hoxnox@gmail.com>
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

cmake_minimum_required(VERSION 3.3.0)

########################################################################
# options

option(WITH_TESTS          "Build tests."  OFF)
option(WITH_DOCS           "Generate docs" OFF)
option(WITH_SYSTEM_BOOST   "Don't build boost with the project" ON)
option(WITH_SYSTEM_OPENSSL "Don't build openssl with the project" ON)

########################################################################
# general

project(yadisk-upload)

set(CMAKE_CXX_STANDARD 14)
set(yadisk_upload_VERSION_MAJOR 0)
set(yadisk_upload_VERSION_MINOR 0)
set(yadisk_upload_VERSION_PATCH 1)
add_definitions("-DVERSION=\"${yadisk_upload_VERSION_MAJOR}.${yadisk_upload_VERSION_MINOR}.${yadisk_upload_VERSION_PATCH}\"")

set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}"
                      "${PROJECT_SOURCE_DIR}/cmake/Modules")

include_directories("${PROJECT_SOURCE_DIR}/include")
set(STAGING_DIR ${PROJECT_SOURCE_DIR}/build/staging)


get_property(LIB64 GLOBAL PROPERTY FIND_LIBRARY_USE_LIB64_PATHS)
if (${LIB64} STREQUAL "TRUE")
    set(LIBSUFFIX 64)
else()
    set(LIBSUFFIX "")
endif()

include(Vendoring)
include(ExternalProject)

########################################################################
# threads

find_package(Threads REQUIRED)
list(APPEND LIBRARIES ${CMAKE_THREAD_LIBS_INIT})

#########################################################################
# openssl

if (WITH_SYSTEM_OPENSSL)
	if(NOT OPENSSL_ROOT)
		set(OPENSSL_ROOT $ENV{OPENSSL_ROOT})
	endif()
	find_package(OpenSSL REQUIRED)
else()

	set(ZLIB_ROOT "${STAGING_DIR}")
	if (SECOND_PASS)
		find_package(ZLIB)
	else()
		sources_url(ZLIB
			"zlib.net/zlib/zlib-1.2.11.tar.gz"
			"http://zlib.net/zlib-1.2.11.tar.gz")
		ExternalProject_Add(zlib
			URL ${ZLIB_URL}
			URL_HASH SHA256=c3e5e9fdd5004dcb542feda5ee4f0ff0744628baf8ed2dd5d66f8ca1197cb1a1
			CONFIGURE_COMMAND ./configure --prefix=<INSTALL_DIR> --static
			INSTALL_DIR "${STAGING_DIR}"
			BUILD_IN_SOURCE 1
			LOG_DOWNLOAD 1
			LOG_UPDATE 1
			LOG_CONFIGURE 1
			LOG_BUILD 1
			LOG_TEST 1
			LOG_INSTALL 1
		)
		list(APPEND TOBUILD_DEPS zlib)
	endif()

	set(OPENSSL_ROOT_DIR "${STAGING_DIR}")
	set(OPENSSL_USE_STATIC_LIBS ON)
	if (SECOND_PASS)
		find_package(OpenSSL)
	else()
		sources_url(OPENSSL
			"openssl/openssl/openssl-1.0.1t.tar.gz"
			"https://www.openssl.org/source/openssl-1.0.1t.tar.gz")
		ExternalProject_Add(openssl
			URL ${OPENSSL_URL}
			URL_HASH SHA256=4a6ee491a2fdb22e519c76fdc2a628bb3cec12762cd456861d207996c8a07088
			CONFIGURE_COMMAND ./config --prefix=<INSTALL_DIR> no-shared no-zlib
			INSTALL_DIR "${STAGING_DIR}"
			BUILD_IN_SOURCE 1
			LOG_DOWNLOAD 1
			LOG_UPDATE 1
			LOG_CONFIGURE 1
			LOG_BUILD 1
			LOG_TEST 1
			LOG_INSTALL 1
		)
		list(APPEND TOBUILD_DEPS openssl)
	endif()

endif()

if (ZLIB_FOUND)
	include_directories(BEFORE ${ZLIB_INCLUDE_DIR})
	list(APPEND LIBRARIES ${ZLIB_LIBRARIES})
endif()

if (OpenSSL_FOUND)
	include_directories(BEFORE ${OPENSSL_INCLUDE_DIR})
	list(APPEND LIBRARIES ${OPENSSL_LIBRARIES})
endif()

#########################################################################
# boost

if (WITH_SYSTEM_BOOST)
	if(NOT BOOST_ROOT)
		set(BOOST_ROOT $ENV{BOOST_ROOT})
	endif()
	find_package(Boost 1.47.0 COMPONENTS filesystem system REQUIRED)
else()
	set(BOOST_ROOT "${STAGING_DIR}")
	set(Boost_USE_STATIC_LIBS ON)
	if (SECOND_PASS)
		find_package(Boost 1.47.0 COMPONENTS filesystem system REQUIRED)
	else()
		sources_url(BOOST
			"boost.org/boost/boost_1_61_0.tar.gz"
			"https://sourceforge.net/projects/boost/files/boost/1.61.0/boost_1_61_0.tar.gz/download")
		ExternalProject_Add(boost
			URL ${BOOST_URL}
			DOWNLOAD_NAME boost_1_61_0.tar.gz
			URL_HASH SHA256=a77c7cc660ec02704c6884fbb20c552d52d60a18f26573c9cee0788bf00ed7e6
			CONFIGURE_COMMAND "./bootstrap.sh" --prefix=<INSTALL_DIR> --with-libraries=system,filesystem
			BUILD_COMMAND "./b2" --ignore-site-config
			INSTALL_COMMAND "./b2" --ignore-site-config install
			BUILD_IN_SOURCE 1
			INSTALL_DIR "${STAGING_DIR}"
			LOG_DOWNLOAD 1
			LOG_UPDATE 1
			LOG_CONFIGURE 1
			LOG_BUILD 1
			LOG_TEST 1
			LOG_INSTALL 1
		)
		list(APPEND TOBUILD_DEPS boost)
	endif()
endif()

if (Boost_FOUND)
	include_directories(BEFORE ${Boost_INCLUDE_DIRS})
	list(APPEND LIBRARIES ${Boost_LIBRARIES})
endif()

########################################################################
# docopt

if (WITH_SYSTEM_DOCOPT)
	set(DOCOPT_USE_STATIC_LIBS ON)
	if(NOT DOCOPT_ROOT)
		set(DOCOPT_ROOT $ENV{DOCOPT_ROOT})
	endif()
	find_package(Docopt REQUIRED)
	add_custom_target(docopt)
else()
	sources_url(DOCOPT
		"docopt/docopt.cpp/docopt.cpp-725519e2441b47e51f4c3ee35fc4edb926bcc262.tar.gz"
		"https://github.com/docopt/docopt.cpp/archive/725519e2441b47e51f4c3ee35fc4edb926bcc262.tar.gz")
	ExternalProject_Add(docopt
		URL ${DOCOPT_URL}
		DOWNLOAD_NAME docopt.cpp.tar.gz
		URL_HASH SHA256=cc340fce5c6694c5d1ae0bf999c5134e46bc06bc00fd4e7be1790d40697f807a
		CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR> -DCMAKE_BUILD_TYPE=Release
		BUILD_IN_SOURCE 1
		INSTALL_DIR "${STAGING_DIR}"
		LOG_DOWNLOAD 1
		LOG_UPDATE 1
		LOG_CONFIGURE 1
		LOG_BUILD 1
		LOG_TEST 1
		LOG_INSTALL 1
	)
	set(DOCOPT_LIBRARIES ${STAGING_DIR}/lib${LIBSUFFIX}/libdocopt.a)
	set(DOCOPT_INCLUDE_DIR ${STAGING_DIR}/include)
endif()
list(APPEND INCLUDE_DIR "${DOCOPT_INCLUDE_DIR}")
list(APPEND LIBRARIES "${DOCOPT_LIBRARIES}")

########################################################################
# easyloggingpp

if (WITH_SYSTEM_EASYLOGGINGPP)
	if(NOT EASYLOGGINGPP_ROOT)
		set(EASYLOGGINGPP_ROOT $ENV{EASYLOGGINGPP_ROOT})
	endif()
	find_package(EASYLOGGINGPP REQUIRED)
	add_custom_target(easyloggingpp)
else()
	sources_url(EASYLOGGINGPP
		"easylogging/easyloggingpp/easyloggingpp_v9.84.tar.gz"
		"https://github.com/easylogging/easyloggingpp/releases/download/9.84/easyloggingpp_v9.84.tar.gz")
	ExternalProject_Add(easyloggingpp
		URL ${EASYLOGGINGPP_URL}
		DOWNLOAD_NAME easyloggingpp_v9.84.tar.gz
		URL_HASH SHA256=c4d51df897180120b3450e11814437121f9a3e6a090b917e4f32adfdb3ebf974
		CONFIGURE_COMMAND mkdir -p "${STAGING_DIR}/include"
		BUILD_COMMAND ""
		INSTALL_COMMAND "cp" ./easylogging++.h <INSTALL_DIR>/include/
		BUILD_IN_SOURCE 1
		INSTALL_DIR "${STAGING_DIR}"
		LOG_DOWNLOAD 1
		LOG_UPDATE 1
		LOG_CONFIGURE 1
		LOG_BUILD 1
		LOG_TEST 1
		LOG_INSTALL 1
	)
	set(EASYLOGGINGPP_INCLUDE_DIR "${STAGING_DIR}/include")
endif()
include_directories(BEFORE "${EASYLOGGINGPP_INCLUDE_DIR}")

# replace logging.hpp
set(LOGGING_REPLACEMENT_DIR "${PROJECT_BINARY_DIR}/log")
configure_file("${PROJECT_SOURCE_DIR}/src/Logging.hpp" "${LOGGING_REPLACEMENT_DIR}/logging.hpp" COPYONLY)
include_directories("${LOGGING_REPLACEMENT_DIR}")

########################################################################

if (TOBUILD_DEPS)
	add_custom_target(build_dependencies ${CMAKE_COMMAND} -DSECOND_PASS=1 ${CMAKE_SOURCE_DIR} DEPENDS ${TOBUILD_DEPS})
else()
	add_custom_target(build_dependencies)
endif()

########################################################################
# yandex api

add_library(yandex_api
	src/yandex/disk.cpp
	src/yandex/tls_transport.cpp
	src/yandex/transport.cpp)
list(APPEND LIBRARIES yandex_api)

target_include_directories(yandex_api PRIVATE "${LOGGING_REPLACEMENT_DIR}")
target_include_directories(yandex_api BEFORE PUBLIC ${OPENSSL_INCLUDE_DIR})
add_dependencies(yandex_api build_dependencies easyloggingpp)

########################################################################
# yadisk-uplaod

add_executable(yadisk-upload
	src/yadisk-upload.cpp
	src/Logging.cpp)
add_dependencies(yadisk-upload docopt yandex_api)
target_link_libraries(yadisk-upload ${LIBRARIES})
target_include_directories(yadisk-upload PRIVATE ${INCLUDE_DIR})

########################################################################
# tests and docs

if(WITH_DOCS)
	add_subdirectory(doc)
endif()

if(WITH_TESTS)
	enable_testing()
	add_subdirectory(test)
endif()

########################################################################
# installation

set(CPACK_SET_DESTDIR ON)

#INSTALL(TARGETS
#	targetname
#	DESTINATION bin)
#INSTALL(FILES filepath DESTINATION bin
#	PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ
#	            GROUP_EXECUTE GROUP_READ
#	            WORLD_READ WORLD_EXECUTE)
#INSTALL(DIRECTORY domedir DESTINATION share/${PROJECT_NAME})
SET(CPACK_PACKAGE_NAME yadisk-upload)
#if (WITH_SYSTEM_SOMELIB)
#	SET(CPACK_DEBIAN_PACKAGE_DEPENDS "libsomelib (>=1.1.1)")
#	SET(CPACK_RPM_PACKAGE_REQUIRES "libsomelib >= 1.1.1")
#endif()
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "")
SET(CPACK_PACKAGE_VENDOR "$uername$ <hoxnox@gmail.com>")
SET(CPACK_PACKAGE_CONTACT ${CPACK_PACKAGE_VENDOR})
SET(CPACK_PACKAGE_DESCRIPTION_FILE "${PROJECT_SOURCE_DIR}/README.md")
SET(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}/LICENSE")
SET(CPACK_PACKAGE_VERSION_MAJOR ${yadisk_upload_VERSION_MAJOR})
SET(CPACK_PACKAGE_VERSION_MINOR ${yadisk_upload_VERSION_MINOR})
SET(CPACK_PACKAGE_VERSION_PATCH ${yadisk_upload_VERSION_PATCH})
SET(CPACK_DEBIAN_PACKAGE_SECTION "misc")
SET(CPACK_RPM_PACKAGE_GROUP "Applications/System")
SET(CPACK_RPM_PACKAGE_LICENSE "Apache-2.0")
SET(CPACK_STRIP_FILES TRUE)
INCLUDE(CPack)

