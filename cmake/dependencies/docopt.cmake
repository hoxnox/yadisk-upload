# @author hoxnox <hoxnox@gmail.com>
# @date 20170419 15:29:04
#
# Settings:
#     WITH_SYSTEM_DOCOPT - don't build, try to find it
#     DOCOPT_ROOT        - where to find
#
# After including this file you'll have the following targets with it's
# properties:
#     docopt
#         INTERFACE_INCLUDE_DIRECTORIES - include dirs
#         IMPORTED_LOCATION             - library

if (NOT TARGET dependencies::docopt)

	include(dependencies/common)
	option(WITH_SYSTEM_DOCOPT "Don't build docopt with the project" OFF)

	add_library(dependencies::docopt UNKNOWN IMPORTED)

	if (WITH_SYSTEM_DOCOPT)

		set(DOCOPT_USE_STATIC_LIBS ON)
		if(NOT DOCOPT_ROOT)
			set(DOCOPT_ROOT $ENV{DOCOPT_ROOT})
		endif()
		find_package(Docopt REQUIRED)

	else()

		sources_url(DOCOPT
			"docopt/docopt.cpp/docopt.cpp-725519e2441b47e51f4c3ee35fc4edb926bcc262.tar.gz"
			"https://github.com/docopt/docopt.cpp/archive/725519e2441b47e51f4c3ee35fc4edb926bcc262.tar.gz")
		ExternalProject_Add(dependencies_docopt
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
		add_dependencies(dependencies::docopt dependencies_docopt)
		get_property(LIB64 GLOBAL PROPERTY FIND_LIBRARY_USE_LIB64_PATHS)
		set(LIBSUFFIX "")
		if (LIB64)
			if (${LIB64} STREQUAL "TRUE")
				set(LIBSUFFIX 64)
			endif()
		endif()
		set(DOCOPT_INCLUDE_DIR "${STAGING_DIR}/include")
		set(DOCOPT_LIBRARIES "${STAGING_DIR}/lib${LIBSUFFIX}/${CMAKE_STATIC_LIBRARY_PREFIX}docopt${CMAKE_STATIC_LIBRARY_SUFFIX}")

	endif()

	set_property(TARGET dependencies::docopt APPEND
		PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${DOCOPT_INCLUDE_DIR})
	set_property(TARGET dependencies::docopt APPEND
		PROPERTY IMPORTED_LOCATION ${DOCOPT_LIBRARIES})

endif()

