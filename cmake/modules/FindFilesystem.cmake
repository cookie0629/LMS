if(TARGET std::filesystem)
    return()
endif()

include(CMakePushCheckState) # 用于保存和恢复 CMake 检查状态
include(CheckIncludeFileCXX) # 用于检查 C++ 头文件是否存在
include(CheckCXXSourceCompiles) # 用于检查 C++ 源代码是否能编译

cmake_push_check_state() # 保存当前的 CMake 检查状态

set(CMAKE_REQUIRED_QUIET ${Filesystem_FIND_QUIETLY})

set(want_components ${Filesystem_FIND_COMPONENTS}) # 获取用户请求的组件列表
if(Filesystem_FIND_COMPONENTS STREQUAL "") # 如果没有指定组件，则默认只查找 Final 组件
    set(want_components Final)
endif()

set(extra_components ${want_components})
list(REMOVE_ITEM extra_components Final Experimental)
foreach(component IN LISTS extra_components) # 遍历并警告用户指定的额外组件
    message(WARNING "Extraneous find_package component for Filesystem: ${component}")
endforeach()

set(find_experimental TRUE) # 设置查找标志，默认同时查找 Final 和 Experimental 组件
set(find_final TRUE)
if(NOT "Final" IN_LIST want_components)
    set(find_final FALSE)
endif()
if(NOT "Experimental" IN_LIST want_components)
    set(find_experimental FALSE)
endif()

if(find_final) # 检查C++文件系统支持情况的代码# 如果find_final为真，则检查标准filesystem头文件是否存在
    check_include_file_cxx("filesystem" _CXX_FILESYSTEM_HAVE_HEADER)
    mark_as_advanced(_CXX_FILESYSTEM_HAVE_HEADER)
    if(_CXX_FILESYSTEM_HAVE_HEADER)
        set(find_experimental FALSE)
    endif()
else()
    set(_CXX_FILESYSTEM_HAVE_HEADER FALSE) # 如果find_final为假，则设置_CXX_FILESYSTEM_HAVE_HEADER为FALSE
endif()

if(find_experimental) # 如果find_experimental为真，则检查实验性filesystem头文件是否存在
    check_include_file_cxx("experimental/filesystem" _CXX_FILESYSTEM_HAVE_EXPERIMENTAL_HEADER)
    mark_as_advanced(_CXX_FILESYSTEM_HAVE_EXPERIMENTAL_HEADER)
else()
    set(_CXX_FILESYSTEM_HAVE_EXPERIMENTAL_HEADER FALSE) # 如果find_experimental为假，则设置_CXX_FILESYSTEM_HAVE_EXPERIMENTAL_HEADER为FALSE
endif()

if(_CXX_FILESYSTEM_HAVE_HEADER) # 根据检查结果设置文件系统相关的变量
    set(_have_fs TRUE)
    set(_fs_header filesystem)
    set(_fs_namespace std::filesystem)
elseif(_CXX_FILESYSTEM_HAVE_EXPERIMENTAL_HEADER)
    set(_have_fs TRUE) # 设置_have_fs为TRUE，表示找到了文件系统支持
    set(_fs_header experimental/filesystem) # 设置使用的头文件为实验性filesystem
    set(_fs_namespace std::experimental::filesystem) # 设置使用的命名空间为std::experimental::filesystem
else()
    set(_have_fs FALSE) # 如果既没有标准filesystem也没有实验性filesystem，则设置_have_fs为FALSE
endif()

set(CXX_FILESYSTEM_HAVE_FS ${_have_fs} CACHE BOOL "TRUE if we have the C++ filesystem headers")
set(CXX_FILESYSTEM_HEADER ${_fs_header} CACHE STRING "The header that should be included to obtain the filesystem APIs")
set(CXX_FILESYSTEM_NAMESPACE ${_fs_namespace} CACHE STRING "The C++ namespace that contains the filesystem APIs")

set(_found FALSE)

if(CXX_FILESYSTEM_HAVE_FS)
    string(CONFIGURE [[
        #include <@CXX_FILESYSTEM_HEADER@>

        int main() {
            auto cwd = @CXX_FILESYSTEM_NAMESPACE@::current_path();
            return static_cast<int>(cwd.string().size());
        }
    ]] code @ONLY)

    check_cxx_source_compiles("${code}" CXX_FILESYSTEM_NO_LINK_NEEDED)

    set(can_link ${CXX_FILESYSTEM_NO_LINK_NEEDED})

    if(NOT CXX_FILESYSTEM_NO_LINK_NEEDED)
	    set(prev_libraries ${CMAKE_REQUIRED_LIBRARIES})
	    set(CMAKE_REQUIRED_LIBRARIES ${prev_libraries} -lstdc++fs)
	    check_cxx_source_compiles("${code}" CXX_FILESYSTEM_STDCPPFS_NEEDED)
	    set(can_link ${CXX_FILESYSTEM_STDCPPFS_NEEDED})
	    if(NOT CXX_FILESYSTEM_STDCPPFS_NEEDED)
		    set(CMAKE_REQUIRED_LIBRARIES ${prev_libraries} -lc++fs)
		    check_cxx_source_compiles("${code}" CXX_FILESYSTEM_CPPFS_NEEDED)
		    set(can_link ${CXX_FILESYSTEM_CPPFS_NEEDED})
		    if(NOT CXX_FILESYSTEM_CPPFS_NEEDED)
			    check_cxx_source_compiles("${code}" CXX_FILESYSTEM_NO_LINK_NEEDED)
			    set(can_link ${CXX_FILESYSTEM_NO_LINK_NEEDED})
		    endif()
	    endif()

    endif()

    if(can_link) # 如果可以链接filesystem库
        add_library(std::filesystem INTERFACE IMPORTED) # 创建一个名为std::filesystem的接口导入库
        target_compile_features(std::filesystem INTERFACE cxx_std_17)
        set(_found TRUE)

        if(CXX_FILESYSTEM_NO_LINK_NEEDED)
        elseif(CXX_FILESYSTEM_STDCPPFS_NEEDED)
            target_link_libraries(std::filesystem INTERFACE -lstdc++fs)
        elseif(CXX_FILESYSTEM_CPPFS_NEEDED)
            target_link_libraries(std::filesystem INTERFACE -lc++fs)
        endif()
    endif()
endif()

cmake_pop_check_state() # 恢复之前的检查状态

set(Filesystem_FOUND ${_found} CACHE BOOL "TRUE if we can compile and link a program using std::filesystem" FORCE)

if(Filesystem_FIND_REQUIRED AND NOT Filesystem_FOUND) # 如果要求必须找到filesystem但实际未找到，则报错
    message(FATAL_ERROR "Cannot Compile simple program using std::filesystem")
endif()
