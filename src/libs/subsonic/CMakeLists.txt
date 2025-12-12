
add_library(lmssubsonic STATIC
	impl/endpoints/AlbumSongLists.cpp # 端点实现文件
	impl/endpoints/Bookmarks.cpp
	impl/endpoints/Browsing.cpp
	impl/endpoints/MediaAnnotation.cpp
	impl/endpoints/MediaLibraryScanning.cpp
	impl/endpoints/MediaRetrieval.cpp
	impl/endpoints/Playlists.cpp
	impl/endpoints/Podcast.cpp
	impl/endpoints/Searching.cpp
	impl/endpoints/System.cpp
	impl/endpoints/UserManagement.cpp
	impl/responses/Album.cpp # 响应类实现文件
	impl/responses/AlbumInfo.cpp
	impl/responses/Artist.cpp
	impl/responses/Bookmark.cpp
	impl/responses/Contributor.cpp
	impl/responses/DiscTitle.cpp
	impl/responses/ItemDate.cpp
	impl/responses/ItemGenre.cpp
	impl/responses/Genre.cpp
	impl/responses/Lyrics.cpp
	impl/responses/Playlist.cpp
	impl/responses/Podcast.cpp
	impl/responses/RecordLabel.cpp
	impl/responses/ReplayGain.cpp
	impl/responses/Song.cpp
	impl/responses/User.cpp
	impl/CoverArtId.cpp
	impl/RequestContext.cpp
	impl/ResponseFormat.cpp
	impl/ProtocolVersion.cpp
	impl/ParameterParsing.cpp
	impl/SubsonicId.cpp
	impl/SubsonicResource.cpp
	impl/SubsonicResourceConfig.cpp
	impl/SubsonicResponse.cpp
	)

target_include_directories(lmssubsonic INTERFACE # 设置目标库的公共包含目录，供其他依赖此库的项目使用
	include
	)

target_include_directories(lmssubsonic PRIVATE
	impl
	include
	)

target_link_libraries(lmssubsonic PRIVATE
	lmsartwork
	lmsauth
	lmsaudio
	lmsdatabase
	lmsfeedback
	lmspodcast
	lmsrecommendation
	lmsscanner
	lmsscrobbling
	lmstranscoding
	lmscore
	std::filesystem
	)

target_link_libraries(lmssubsonic PUBLIC
	Wt::Wt
	)

if(BUILD_TESTING AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/test/CMakeLists.txt")
	add_subdirectory(test)
endif()

if (BUILD_BENCHMARKS AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/bench/CMakeLists.txt") # 条件编译：如果启用了基准测试构建且基准测试目录存在，则添加基准测试子目录
	add_subdirectory(bench)
endif()