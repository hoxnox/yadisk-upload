# @author hoxnox <hoxnox@gmail.com>
# @date 20170419 15:29:04

include(Vendoring)
include(ExternalProject)
if (NOT STAGING_DIR)
	set(STAGING_DIR ${CMAKE_BINARY_DIR}/staging)
endif()
file(MAKE_DIRECTORY "${STAGING_DIR}/include")
