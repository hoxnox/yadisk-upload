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
option(WITH_TOOLS          "Build yadisk-upload executable" ON)
option(WITH_LIBS           "Install libraries" ON)
option(WITH_LOG            "Logging replacement" "")

########################################################################
# general

project(yadisk-upload)

set(CMAKE_CXX_STANDARD 14)
set(yadisk_upload_VERSION_MAJOR 0)
set(yadisk_upload_VERSION_MINOR 0)
set(yadisk_upload_VERSION_PATCH 9)
add_definitions("-DVERSION=\"${yadisk_upload_VERSION_MAJOR}.${yadisk_upload_VERSION_MINOR}.${yadisk_upload_VERSION_PATCH}\"")

include_directories("${PROJECT_SOURCE_DIR}/include")
set(STAGING_DIR "${CMAKE_BINARY_DIR}/staging")

########################################################################
# threads

find_package(Threads REQUIRED)

########################################################################
# yandex api


set(YANDEX_API_SRC
	src/yandex/disk.cpp
	src/yandex/tls_transport.cpp
	src/yandex/transport.cpp
)

add_library(yandex_api ${YANDEX_API_SRC})

########################################################################
# requirements

find_package(Boost COMPONENTS headers system filesystem REQUIRED)
find_package(OpenSSL REQUIRED)
target_link_libraries(yandex_api Boost::headers Boost::system Boost::filesystem openssl::openssl)

########################################################################
# pluggable logging

# replace logging.hpp
if (NOT WITH_LOG)
	set(WITH_LOG "${PROJECT_BINARY_DIR}/log")
	configure_file("${PROJECT_SOURCE_DIR}/src/logging.hpp" "${PROJECT_BINARY_DIR}/log/logging.hpp" COPYONLY)
	include_directories(BEFORE "${PROJECT_BINARY_DIR}/log")
else()
	include_directories(BEFORE "${WITH_LOG}")
endif()

########################################################################
# yadisk-uplaod

if (WITH_TOOLS)
	add_executable(yadisk-upload
		src/yadisk-upload.cpp
		src/stdlogger.cpp)
	add_dependencies(yadisk-upload yandex_api)
	find_package(docopt REQUIRED)
	target_link_libraries(yadisk-upload yandex_api docopt_s ${CMAKE_THREAD_LIBS_INIT})
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

