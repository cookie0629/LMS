find_path(STB_IMAGE_INCLUDE_DIR stb_image.h PATH_SUFFIXES stb) # 查找 stb_image.h 头文件路径，设置 STB_IMAGE_INCLUDE_DIR 变量# PATH_SUFFIXES stb 表示在 stb 子目录中查找

find_path(STB_IMAGE_RESIZE2_INCLUDE_DIR stb_image_resize2.h PATH_SUFFIXES stb) # 查找 stb_image_resize2.h 头文件的位置，并设置变量 STB_IMAGE_RESIZE2_INCLUDE_DIR
if(STB_IMAGE_RESIZE2_INCLUDE_DIR)
	set(STB_IMAGE_RESIZE_VERSION 2)
else()
	find_path(STB_IMAGE_RESIZE_INCLUDE_DIR stb_image_resize.h PATH_SUFFIXES stb)
	if(STB_IMAGE_RESIZE_INCLUDE_DIR)
		set(STB_IMAGE_RESIZE_VERSION 1)
	endif()
endif()

include(FindPackageHandleStandardArgs) # 引入 FindPackageHandleStandardArgs 模块，用于处理标准参数

FIND_PACKAGE_HANDLE_STANDARD_ARGS(
		StbImage
		REQUIRED_VARS STB_IMAGE_INCLUDE_DIR STB_IMAGE_RESIZE_VERSION
)
