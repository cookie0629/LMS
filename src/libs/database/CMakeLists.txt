add_library(lmsdatabase STATIC # 创建一个名为 lmsdatabase 的静态库# STATIC 表示这是一个静态链接库
	impl/objects/Artist.cpp
	impl/objects/ArtistInfo.cpp
	impl/objects/Artwork.cpp
	impl/objects/AuthToken.cpp
	impl/objects/Cluster.cpp
	impl/objects/Directory.cpp
	impl/objects/Image.cpp
	impl/objects/Listen.cpp
	impl/objects/MediaLibrary.cpp
	impl/objects/Medium.cpp
	impl/objects/PlayListFile.cpp
	impl/objects/PlayQueue.cpp
	impl/objects/Podcast.cpp
	impl/objects/PodcastEpisode.cpp
	impl/objects/TrackArtistLink.cpp
	impl/objects/TrackFeatures.cpp
	impl/objects/TrackList.cpp
	impl/objects/RatedArtist.cpp
	impl/objects/RatedRelease.cpp
	impl/objects/RatedTrack.cpp
	impl/objects/Release.cpp
	impl/objects/ScanSettings.cpp
	impl/objects/StarredArtist.cpp
	impl/objects/StarredRelease.cpp
	impl/objects/StarredTrack.cpp
	impl/objects/Track.cpp
	impl/objects/TrackBookmark.cpp
	impl/objects/TrackEmbeddedImage.cpp
	impl/objects/TrackEmbeddedImageLink.cpp
	impl/objects/TrackLyrics.cpp
	impl/objects/UIState.cpp
	impl/objects/User.cpp
	impl/Db.cpp
	impl/IdType.cpp
	impl/Migration.cpp
	impl/Object.cpp
	impl/QueryPlanRecorder.cpp
	impl/Session.cpp
	impl/SqlQuery.cpp
	impl/Transaction.cpp
	impl/Types.cpp
	impl/Utils.cpp
	)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
	target_sources(lmsdatabase PRIVATE impl/TransactionChecker.cpp) # 在Debug模式下添加TransactionChecker.cpp到目标lmsdatabase的私有源文件中
endif()

target_include_directories(lmsdatabase INTERFACE
	include
	)

target_include_directories(lmsdatabase PRIVATE
	impl
	include
	)

target_link_libraries(lmsdatabase PRIVATE
	Wt::DboSqlite3
	)

target_link_libraries(lmsdatabase PUBLIC
	lmscore
	std::filesystem
	Wt::Dbo
	)

if(BUILD_TESTING AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/test/CMakeLists.txt")
	add_subdirectory(test)
endif()
