add_library(lmsaudio STATIC
    impl/AudioFileInfo.cpp
)

target_include_directories(lmsaudio PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(lmsaudio PUBLIC
    lmscore
)

# 注意：完整版本需要链接音频处理库（如 TagLib）
# target_link_libraries(lmsaudio PUBLIC TagLib::TagLib)

