#pragma once

#include <filesystem>
#include <functional>
#include <string>
#include <string_view>

#include <Wt/Dbo/Field.h>
#include <Wt/Dbo/collection.h>

#include "database/objects/DirectoryId.hpp"
#include "database/objects/MediaLibraryId.hpp"
#include "database/Object.hpp"

namespace lms::db
{
    class MediaLibrary;
    class Session;

    /**
     * @brief 目录对象（简化版）
     */
    class Directory final : public Object<Directory, DirectoryId>
    {
    public:
        Directory() = default;

        // 静态方法
        static std::size_t getCount(Session& session);
        static pointer find(Session& session, DirectoryId id);
        static pointer find(Session& session, const std::filesystem::path& path);
        static void find(Session& session, std::function<void(const pointer&)> func);
        static pointer getOrCreate(Session& session, const std::filesystem::path& path, ObjectPtr<MediaLibrary> mediaLibrary, ObjectPtr<Directory> parent = {});

        // 访问器
        std::filesystem::path getAbsolutePath() const { return _absolutePath; }
        void setAbsolutePath(const std::filesystem::path& p);

        std::string_view getName() const { return _name; }

        ObjectPtr<Directory> getParentDirectory() const { return _parent; }
        DirectoryId getParentDirectoryId() const;
        void setParent(ObjectPtr<Directory> parent);

        ObjectPtr<MediaLibrary> getMediaLibrary() const { return _mediaLibrary; }
        MediaLibraryId getMediaLibraryId() const;
        void setMediaLibrary(ObjectPtr<MediaLibrary> mediaLibrary);

        template<class Action>
        void persist(Action& a)
        {
            Wt::Dbo::field(a, _absolutePath, "absolute_path");
            Wt::Dbo::field(a, _name, "name");
            Wt::Dbo::belongsTo(a, _parent, "parent_directory_id", Wt::Dbo::OnDeleteCascade);
            Wt::Dbo::belongsTo(a, _mediaLibrary, "media_library_id", Wt::Dbo::OnDeleteSetNull);
        }

    private:
        friend class Session;
        Directory(const std::filesystem::path& p);
        static pointer create(Session& session, const std::filesystem::path& p, ObjectPtr<MediaLibrary> mediaLibrary, ObjectPtr<Directory> parent);

        std::string _absolutePath;  // 存储为字符串
        std::string _name;

        Wt::Dbo::ptr<Directory> _parent;
        Wt::Dbo::ptr<MediaLibrary> _mediaLibrary;
    };
} // namespace lms::db

