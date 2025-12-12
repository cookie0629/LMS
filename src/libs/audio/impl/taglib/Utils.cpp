
/*
 * @file Utils.cpp
 * @brief 工具函数实现，用于处理音频文件的元数据和属性
 */
#include "Utils.hpp"

#include "TagLibDefs.hpp"

// 引入TagLib支持的多种音频文件格式头文件
#include <taglib/aifffile.h>
#include <taglib/apefile.h>
#include <taglib/asffile.h>
#include <taglib/audioproperties.h>
#include <taglib/flacfile.h>
#include <taglib/id3v2framefactory.h>
#include <taglib/mp4file.h>
#include <taglib/mpcfile.h>
#include <taglib/mpegfile.h>
#include <taglib/oggflacfile.h>
#include <taglib/opusfile.h>
#include <taglib/speexfile.h>
#include <taglib/tfile.h>
#include <taglib/tfilestream.h>
#include <taglib/trueaudiofile.h>
#include <taglib/vorbisfile.h>
#include <taglib/wavfile.h>
#include <taglib/wavpackfile.h>
#if LMS_TAGLIB_HAS_DSF
    #include <taglib/dsffile.h>
#endif

// 引入核心日志和字符串处理头文件
#include "core/ILogger.hpp"
#include "core/ITraceLogger.hpp"
#include "core/String.hpp"

// 引入音频文件信息接口头文件
#include "audio/IAudioFileInfo.hpp"

namespace lms::audio::taglib::utils
{
    /*
     * @brief 获取支持的音频文件扩展名列表
     * @return 返回支持的音频文件扩展名的span视图
     */
    std::span<const std::filesystem::path> getSupportedExtensions()
    {
        // 静态存储支持的音频文件扩展名列表
        static const std::vector<std::filesystem::path> supportedExtensions{
            ".mp3",
            ".mp2",
            ".ogg",
            ".oga",
            ".flac",
            ".spx",
            ".opus",
            ".mpc",
            ".wv",
            ".ape",
            ".tta",
            ".m4a",
            ".m4r",
            ".m4b",
            ".m4p",
            ".3g2",
            ".m4v",
            ".wma",
            ".asf",
            ".aif",
            ".aiff",
            ".afc",
            ".aifc",
            ".wav",
#if LMS_TAGLIB_HAS_ADTS
            ".aac",
#endif
#if LMS_TAGLIB_HAS_DSF
            ".dsf",
#endif
        };

        return std::span<const std::filesystem::path>{ supportedExtensions };
    }

    TagLib::AudioProperties::ReadStyle readStyleToTagLibReadStyle(ParserOptions::ParserOptions::AudioPropertiesReadStyle readStyle)
    {
        switch (readStyle)
        {
        case ParserOptions::AudioPropertiesReadStyle::Fast:
            return TagLib::AudioProperties::ReadStyle::Fast;
        case ParserOptions::AudioPropertiesReadStyle::Average:
            return TagLib::AudioProperties::ReadStyle::Average;
        case ParserOptions::AudioPropertiesReadStyle::Accurate:
            return TagLib::AudioProperties::ReadStyle::Accurate;
        }

        throw Exception{ "Cannot convert read style" };
    }

    TagLib::FileStream createFileStream(const std::filesystem::path& p)
    {
        FILE* file{ std::fopen(p.c_str(), "r") };
        if (!file)
        {
            const std::error_code ec{ errno, std::generic_category() };
            LMS_LOG(METADATA, DEBUG, "fopen failed for " << p << ": " << ec.message());
            throw IOException{ "fopen failed", ec };
        }

        int fd{ ::fileno(file) };
        if (fd == -1)
        {
            const std::error_code ec{ errno, std::generic_category() };
            LMS_LOG(METADATA, DEBUG, "fileno failed for " << p << ": " << ec.message());
            throw IOException{ "fileno failed", ec };
        }

        return TagLib::FileStream{ fd, true };
    }

