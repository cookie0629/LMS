#pragma once

#include <filesystem>
#include <vector>

#include "image/IEncodedImage.hpp"

namespace lms::image
{
    /**
     * @brief 编码图像实现
     */
    class EncodedImage : public IEncodedImage
    {
    public:
        /**
         * @brief 从文件路径构造
         */
        EncodedImage(const std::filesystem::path& path, std::string_view mimeType = "");

        /**
         * @brief 从数据构造
         */
        EncodedImage(std::vector<std::byte>&& data, std::string_view mimeType);

        /**
         * @brief 从数据 span 构造
         */
        EncodedImage(std::span<const std::byte> data, std::string_view mimeType);

        ~EncodedImage() override = default;
        EncodedImage(const EncodedImage&) = delete;
        EncodedImage& operator=(const EncodedImage&) = delete;

        std::span<const std::byte> getData() const override { return _data; }
        std::string_view getMimeType() const override { return _mimeType; }

    private:
        const std::vector<std::byte> _data;
        const std::string _mimeType;
    };
} // namespace lms::image

