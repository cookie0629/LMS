
#pragma once

#include "RequestContext.hpp"
#include "SubsonicResponse.hpp"

namespace lms::api::subsonic
{
    Response handleGetMusicFoldersRequest(RequestContext& context);
    Response handleGetIndexesRequest(RequestContext& context);
    Response handleGetMusicDirectoryRequest(RequestContext& context);
    Response handleGetGenresRequest(RequestContext& context);
    Response handleGetArtistsRequest(RequestContext& context);
    Response handleGetArtistRequest(RequestContext& context);
    Response handleGetAlbumRequest(RequestContext& context);
    Response handleGetSongRequest(RequestContext& context);
    Response handleGetArtistInfo2Request(RequestContext& context);
    Response handleGetAlbumInfo(RequestContext& context);
    Response handleGetAlbumInfo2(RequestContext& context);
    Response handleGetSimilarSongsRequest(RequestContext& context);
    Response handleGetSimilarSongs2Request(RequestContext& context);
    Response handleGetTopSongs(RequestContext& context);
} // namespace lms::api::subsonic
