// 音频文件基础信息解析与缓存入口
#include "audio/IAudioFileInfo.hpp"  // 包含音频文件信息接口的头文件

#include <array>                    // 包含数组容器
#include <cstring>                  // 包含C风格字符串操作
#include <filesystem>               // 包含文件系统操作
#include <map>                      // 包含映射容器
#include <memory>                   // 包含智能指针
#include <optional>                 // 包含可选值类型
#include <span>                     // 包含span视图容器
#include <string>                   // 包含字符串类
#include <system_error>             // 包含系统错误处理
#include <utility>                  // 包含实用工具
#include <vector>                   // 包含动态数组容器

// 如果定义了LMS_AUDIO_HAS_TAGLIB宏，则包含TagLib相关的头文件
#if defined(LMS_AUDIO_HAS_TAGLIB)
#include <taglib/audioproperties.h>     // TagLib音频属性
#include <taglib/attachedpictureframe.h> // TagLib ID3v2附加图片帧
#include <taglib/fileref.h>             // TagLib文件引用
#include <taglib/flacfile.h>            // TagLib FLAC文件
#include <taglib/flacpicture.h>         // TagLib FLAC图片
#include <taglib/id3v2tag.h>            // TagLib ID3v2标签
#include <taglib/mpegfile.h>            // TagLib MPEG文件
#include <taglib/mp4coverart.h>         // TagLib MP4封面艺术
#include <taglib/mp4file.h>             // TagLib MP4文件
#include <taglib/mp4tag.h>              // TagLib MP4标签
#include <taglib/tag.h>                 // TagLib标签
#include <taglib/tbytevector.h>         // TagLib字节向量
#include <taglib/tpropertymap.h>        // TagLib属性映射
#endif

#include "audio/AudioProperties.hpp"    // 包含音频属性
#include "audio/Exception.hpp"          // 包含异常类
#include "audio/IImageReader.hpp"       // 包含图像读取器接口
#include "audio/ITagReader.hpp"         // 包含标签读取器接口

namespace lms::audio  // 定义lms::audio命名空间
{
    namespace  // 匿名命名空间，用于内部实现
    {
        /**
         * @brief 简单的标签读取器实现
         */
        class SimpleTagReader : public ITagReader
        {
        public:
            // 获取指定类型的单个标签值
            std::optional<std::string> getTag(TagType tagType) const override
            {
                auto it = _values.find(tagType);
                if (it == _values.end() || it->second.empty())
                {
                    return std::nullopt;
                }

                return it->second.front();
            }

            // 获取指定类型的多个标签值
            std::vector<std::string> getMultiTag(TagType tagType) const override
            {
                auto it = _values.find(tagType);
                if (it == _values.end())
                {
                    return {};
                }
                return it->second;
            }

            // 检查是否存在指定类型的标签
            bool hasTag(TagType tagType) const override
            {
                return _values.contains(tagType) && !_values.at(tagType).empty();
            }

            // 设置指定类型的标签值
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

            // 设置指定类型的多个标签值
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
            std::map<TagType, std::vector<std::string>> _values;  // 存储标签值的映射
        };
    } // namespace

    /**
     * @brief 音频文件信息实现类
     */
    class AudioFileInfo : public IAudioFileInfo
    {
    public:
        // 默认构造函数
        AudioFileInfo()
            : AudioFileInfo(AudioProperties{}, std::make_unique<ImageReader>(), std::make_unique<TagReader>())
        {
        }

        // 带参数的构造函数
        AudioFileInfo(AudioProperties audioProps,
                      std::unique_ptr<IImageReader> imageReader,
                      std::unique_ptr<ITagReader> tagReader)
            : _audioProperties{ std::move(audioProps) }
            , _imageReader{ imageReader ? std::move(imageReader) : std::make_unique<ImageReader>() }
            , _tagReader{ tagReader ? std::move(tagReader) : std::make_unique<TagReader>() }
        {
        }

        // 获取音频属性
        const AudioProperties& getAudioProperties() const override
        {
            return _audioProperties;
        }

        // 获取图像读取器
        const IImageReader& getImageReader() const override
        {
            return *_imageReader;
        }

        // 获取标签读取器
        const ITagReader& getTagReader() const override
        {
            return *_tagReader;
        }

    private:
        AudioProperties _audioProperties;  // 音频属性
        std::unique_ptr<IImageReader> _imageReader;  // 图像读取器
        std::unique_ptr<ITagReader> _tagReader;      // 标签读取器
    };

#if defined(LMS_AUDIO_HAS_TAGLIB)
    namespace  // 匿名命名空间，用于TagLib相关实现
    {
        // 将TagLib字符串转换为UTF8字符串
        std::string toUtf8(const TagLib::String& str)
        {
            return str.toCString(true);
        }

        // 将解析器选项转换为TagLib读取风格
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

        // 将TagLib字符串列表转换为字符串向量
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

        // 填充标准标签信息
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

        // 填充属性映射标签信息
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

