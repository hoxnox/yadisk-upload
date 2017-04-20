# @author hoxnox <hoxnox@gmail.com>
# @date 20170419 15:29:04
#
# Settings:
#     WITH_SYSTEM_ZLIB - don't build, try to find it
#     ZLIB_ROOT     - where to find
#
# After including this file you'll have the following targets with it's
# properties:
#     zlib
#         INTERFACE_INCLUDE_DIRECTORIES - include dirs
#         IMPORTED_LOCATION             - library


if (NOT TARGET dependencies::zlib)

	include(dependencies/common)
	option(WITH_SYSTEM_ZLIB "Don't build zlib" ON)
	
	add_library(dependencies::zlib UNKNOWN IMPORTED)

	if (WITH_SYSTEM_ZLIB)

		if(NOT ZLIB_ROOT)
			set(ZLIB_ROOT $ENV{ZLIB_ROOT})
		endif()
		find_package(ZLIB REQUIRED)

	else()

		sources_url(ZLIB
			"zlib.net/zlib/zlib-1.2.11.tar.gz"
			"http://zlib.net/zlib-1.2.11.tar.gz")
		ExternalProject_Add(dependencies_zlib
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
		add_dependencies(dependencies::zlib dependencies_zlib)
		set(ZLIB_INCLUDE_DIRS "${STAGING_DIR}/include")
		set(ZLIB_LIBRARIES "${STAGING_DIR}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}z${CMAKE_STATIC_LIBRARY_SUFFIX}")

	endif()

	set_target_properties(dependencies::zlib PROPERTIES
		INTERFACE_INCLUDE_DIRECTORIES "${ZLIB_INCLUDE_DIRS}")
	set_target_properties(dependencies::zlib PROPERTIES
		IMPORTED_LOCATION ${ZLIB_LIBRARIES})

endif()