    std::unique_ptr<TagLib::File> parseFileByExtension(TagLib::FileStream* stream, const std::filesystem::path& extension, TagLib::AudioProperties::ReadStyle audioPropertiesStyle)
    {
        constexpr bool readAudioProperties{ true };
        std::unique_ptr<TagLib::File> file;

        if (extension.empty())
            return file;

        const std::string ext{ core::stringUtils::stringToUpper(extension.string().substr(1)) };

        // MP3
        if (ext == "MP3" || ext == "MP2" || ext == "AAC")
            file = std::make_unique<TagLib::MPEG::File>(stream, TagLib::ID3v2::FrameFactory::instance(), readAudioProperties, audioPropertiesStyle);
        // VORBIS
        else if (ext == "OGG")
            file = std::make_unique<TagLib::Ogg::Vorbis::File>(stream, readAudioProperties, audioPropertiesStyle);
        else if (ext == "OGA")
        {
            /* .oga can be any audio in the Ogg container. First try FLAC, then Vorbis. */
            file = std::make_unique<TagLib::Ogg::FLAC::File>(stream, readAudioProperties, audioPropertiesStyle);
            if (!file->isValid())
                file = std::make_unique<TagLib::Ogg::Vorbis::File>(stream, readAudioProperties, audioPropertiesStyle);
        }
        else if (ext == "FLAC")
            file = std::make_unique<TagLib::FLAC::File>(stream, TagLib::ID3v2::FrameFactory::instance(), readAudioProperties, audioPropertiesStyle);
        else if (ext == "SPX")
            file = std::make_unique<TagLib::Ogg::Speex::File>(stream, readAudioProperties, audioPropertiesStyle);
        else if (ext == "OPUS")
            file = std::make_unique<TagLib::Ogg::Opus::File>(stream, readAudioProperties, audioPropertiesStyle);
        // APE
        else if (ext == "MPC")
            file = std::make_unique<TagLib::MPC::File>(stream, readAudioProperties, audioPropertiesStyle);
        else if (ext == "WV")
            file = std::make_unique<TagLib::WavPack::File>(stream, readAudioProperties, audioPropertiesStyle);
        else if (ext == "APE")
            file = std::make_unique<TagLib::APE::File>(stream, readAudioProperties, audioPropertiesStyle);
        // TRUEAUDIO
        else if (ext == "TTA")
            file = std::make_unique<TagLib::TrueAudio::File>(stream, readAudioProperties, audioPropertiesStyle);
        // MP4
        else if (ext == "M4A" || ext == "M4R" || ext == "M4B" || ext == "M4P" || ext == "MP4" || ext == "3G2" || ext == "M4V")
            file = std::make_unique<TagLib::MP4::File>(stream, readAudioProperties, audioPropertiesStyle);
        // ASF
        else if (ext == "WMA" || ext == "ASF")
            file = std::make_unique<TagLib::ASF::File>(stream, readAudioProperties, audioPropertiesStyle);
        // RIFF
        else if (ext == "AIF" || ext == "AIFF" || ext == "AFC" || ext == "AIFC")
            file = std::make_unique<TagLib::RIFF::AIFF::File>(stream, readAudioProperties, audioPropertiesStyle);
        else if (ext == "WAV")
            file = std::make_unique<TagLib::RIFF::WAV::File>(stream, readAudioProperties, audioPropertiesStyle);
#if LMS_TAGLIB_HAS_DSF
        else if (ext == "DSF")
            file = std::make_unique<TagLib::DSF::File>(stream, readAudioProperties, audioPropertiesStyle);
#endif

        if (file && !file->isValid())
            file.reset();

        return file;
    }

