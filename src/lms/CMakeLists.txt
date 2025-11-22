# 主程序

# 源文件（待创建）
set(LMS_SOURCES
    main.cpp
    # ui/LmsApplication.cpp
    # ui/LmsInitApplication.cpp
)

# 创建可执行文件
add_executable(lms ${LMS_SOURCES})

# 链接库
target_link_libraries(lms
    PRIVATE
    ${WT_LIBRARIES}
    ${Boost_LIBRARIES}
    ${AVCODEC_LIBRARIES}
    ${AVFORMAT_LIBRARIES}
    ${AVUTIL_LIBRARIES}
    ${TAGLIB_LIBRARIES}
    SQLite::SQLite3
    OpenSSL::SSL
    OpenSSL::Crypto
    ${PUGIXML_LIBRARIES}
    ${CONFIGPP_LIBRARIES}
    pthread
)

# 编译选项（从 WT_CFLAGS_OTHER 中提取，但过滤掉无效的）
target_compile_options(lms PRIVATE
    -std=c++11
)

# 编译定义（移除可能导致问题的标志）
# target_compile_definitions(lms PRIVATE
#     ${WT_CFLAGS_OTHER}
# )

# 安装
install(TARGETS lms RUNTIME DESTINATION bin)

