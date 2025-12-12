
/*
 * 将图像类型转换为对应的字符串表示
 * 该函数属于 lms::audio 命名空间
 */
#include "audio/IImageReader.hpp"

namespace lms::audio
{
    /**
     * @brief 将图像类型枚举值转换为对应的字符串
     * @param type 图像类型枚举值
     * @return 返回对应图像类型的字符串表示，如果类型未知则返回"Unknown"
     */
    core::LiteralString imageTypeToString(Image::Type type)
    {
        switch (type)  // 使用switch语句处理不同的图像类型
        {
        case Image::Type::Other:  // 其他类型
            return "Other";
        case Image::Type::FileIcon:  // 文件图标
            return "FileIcon";
        case Image::Type::OtherFileIcon:  // 其他文件图标
            return "OtherFileIcon";
        case Image::Type::FrontCover:  // 前封面
            return "FrontCover";
        case Image::Type::BackCover:  // 后封面
            return "BackCover";
        case Image::Type::LeafletPage:  // 小册子页面
            return "LeafletPage";
        case Image::Type::Media:  // 媒体
            return "Media";
        case Image::Type::LeadArtist:  // 主艺术家
            return "LeadArtist";
        case Image::Type::Artist:  // 艺术家
            return "Artist";
        case Image::Type::Conductor:  // 指挥家
            return "Conductor";
        case Image::Type::Band:  // 乐队
            return "Band";
        case Image::Type::Composer:  // 作曲家
            return "Composer";
        case Image::Type::Lyricist:  // 作词家
            return "Lyricist";
        case Image::Type::RecordingLocation:  // 录音地点
            return "RecordingLocation";
        case Image::Type::DuringRecording:  // 录音过程中
            return "DuringRecording";
        case Image::Type::DuringPerformance:  // 表演过程中
            return "DuringPerformance";
        case Image::Type::MovieScreenCapture:  // 电影屏幕截图
            return "MovieScreenCapture";
        case Image::Type::ColouredFish:  // 彩色鱼
            return "ColouredFish";
        case Image::Type::Illustration:  // 插图
            return "Illustration";
        case Image::Type::BandLogo:  // 乐队标志
            return "BandLogo";
        case Image::Type::PublisherLogo:  // 发布者标志
            return "PublisherLogo";
        case Image::Type::Unknown:  // 未知类型
            break;
        }

        // 如果没有匹配的类型，返回"Unknown"
        return "Unknown";
    }

} // namespace lms::audio