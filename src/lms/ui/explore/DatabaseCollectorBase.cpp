
#include "DatabaseCollectorBase.hpp"

#include <algorithm>

#include "core/String.hpp"

#include "explore/Filters.hpp"

namespace lms::ui
{
    DatabaseCollectorBase::DatabaseCollectorBase(Filters& filters, Mode defaultMode, std::size_t maxCount)
        : _filters{ filters }
        , _mode{ defaultMode }
        , _maxCount{ maxCount }
    {
    }

    DatabaseCollectorBase::Range DatabaseCollectorBase::getActualRange(std::optional<db::Range> requestedRange) const
    {
        db::Range res;

        if (!requestedRange)
        {
            res.offset = 0;
            res.size = _maxCount;
        }
        else
        {
            res.offset = requestedRange->offset;
            if (requestedRange->offset < _maxCount)
                res.size = std::min(_maxCount - requestedRange->offset, requestedRange->size);
            else
                res.size = 0;
        }

        return res;
    }

    std::size_t DatabaseCollectorBase::getMaxCount() const
    {
        return _maxCount;
    }

    const db::Filters& DatabaseCollectorBase::getDbFilters() const
    {
        return _filters.getDbFilters();
    }

    void DatabaseCollectorBase::setSearch(std::string_view searchText)
    {
        _searchText = searchText;
        if (!searchText.empty())
            _searchKeywords = core::stringUtils::splitString(_searchText, ' ');
        else
            _searchKeywords.clear();
    }

} // namespace lms::ui
