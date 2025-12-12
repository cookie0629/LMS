// 基于 TagLib 解析并归一化音频标签的接口声明

/*
 * 标记读取器类，继承自ITagReader接口，用于从音频文件中读取标签信息
 * 使用TagLib库实现具体的标签读取功能
 */
#pragma once

#include <map>
#include <string>

#include <taglib/tpropertymap.h>

#include "audio/ITagReader.hpp"

namespace TagLib
{
    class File;
}

namespace lms::audio::taglib
{
    class TagReader : public ITagReader
    {
    public:
        TagReader(::TagLib::File& file, bool enableExtraDebugLogs);
        ~TagReader() override;

        TagReader(const TagReader&) = delete;
        TagReader& operator=(const TagReader&) = delete;

    private:
        void visitTagValues(TagType tag, TagValueVisitor visitor) const override;
        void visitTagValues(std::string_view tag, TagValueVisitor visitor) const override;
        void visitPerformerTags(PerformerVisitor visitor) const override;
        void visitLyricsTags(LyricsVisitor visitor) const override;

        ::TagLib::File& _file;
        ::TagLib::PropertyMap _propertyMap; // case-insensitive keys
        std::multimap<std::string /* language*/, std::string /* lyrics */> _id3v2Lyrics;
    };
} // namespace lms::audio::taglib
