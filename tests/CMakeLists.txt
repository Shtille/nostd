project(test_nostd)

set(SRC_FILES
	main.cpp
	containers/forward_list_test.cpp
	containers/list_test.cpp
	containers/map_test.cpp
	containers/set_test.cpp
	containers/stack_test.cpp
	containers/vector_test.cpp
)

set(libraries
	gtest::gtest
	nostd
)

find_package(GTest REQUIRED)

add_executable(${PROJECT_NAME} ${SRC_FILES})
target_link_libraries(${PROJECT_NAME} PRIVATE ${libraries})

add_test(${PROJECT_NAME} ${PROJECT_NAME})