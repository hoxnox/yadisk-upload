# @author hoxnox <hoxnox@gmail.com>
# @date 20170419 15:29:04
#
# Settings:
#     WITH_SYSTEM_LIBRESSL - don't build libressl, try to find it
#     LIBRESSL_ROOT        - where to find libressl
#
# After including this file you'll have the following targets with it's
# properties:
#     libressl
#         INTERFACE_INCLUDE_DIRECTORIES - include dirs
#         INTERFACE_LINK_LIBRARIES      - dependencies
#     libressl::ssl
#         IMPORTED_LOCATION             - ssl library
#     libressl::crypto
#         IMPORTED_LOCATION             - crypto library

if (NOT TARGET dependencies::libressl)

	include(dependencies/common)
	option(WITH_SYSTEM_LIBRESSL "Don't build libressl with the project" ON)
	
	add_library(dependencies::libressl INTERFACE IMPORTED)

	add_library(dependencies::libressl::ssl UNKNOWN IMPORTED)
	add_dependencies(dependencies::libressl::ssl dependencies_libressl)

	add_library(dependencies::libressl::crypto UNKNOWN IMPORTED)
	add_dependencies(dependencies::libressl::crypto dependencies_libressl)

	if (WITH_SYSTEM_LIBRESSL)

		if(NOT LIBRESSL_ROOT)
			set(LIBRESSL_ROOT $ENV{LIBRESSL_ROOT})
		endif()
		find_package(LibreSSL REQUIRED)

	else()

		sources_url(LIBRESSL
			"libressl.org/libressl/libressl-2.5.3.tar.gz"
			"https://ftp.openbsd.org/pub/OpenBSD/LibreSSL/libressl-2.5.3.tar.gz")
		ExternalProject_Add(dependencies_libressl
			URL ${LIBRESSL_URL}
			URL_HASH SHA256=14e34cc586ec4ce5763f76046dcf366c45104b2cc71d77b63be5505608e68a30
			CMAKE_ARGS ${CMAKE_STATIC_RUNTIME_EXTRA_ARGS} -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR> -DBUILD_NC=False -DBUILD_SHARED=False
			INSTALL_DIR "${STAGING_DIR}"
			BUILD_IN_SOURCE 1
			LOG_DOWNLOAD 1
			LOG_UPDATE 1
			LOG_CONFIGURE 1
			LOG_BUILD 1
			LOG_TEST 1
			LOG_INSTALL 1
		)
		add_dependencies(dependencies::libressl dependencies_libressl)
		set(LIBRESSL_INCLUDE_DIR "${STAGING_DIR}/include")
		set(LIBRESSL_SSL_LIBRARY    "${STAGING_DIR}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}ssl${CMAKE_STATIC_LIBRARY_SUFFIX}")
		set(LIBRESSL_CRYPTO_LIBRARY "${STAGING_DIR}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}crypto${CMAKE_STATIC_LIBRARY_SUFFIX}")

	endif()

	set_property(TARGET dependencies::libressl APPEND PROPERTY
		INTERFACE_INCLUDE_DIRECTORIES "${LIBRESSL_INCLUDE_DIR}")
	set_target_properties(dependencies::libressl::ssl PROPERTIES
		IMPORTED_LOCATION "${LIBRESSL_SSL_LIBRARY}")
	set_target_properties(dependencies::libressl::crypto PROPERTIES
		IMPORTED_LOCATION "${LIBRESSL_CRYPTO_LIBRARY}")

endif()

