
#pragma once

#include <filesystem>
#include <functional>
#include <string>
#include <string_view>

#include <Wt/Dbo/Field.h>

#include "database/Object.hpp"
#include "database/objects/MediaLibraryId.hpp"

namespace lms::db
{
    class Session;

    class MediaLibrary final : public Object<MediaLibrary, MediaLibraryId>
    {
    public:
        static const std::size_t maxNameLength{ 128 };

        MediaLibrary() = default;

        // find
        static std::size_t getCount(Session& session);
        static pointer find(Session& session, MediaLibraryId id);
        static pointer find(Session& session, std::string_view name);
        static pointer find(Session& session, const std::filesystem::path& path);
        static void find(Session& session, std::function<void(const pointer&)> func);
        static std::vector<pointer> find(Session& session);

        // getters
        std::string_view getName() const { return _name; }
        const std::filesystem::path& getPath() const { return _path; }
        bool isEmpty() const;

        // setters
        void setName(std::string_view name) { _name = name; }
        void setPath(const std::filesystem::path& p);

        template<class Action>
        void persist(Action& a)
        {
            Wt::Dbo::field(a, _path, "path");
            Wt::Dbo::field(a, _name, "name");
        }

    private:
        friend class Session;
        MediaLibrary(std::string_view name, const std::filesystem::path& p);
        static pointer create(Session& session, std::string_view name, const std::filesystem::path& p);

        std::filesystem::path _path;
        std::string _name;
    };
} // namespace lms::db
