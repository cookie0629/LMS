# 核心库
# 这里将添加各个库的 CMakeLists.txt

# Core 库（基础工具库）
add_subdirectory(core)
add_subdirectory(database)
add_subdirectory(audio)
add_subdirectory(image)
add_subdirectory(services)
add_subdirectory(som)
# TODO: Subsonic 模块尚未完整迁移，先暂时关闭，待源码补全后再启用
# add_subdirectory(subsonic)


