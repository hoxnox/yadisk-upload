# @author hoxnox <hoxnox@gmail.com>
# @date 20170419 15:29:04
#
# Settings:
#     WITH_SYSTEM_EASYLOGGING - don't build, try to find it
#     EASYLOGGING_ROOT        - where to find
#
# After including this file you'll have the following targets with it's
# properties:
#     easylogging
#         INTERFACE_INCLUDE_DIRECTORIES - include dirs

if (NOT TARGET dependencies::easyloggingpp)

	include(dependencies/common)
	option(WITH_SYSTEM_EASYLOGGING "Don't build easylogging with the project" OFF)

	add_library(dependencies::easyloggingpp UNKNOWN IMPORTED)

	if (WITH_SYSTEM_EASYLOGGINGPP)

		add_custom_target(easyloggingpp)
		if(NOT EASYLOGGINGPP_ROOT)
			set(EASYLOGGINGPP_ROOT $ENV{EASYLOGGINGPP_ROOT})
		endif()
		find_package(EASYLOGGINGPP REQUIRED)

	else()

		sources_url(EASYLOGGINGPP
			"easylogging/easyloggingpp/easyloggingpp_v9.84.tar.gz"
			"https://github.com/easylogging/easyloggingpp/releases/download/9.84/easyloggingpp_v9.84.tar.gz")
		ExternalProject_Add(dependencies_easyloggingpp
			URL ${EASYLOGGINGPP_URL}
			DOWNLOAD_NAME easyloggingpp_v9.84.tar.gz
			URL_HASH SHA256=c4d51df897180120b3450e11814437121f9a3e6a090b917e4f32adfdb3ebf974
			CONFIGURE_COMMAND ""
			BUILD_COMMAND ""
			INSTALL_COMMAND "${CMAKE_COMMAND}" -E copy ./easylogging++.h <INSTALL_DIR>/include/
			BUILD_IN_SOURCE 1
			INSTALL_DIR "${STAGING_DIR}"
			LOG_DOWNLOAD 1
			LOG_UPDATE 1
			LOG_CONFIGURE 1
			LOG_BUILD 1
			LOG_TEST 1
			LOG_INSTALL 1
		)
		add_dependencies(dependencies::easyloggingpp dependencies_easyloggingpp)
		set(${EASYLOGGINGPP_INCLUDE_DIR} "${STAGING_DIR}/include")

	endif()

	set_property(TARGET dependencies::easyloggingpp APPEND
		PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${EASYLOGGINGPP_INCLUDE_DIR})

endif()
