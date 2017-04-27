# @author hoxnox <hoxnox@gmail.com>
# @date 20170419 15:29:04
#
# Settings:
#     WITH_SYSTEM_OPENSSL - don't build openssl, try to find it
#     OPENSSL_ROOT        - where to find openssl
#
# After including this file you'll have the following targets with it's
# properties:
#     openssl
#         INTERFACE_INCLUDE_DIRECTORIES - include dirs
#         INTERFACE_LINK_LIBRARIES      - dependencies (may include zlib)
#     openssl::ssl
#         IMPORTED_LOCATION             - ssl library
#     openssl::crypto
#         IMPORTED_LOCATION             - crypto library

if (NOT TARGET dependencies::openssl)

	include(dependencies/common)
	option(WITH_SYSTEM_OPENSSL "Don't build openssl with the project" ON)
	
	add_library(dependencies::openssl INTERFACE IMPORTED)

	add_library(dependencies::openssl::ssl UNKNOWN IMPORTED)
	add_dependencies(dependencies::openssl::ssl dependencies_openssl)

	add_library(dependencies::openssl::crypto UNKNOWN IMPORTED)
	add_dependencies(dependencies::openssl::crypto dependencies_openssl)

	if (WITH_SYSTEM_OPENSSL)

		if(NOT OPENSSL_ROOT)
			set(OPENSSL_ROOT $ENV{OPENSSL_ROOT})
		endif()
		find_package(OpenSSL REQUIRED)

	else()

		if (TARGET dependencies::zlib)
			get_property(ZLIB_INCLUDE_DIR TARGET dependencies::zlib
				PROPERTY INTERFACE_INCLUDE_DIRECTORIES)
			set_property(TARGET dependencies::openssl APPEND PROPERTY
				INTERFACE_INCLUDE_DIRECTORIES "${ZLIB_DIRECTORIES}")

			set(CFG_SUFFIX "zlib -I${ZLIB_INCLUDE_DIR} -L${STAGING_DIR}/lib")

			get_property(ZLIB_LIBRARY TARGET dependencies::zlib PROPERTY IMPORTED_LOCATION)
			set_target_properties(dependencies::openssl PROPERTIES
				INTERFACE_LINK_LIBRARIES ${ZLIB_LIBRARY})

			set(DEPENDENCIES dependencies::zlib)
		else()
			set(CFG_SUFFIX "no-zlib no-zlib-dynamic")
		endif()

		sources_url(OPENSSL
			"openssl/openssl/openssl-1.1.0e.tar.gz"
			"https://www.openssl.org/source/openssl-1.1.0e.tar.gz")
		ExternalProject_Add(dependencies_openssl
			URL ${OPENSSL_URL}
			URL_HASH SHA256=57be8618979d80c910728cfc99369bf97b2a1abd8f366ab6ebdee8975ad3874c
			CONFIGURE_COMMAND ./config --prefix=<INSTALL_DIR> no-shared ${CFG_SUFFIX}
			INSTALL_DIR "${STAGING_DIR}"
			BUILD_IN_SOURCE 1
			LOG_DOWNLOAD 1
			LOG_UPDATE 1
			LOG_CONFIGURE 1
			LOG_BUILD 1
			LOG_TEST 1
			LOG_INSTALL 1
		)
		add_dependencies(dependencies::openssl dependencies_openssl)
		add_dependencies(dependencies_openssl ${DEPENDENCIES})
		set(OPENSSL_INCLUDE_DIR "${STAGING_DIR}/include")
		set(OPENSSL_SSL_LIBRARY    "${STAGING_DIR}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}ssl${CMAKE_STATIC_LIBRARY_SUFFIX}")
		set(OPENSSL_CRYPTO_LIBRARY "${STAGING_DIR}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}crypto${CMAKE_STATIC_LIBRARY_SUFFIX}")

	endif()

	set_property(TARGET dependencies::openssl APPEND PROPERTY
		INTERFACE_INCLUDE_DIRECTORIES "${OPENSSL_INCLUDE_DIR}")
	set_target_properties(dependencies::openssl::ssl PROPERTIES
		IMPORTED_LOCATION "${OPENSSL_SSL_LIBRARY}")
	set_target_properties(dependencies::openssl::crypto PROPERTIES
		IMPORTED_LOCATION "${OPENSSL_CRYPTO_LIBRARY}")

endif()

