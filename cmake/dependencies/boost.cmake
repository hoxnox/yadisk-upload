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

		if (TARGET dependencies::openssl)
			get_property(OPENSSL_INCLUDE_DIR TARGET dependencies::openssl
				PROPERTY INTERFACE_INCLUDE_DIRECTORIES)
			set(EXTRA_FLAGS "cflags=-I${OPENSSL_INCLUDE_DIR}"
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

		sources_url(BOOST
			"boost.org/boost/boost_1_63_0.tar.gz"
			"https://sourceforge.net/projects/boost/files/boost/1.63.0/boost_1_63_0.tar.gz/download")
		ExternalProject_Add(dependencies_boost
			URL ${BOOST_URL}
			DOWNLOAD_NAME boost_1_63_0.tar.gz
			URL_HASH SHA256=fe34a4e119798e10b8cc9e565b3b0284e9fd3977ec8a1b19586ad1dec397088b
			CONFIGURE_COMMAND "./bootstrap.sh" --prefix=<INSTALL_DIR> --with-libraries=system,filesystem
			BUILD_COMMAND ""
			INSTALL_COMMAND "./b2" ${EXTRA_FLAGS} link=static threading=multi --ignore-site-config install
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
		set(Boost_SYSTEM_LIBRARY     "${STAGING_DIR}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}boost_system${CMAKE_STATIC_LIBRARY_SUFFIX}")
		set(Boost_FILESYSTEM_LIBRARY "${STAGING_DIR}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}boost_filesystem${CMAKE_STATIC_LIBRARY_SUFFIX}")

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

