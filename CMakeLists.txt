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
option(WITH_TOOLS          "Build yadisk-upload executable" ON)
option(WITH_LIBS           "Install libraries" OFF)
option(WITH_LOG            "Logging replacement" "")

########################################################################
# general

project(yadisk-upload)

set(CMAKE_CXX_STANDARD 14)
set(yadisk_upload_VERSION_MAJOR 0)
set(yadisk_upload_VERSION_MINOR 0)
set(yadisk_upload_VERSION_PATCH 4)
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
list(APPEND LIBRARIES ${CMAKE_THREAD_LIBS_INIT} ${CMAKE_DL_LIBS})

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
			"openssl/openssl/openssl-1.1.0c.tar.gz"
			"https://www.openssl.org/source/openssl-1.1.0c.tar.gz")
		ExternalProject_Add(openssl
			URL ${OPENSSL_URL}
			URL_HASH SHA256=fc436441a2e05752d31b4e46115eb89709a28aef96d4fe786abe92409b2fd6f5
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
		find_package(Boost 1.63.0 COMPONENTS filesystem system REQUIRED)
	else()
		sources_url(BOOST
			"boost.org/boost/boost_1_63_0.tar.gz"
			"https://sourceforge.net/projects/boost/files/boost/1.63.0/boost_1_63_0.tar.gz/download")
		ExternalProject_Add(boost
			URL ${BOOST_URL}
			DOWNLOAD_NAME boost_1_63_0.tar.gz
			URL_HASH SHA256=fe34a4e119798e10b8cc9e565b3b0284e9fd3977ec8a1b19586ad1dec397088b
			CONFIGURE_COMMAND "./bootstrap.sh" --prefix=<INSTALL_DIR> --with-libraries=system,filesystem
			BUILD_COMMAND ""
			INSTALL_COMMAND "./b2" link=static threading=multi --ignore-site-config install
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

if (WITH_TOOLS)
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
endif()
	
########################################################################
# easyloggingpp

if (NOT WITH_LOG)
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
endif()

########################################################################
# yandex api

add_library(yandex_api
	src/yandex/disk.cpp
	src/yandex/tls_transport.cpp
	src/yandex/transport.cpp)

target_include_directories(yandex_api PRIVATE "${LOGGING_REPLACEMENT_DIR}")
target_include_directories(yandex_api BEFORE PUBLIC ${OPENSSL_INCLUDE_DIR})
if (NOT WITH_LOG)
	add_dependencies(yandex_api easyloggingpp)
endif()

# replace logging.hpp
if (NOT WITH_LOG)
	set(WITH_LOG "${PROJECT_BINARY_DIR}/log")
	configure_file("${PROJECT_SOURCE_DIR}/src/Logging.hpp" "${WITH_LOG}/logging.hpp" COPYONLY)
endif()
include_directories("${EASYLOGGINGPP_INCLUDE_DIR}" "${WITH_LOG}")

########################################################################

if (TOBUILD_DEPS)
	add_custom_target(build_dependencies ${CMAKE_COMMAND} -DSECOND_PASS=1 ${CMAKE_SOURCE_DIR} DEPENDS ${TOBUILD_DEPS})
else()
	add_custom_target(build_dependencies)
endif()

########################################################################
# yadisk-uplaod

if (WITH_TOOLS)
	add_executable(yadisk-upload
		src/yadisk-upload.cpp
		src/Logging.cpp)
	add_dependencies(yadisk-upload build_dependencies yandex_api)
	target_link_libraries(yadisk-upload yandex_api ${LIBRARIES})
	target_include_directories(yadisk-upload PRIVATE ${INCLUDE_DIR})
endif()

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
# remote build

set(REMOTE_SERVER $ENV{REMOTE_BUILD_SERVER})
set(REMOTE_SSHCONF $ENV{REMOTE_BUILD_SERVER_SSHCONF})
set(REMOTE_CMAKE_PARAMS "-DCMAKE_BUILD_TYPE=Release -DWITH_SYSTEM_OPENSSL=1 -DWITH_SYSTEM_BOOST=0 -DWITH_TOOLS=1 -DWITH_LIBS=0")
set(REMOTE_VENDOR_FILES "docopt/docopt.cpp/docopt.cpp-725519e2441b47e51f4c3ee35fc4edb926bcc262.tar.gz easylogging/easyloggingpp/easyloggingpp_v9.84.tar.gz boost.org/boost/boost_1_63_0.tar.gz")
add_custom_target(remote COMMAND
	${CMAKE_CURRENT_SOURCE_DIR}/misc/build.remote ${REMOTE_SERVER}
	                                              ${REMOTE_SSHCONF}
	                                              ${REMOTE_CMAKE_PARAMS}
	                                              ${REMOTE_VENDOR_FILES}
	                                              ${PROJECT_SOURCE_DIR})

########################################################################
# installation

set(CPACK_SET_DESTDIR ON)

if (WITH_LIBS)
	INSTALL(TARGETS yandex_api DESTINATION lib)
	INSTALL(DIRECTORY include/yandex DESTINATION include)
endif()
if (WITH_TOOLS)
	INSTALL(TARGETS yadisk-upload DESTINATION bin)
endif()
#INSTALL(DIRECTORY domedir DESTINATION share/${PROJECT_NAME})
SET(CPACK_PACKAGE_NAME yadisk-upload)
set(DELIM "")
if (WITH_SYSTEM_OPENSSL)
	set(CPACK_DEBIAN_PACKAGE_DEPENDS "openssl (>= 1.0.1)${DELIM}${CPACK_DEBIAN_PACKAGE_DEPENDS}")
	set(DELIM ", ")
endif()
if (WITH_SYSTEM_BOOST)
	set(CPACK_DEBIAN_PACKAGE_DEPENDS "libboost-dev (>= 1.47)${DELIM}${CPACK_DEBIAN_PACKAGE_DEPENDS}")
	set(DELIM ", ")
endif()
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Yandex Disk files upload utility and library.")
SET(CPACK_PACKAGE_VENDOR "Merder Kim <hoxnox@gmail.com>")
SET(CPACK_PACKAGE_CONTACT "Merder Kim <hoxnox@gmail.com>")
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

