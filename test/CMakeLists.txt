# @author hoxnox <hoxnox@gmail.com>
# @date 20170322 16:08:27
# yadisk-upload cmake tests build script

if (NOT WITH_CONAN)
	include(dependencies/gtest)
endif()

add_executable(test_unit_${PROJECT_NAME} unit.cpp
	utils/TlsEcho.cpp
	${PROJECT_SOURCE_DIR}/src/Logging.cpp)

add_dependencies(test_unit_${PROJECT_NAME} yandex_api)
if (NOT WITH_CONAN)
	add_dependencies(test_unit_${PROJECT_NAME} dependencies::boost dependencies::gtest)
	target_include_directories(test_unit_${PROJECT_NAME} BEFORE PRIVATE
			$<TARGET_PROPERTY:dependencies::gtest,INTERFACE_INCLUDE_DIRECTORIES>)
	target_link_libraries(test_unit_${PROJECT_NAME} yandex_api
			dependencies::boost::filesystem
			dependencies::boost::system
			$<TARGET_PROPERTY:dependencies::boost,INTERFACE_LINK_LIBRARIES>
			dependencies::gtest::gtest
			dependencies::gtest::main
			$<TARGET_PROPERTY:dependencies::gtest,INTERFACE_LINK_LIBRARIES>)
else()
	target_include_directories(test_unit_${PROJECT_NAME} PRIVATE "${PROJECT_SOURCE_DIR}/src")
	target_include_directories(test_unit_${PROJECT_NAME} PRIVATE "${PROJECT_SOURCE_DIR}/test/utils")
	target_link_libraries(test_unit_${PROJECT_NAME} yandex_api ${CONAN_LIBS} ${CMAKE_THREAD_LIBS_INIT})
endif()


#add_executable(server ./server.cpp)
#add_dependencies(server build_dependencies)
#target_link_libraries(server ${LIBRARIES})
