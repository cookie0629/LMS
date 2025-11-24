add_library(lmsaudio STATIC
    impl/AudioFileInfo.cpp
)

target_include_directories(lmsaudio PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(lmsaudio PUBLIC
    lmscore
)

find_package(TagLib QUIET)
if (TagLib_FOUND)
    target_link_libraries(lmsaudio PUBLIC TagLib::TagLib)
    target_compile_definitions(lmsaudio PRIVATE LMS_AUDIO_HAS_TAGLIB=1)
endif()

