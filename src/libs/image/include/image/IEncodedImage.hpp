#pragma once

#include <cstddef>
#include <span>
#include <string_view>

namespace lms::image
{
    using ImageSize = std::size_t;

    /**
     * @brief 编码图像接口
     */
    class IEncodedImage
    {
    public:
        virtual ~IEncodedImage() = default;

        /**
         * @brief 获取图像数据
         */
        virtual std::span<const std::byte> getData() const = 0;

        /**
         * @brief 获取 MIME 类型
         */
        virtual std::string_view getMimeType() const = 0;
    };
} // namespace lms::image

