#include "image/EncodedImage.hpp"

#include <cstring>
#include <filesystem>
#include <fstream>

#include "core/ILogger.hpp"
#include "core/Service.hpp"

namespace lms::image
{
    EncodedImage::EncodedImage(const std::filesystem::path& path, std::string_view mimeType)
        : EncodedImage([&]() -> std::vector<std::byte> {
            if (!std::filesystem::exists(path))
            {
                if (auto* logger = lms::core::Service<lms::core::logging::ILogger>::get())
                {
                    LMS_LOG(COVER, ERROR, "Image file does not exist: " << path);
                }
                return {};
            }

            std::ifstream file(path, std::ios::binary | std::ios::ate);
            if (!file)
            {
                if (auto* logger = lms::core::Service<lms::core::logging::ILogger>::get())
                {
                    LMS_LOG(COVER, ERROR, "Cannot open image file: " << path);
                }
                return {};
            }

            const auto fileSize = file.tellg();
            file.seekg(0, std::ios::beg);

            std::vector<char> buffer(static_cast<std::size_t>(fileSize));
            file.read(buffer.data(), fileSize);
            
            std::vector<std::byte> data(buffer.size());
            std::memcpy(data.data(), buffer.data(), buffer.size());
            return data;
        }(), mimeType)
    {
    }

    EncodedImage::EncodedImage(std::vector<std::byte>&& data, std::string_view mimeType)
        : _data{ std::move(data) }
        , _mimeType{ mimeType }
    {
    }

    EncodedImage::EncodedImage(std::span<const std::byte> data, std::string_view mimeType)
        : _data{ data.begin(), data.end() }
        , _mimeType{ mimeType }
    {
    }
} // namespace lms::image

