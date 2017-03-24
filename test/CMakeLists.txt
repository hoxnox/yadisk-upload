# @author hoxnox <hoxnox@gmail.com>
# @date 20170322 16:08:27
# yadisk-upload cmake tests build script

include(gtest)
add_executable(test_unit_${PROJECT_NAME} unit.cpp
	utils/TLSEcho.cpp
	${PROJECT_SOURCE_DIR}/src/Logging.cpp)
add_dependencies(test_unit_${PROJECT_NAME} gtestlib yandex_api)
target_link_libraries(test_unit_${PROJECT_NAME} ${LIBRARIES} ${GTEST_LIBRARIES})
target_include_directories(test_unit_${PROJECT_NAME} BEFORE PRIVATE "${GTEST_INCLUDE_DIRS}")
target_include_directories(test_unit_${PROJECT_NAME} PRIVATE "${PROJECT_SOURCE_DIR}/src")
target_include_directories(test_unit_${PROJECT_NAME} PRIVATE "${PROJECT_SOURCE_DIR}/test/utils")

#add_executable(server ./server.cpp)
#add_dependencies(server build_dependencies)
#target_link_libraries(server ${LIBRARIES})
