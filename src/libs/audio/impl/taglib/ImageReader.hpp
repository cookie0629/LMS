
#pragma once

#include "audio/IImageReader.hpp"

namespace TagLib
{
    class File;
}

namespace lms::audio::taglib
{
    class ImageReader : public IImageReader
    {
    public:
        ImageReader(TagLib::File& _file);
        ~ImageReader() override;
        ImageReader(const ImageReader&) = delete;
        ImageReader& operator=(const ImageReader&) = delete;

    private:
        void visitImages(const ImageVisitor& visitor) const override;

        TagLib::File& _file;
    };
} // namespace lms::audio::taglib
