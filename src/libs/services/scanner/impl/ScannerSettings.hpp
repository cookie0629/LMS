#pragma once

#include "scanners/audiofile/TrackMetadataParser.hpp"

namespace lms::scanner
{
    /**
     * @brief 扫描运行时设置
     */
    struct ScannerSettings
    {
        bool enableOptimize{ true };                // 是否执行优化步骤
        bool enableCompact{};                       // 是否执行压缩步骤
        bool preferAlbumArtistFallback{ true };     // 无 artist 时是否回退到 album artist
        TrackMetadataParser::Parameters metadataParserParameters; // 元数据解析参数
    };
} // namespace lms::scanner


