
#pragma once

#include <optional>

#include <Wt/WStringListModel.h>

namespace lms::ui
{
    // Helper class
    template<typename T>
    class ValueStringModel : public Wt::WStringListModel
    {
    public:
        T getValue(std::size_t row) const
        {
            return Wt::cpp17::any_cast<T>(data(index(static_cast<int>(row), 0), Wt::ItemDataRole::User));
        }

        Wt::WString getString(std::size_t row) const
        {
            return Wt::cpp17::any_cast<Wt::WString>(data(index(static_cast<int>(row), 0), Wt::ItemDataRole::Display));
        }

        std::optional<std::size_t> getRowFromString(const Wt::WString& value) const
        {
            for (std::size_t i{}; i < static_cast<std::size_t>(rowCount()); ++i)
            {
                if (getString(i) == value)
                    return i;
            }

            return std::nullopt;
        }

        std::optional<std::size_t> getRowFromValue(const T& value) const
        {
            for (std::size_t i{}; i < static_cast<std::size_t>(rowCount()); ++i)
            {
                if (getValue(i) == value)
                    return i;
            }

            return std::nullopt;
        }

        void add(const Wt::WString& str, const T& value)
        {
            insertRows(rowCount(), 1);
            setData(rowCount() - 1, 0, value, Wt::ItemDataRole::User);
            setData(rowCount() - 1, 0, str, Wt::ItemDataRole::Display);
        }

        void clear()
        {
            removeRows(0, rowCount());
        }
    };
} // namespace lms::ui
