cmake_minimum_required(VERSION 3.20)

# Services 库的 CMakeLists.txt
# 目前只包含 auth 服务

add_subdirectory(artwork)
add_subdirectory(auth)
add_subdirectory(feedback)
add_subdirectory(podcast)
add_subdirectory(recommendation)
add_subdirectory(scanner)
add_subdirectory(scrobbling)
add_subdirectory(transcoding)

