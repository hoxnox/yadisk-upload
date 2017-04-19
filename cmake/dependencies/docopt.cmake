# @author hoxnox <hoxnox@gmail.com>
# @date 20170419 15:29:04

if (NOT TARGET dependencies::docopt)
	option(WITH_SYSTEM_DOCOPT "Don't build docopt with the project" OFF)

	if (WITH_SYSTEM_DOCOPT)
		set(DOCOPT_USE_STATIC_LIBS ON)
		if(NOT DOCOPT_ROOT)
			set(DOCOPT_ROOT $ENV{DOCOPT_ROOT})
		endif()
		find_package(Docopt REQUIRED)
		add_library(dependencies::docopt UNKNOWN IMPORTED)
		set_property(TARGET depepndencies::docopt APPEND
			PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${DOCOPT_INCLUDE_DIR})
		set_property(TARGET depepndencies::docopt APPEND
			PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${DOCOPT_LIBRARIES})
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
		add_library(dependencies::docopt STATIC IMPORTED)
		add_dependencies(dependencies::docopt dependencies_docopt)
		set_target_properties(dependencies::docopt PROPERTIES
			INTERFACE_INCLUDE_DIRECTORIES "${STAGING_DIR}/include")
		get_property(LIB64 GLOBAL PROPERTY FIND_LIBRARY_USE_LIB64_PATHS)
		if (${LIB64} STREQUAL "TRUE")
			set(LIBSUFFIX 64)
		else()
			set(LIBSUFFIX "")
		endif()
		set_target_properties(dependencies::docopt PROPERTIES IMPORTED_LOCATION
			"${STAGING_DIR}/lib${LIBSUFFIX}/${CMAKE_STATIC_LIBRARY_PREFIX}docopt${CMAKE_STATIC_LIBRARY_SUFFIX}")
	endif()
endif()

