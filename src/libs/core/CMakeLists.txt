pkg_check_modules(Config++ REQUIRED IMPORTED_TARGET libconfig++)
pkg_check_modules(Archive REQUIRED IMPORTED_TARGET libarchive)
pkg_check_modules(XXHASH REQUIRED IMPORTED_TARGET libxxhash)

set(LMS_VERSION ${PROJECT_VERSION}) # 设置LMS_VERSION变量为项目版本号

configure_file( # 配置文件，将模板文件impl/Version.cpp.in处理为impl/Version.cpp# @ONLY表示只替换@VAR@格式的变量
	impl/Version.cpp.in
	impl/Version.cpp
	@ONLY
	)

add_library(lmscore STATIC # 创建名为lmscore的静态库，包含以下源文件
	impl/http/Client.cpp
	impl/http/SendQueue.cpp
	impl/ArchiveZipper.cpp
	impl/ChildProcess.cpp
	impl/ChildProcessManager.cpp
	impl/Config.cpp
	impl/FileResourceHandler.cpp
	impl/JobScheduler.cpp
	impl/IOContextRunner.cpp
	impl/Logger.cpp
	impl/MimeTypes.cpp
	impl/NetAddress.cpp
	impl/PartialDateTime.cpp
	impl/Path.cpp
	impl/Random.cpp
	impl/RecursiveSharedMutex.cpp
	impl/String.cpp
	impl/TraceLogger.cpp
	impl/UUID.cpp
	impl/XxHash3.cpp
	${CMAKE_CURRENT_BINARY_DIR}/impl/Version.cpp
	)

target_include_directories(lmscore INTERFACE
	include
	)

target_include_directories(lmscore PRIVATE
	include
	${XXHASH_INCLUDE_DIRS} # 包含XXHash库的头文件目录
	)

target_link_libraries(lmscore PRIVATE # 链接私有库到lmscore目标
	PkgConfig::Config++
	PkgConfig::Archive
	OpenSSL::Crypto
	)

target_link_libraries(lmscore PUBLIC # 链接公共库到lmscore目标
	std::filesystem
	Wt::Wt
	)

if(BUILD_TESTING AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/test/CMakeLists.txt")
	add_subdirectory(test)
endif()

if (BUILD_BENCHMARKS AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/bench/CMakeLists.txt")
	add_subdirectory(bench)
endif()
