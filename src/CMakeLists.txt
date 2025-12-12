add_compile_options(-Wall -Wextra -pedantic)

add_subdirectory(libs) # 添加子目录 libs 到构建系统
add_subdirectory(lms) # 添加子目录 lms 到构建系统
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tools/CMakeLists.txt") # 条件判断：如果 tools 目录下存在 CMakeLists.txt 文件，则添加 tools 子目录
	add_subdirectory(tools)
endif()



