
#pragma once

#include <Wt/WSignal.h>
#include <Wt/WTemplateFormView.h>

#include "database/objects/MediaLibraryId.hpp"

namespace lms::ui
{
    class MediaLibraryModal : public Wt::WTemplateFormView
    {
    public:
        MediaLibraryModal(db::MediaLibraryId mediaLibraryId);

        Wt::Signal<db::MediaLibraryId>& saved() { return _saved; };
        Wt::Signal<>& cancelled() { return _cancelled; }

    private:
        Wt::Signal<db::MediaLibraryId> _saved;
        Wt::Signal<> _cancelled;
    };
} // namespace lms::ui