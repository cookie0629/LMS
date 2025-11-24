#include "audio/IAudioFileInfo.hpp"

#include <array>
#include <filesystem>
#include <map>
#include <memory>
#include <optional>
#include <span>
#include <string>
#include <system_error>
#include <utility>
#include <vector>

#if defined(LMS_AUDIO_HAS_TAGLIB)
#include <taglib/audioproperties.h>
#include <taglib/fileref.h>
#include <taglib/tag.h>
#include <taglib/tpropertymap.h>
#endif

#include "audio/AudioProperties.hpp"
#include "audio/Exception.hpp"
#include "audio/IImageReader.hpp"
#include "audio/ITagReader.hpp"

namespace lms::audio
{
    namespace
    {
        class SimpleTagReader : public ITagReader
        {
        public:
            std::optional<std::string> getTag(TagType tagType) const override
            {
                auto it = _values.find(tagType);
                if (it == _values.end() || it->second.empty())
                {
                    return std::nullopt;
                }

                return it->second.front();
            }

            std::vector<std::string> getMultiTag(TagType tagType) const override
            {
                auto it = _values.find(tagType);
                if (it == _values.end())
                {
                    return {};
                }
                return it->second;
            }

            bool hasTag(TagType tagType) const override
            {
                return _values.contains(tagType) && !_values.at(tagType).empty();
            }

            void setValue(TagType tagType, std::string value, bool overwriteIfExists = false)
            {
                if (value.empty())
                {
                    return;
                }

                if (!overwriteIfExists && hasTag(tagType))
                {
                    return;
                }

                _values[tagType] = { std::move(value) };
            }

            void setValues(TagType tagType, std::vector<std::string> values, bool overwriteIfExists = false)
            {
                if (values.empty())
                {
                    return;
                }

                if (!overwriteIfExists && hasTag(tagType))
                {
                    return;
                }

                _values[tagType] = std::move(values);
            }

        private:
            std::map<TagType, std::vector<std::string>> _values;
        };
    } // namespace

    /**
     * @brief 音频文件信息实现
     */
    class AudioFileInfo : public IAudioFileInfo
    {
    public:
        AudioFileInfo()
            : AudioFileInfo(AudioProperties{}, std::make_unique<ImageReader>(), std::make_unique<TagReader>())
        {
        }

        AudioFileInfo(AudioProperties audioProps,
                      std::unique_ptr<IImageReader> imageReader,
                      std::unique_ptr<ITagReader> tagReader)
            : _audioProperties{ std::move(audioProps) }
            , _imageReader{ imageReader ? std::move(imageReader) : std::make_unique<ImageReader>() }
            , _tagReader{ tagReader ? std::move(tagReader) : std::make_unique<TagReader>() }
        {
        }

        const AudioProperties& getAudioProperties() const override
        {
            return _audioProperties;
        }

        const IImageReader& getImageReader() const override
        {
            return *_imageReader;
        }

        const ITagReader& getTagReader() const override
        {
            return *_tagReader;
        }

    private:
        AudioProperties _audioProperties;
        std::unique_ptr<IImageReader> _imageReader;
        std::unique_ptr<ITagReader> _tagReader;
    };

#if defined(LMS_AUDIO_HAS_TAGLIB)
    namespace
    {
        std::string toUtf8(const TagLib::String& str)
        {
            return str.toCString(true);
        }

        TagLib::AudioProperties::ReadStyle toTagLibReadStyle(ParserOptions::AudioPropertiesReadStyle style)
        {
            switch (style)
            {
            case ParserOptions::AudioPropertiesReadStyle::Fast:
                return TagLib::AudioProperties::Fast;
            case ParserOptions::AudioPropertiesReadStyle::Accurate:
                return TagLib::AudioProperties::Accurate;
            case ParserOptions::AudioPropertiesReadStyle::Average:
            default:
                return TagLib::AudioProperties::Average;
            }
        }

        std::vector<std::string> toStringVector(const TagLib::StringList& list)
        {
            std::vector<std::string> values;
            values.reserve(list.size());
            for (const auto& entry : list)
            {
                auto text = toUtf8(entry);
                if (!text.empty())
                {
                    values.emplace_back(std::move(text));
                }
            }
            return values;
        }

