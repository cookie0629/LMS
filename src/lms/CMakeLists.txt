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
    lmscore
    lmsdatabase
    lmsscanner
    lmsartwork
    lmsaudio
    lmsimage
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

# 安装
install(TARGETS lms RUNTIME DESTINATION bin)

