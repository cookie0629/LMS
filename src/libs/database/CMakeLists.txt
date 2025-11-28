cmake_minimum_required(VERSION 3.20)

# 创建 Database 库
add_library(lmsdatabase STATIC
    impl/Transaction.cpp
    impl/Object.cpp
    impl/Db.cpp
    impl/Session.cpp
    impl/Utils.cpp
    impl/Migration.cpp
	impl/objects/User.cpp
	impl/objects/AuthToken.cpp
	impl/objects/Artist.cpp
	impl/objects/Release.cpp
	impl/objects/Medium.cpp
	impl/objects/Track.cpp
	impl/objects/TrackEmbeddedImage.cpp
	impl/objects/MediaLibrary.cpp
	impl/objects/Directory.cpp
)

target_include_directories(lmsdatabase INTERFACE
    include
)

target_include_directories(lmsdatabase PRIVATE
    impl
    include
)

# 链接依赖
target_link_libraries(lmsdatabase PUBLIC
    lmscore
    ${WT_LIBRARIES}
)

target_link_libraries(lmsdatabase PRIVATE
    ${SQLite3_LIBRARIES}
)

# 设置 C++20 标准
set_target_properties(lmsdatabase PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
)

# 添加测试程序（简化版，直接编译）
add_executable(test_database test/test_database.cpp)
target_link_libraries(test_database PRIVATE 
    lmsdatabase
    lmscore
)
target_include_directories(test_database PRIVATE
    include
)