        // 嵌入式图片数据结构
        struct EmbeddedImageData
        {
            std::vector<std::byte> bytes;  // 图片字节数据
            std::string mimeType{ "application/octet-stream" };  // MIME类型
        };

        // 将TagLib字节向量转换为字节向量
        std::vector<std::byte> toByteVector(const TagLib::ByteVector& byteVector)
        {
            if (byteVector.isEmpty())
            {
                return {};
            }

            std::vector<std::byte> buffer(byteVector.size());
            std::memcpy(buffer.data(), byteVector.data(), byteVector.size());
            return buffer;
        }

        // 构建嵌入式图片数据
        EmbeddedImageData buildEmbeddedImageData(const TagLib::ByteVector& byteVector, std::string mimeType)
        {
            EmbeddedImageData data;
            data.bytes = toByteVector(byteVector);
            data.mimeType = mimeType.empty() ? "application/octet-stream" : std::move(mimeType);
            return data;
        }

        // 从ID3v2标签中提取图片
        std::optional<EmbeddedImageData> extractFromId3v2(const TagLib::ID3v2::Tag& tag)
        {
            const auto& frames = tag.frameListMap();
            auto it = frames.find("APIC");
            if (it == frames.end())
            {
                return std::nullopt;
            }

            for (const auto* frame : it->second)
            {
                const auto* pictureFrame = dynamic_cast<const TagLib::ID3v2::AttachedPictureFrame*>(frame);
                if (!pictureFrame)
                {
                    continue;
                }

                const auto picture = pictureFrame->picture();
                if (picture.isEmpty())
                {
                    continue;
                }

                return buildEmbeddedImageData(picture, pictureFrame->mimeType().toCString(true));
            }

            return std::nullopt;
        }

        // 从FLAC图片中提取图片
        std::optional<EmbeddedImageData> extractFromFlacPictures(const TagLib::List<TagLib::FLAC::Picture*>& pictures)
        {
            for (const auto* flacPicture : pictures)
            {
                if (!flacPicture)
                {
                    continue;
                }

                const auto picture = flacPicture->data();
                if (picture.isEmpty())
                {
                    continue;
                }

                return buildEmbeddedImageData(picture, flacPicture->mimeType().toCString(true));
            }

            return std::nullopt;
        }

        // 将MP4图片格式转换为MIME类型
        const char* mp4ImageFormatToMimeType(TagLib::MP4::CoverArt::Format format)
        {
            switch (format)
            {
            case TagLib::MP4::CoverArt::Format::BMP:
                return "image/bmp";
            case TagLib::MP4::CoverArt::Format::GIF:
                return "image/gif";
            case TagLib::MP4::CoverArt::Format::JPEG:
                return "image/jpeg";
            case TagLib::MP4::CoverArt::Format::PNG:
                return "image/png";
            case TagLib::MP4::CoverArt::Format::Unknown:
            default:
                return "application/octet-stream";
            }
        }

        // 从MP4文件中提取图片
        std::optional<EmbeddedImageData> extractFromMp4(const TagLib::MP4::File& mp4File)
        {
            const auto* tag = mp4File.tag();
            if (!tag)
            {
                return std::nullopt;
            }

            const auto item = tag->item("covr");
            if (!item.isValid())
            {
                return std::nullopt;
            }

            const auto coverList = item.toCoverArtList();
            for (const auto& cover : coverList)
            {
                const auto picture = cover.data();
                if (picture.isEmpty())
                {
                    continue;
                }

                return buildEmbeddedImageData(picture, mp4ImageFormatToMimeType(cover.format()));
            }

            return std::nullopt;
        }

        // 从音频文件中提取嵌入式图片
        std::optional<EmbeddedImageData> extractEmbeddedImage(TagLib::File& file)
        {
            if (auto* mpegFile = dynamic_cast<TagLib::MPEG::File*>(&file))
            {
                if (mpegFile->hasID3v2Tag())
                {
                    if (auto image = extractFromId3v2(*mpegFile->ID3v2Tag()))
                    {
                        return image;
                    }
                }
            }
            else if (auto* flacFile = dynamic_cast<TagLib::FLAC::File*>(&file))
            {
                if (auto image = extractFromFlacPictures(flacFile->pictureList()))
                {
                    return image;
                }

                if (flacFile->hasID3v2Tag())
                {
                    if (auto image = extractFromId3v2(*flacFile->ID3v2Tag()))
                    {
                        return image;
                    }
                }
            }
            else if (auto* mp4File = dynamic_cast<TagLib::MP4::File*>(&file))
            {
                if (auto image = extractFromMp4(*mp4File))
                {
                    return image;
                }
            }

            return std::nullopt;
        }

        // 构建音频属性
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

        auto imageReader = std::make_unique<ImageReader>();
        if (auto* file = fileRef.file())
        {
            if (auto image = extractEmbeddedImage(*file))
            {
                imageReader->setImage(std::move(image->bytes), std::move(image->mimeType));
            }
        }

        return std::make_unique<AudioFileInfo>(
            std::move(audioProps),
            std::move(imageReader),
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

