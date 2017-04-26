# @author hoxnox <hoxnox@gmail.com>
# @date 20170419 15:29:04
#
# Settings:
#     WITH_SYSTEM_BOOST - don't build boost, try to find it
#     BOOST_ROOT        - where to find boost
#
# After including this file you'll have the following targets with it's
# properties:
#     boost
#         INTERFACE_INCLUDE_DIRECTORIES - include dirs
#         INTERFACE_LINK_LIBRARIES      - dependencies (may include openssl and zlib)
#     boost::system
#         IMPORTED_LOCATION             - system library
#     boost::filesystem
#         IMPORTED_LOCATION             - filesystem library

if (NOT TARGET dependencies::boost)

	include(dependencies/common)
	option(WITH_SYSTEM_BOOST "Don't build boost with the project" ON)
	option(BOOST_STATIC_CRT  "Use static runtime libraries" ON)
	
	add_library(dependencies::boost UNKNOWN IMPORTED)

	add_library(dependencies::boost::system UNKNOWN IMPORTED)
	add_dependencies(dependencies::boost::system dependencies::boost)

	add_library(dependencies::boost::filesystem UNKNOWN IMPORTED)
	add_dependencies(dependencies::boost::filesystem dependencies::boost)

	if (WITH_SYSTEM_BOOST)

		if(NOT BOOST_ROOT)
			set(BOOST_ROOT $ENV{BOOST_ROOT})
		endif()
		find_package(Boost 1.47.0 COMPONENTS system filesystem REQUIRED)

	else()
		if (BOOST_STATIC_CRT)
			set(EXTRA_FLAGS ${EXTRA_FLAGS} "runtime-link=static")
		endif()

		if (NOT WIN32)
			set(BOOTSTRAP_CMD "./bootstrap.sh")
			if (TARGET dependencies::openssl)
				get_property(OPENSSL_INCLUDE_DIR TARGET dependencies::openssl
					PROPERTY INTERFACE_INCLUDE_DIRECTORIES)
				set(EXTRA_FLAGS ${EXTRA_FLAGS}
						"cflags=-I${OPENSSL_INCLUDE_DIR}"
			                        "cxxflags=-I${OPENSSL_INCLUDE_DIR}")
				set_property(TARGET dependencies::boost  APPEND PROPERTY
					INTERFACE_INCLUDE_DIRECTORIES ${OPENSSL_INCLUDE_DIR})

				get_property(OPENSSL_CRYPTO_LIBRARY TARGET dependencies::openssl::crypto
					PROPERTY IMPORTED_LOCATION)
				get_property(OPENSSL_SSL_LIBRARY TARGET dependencies::openssl::ssl
					PROPERTY IMPORTED_LOCATION)
				get_property(OPENSSL_LIB_DEPENDENCIES TARGET dependencies::openssl
					PROPERTY INTERFACE_LINK_LIBRARIES)
				set_property(TARGET dependencies::boost APPEND PROPERTY
					INTERFACE_LINK_LIBRARIES ${OPENSSL_SSL_LIBRARY}
			                                         ${OPENSSL_CRYPTO_LIBRARY}
			                                         ${OPENSSL_LIB_DEPENDENCIES})
				set(DEPENDENCIES dependencies::openssl)
			endif()
		else()
			add_definitions(-DBOOST_ALL_NO_LIB)
			set(BOOTSTRAP_CMD "bootstrap.bat")
			if (TARGET dependencies::libressl)
				get_property(LIBRESSL_INCLUDE_DIR TARGET dependencies::libressl
					PROPERTY INTERFACE_INCLUDE_DIRECTORIES)
				set(EXTRA_FLAGS ${EXTRA_FLAGS}
						"cflags=-I${LIBRESSL_INCLUDE_DIR}"
			                        "cxxflags=-I${LIBRESSL_INCLUDE_DIR}")
				set_property(TARGET dependencies::boost  APPEND PROPERTY
					INTERFACE_INCLUDE_DIRECTORIES ${LIBRESSL_INCLUDE_DIR})

				get_property(LIBRESSL_CRYPTO_LIBRARY TARGET dependencies::libressl::crypto
					PROPERTY IMPORTED_LOCATION)
				get_property(LIBRESSL_SSL_LIBRARY TARGET dependencies::libressl::ssl
					PROPERTY IMPORTED_LOCATION)
				get_property(LIBRESSL_LIB_DEPENDENCIES TARGET dependencies::libressl
					PROPERTY INTERFACE_LINK_LIBRARIES)
				set_property(TARGET dependencies::boost APPEND PROPERTY
					INTERFACE_LINK_LIBRARIES ${LIBRESSL_SSL_LIBRARY}
			                                         ${LIBRESSL_CRYPTO_LIBRARY}
			                                         ${LIBRESSL_LIB_DEPENDENCIES})
				set(DEPENDENCIES dependencies::libressl)
			endif()
		endif()

		sources_url(BOOST
			"boost.org/boost/boost_1_64_0.tar.gz"
			"https://dl.bintray.com/boostorg/release/1.64.0/source/:boost_1_64_0.tar.gz")
		ExternalProject_Add(dependencies_boost
			URL ${BOOST_URL}
			DOWNLOAD_NAME boost_1_64_0.tar.gz
			URL_HASH SHA256=0445c22a5ef3bd69f5dfb48354978421a85ab395254a26b1ffb0aa1bfd63a108
			CONFIGURE_COMMAND ${BOOTSTRAP_CMD}
			BUILD_COMMAND ""
			INSTALL_COMMAND "./b2" ${EXTRA_FLAGS} link=static include=static variant=release threading=multi --with-system --with-filesystem --prefix=<INSTALL_DIR> --layout=system --ignore-site-config install
			BUILD_IN_SOURCE 1
			INSTALL_DIR "${STAGING_DIR}"
			LOG_DOWNLOAD 1
			LOG_UPDATE 1
			LOG_CONFIGURE 1
			LOG_BUILD 1
			LOG_TEST 1
			LOG_INSTALL 1
		)
		add_dependencies(dependencies::boost dependencies_boost)
		add_dependencies(dependencies_boost ${DEPENDENCIES})
		set(Boost_INCLUDE_DIRS "${STAGING_DIR}/include")
		set(Boost_SYSTEM_LIBRARY     "${STAGING_DIR}/lib/libboost_system${CMAKE_STATIC_LIBRARY_SUFFIX}")
		set(Boost_FILESYSTEM_LIBRARY "${STAGING_DIR}/lib/libboost_filesystem${CMAKE_STATIC_LIBRARY_SUFFIX}")

	endif()

	set_target_properties(dependencies::boost PROPERTIES 
		INTERFACE_INCLUDE_DIRECTORIES ${Boost_INCLUDE_DIRS})
	set_target_properties(dependencies::boost::system PROPERTIES
		IMPORTED_LOCATION ${Boost_SYSTEM_LIBRARY})
	set_target_properties(dependencies::boost::filesystem PROPERTIES
		IMPORTED_LOCATION ${Boost_FILESYSTEM_LIBRARY})

	find_package(Threads)
	set_property(TARGET dependencies::boost APPEND PROPERTY
		INTERFACE_LINK_LIBRARIES ${CMAKE_THREAD_LIBS_INIT}
	                             ${CMAKE_DL_LIBS})

endif()

