#pragma once

#include <chrono>
#include <filesystem>
#include <map>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include <Wt/Dbo/Field.h>
#include <Wt/WDateTime.h>

#include "database/Object.hpp"
#include "database/Types.hpp"
#include "database/objects/TrackId.hpp"
#include "database/objects/TrackLyricsId.hpp"

namespace lms::db
{
    class Directory;
    class Session;
    class Track;

    /**
     * @brief 歌词对象（简化版）
     */
    class TrackLyrics final : public Object<TrackLyrics, TrackLyricsId>
    {
    public:
        TrackLyrics() = default;

        struct FindParameters
        {
            std::optional<Range> range;
            TrackId track;
            std::optional<bool> external; // if set, true means external, false means embedded
            TrackLyricsSortMethod sortMethod{ TrackLyricsSortMethod::None };

            FindParameters& setRange(std::optional<Range> _range)
            {
                range = _range;
                return *this;
            }
            FindParameters& setTrack(TrackId _track)
            {
                track = _track;
                return *this;
            }
            FindParameters& setExternal(std::optional<bool> _external)
            {
                external = _external;
                return *this;
            }
            FindParameters& setSortMethod(TrackLyricsSortMethod _sortMethod)
            {
                sortMethod = _sortMethod;
                return *this;
            }
        };

        // 同步歌词行（时间戳 -> 歌词文本）
        using SynchronizedLines = std::map<std::chrono::milliseconds, std::string>;

        // find
        static std::size_t getCount(Session& session);
        static pointer find(Session& session, TrackLyricsId id);
        static pointer find(Session& session, TrackId trackId);
        static pointer find(Session& session, const std::filesystem::path& absolutePath);
        static void find(Session& session, const FindParameters& params, const std::function<void(const pointer&)>& func);

        // accessors
        const std::filesystem::path& getAbsoluteFilePath() const { return _fileAbsolutePath; }
        std::string_view getFileStem() const { return _fileStem; }
        const Wt::WDateTime& getLastWriteTime() const { return _fileLastWrite; }
        std::size_t getFileSize() const { return _fileSize; }
        std::string_view getLanguage() const { return _language; }
        std::string_view getDisplayArtist() const { return _displayArtist; }
        std::string_view getDisplayTitle() const { return _displayTitle; }
        std::chrono::milliseconds getOffset() const { return _offset; }
        bool isSynchronized() const { return _synchronized; }
        bool isExternal() const { return !_fileAbsolutePath.empty(); }
        SynchronizedLines getSynchronizedLines() const;
        std::vector<std::string> getUnsynchronizedLines() const;
        ObjectPtr<Track> getTrack() const;
        ObjectPtr<Directory> getDirectory() const;

        // modifiers
        void setAbsoluteFilePath(const std::filesystem::path& path);
        void setLastWriteTime(const Wt::WDateTime& fileLastWrite) { _fileLastWrite = fileLastWrite; }
        void setFileSize(std::size_t fileSize) { _fileSize = fileSize; }
        void setLanguage(std::string_view language) { _language = language; }
        void setOffset(std::chrono::milliseconds offset) { _offset = offset; }
        void setDisplayArtist(std::string_view displayArtist) { _displayArtist = displayArtist; }
        void setDisplayTitle(std::string_view displayTitle) { _displayTitle = displayTitle; }
        void setSynchronizedLines(const SynchronizedLines& lines);
        void setUnsynchronizedLines(const std::vector<std::string>& lines);
        void setTrack(ObjectPtr<Track> track);
        void setDirectory(ObjectPtr<Directory> directory);

        template<class Action>
        void persist(Action& a)
        {
            Wt::Dbo::field(a, _fileAbsolutePath, "absolute_file_path");
            Wt::Dbo::field(a, _fileStem, "stem");
            Wt::Dbo::field(a, _fileLastWrite, "file_last_write");
            Wt::Dbo::field(a, _fileSize, "file_size");
            Wt::Dbo::field(a, _lines, "lines");
            Wt::Dbo::field(a, _language, "language");
            Wt::Dbo::field(a, _offset, "offset");
            Wt::Dbo::field(a, _displayArtist, "display_artist");
            Wt::Dbo::field(a, _displayTitle, "display_title");
            Wt::Dbo::field(a, _synchronized, "synchronized");
            Wt::Dbo::belongsTo(a, _track, "track", Wt::Dbo::OnDeleteCascade);
            Wt::Dbo::belongsTo(a, _directory, "directory", Wt::Dbo::OnDeleteCascade);
        }

    private:
        friend class Session;
        static pointer create(Session& session);

        std::filesystem::path _fileAbsolutePath; // 外部歌词文件路径（如果为空则表示嵌入式歌词）
        std::string _fileStem;                   // 文件名（不含扩展名）
        Wt::WDateTime _fileLastWrite;            // 文件最后修改时间
        int _fileSize{};                         // 文件大小
        std::string _lines;                      // JSON 编码的歌词行（可能包含时间戳）
        std::string _language;                   // 语言代码
        std::chrono::duration<int, std::milli> _offset{}; // 时间偏移
        std::string _displayArtist;             // 显示艺术家
        std::string _displayTitle;              // 显示标题
        bool _synchronized{};                   // 是否为同步歌词

        Wt::Dbo::ptr<Track> _track;
        Wt::Dbo::ptr<Directory> _directory;
    };
} // namespace lms::db

