
#include "Playlists.hpp"

#include "database/Session.hpp"
#include "database/objects/Track.hpp"
#include "database/objects/TrackList.hpp"
#include "database/objects/User.hpp"

#include "ParameterParsing.hpp"
#include "SubsonicId.hpp"
#include "responses/Playlist.hpp"
#include "responses/Song.hpp"

namespace lms::api::subsonic
{
    using namespace db;

    namespace
    {
        void checkTrackListModificationAccess(const db::TrackList::pointer& trackList, const db::UserId currentUserId)
        {
            if (!trackList || trackList->getType() != TrackListType::PlayList)
                throw RequestedDataNotFoundError{};

            // Can only modify own playlists
            if (trackList->getUserId() != currentUserId)
            {
                if (trackList->getVisibility() == TrackList::Visibility::Public)
                    throw UserNotAuthorizedError{};

                throw RequestedDataNotFoundError{};
            }
        }
    } // namespace

    Response handleGetPlaylistsRequest(RequestContext& context)
    {
        auto transaction{ context.getDbSession().createReadTransaction() };

        Response response{ Response::createOkResponse(context.getServerProtocolVersion()) };
        Response::Node& playlistsNode{ response.createNode("playlists") };

        auto addTrackList{ [&](const db::TrackList::pointer& trackList) {
            playlistsNode.addArrayChild("playlist", createPlaylistNode(context, trackList));
        } };

        // First add user's playlists
        {
            TrackList::FindParameters params;
            params.setUser(context.getUser()->getId());
            params.setType(TrackListType::PlayList);

            db::TrackList::find(context.getDbSession(), params, [&](const db::TrackList::pointer& trackList) {
                addTrackList(trackList);
            });
        }

        // Then add others public playlists
        {
            TrackList::FindParameters params;
            params.setVisibility(TrackList::Visibility::Public);
            params.setType(TrackListType::PlayList);
            params.setExcludedUser(context.getUser()->getId());

            db::TrackList::find(context.getDbSession(), params, [&](const db::TrackList::pointer& trackList) {
                assert(trackList->getUserId() != context.getUser()->getId());
                addTrackList(trackList);
            });
        }

        return response;
    }

    Response handleGetPlaylistRequest(RequestContext& context)
    {
        // Mandatory params
        TrackListId trackListId{ getMandatoryParameterAs<TrackListId>(context.getParameters(), "id") };

        auto transaction{ context.getDbSession().createReadTransaction() };

        TrackList::pointer trackList{ TrackList::find(context.getDbSession(), trackListId) };
        if (!trackList || trackList->getType() != TrackListType::PlayList)
            throw RequestedDataNotFoundError{};

        if (trackList->getUserId() != context.getUser()->getId() && trackList->getVisibility() != TrackList::Visibility::Public)
            throw RequestedDataNotFoundError{};

        Response response{ Response::createOkResponse(context.getServerProtocolVersion()) };
        Response::Node playlistNode{ createPlaylistNode(context, trackList) };

        auto entries{ trackList->getEntries() };
        for (const TrackListEntry::pointer& entry : entries.results)
            playlistNode.addArrayChild("entry", createSongNode(context, entry->getTrack(), context.getUser()));

        response.addNode("playlist", std::move(playlistNode));

        return response;
    }

    Response handleCreatePlaylistRequest(RequestContext& context)
    {
        // Optional params
        const auto id{ getParameterAs<TrackListId>(context.getParameters(), "playlistId") };
        auto name{ getParameterAs<std::string>(context.getParameters(), "name") };

        std::vector<TrackId> trackIds{ getMultiParametersAs<TrackId>(context.getParameters(), "songId") };

        if (!name && !id)
            throw RequiredParameterMissingError{ "name or playlistId" };

        auto transaction{ context.getDbSession().createWriteTransaction() };

        TrackList::pointer trackList;
        if (id)
        {
            trackList = TrackList::find(context.getDbSession(), *id);
            checkTrackListModificationAccess(trackList, context.getUser()->getId());

            if (name)
                trackList.modify()->setName(*name);

            trackList.modify()->clear();
            trackList.modify()->setLastModifiedDateTime(Wt::WDateTime::currentDateTime());
        }
        else
        {
            trackList = context.getDbSession().create<TrackList>(*name, TrackListType::PlayList);
            trackList.modify()->setUser(context.getUser());
            trackList.modify()->setVisibility(TrackList::Visibility::Private);
        }

        for (const TrackId trackId : trackIds)
        {
            Track::pointer track{ Track::find(context.getDbSession(), trackId) };
            if (!track)
                continue;

            context.getDbSession().create<TrackListEntry>(track, trackList);
        }

        Response response{ Response::createOkResponse(context.getServerProtocolVersion()) };
        Response::Node playlistNode{ createPlaylistNode(context, trackList) };

        auto entries{ trackList->getEntries() };
        for (const TrackListEntry::pointer& entry : entries.results)
            playlistNode.addArrayChild("entry", createSongNode(context, entry->getTrack(), context.getUser()));

        response.addNode("playlist", std::move(playlistNode));

        return response;
    }

    Response handleUpdatePlaylistRequest(RequestContext& context)
    {
        // Mandatory params
        TrackListId id{ getMandatoryParameterAs<TrackListId>(context.getParameters(), "playlistId") };

        // Optional parameters
        auto name{ getParameterAs<std::string>(context.getParameters(), "name") };
        auto isPublic{ getParameterAs<bool>(context.getParameters(), "public") };

        std::vector<TrackId> trackIdsToAdd{ getMultiParametersAs<TrackId>(context.getParameters(), "songIdToAdd") };
        std::vector<std::size_t> trackPositionsToRemove{ getMultiParametersAs<std::size_t>(context.getParameters(), "songIndexToRemove") };

        auto transaction{ context.getDbSession().createWriteTransaction() };

        TrackList::pointer trackList{ TrackList::find(context.getDbSession(), id) };
        checkTrackListModificationAccess(trackList, context.getUser()->getId());

        if (name)
            trackList.modify()->setName(*name);

        trackList.modify()->setVisibility(isPublic ? db::TrackList::Visibility::Public : db::TrackList::Visibility::Private);
        trackList.modify()->setLastModifiedDateTime(Wt::WDateTime::currentDateTime());

        {
            // Remove from end to make indexes stable
            std::sort(std::begin(trackPositionsToRemove), std::end(trackPositionsToRemove), std::greater<std::size_t>());

            for (std::size_t trackPositionToRemove : trackPositionsToRemove)
            {
                auto entry{ trackList->getEntry(trackPositionToRemove) };
                if (entry)
                    entry.remove();
            }
        }

        // Add tracks
        for (const TrackId trackIdToAdd : trackIdsToAdd)
        {
            Track::pointer track{ Track::find(context.getDbSession(), trackIdToAdd) };
            if (!track)
                continue;

            context.getDbSession().create<TrackListEntry>(track, trackList);
        }

        return Response::createOkResponse(context.getServerProtocolVersion());
    }

    Response handleDeletePlaylistRequest(RequestContext& context)
    {
        TrackListId id{ getMandatoryParameterAs<TrackListId>(context.getParameters(), "id") };

        auto transaction{ context.getDbSession().createWriteTransaction() };

        TrackList::pointer trackList{ TrackList::find(context.getDbSession(), id) };
        checkTrackListModificationAccess(trackList, context.getUser()->getId());

        trackList.remove();

        return Response::createOkResponse(context.getServerProtocolVersion());
    }
} // namespace lms::api::subsonic