    std::unique_ptr<TagLib::File> parseFileByContent(TagLib::FileStream* stream, TagLib::AudioProperties::ReadStyle audioPropertiesStyle)
    {
        constexpr bool readAudioProperties{ true };
        std::unique_ptr<TagLib::File> file;

        if (TagLib::MPEG::File::isSupported(stream))
            file = std::make_unique<TagLib::MPEG::File>(stream, TagLib::ID3v2::FrameFactory::instance(), readAudioProperties, audioPropertiesStyle);
        // VORBIS
        else if (TagLib::Ogg::Vorbis::File::isSupported(stream))
            file = std::make_unique<TagLib::Ogg::Vorbis::File>(stream, readAudioProperties, audioPropertiesStyle);
        else if (TagLib::Ogg::FLAC::File::isSupported(stream))
            file = std::make_unique<TagLib::Ogg::FLAC::File>(stream, readAudioProperties, audioPropertiesStyle);
        else if (TagLib::FLAC::File::isSupported(stream))
            file = std::make_unique<TagLib::FLAC::File>(stream, TagLib::ID3v2::FrameFactory::instance(), readAudioProperties, audioPropertiesStyle);
        else if (TagLib::Ogg::Speex::File::isSupported(stream))
            file = std::make_unique<TagLib::Ogg::Speex::File>(stream, readAudioProperties, audioPropertiesStyle);
        else if (TagLib::Ogg::Opus::File::isSupported(stream))
            file = std::make_unique<TagLib::Ogg::Opus::File>(stream, readAudioProperties, audioPropertiesStyle);
        // APE
        else if (TagLib::MPC::File::isSupported(stream))
            file = std::make_unique<TagLib::MPC::File>(stream, readAudioProperties, audioPropertiesStyle);
        else if (TagLib::WavPack::File::isSupported(stream))
            file = std::make_unique<TagLib::WavPack::File>(stream, readAudioProperties, audioPropertiesStyle);
        else if (TagLib::APE::File::isSupported(stream))
            file = std::make_unique<TagLib::APE::File>(stream, readAudioProperties, audioPropertiesStyle);
        // TRUEAUDIO
        else if (TagLib::TrueAudio::File::isSupported(stream))
            file = std::make_unique<TagLib::TrueAudio::File>(stream, readAudioProperties, audioPropertiesStyle);
        // MP4
        else if (TagLib::MP4::File::isSupported(stream))
            file = std::make_unique<TagLib::MP4::File>(stream, readAudioProperties, audioPropertiesStyle);
        //_ASF
        else if (TagLib::ASF::File::isSupported(stream))
            file = std::make_unique<TagLib::ASF::File>(stream, readAudioProperties, audioPropertiesStyle);
        // RIFF
        else if (TagLib::RIFF::AIFF::File::isSupported(stream))
            file = std::make_unique<TagLib::RIFF::AIFF::File>(stream, readAudioProperties, audioPropertiesStyle);
        else if (TagLib::RIFF::WAV::File::isSupported(stream))
            file = std::make_unique<TagLib::RIFF::WAV::File>(stream, readAudioProperties, audioPropertiesStyle);
#if LMS_TAGLIB_HAS_DSF
        else if (TagLib::DSF::File::isSupported(stream))
            file = std::make_unique<TagLib::DSF::File>(stream, readAudioProperties, audioPropertiesStyle);
#endif

        if (file && !file->isValid())
            file.reset();

        return file;
    }

    std::unique_ptr<TagLib::File> parseFile(const std::filesystem::path& p, ParserOptions::AudioPropertiesReadStyle readStyle)
    {
        LMS_SCOPED_TRACE_DETAILED("MetaData", "TagLibParseFile");

        const ::TagLib::AudioProperties::ReadStyle tagLibReadStyle{ readStyleToTagLibReadStyle(readStyle) };
        TagLib::FileStream fileStream{ createFileStream(p) };
        std::unique_ptr<TagLib::File> file{ parseFileByExtension(&fileStream, p.extension(), tagLibReadStyle) };
        if (!file)
        {
            LMS_LOG(METADATA, DEBUG, "File " << p << ": failed to parse by extension");
            file = parseFileByContent(&fileStream, tagLibReadStyle);
            if (!file)
                LMS_LOG(METADATA, DEBUG, "File " << p << ": failed to parse by content");
        }

        if (!file)
            throw AudioFileParsingException{ "Parsing failed" };

        if (!file->audioProperties())
            throw AudioFileParsingException{ "No audio properties" };

        return file;
    }
} // namespace lms::audio::taglib::utils