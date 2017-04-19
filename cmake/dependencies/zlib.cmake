# @author hoxnox <hoxnox@gmail.com>
# @date 20170419 15:29:04

if (NOT TARGET dependencies::zlib)
	option(WITH_SYSTEM_ZLIB "Don't build zlib" ON)
	
	if (WITH_SYSTEM_ZLIB)
		if(NOT ZLIB_ROOT)
			set(ZLIB_ROOT $ENV{ZLIB_ROOT})
		endif()
		find_package(ZLIB REQUIRED)
	   	include_directories(BEFORE ${ZLIB_INCLUDE_DIR})
		add_library(dependencies::zlib UNKNOWN IMPORTED)
		set_target_properties(dependencies::zlib PROPERTIES IMPORTED_LOCATION ${ZLIB_LIBRARIES})
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
		add_library(dependencies::zlib STATIC IMPORTED)
		add_dependencies(dependencies::zlib dependencies_zlib)
		set_target_properties(dependencies::zlib PROPERTIES IMPORTED_LOCATION
			"${STAGING_DIR}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}z${CMAKE_STATIC_LIBRARY_SUFFIX}")
	endif()
endif()

