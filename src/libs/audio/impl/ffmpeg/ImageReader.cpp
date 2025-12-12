
/*
 * ImageReader类的实现文件，用于处理音频文件中的图像数据
 * 该类继承自AudioFile，提供访问音频文件中附加图片的功能
 */
#include "ImageReader.hpp" 

#include <algorithm>

#include "core/String.hpp"

#include "AudioFile.hpp"

namespace lms::audio::ffmpeg
{
    ImageReader::ImageReader(const AudioFile& audioFile)
        : _audioFile{ audioFile }
    {
    }

    ImageReader::~ImageReader() = default;

    void ImageReader::visitImages(const ImageVisitor& visitor) const
    {
        auto metaDataHasKeyword{ [](const AudioFile::MetadataMap& metadata, std::string_view keyword) {
            return std::any_of(std::cbegin(metadata), std::cend(metadata), [&](const auto& keyValue) { return core::stringUtils::stringCaseInsensitiveContains(keyValue.second, keyword); });
        } };

        _audioFile.visitAttachedPictures([&](const Picture& picture, const AudioFile::MetadataMap& metaData) {
            Image image;
            image.data = picture.data;
            image.mimeType = picture.mimeType;
            if (metaDataHasKeyword(metaData, "front"))
                image.type = Image::Type::FrontCover;
            else if (metaDataHasKeyword(metaData, "back"))
                image.type = Image::Type::BackCover;

            visitor(image);
        });
    }

} // namespace lms::audio::ffmpeg
