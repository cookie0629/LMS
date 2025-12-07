add_compile_options(-Wall -Wextra -pedantic)

add_subdirectory(libs)
add_subdirectory(lms)
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tools/CMakeLists.txt")
	add_subdirectory(tools)
endif()



