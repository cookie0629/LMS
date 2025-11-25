cmake_minimum_required(VERSION 3.20)

# Scanner Service 库

# 创建 Scanner Service 库（简化版，暂时只包含基础文件）
add_library(lmsscanner STATIC
    impl/ScannerService.cpp
    impl/ScannerStats.cpp
    impl/scanners/FileScanOperationBase.cpp
    impl/scanners/Utils.cpp
    impl/scanners/audiofile/AudioFileScanner.cpp
    impl/scanners/audiofile/AudioFileScanOperation.cpp
    impl/scanners/audiofile/TrackMetadataParser.cpp
    impl/steps/ScanStepBase.cpp
    impl/steps/ScanStepScanFiles.cpp
    impl/steps/ScanStepCheckForRemovedFiles.cpp
    impl/steps/ScanStepCheckForDuplicatedFiles.cpp
    impl/steps/ScanStepUpdateLibraryFields.cpp
)

target_include_directories(lmsscanner INTERFACE
    include
)

target_include_directories(lmsscanner PRIVATE
    include
    impl
)

# 链接依赖
target_link_libraries(lmsscanner PUBLIC
    lmscore
    lmsdatabase
    lmsaudio
    ${Boost_LIBRARIES}
)

target_link_libraries(lmsscanner PRIVATE
    ${SQLite3_LIBRARIES}
)

# 设置 C++20 标准
set_target_properties(lmsscanner PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
)

# 测试程序
add_executable(test_scanner
    test/test_scanner.cpp
)

target_link_libraries(test_scanner PRIVATE
    lmsscanner
    lmscore
    lmsdatabase
    lmsaudio
    ${Boost_LIBRARIES}
    ${SQLite3_LIBRARIES}
)

# 设置输出目录
set_target_properties(test_scanner PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

