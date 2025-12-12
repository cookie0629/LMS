// 基于 FFmpeg 提取并规范化音频标签的接口声明

/*
 * @brief TagReader 类，用于从音频文件中读取标签信息
 * @details 该类继承自 ITagReader 接口，实现了从音频文件中读取元数据的功能
 */
#pragma once 

#include "audio/ITagReader.hpp"

#include "AudioFile.hpp"

namespace lms::audio::ffmpeg
{
    class TagReader : public ITagReader
    {
    public:
        TagReader(const AudioFile& audioFile, bool enableExtraDebugLogs);
        ~TagReader() override;
        TagReader(const TagReader&) = delete;
        TagReader& operator=(const TagReader&) = delete;

    private:
        void visitTagValues(TagType tag, TagValueVisitor visitor) const override;
        void visitTagValues(std::string_view tag, TagValueVisitor visitor) const override;
        void visitPerformerTags(PerformerVisitor visitor) const override;
        void visitLyricsTags(LyricsVisitor visitor) const override;

        const AudioFile& _audioFile;
        AudioFile::MetadataMap _metaDataMap;
    };
} // namespace lms::audio::ffmpeg
