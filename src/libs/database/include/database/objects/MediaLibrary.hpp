#pragma once

#include <filesystem>
#include <functional>
#include <string>
#include <string_view>
#include <vector>

#include <Wt/Dbo/Field.h>

#include "database/objects/MediaLibraryId.hpp"
#include "database/Object.hpp"

namespace lms::db
{
    class Session;

    /**
     * @brief 媒体库对象（简化版）
     */
    class MediaLibrary final : public Object<MediaLibrary, MediaLibraryId>
    {
    public:
        static constexpr std::size_t maxNameLength{ 128 };

        MediaLibrary() = default;

        // 静态方法
        static std::size_t getCount(Session& session);
        static pointer find(Session& session, MediaLibraryId id);
        static pointer find(Session& session, std::string_view name);
        static pointer find(Session& session, const std::filesystem::path& path);
        static void find(Session& session, std::function<void(const pointer&)> func);
        static std::vector<pointer> findAll(Session& session);
        static pointer getOrCreate(Session& session, std::string_view name, const std::filesystem::path& path);

        // 访问器
        std::string_view getName() const { return _name; }
        void setName(std::string_view name);

        std::filesystem::path getPath() const { return _path; }
        void setPath(const std::filesystem::path& p);

        bool isEmpty() const;

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

        std::string _path;  // 存储为字符串
        std::string _name;
    };
} // namespace lms::db

