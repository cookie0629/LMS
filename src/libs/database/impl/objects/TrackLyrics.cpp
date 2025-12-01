#include "database/objects/TrackLyrics.hpp"

#include <Wt/Dbo/Impl.h>
#include <Wt/Dbo/WtSqlTraits.h>
#include <Wt/Json/Array.h>
#include <Wt/Json/Object.h>
#include <Wt/Json/Parser.h>
#include <Wt/Json/Serializer.h>

#include "database/Session.hpp"
#include "database/Transaction.hpp"
#include "database/objects/Directory.hpp"
#include "database/objects/Track.hpp"
#include "traits/PathTraits.hpp"

DBO_INSTANTIATE_TEMPLATES(lms::db::TrackLyrics)

namespace lms::db
{
    TrackLyrics::pointer TrackLyrics::create(Session& session)
    {
        session.checkWriteTransaction();
        auto lyrics = std::unique_ptr<TrackLyrics>(new TrackLyrics{});
        auto dboPtr = session.getDboSession()->add(std::move(lyrics));
        return pointer(dboPtr);
    }

    std::size_t TrackLyrics::getCount(Session& session)
    {
        session.checkReadTransaction();
        return session.getDboSession()->find<TrackLyrics>().resultList().size();
    }

    TrackLyrics::pointer TrackLyrics::find(Session& session, TrackLyricsId id)
    {
        session.checkReadTransaction();
        auto result = session.getDboSession()
                          ->find<TrackLyrics>()
                          .where("id = ?")
                          .bind(id.getValue())
                          .resultValue();
        if (!result)
        {
            return {};
        }
        return pointer(result);
    }

    TrackLyrics::pointer TrackLyrics::find(Session& session, const std::filesystem::path& absolutePath)
    {
        session.checkReadTransaction();
        auto result = session.getDboSession()
                          ->find<TrackLyrics>()
                          .where("absolute_file_path = ?")
                          .bind(absolutePath.string())
                          .resultValue();
        if (!result)
        {
            return {};
        }
        return pointer(result);
    }

    TrackLyrics::pointer TrackLyrics::find(Session& session, TrackId trackId)
    {
        session.checkReadTransaction();
        auto result = session.getDboSession()
                          ->find<TrackLyrics>()
                          .where("track_id = ?")
                          .bind(trackId.getValue())
                          .limit(1)
                          .resultValue();
        if (!result)
        {
            return {};
        }
        return pointer(result);
    }

    void TrackLyrics::find(Session& session, const FindParameters& params, const std::function<void(const pointer&)>& func)
    {
        session.checkReadTransaction();
        auto query = session.getDboSession()->find<TrackLyrics>();

        if (params.track.getValue() != 0)
        {
            query.where("track_id = ?").bind(params.track.getValue());
        }

        if (params.external.has_value())
        {
            if (*params.external)
            {
                query.where("absolute_file_path <> ''");
            }
            else
            {
                query.where("absolute_file_path = ''");
            }
        }

        switch (params.sortMethod)
        {
        case TrackLyricsSortMethod::None:
            break;
        case TrackLyricsSortMethod::ExternalFirst:
            query.orderBy("CASE WHEN absolute_file_path <> '' THEN 0 ELSE 1 END");
            break;
        case TrackLyricsSortMethod::EmbeddedFirst:
            query.orderBy("CASE WHEN absolute_file_path = '' THEN 0 ELSE 1 END");
            break;
        }

        if (params.range)
        {
            query.offset(params.range->offset).limit(params.range->size);
        }

        auto results = query.resultList();
        for (auto& lyrics : results)
        {
            func(pointer(lyrics));
        }
    }

    TrackLyrics::SynchronizedLines TrackLyrics::getSynchronizedLines() const
    {
        SynchronizedLines synchronizedLines;
        if (!_synchronized || _lines.empty())
        {
            return synchronizedLines;
        }

        try
        {
            Wt::Json::Object root;
            Wt::Json::parse(_lines, root);

            if (root.type("lines") == Wt::Json::Type::Array)
            {
                const Wt::Json::Array& linesArray = root.get("lines");
                for (const Wt::Json::Value& line : linesArray)
                {
                    if (line.type() == Wt::Json::Type::Object)
                    {
                        const Wt::Json::Object& entry = line;
                        std::chrono::milliseconds timestamp{ entry.get("timestamp") };
                        std::string lineText{ static_cast<std::string>(entry.get("value")) };
                        synchronizedLines.emplace(timestamp, std::move(lineText));
                    }
                }
            }
        }
        catch (...)
        {
            // JSON 解析失败，返回空结果
        }

        return synchronizedLines;
    }

    std::vector<std::string> TrackLyrics::getUnsynchronizedLines() const
    {
        std::vector<std::string> unsynchronizedLines;
        if (_synchronized || _lines.empty())
        {
            return unsynchronizedLines;
        }

        try
        {
            Wt::Json::Object root;
            Wt::Json::parse(_lines, root);

            if (root.type("lines") == Wt::Json::Type::Array)
            {
                const Wt::Json::Array& linesArray = root.get("lines");
                for (const Wt::Json::Value& line : linesArray)
                {
                    unsynchronizedLines.push_back(line.toString());
                }
            }
        }
        catch (...)
        {
            // JSON 解析失败，返回空结果
        }

        return unsynchronizedLines;
    }

    void TrackLyrics::setAbsoluteFilePath(const std::filesystem::path& path)
    {
        if (path.is_absolute())
        {
            _fileAbsolutePath = path;
            _fileStem = path.stem().string();
        }
    }

    void TrackLyrics::setSynchronizedLines(const SynchronizedLines& synchronizedLines)
    {
        Wt::Json::Object root;
        Wt::Json::Array lines;

        for (const auto& [timestamp, lineText] : synchronizedLines)
        {
            Wt::Json::Object line;
            line["timestamp"] = Wt::Json::Value{ static_cast<long long>(timestamp.count()) };
            line["value"] = Wt::Json::Value{ lineText };
            lines.push_back(std::move(line));
        }

        root["lines"] = std::move(lines);
        _synchronized = true;
        _lines = Wt::Json::serialize(root);
    }

    void TrackLyrics::setUnsynchronizedLines(const std::vector<std::string>& unsynchronizedLines)
    {
        Wt::Json::Object root;
        Wt::Json::Array lines;

        for (const auto& lineText : unsynchronizedLines)
        {
            lines.push_back(Wt::Json::Value{ lineText });
        }

        root["lines"] = std::move(lines);
        _synchronized = false;
        _lines = Wt::Json::serialize(root);
    }

    ObjectPtr<Track> TrackLyrics::getTrack() const
    {
        if (!_track)
        {
            return {};
        }
        return ObjectPtr<Track>(_track);
    }

    ObjectPtr<Directory> TrackLyrics::getDirectory() const
    {
        if (!_directory)
        {
            return {};
        }
        return ObjectPtr<Directory>(_directory);
    }

    void TrackLyrics::setTrack(ObjectPtr<Track> track)
    {
        _track = getDboPtr(track);
    }

    void TrackLyrics::setDirectory(ObjectPtr<Directory> directory)
    {
        _directory = getDboPtr(directory);
    }
} // namespace lms::db

