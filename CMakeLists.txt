# CMakeLists file for nostd library
cmake_minimum_required(VERSION 3.15 FATAL_ERROR)

project(nostd CXX)

# ----- Build library -----

set(HEADER_FILES
	include/nostd/allocator.h
	include/nostd/default_allocator.h
	include/nostd/forward_list.h
	include/nostd/list.h
	include/nostd/map.h
	include/nostd/non_copyable.h
	include/nostd/pool_allocator.h
	include/nostd/set.h
	include/nostd/stack.h
	include/nostd/stack_linked_list.h
	include/nostd/test_allocator.h
	include/nostd/utility.h
	include/nostd/vector.h
)

set(SRC_FILES
	src/default_allocator.cpp
	src/pool_allocator.cpp
	src/test_allocator.cpp
)

set(includes
	./include
)

add_library(${PROJECT_NAME} STATIC ${SRC_FILES})
target_include_directories(${PROJECT_NAME} 
						   PUBLIC 
						   "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include"
						   "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")
#set_target_properties(${PROJECT_NAME} PROPERTIES PUBLIC_HEADER ${HEADER_FILES})
install(TARGETS ${PROJECT_NAME})
install(DIRECTORY include/nostd DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

# ----- Build unit tests -----

if (NOT BUILD_TESTING STREQUAL OFF)
	enable_testing()
	add_subdirectory(tests)
endif()