/*
 * Copyright (C) 2014 Emeric Poupon
 *
 * This file is part of LMS.
 *
 * LMS is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * LMS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with LMS.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include <optional>

#include <Wt/WResource.h>

#include "database/objects/ArtworkId.hpp"

namespace lms::ui
{
    // ArtworkResource: 封面图片资源处理器，提供专辑、艺术家和曲目的封面图片。
    // ArtworkResource: обработчик ресурсов изображений обложек, предоставляет обложки альбомов, исполнителей и треков.
    //
    // 功能：
    // - 处理封面图片的 HTTP 请求
    // - 支持不同尺寸的图片（小图 128px，大图 512px）
    // - 如果找不到封面，返回默认占位图片
    // - 支持艺术家、专辑和曲目的默认图片
    //
    // Функции:
    // - Обрабатывает HTTP-запросы изображений обложек
    // - Поддерживает изображения разных размеров (маленькое 128px, большое 512px)
    // - Возвращает изображение-заглушку, если обложка не найдена
    // - Поддерживает изображения по умолчанию для исполнителей, альбомов и треков
    class ArtworkResource : public Wt::WResource
    {
    public:
        static const std::size_t maxSize{ 512 };  // 最大图片尺寸（像素）/ максимальный размер изображения (пиксели)

        ArtworkResource();
        ~ArtworkResource() override;

        // Size: 封面图片尺寸枚举。
        // Size: перечисление размеров изображений обложек.
        enum class Size : std::size_t
        {
            Small = 128,  // 小图：128x128 像素 / маленькое: 128x128 пикселей
            Large = 512,  // 大图：512x512 像素 / большое: 512x512 пикселей
        };

        // DefaultArtworkType: 默认封面类型（当找不到实际封面时使用）。
        // DefaultArtworkType: тип обложки по умолчанию (используется, когда реальная обложка не найдена).
        enum class DefaultArtworkType
        {
            Artist,   // 艺术家默认图片 / изображение исполнителя по умолчанию
            Release,  // 专辑默认图片 / изображение альбома по умолчанию
            Track,    // 曲目默认图片 / изображение трека по умолчанию
        };

        // getArtworkUrl: 获取封面图片的 URL。如果找不到封面，使用 fallbackType 指定的默认图片。
        // getArtworkUrl: получает URL изображения обложки. Если обложка не найдена, использует изображение по умолчанию, указанное в fallbackType.
        std::string getArtworkUrl(db::ArtworkId artworkId, DefaultArtworkType fallbackType, std::optional<Size> size = std::nullopt) const;
        
        // getDefaultArtworkUrl: 获取默认封面图片的 URL（当没有实际封面时使用）。
        // getDefaultArtworkUrl: получает URL изображения обложки по умолчанию (используется, когда нет реальной обложки).
        std::string getDefaultArtworkUrl(DefaultArtworkType type) const;

    private:
        // handleRequest: 处理封面图片的 HTTP 请求，返回图片数据。
        // handleRequest: обрабатывает HTTP-запрос изображения обложки, возвращает данные изображения.
        void handleRequest(const Wt::Http::Request& request, Wt::Http::Response& response) override;
    };
} // namespace lms::ui
