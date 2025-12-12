
/*
 * ImageReader类声明，继承自IImageReader接口
 * 用于从音频文件中读取图像数据
 */
#pragma once

#include "audio/IImageReader.hpp"

namespace lms::audio::ffmpeg
{
    class AudioFile;

    class ImageReader : public IImageReader
    {
    public:
        ImageReader(const AudioFile& audioFile);
        ~ImageReader() override;

        ImageReader(const ImageReader&) = delete;
        ImageReader& operator=(const ImageReader&) = delete;

    private:
        void visitImages(const ImageVisitor& visitor) const override;

        const AudioFile& _audioFile;
    };
} // namespace lms::audio::ffmpeg
