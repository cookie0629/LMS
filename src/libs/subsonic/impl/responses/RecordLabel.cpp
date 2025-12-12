
#include "responses/RecordLabel.hpp"

#include "database/objects/Release.hpp"

namespace lms::api::subsonic
{
    Response::Node createRecordLabel(const db::ObjectPtr<db::Label>& label)
    {
        Response::Node recordLabelNode;

        recordLabelNode.setAttribute("name", label->getName());

        return recordLabelNode;
    }
} // namespace lms::api::subsonic