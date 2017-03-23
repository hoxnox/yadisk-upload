# @author hoxnox <hoxnox@gmail.com>
# @date 20170322 16:08:27
# yadisk-upload cmake tests build script

include(gtest)
add_executable(test_unit_${PROJECT_NAME} unit.cpp)
add_dependencies(test_unit_${PROJECT_NAME} gtestlib)
target_link_libraries(test_unit_${PROJECT_NAME} ${LIBRARIES} ${GTEST_LIBRARIES})
target_include_directories(test_unit_${PROJECT_NAME} PRIVATE ${GTEST_INCLUDE_DIRS})

