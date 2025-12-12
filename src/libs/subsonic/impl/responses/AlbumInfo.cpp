
#include "AlbumInfo.hpp"

#include "database/objects/Release.hpp"

#include "RequestContext.hpp"

namespace lms::api::subsonic
{
    Response::Node createAlbumInfoNode(RequestContext& context, const db::ObjectPtr<db::Release>& release)
    {
        Response::Node albumInfo;

        if (const auto releaseMBID{ release->getMBID() })
        {
            switch (context.getResponseFormat())
            {
            case ResponseFormat::json:
                albumInfo.setAttribute("musicBrainzId", releaseMBID->getAsString());
                break;
            case ResponseFormat::xml:
                albumInfo.createChild("musicBrainzId").setValue(releaseMBID->getAsString());
                break;
            }
        }

        return albumInfo;
    }
} // namespace lms::api::subsonic