        void populateStandardTags(const TagLib::Tag& tag, SimpleTagReader& reader)
        {
            reader.setValue(TagType::Title, toUtf8(tag.title()));
            reader.setValue(TagType::Artist, toUtf8(tag.artist()));
            reader.setValue(TagType::Album, toUtf8(tag.album()));
            reader.setValue(TagType::Comment, toUtf8(tag.comment()));
            reader.setValue(TagType::Genre, toUtf8(tag.genre()));

            if (auto year = tag.year(); year != 0)
            {
                reader.setValue(TagType::Date, std::to_string(year));
            }

            if (auto track = tag.track(); track != 0)
            {
                reader.setValue(TagType::TrackNumber, std::to_string(track), true);
            }
        }

        void populatePropertyMapTags(const TagLib::File& file, SimpleTagReader& reader)
        {
            static const std::array<std::pair<const char*, TagType>, 6> propertyMappings = {
                {
                    { "ALBUMARTIST", TagType::AlbumArtist },
                    { "ARTIST", TagType::Artist },
                    { "ALBUM", TagType::Album },
                    { "TITLE", TagType::Title },
                    { "TRACKNUMBER", TagType::TrackNumber },
                    { "DISCNUMBER", TagType::DiscNumber },
                }
            };

            const auto properties = file.properties();
            for (const auto& [key, tagType] : propertyMappings)
            {
                auto propertyIt = properties.find(TagLib::String(key, TagLib::String::Latin1));
                if (propertyIt != properties.end())
                {
                    reader.setValues(tagType, toStringVector(propertyIt->second), true);
                }
            }
        }

        AudioProperties buildAudioProperties(const TagLib::AudioProperties* props)
        {
            if (!props)
            {
                return AudioProperties{};
            }

            return AudioProperties(
                static_cast<std::size_t>(props->lengthInMilliseconds()),
                static_cast<std::size_t>(props->bitrate()),
                static_cast<std::size_t>(props->sampleRate()),
                static_cast<std::size_t>(props->channels()),
                0);
        }
    } // namespace
#endif

    std::unique_ptr<IAudioFileInfo> parseAudioFile(const std::filesystem::path& p, const ParserOptions& parserOptions)
    {
#if defined(LMS_AUDIO_HAS_TAGLIB)
        std::error_code ec;
        if (!std::filesystem::exists(p, ec))
        {
            throw IOException("Audio file does not exist", ec);
        }

        const auto readStyle = toTagLibReadStyle(parserOptions.readStyle);
        TagLib::FileRef fileRef(p.c_str(), true, readStyle);

        if (fileRef.isNull() || fileRef.file() == nullptr)
        {
            throw AudioFileParsingException("Unable to parse audio file: " + p.string());
        }

        auto tagReader = std::make_unique<SimpleTagReader>();

        if (const auto* tag = fileRef.tag())
        {
            populateStandardTags(*tag, *tagReader);
        }

        populatePropertyMapTags(*fileRef.file(), *tagReader);

        auto audioProps = buildAudioProperties(fileRef.audioProperties());

        return std::make_unique<AudioFileInfo>(
            std::move(audioProps),
            std::make_unique<ImageReader>(),
            std::move(tagReader));
#else
        (void)p;
        (void)parserOptions;
        return std::make_unique<AudioFileInfo>();
#endif
    }

    std::span<const std::filesystem::path> getSupportedExtensions(ParserOptions::Parser parser)
    {
        static constexpr std::array<std::filesystem::path, 0> kEmpty{};

#if defined(LMS_AUDIO_HAS_TAGLIB)
        static constexpr std::array<std::filesystem::path, 12> kTagLibExtensions{
            ".mp3", ".flac", ".ogg", ".oga", ".wav", ".aiff", ".aif", ".m4a", ".mp4", ".wma", ".wv", ".ape"
        };

        if (parser == ParserOptions::Parser::TagLib)
        {
            return kTagLibExtensions;
        }
#endif

        (void)parser;
        return kEmpty;
    }
} // namespace lms::audio

