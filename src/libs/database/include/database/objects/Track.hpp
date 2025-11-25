#pragma once

#include <chrono>
#include <filesystem>
#include <functional>
#include <optional>
#include <string>
#include <string_view>

#include <Wt/Dbo/Field.h>
#include <Wt/Dbo/collection.h>
#include <Wt/WDateTime.h>

#include "database/Object.hpp"
#include "database/objects/TrackId.hpp"
#include "database/objects/ReleaseId.hpp"
#include "database/objects/MediumId.hpp"
#include "database/objects/DirectoryId.hpp"

namespace lms::db
{
    class Directory;
    class Medium;
    class Release;
    class Session;

    /**
     * @brief 轨道/曲目对象（简化版）
     */
    class Track final : public Object<Track, TrackId>
    {
    public:
        static constexpr std::size_t maxNameLength{ 512 };

        Track() = default;

        // 静态方法
        static std::size_t getCount(Session& session);
        static pointer find(Session& session, TrackId id);
        static pointer findByPath(Session& session, const std::filesystem::path& path);
        static void find(Session& session, std::function<void(const pointer&)> func);
        static pointer getOrCreate(Session& session, const std::filesystem::path& path);

        // 访问器
        std::string_view getName() const { return _name; }
        void setName(std::string_view name);

        std::filesystem::path getAbsoluteFilePath() const { return _absoluteFilePath; }
        void setAbsoluteFilePath(const std::filesystem::path& filePath);

        long long getFileSize() const { return _fileSize; }
        void setFileSize(long long fileSize) { _fileSize = fileSize; }

        Wt::WDateTime getLastWriteTime() const { return _fileLastWrite; }
        void setLastWriteTime(const Wt::WDateTime& time) { _fileLastWrite = time; }

        long long getDurationMs() const { return _durationMs; }
        std::chrono::milliseconds getDuration() const { return std::chrono::milliseconds(_durationMs); }
        void setDuration(std::chrono::milliseconds duration) { _durationMs = duration.count(); }

        std::optional<int> getTrackNumber() const { return _trackNumber; }
        void setTrackNumber(std::optional<int> num) { _trackNumber = num; }

        ReleaseId getReleaseId() const;
        void setRelease(ObjectPtr<Release> release);

        MediumId getMediumId() const;
        void setMedium(ObjectPtr<Medium> medium);

        DirectoryId getDirectoryId() const;
        void setDirectory(ObjectPtr<Directory> directory);

        template<class Action>
        void persist(Action& a)
        {
            Wt::Dbo::field(a, _name, "name");
            Wt::Dbo::field(a, _absoluteFilePath, "file_path");
            Wt::Dbo::field(a, _fileSize, "file_size");
            Wt::Dbo::field(a, _fileLastWrite, "file_last_write");
            Wt::Dbo::field(a, _durationMs, "duration_ms");
            Wt::Dbo::field(a, _trackNumber, "track_number");
            Wt::Dbo::belongsTo(a, _release, "release_id");
            Wt::Dbo::belongsTo(a, _medium, "medium_id");
            Wt::Dbo::belongsTo(a, _directory, "directory_id");
        }

    private:
        friend class Session;
        Track(const std::filesystem::path& filePath);
        static pointer create(Session& session, const std::filesystem::path& filePath);

        std::string _name;
        std::string _absoluteFilePath;  // 存储为字符串
        long long _fileSize{};
        Wt::WDateTime _fileLastWrite;
        long long _durationMs{};  // 存储为毫秒数
        std::optional<int> _trackNumber;
        Wt::Dbo::ptr<Release> _release;
        Wt::Dbo::ptr<Medium> _medium;
        Wt::Dbo::ptr<Directory> _directory;
    };
} // namespace lms::db

