#pragma once

#include <cstddef>
#include <span>
#include <string>
#include <string_view>
#include <vector>

namespace lms::audio
{
    /**
     * @brief 图像读取器接口（简化版）
     */
    class IImageReader
    {
    public:
        virtual ~IImageReader() = default;

        /**
         * @brief 获取嵌入图像数据
         * @return 图像数据（如果存在）
         */
        virtual std::span<const std::byte> getImageData() const = 0;

        /**
         * @brief 检查是否有嵌入图像
         */
        virtual bool hasImage() const = 0;

        /**
         * @brief 返回图像的 MIME 类型
         */
        virtual std::string_view getMimeType() const = 0;
    };

    /**
     * @brief 图像读取器简化实现（空实现）
     */
    class ImageReader : public IImageReader
    {
    public:
        void setImage(std::vector<std::byte> data, std::string mimeType = "application/octet-stream")
        {
            _data = std::move(data);
            _mimeType = mimeType.empty() ? "application/octet-stream" : std::move(mimeType);
        }

        void clear()
        {
            _data.clear();
            _mimeType = "application/octet-stream";
        }

        std::span<const std::byte> getImageData() const override
        {
            return _data;
        }

        bool hasImage() const override
        {
            return !_data.empty();
        }

        std::string_view getMimeType() const override
        {
            return _mimeType;
        }

    private:
        std::vector<std::byte> _data;
        std::string _mimeType{ "application/octet-stream" };
    };
} // namespace lms::audio

