/*
 * Copyright (C) 2020 Emeric Poupon
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

#include <Wt/WResource.h>
#include <memory>

#include "core/IZipper.hpp"
#include "database/objects/ArtistId.hpp"
#include "database/objects/ReleaseId.hpp"
#include "database/objects/TrackId.hpp"
#include "database/objects/TrackListId.hpp"

namespace lms::ui
{
    // DownloadResource: 下载资源基类，提供将音乐文件打包为 ZIP 文件下载的功能。
    // DownloadResource: базовый класс ресурсов загрузки, предоставляет функциональность упаковки музыкальных файлов в ZIP для загрузки.
    //
    // 功能：
    // - 将多个音频文件打包成 ZIP 文件
    // - 支持流式传输 ZIP 文件（不占用大量内存）
    // - 使用模板方法模式，子类实现具体的打包逻辑
    //
    // Функции:
    // - Упаковывает несколько аудиофайлов в ZIP-архив
    // - Поддерживает потоковую передачу ZIP-файла (не занимает много памяти)
    // - Использует паттерн Template Method, подклассы реализуют конкретную логику упаковки
    class DownloadResource : public Wt::WResource
    {
    public:
        static constexpr std::size_t bufferSize{ 32768 };  // 缓冲区大小（32 KB）/ размер буфера (32 КБ)

        ~DownloadResource() override;

    private:
        // handleRequest: 处理下载请求，创建 ZIP 文件并流式传输。
        // handleRequest: обрабатывает запрос загрузки, создаёт ZIP-файл и передаёт его потоком.
        void handleRequest(const Wt::Http::Request& request, Wt::Http::Response& response) override;
        
        // createZipper: 创建 ZIP 打包器（模板方法，由子类实现）。
        // createZipper: создаёт упаковщик ZIP (шаблонный метод, реализуется подклассами).
        virtual std::unique_ptr<zip::IZipper> createZipper() = 0;
    };

    // DownloadArtistResource: 下载艺术家的所有曲目。
    // DownloadArtistResource: загружает все треки исполнителя.
    class DownloadArtistResource : public DownloadResource
    {
    public:
        DownloadArtistResource(db::ArtistId artistId);

    private:
        std::unique_ptr<zip::IZipper> createZipper() override;
        db::ArtistId _artistId;
    };

    // DownloadReleaseResource: 下载专辑的所有曲目。
    // DownloadReleaseResource: загружает все треки альбома.
    class DownloadReleaseResource : public DownloadResource
    {
    public:
        DownloadReleaseResource(db::ReleaseId releaseId);

    private:
        std::unique_ptr<zip::IZipper> createZipper() override;
        db::ReleaseId _releaseId;
    };

    // DownloadTrackResource: 下载单个曲目。
    // DownloadTrackResource: загружает один трек.
    class DownloadTrackResource : public DownloadResource
    {
    public:
        DownloadTrackResource(db::TrackId trackId);

    private:
        std::unique_ptr<zip::IZipper> createZipper() override;
        db::TrackId _trackId;
    };

    // DownloadTrackListResource: 下载播放列表的所有曲目。
    // DownloadTrackListResource: загружает все треки плейлиста.
    class DownloadTrackListResource : public DownloadResource
    {
    public:
        DownloadTrackListResource(db::TrackListId trackListId);

    private:
        std::unique_ptr<zip::IZipper> createZipper() override;
        db::TrackListId _trackListId;
    };
} // namespace lms::ui
