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

option(WITH_TESTS "Build tests."  OFF)
option(WITH_DOCS  "Generate docs" OFF)

########################################################################
# general

project(yadisk-upload)

set(CMAKE_CXX_STANDARD 11)
set(yadisk_upload_VERSION_MAJOR 0)
set(yadisk_upload_VERSION_MINOR 0)
set(yadisk_upload_VERSION_PATCH 0)
add_definitions("-DVERSION=\"${yadisk_upload_VERSION_MAJOR}.${yadisk_upload_VERSION_MINOR}.${yadisk_upload_VERSION_PATCH}\"")

set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}"
                      "${PROJECT_SOURCE_DIR}/cmake/Modules")

include_directories("${PROJECT_SOURCE_DIR}/src")
set(STAGING_DIR ${PROJECT_SOURCE_DIR}/build/staging)


get_property(LIB64 GLOBAL PROPERTY FIND_LIBRARY_USE_LIB64_PATHS)
if (${LIB64} STREQUAL "TRUE")
    set(LIBSUFFIX 64)
else()
    set(LIBSUFFIX "")
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
	include(ExternalProject)
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
	include(ExternalProject)
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
list(APPEND INCLUDE_DIR "${EASYLOGGING_INCLUDE_DIR}")

########################################################################
# yadisk-uplaod

add_executable(yadisk-upload
	src/yadisk-upload.cpp
	src/Logging.cpp)
add_dependencies(yadisk-upload docopt easyloggingpp)
target_link_libraries(yadisk-upload ${LIBRARIES})
target_include_directories(yadisk-upload PRIVATE ${INCLUDE_DIR})

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

