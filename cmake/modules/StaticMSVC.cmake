set(CMAKE_USER_MAKE_RULES_OVERRIDE "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules/StaticMSVC_C.cmake")
set(CMAKE_USER_MAKE_RULES_OVERRIDE_CXX "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules/StaticMSVC_CXX.cmake")
set(CMAKE_STATIC_RUNTIME_EXTRA_ARGS
	"-DCMAKE_USER_MAKE_RULES_OVERRIDE=${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules/StaticMSVC_C.cmake"
	"-DCMAKE_USER_MAKE_RULES_OVERRIDE_CXX=${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules/StaticMSVC_CXX.cmake")