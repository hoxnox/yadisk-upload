# @author hoxnox <hoxnox@gmail.com>
# @date 20170419 15:29:04

if (NOT TARGET dependencies::boost)
	option(WITH_SYSTEM_BOOST "Don't build boost with the project" ON)
	
	if (WITH_SYSTEM_BOOST)
		if(NOT BOOST_ROOT)
			set(BOOST_ROOT $ENV{BOOST_ROOT})
		endif()
		find_package(Boost 1.47.0 COMPONENTS system filesystem REQUIRED)

		add_library(dependencies::boost UNKNOWN IMPORTED)
		add_library(dependencies::boost::system UNKNOWN IMPORTED)
		add_library(dependencies::boost::filesystem UNKNOWN IMPORTED)

		set_target_properties(dependencies::boost::system PROPERTIES
			IMPORTED_LOCATION ${Boost_SYSTEM_LIBRARY})
		set_target_properties(dependencies::boost::filesystem PROPERTIES
			IMPORTED_LOCATION ${Boost_FILESYSTEM_LIBRARY})

		set_target_properties(dependencies::boost PROPERTIES 
			INTERFACE_INCLUDE_DIRECTORIES ${Boost_INCLUDE_DIRS})
	else()
		add_library(dependencies::boost INTERFACE IMPORTED)
		add_library(dependencies::boost::system STATIC IMPORTED)
		add_library(dependencies::boost::filesystem STATIC IMPORTED)
		if (TARGET dependencies::openssl)
			set(EXTRA_FLAGS
					"cflags=-I$<TARGET_PROPERTY:dependencies::openssl,INTERFACE_INCLUDE_DIRECTORIES>"
					"cxxflags=-I$<TARGET_PROPERTY:dependencies::openssl,INTERFACE_INCLUDE_DIRECTORIES>")
				set_property(TARGET dependencies::boost  APPEND PROPERTY INTERFACE_INCLUDE_DIRECTORIES
					$<TARGET_PROPERTY:dependencies::openssl,INTERFACE_INCLUDE_DIRECTORIES>)
			add_dependencies(dependencies::boost::system dependencies::openssl)
		endif()
		sources_url(BOOST
			"boost.org/boost/boost_1_63_0.tar.gz"
			"https://sourceforge.net/projects/boost/files/boost/1.63.0/boost_1_63_0.tar.gz/download")
		ExternalProject_Add(boost
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
		set_target_properties(dependencies::boost PROPERTIES
			INTERFACE_INCLUDE_DIRECTORIES "${STAGING_DIR}/include")
		set_target_properties(dependencies::boost::system PROPERTIES IMPORTED_LOCATION
			"${STAGING_DIR}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}boost_system${CMAKE_STATIC_LIBRARY_SUFFIX}")
		set_target_properties(dependencies::boost::filesystem PROPERTIES IMPORTED_LOCATION
			"${STAGING_DIR}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}boost_filesystem${CMAKE_STATIC_LIBRARY_SUFFIX}")

		set_target_properties(dependencies::boost PROPERTIES
			INTERFACE_INCLUDE_DIRECTORIES "${STAGING_DIR}/include")
	endif()
endif()

