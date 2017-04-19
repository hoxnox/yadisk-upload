# @author hoxnox <hoxnox@gmail.com>
# @date 20170419 15:29:04

if (NOT TARGET dependencies::easyloggingpp)
	option(WITH_SYSTEM_EASYLOGGING "Don't build easylogging with the project" OFF)

	if (NOT WITH_LOG)
		if (WITH_SYSTEM_EASYLOGGINGPP)
			add_custom_target(easyloggingpp)
			if(NOT EASYLOGGINGPP_ROOT)
				set(EASYLOGGINGPP_ROOT $ENV{EASYLOGGINGPP_ROOT})
			endif()
			find_package(EASYLOGGINGPP REQUIRED)
			add_library(dependencies::easyloggingpp UNKNOWN IMPORTED)
			set_property(TARGET depepndencies::easyloggingpp APPEND
				PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${EASYLOGGINGPP_INCLUDE_DIR})
		else()
			sources_url(EASYLOGGINGPP
				"easylogging/easyloggingpp/easyloggingpp_v9.84.tar.gz"
				"https://github.com/easylogging/easyloggingpp/releases/download/9.84/easyloggingpp_v9.84.tar.gz")
			ExternalProject_Add(dependencies_easyloggingpp
				URL ${EASYLOGGINGPP_URL}
				DOWNLOAD_NAME easyloggingpp_v9.84.tar.gz
				URL_HASH SHA256=c4d51df897180120b3450e11814437121f9a3e6a090b917e4f32adfdb3ebf974
				CONFIGURE_COMMAND mkdir -p "${STAGING_DIR}/include"
				BUILD_COMMAND ""
				INSTALL_COMMAND "cp" ./easylogging++.h <INSTALL_DIR>/include/
				BUILD_IN_SOURCE 1
				INSTALL_DIR "${STAGING_DIR}"
				LOG_DOWNLOAD 1
				LOG_UPDATE 1
				LOG_CONFIGURE 1
				LOG_BUILD 1
				LOG_TEST 1
				LOG_INSTALL 1
			)
			add_library(dependencies::easyloggingpp INTERFACE IMPORTED)
			add_dependencies(dependencies::easyloggingpp dependencies_easyloggingpp)
			set_property(TARGET dependencies::easyloggingpp APPEND
				PROPERTY INTERFACE_INCLUDE_DIRECTORIES "${STAGING_DIR}/include")
		endif()
	endif()
endif()
