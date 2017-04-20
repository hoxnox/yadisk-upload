# @author hoxnox <hoxnox@gmail.com>
# @date 20170419 15:29:04

if (NOT TARGET dependencies::openssl)
	option(WITH_SYSTEM_OPENSSL "Don't build openssl with the project" ON)
	
	if (WITH_SYSTEM_OPENSSL)
		if(NOT OPENSSL_ROOT)
			set(OPENSSL_ROOT $ENV{OPENSSL_ROOT})
		endif()
		find_package(OpenSSL REQUIRED)

		add_library(dependencies::openssl INTERFACE IMPORTED)
		add_library(dependencies::openssl::ssl UNKNOWN IMPORTED)
		add_library(dependencies::openssl::crypto UNKNOWN IMPORTED)

		set_target_properties(dependencies::openssl PROPERTIES
			INTERFACE_INCLUDE_DIRECTORIES "${OPENSSL_INCLUDE_DIR}")
		set_target_properties(dependencies::openssl::ssl PROPERTIES
			IMPORTED_LOCATION "${OPENSSL_SSL_LIBRARY}")
		set_target_properties(dependencies::openssl::crypto PROPERTIES
			IMPORTED_LOCATION "${OPENSSL_CRYPTO_LIBRARY}")
	else()
		add_library(dependencies::openssl INTERFACE IMPORTED)
		add_library(dependencies::openssl::ssl STATIC IMPORTED)
		add_library(dependencies::openssl::crypto STATIC IMPORTED)
		add_dependencies(dependencies::openssl::ssl dependencies_openssl)
		add_dependencies(dependencies::openssl::crypto dependencies_openssl)

		sources_url(OPENSSL
			"openssl/openssl/openssl-1.1.0c.tar.gz"
			"https://www.openssl.org/source/openssl-1.1.0c.tar.gz")
		if (TARGET dependencies::zlib)
			set(CFG_SUFFIX "zlib -I${STAGING_DIR}/include -L${STAGING_DIR}/lib")
			get_property(ZLIB_LIBRARY TARGET dependencies::zlib PROPERTY IMPORTED_LOCATION)
			set_target_properties(dependencies::openssl PROPERTIES
				INTERFACE_LINK_LIBRARIES ${ZLIB_LIBRARY})
			set(DEPENDENCIES dependencies::zlib)
		else()
			set(CFG_SUFFIX "no-zlib no-zlib-dynamic")
		endif()
		message(STATUS "OpenSSL CFG_SUFFIX: ${CFG_SUFFIX}")
		ExternalProject_Add(dependencies_openssl
			URL ${OPENSSL_URL}
			URL_HASH SHA256=fc436441a2e05752d31b4e46115eb89709a28aef96d4fe786abe92409b2fd6f5
			CONFIGURE_COMMAND ./config --prefix=<INSTALL_DIR> no-shared ${CFG_SUFFIX}
			INSTALL_DIR "${STAGING_DIR}"
			BUILD_IN_SOURCE 1
			LOG_DOWNLOAD 1
			LOG_UPDATE 1
			LOG_CONFIGURE 0
			LOG_BUILD 1
			LOG_TEST 1
			LOG_INSTALL 1
		)
		add_dependencies(dependencies::openssl dependencies_openssl)
		add_dependencies(dependencies_openssl ${DEPENDENCIES})

		set_target_properties(dependencies::openssl PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${STAGING_DIR}/include")
		set_target_properties(dependencies::openssl::ssl PROPERTIES IMPORTED_LOCATION
			"${STAGING_DIR}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}ssl${CMAKE_STATIC_LIBRARY_SUFFIX}")
		set_target_properties(dependencies::openssl::crypto PROPERTIES IMPORTED_LOCATION
			"${STAGING_DIR}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}crypto${CMAKE_STATIC_LIBRARY_SUFFIX}")

	endif()
endif()

