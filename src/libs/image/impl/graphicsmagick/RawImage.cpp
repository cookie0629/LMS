
#include "RawImage.hpp"

#include <magick/resource.h>

#include "core/ILogger.hpp"
#include "core/ITraceLogger.hpp"
#include "image/Exception.hpp"

namespace lms::image::GraphicsMagick
{
    RawImage::RawImage(std::span<const std::byte> encodedData)
    {
        try
        {
            Magick::Blob blob{ encodedData.data(), encodedData.size() };
            _image.read(blob);
        }
        catch (Magick::WarningCoder& e)
        {
            LMS_LOG(COVER, WARNING, "Caught Magick WarningCoder: " << e.what());
        }
        catch (Magick::Warning& e)
        {
            LMS_LOG(COVER, WARNING, "Caught Magick warning: " << e.what());
            throw Exception{ std::string{ "Read warning: " } + e.what() };
        }
        catch (Magick::Exception& e)
        {
            LMS_LOG(COVER, ERROR, "Caught Magick exception: " << e.what());
            throw Exception{ std::string{ "Read error: " } + e.what() };
        }
    }

    RawImage::RawImage(const std::filesystem::path& p)
    {
        try
        {
            _image.read(p.c_str());
        }
        catch (Magick::WarningCoder& e)
        {
            LMS_LOG(COVER, WARNING, "Caught Magick WarningCoder: " << e.what());
        }
        catch (Magick::Warning& e)
        {
            LMS_LOG(COVER, WARNING, "Caught Magick warning: " << e.what());
            throw Exception{ std::string{ "Read warning: " } + e.what() };
        }
        catch (Magick::Exception& e)
        {
            LMS_LOG(COVER, ERROR, "Caught Magick exception: " << e.what());
            throw Exception{ std::string{ "Read error: " } + e.what() };
        }
    }

    ImageSize RawImage::getWidth() const
    {
        return _image.size().width();
    }

    ImageSize RawImage::getHeight() const
    {
        return _image.size().height();
    }

    void RawImage::resize(ImageSize width)
    {
        try
        {
            LMS_SCOPED_TRACE_DETAILED("Image", "Resize");

            _image.resize(Magick::Geometry{ static_cast<unsigned int>(width), static_cast<unsigned int>(width) });
        }
        catch (Magick::Exception& e)
        {
            LMS_LOG(COVER, ERROR, "Caught Magick exception while resizing: " << e.what());
            throw Exception{ std::string{ "Resize error: " } + e.what() };
        }
    }

    Magick::Image RawImage::getMagickImage() const
    {
        return _image;
    }
} // namespace lms::image::GraphicsMagick
