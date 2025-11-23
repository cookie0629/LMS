#pragma once

#include <cstddef>
#include <span>
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
    };

    /**
     * @brief 图像读取器简化实现（空实现）
     */
    class ImageReader : public IImageReader
    {
    public:
        std::span<const std::byte> getImageData() const override
        {
            static const std::vector<std::byte> empty;
            return empty;
        }

        bool hasImage() const override
        {
            return false;
        }
    };
} // namespace lms::audio

