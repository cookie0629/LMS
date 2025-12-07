add_library(lmssom STATIC
	impl/DataNormalizer.cpp
	impl/Network.cpp
	)

target_include_directories(lmssom INTERFACE
	include
	)

target_include_directories(lmssom PRIVATE
	include
	)

target_link_libraries(lmssom PUBLIC
	lmscore
	)

set_property(TARGET lmssom PROPERTY POSITION_INDEPENDENT_CODE ON)

if(BUILD_TESTING AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/test/CMakeLists.txt")
	add_subdirectory(test)
endif()

if (BUILD_BENCHMARKS AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/bench/CMakeLists.txt")
	add_subdirectory(bench)
endif()
