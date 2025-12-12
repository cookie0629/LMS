pkg_check_modules(LIBAV IMPORTED_TARGET libavcodec libavutil libavformat)
pkg_check_modules(Taglib REQUIRED IMPORTED_TARGET taglib)

add_library(lmsaudio STATIC # 创建一个名为lmsaudio的静态库
	impl/ffmpeg/AudioFile.cpp # FFmpeg实现相关源文件
	impl/ffmpeg/AudioFileInfo.cpp
	impl/ffmpeg/ImageReader.cpp
	impl/ffmpeg/TagReader.cpp
	impl/ffmpeg/Transcoder.cpp
	impl/ffmpeg/Utils.cpp
	impl/taglib/AudioFileInfo.cpp # Taglib实现相关源文件
	impl/taglib/ImageReader.cpp
	impl/taglib/TagReader.cpp
	impl/taglib/Utils.cpp
	impl/AudioTypes.cpp # 通用实现源文件
	impl/ImageReader.cpp
	impl/ParseAudioFileInfo.cpp
	impl/TagReader.cpp
	)

target_include_directories(lmsaudio INTERFACE # 设置目标库的头文件包含目录# INTERFACE表示这些头文件目录对依赖lmsaudio的其他目标可见
	include
	)

target_include_directories(lmsaudio PRIVATE # 设置目标库的私有头文件包含目录# PRIVATE表示这些头文件目录仅对lmsaudio本身可见
	include
	${AVCODEC_INCLUDE_DIR}
	${AVFORMAT_INCLUDE_DIR}
	${AVUTIL_INCLUDE_DIR}
	)

target_link_libraries(lmsaudio PUBLIC # 设置目标库需要公开链接的库# PUBLIC表示这些依赖库会同时传递给依赖lmsaudio的其他目标
	lmscore
	std::filesystem
	)

target_link_libraries(lmsaudio PRIVATE # 设置目标库需要私有链接的库# PRIVATE表示这些依赖库仅对lmsaudio本身可见，不会传递给依赖它的目标
	PkgConfig::LIBAV
	PkgConfig::Taglib
	)
