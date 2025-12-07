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

#include <chrono>
#include <string>
#include <string_view>

#include <Wt/WContainerWidget.h>
#include <Wt/WImage.h>
#include <Wt/WInteractWidget.h>
#include <Wt/WString.h>

#include "database/Object.hpp"
#include "database/objects/ArtistId.hpp"
#include "database/objects/ClusterId.hpp"
#include "database/objects/ReleaseId.hpp"
#include "database/objects/TrackId.hpp"

#include "database/Types.hpp"
#include "resource/ArtworkResource.hpp"

namespace lms::db
{
    class Artist;
    class Cluster;
    class Release;
    class Track;
    class TrackList;
} // namespace lms::db

namespace lms::ui
{
    class Filters;
}

// utils: UI 工具函数命名空间，提供常用的 UI 组件创建和格式化函数。
// utils: пространство имён утилитных функций UI, предоставляет функции создания и форматирования часто используемых компонентов UI.
namespace lms::ui::utils
{
    // durationToString: 将毫秒时长转换为可读的字符串格式（如 "3:45"）。
    // durationToString: преобразует длительность в миллисекундах в читаемый строковый формат (например, "3:45").
    std::string durationToString(std::chrono::milliseconds msDuration);

    // showArtworkModal: 在模态框中显示封面图片（点击放大查看）。
    // showArtworkModal: отображает изображение обложки в модальном окне (увеличение при клике).
    void showArtworkModal(Wt::WLink image);

    // createArtworkImage: 创建封面图片 widget，如果找不到封面则使用默认图片。
    // createArtworkImage: создаёт виджет изображения обложки, использует изображение по умолчанию, если обложка не найдена.
    std::unique_ptr<Wt::WImage> createArtworkImage(db::ArtworkId artworkId, ArtworkResource::DefaultArtworkType type, ArtworkResource::Size size);
    
    // createDefaultArtworkImage: 创建默认封面图片 widget（当没有实际封面时使用）。
    // createDefaultArtworkImage: создаёт виджет изображения обложки по умолчанию (используется, когда нет реальной обложки).
    std::unique_ptr<Wt::WImage> createDefaultArtworkImage(ArtworkResource::DefaultArtworkType type);

    // createFilter: 创建过滤器标签 widget（用于显示和删除活动过滤器）。
    // createFilter: создаёт виджет метки фильтра (для отображения и удаления активных фильтров).
    std::unique_ptr<Wt::WInteractWidget> createFilter(const Wt::WString& name, const Wt::WString& tooltip, std::string_view colorStyleClass, bool canDelete = false);
    
    // createFilterCluster: 创建聚类过滤器标签 widget。
    // createFilterCluster: создаёт виджет метки фильтра кластера.
    std::unique_ptr<Wt::WInteractWidget> createFilterCluster(db::ClusterId clusterId, bool canDelete = false);
    
    // createFilterClustersForTrack: 为曲目创建聚类过滤器标签列表。
    // createFilterClustersForTrack: создаёт список меток фильтров кластеров для трека.
    std::unique_ptr<Wt::WContainerWidget> createFilterClustersForTrack(db::ObjectPtr<db::Track> track, Filters& filters);

    // createArtistAnchorList: 创建艺术家链接列表 widget。
    // createArtistAnchorList: создаёт виджет списка ссылок на исполнителей.
    std::unique_ptr<Wt::WContainerWidget> createArtistAnchorList(const std::vector<db::ArtistId>& artistIds, std::string_view cssAnchorClass = "link-success");
    
    // createArtistDisplayNameWithAnchors: 创建带链接的艺术家显示名称 widget。
    // createArtistDisplayNameWithAnchors: создаёт виджет отображаемого имени исполнителя со ссылками.
    std::unique_ptr<Wt::WContainerWidget> createArtistDisplayNameWithAnchors(std::string_view displayName, const std::vector<db::ArtistId>& artistIds, std::string_view cssAnchorClass = "link-success");
    
    // createArtistsAnchorsForRelease: 为专辑创建艺术家链接列表 widget。
    // createArtistsAnchorsForRelease: создаёт виджет списка ссылок на исполнителей для альбома.
    std::unique_ptr<Wt::WContainerWidget> createArtistsAnchorsForRelease(db::ObjectPtr<db::Release> release, db::ArtistId omitIfMatchThisArtist = {}, std::string_view cssAnchorClass = "link-success");

    // createArtistLink: 创建指向艺术家页面的链接。
    // createArtistLink: создаёт ссылку на страницу исполнителя.
    Wt::WLink createArtistLink(db::ObjectPtr<db::Artist> artist);
    
    // createArtistAnchor: 创建指向艺术家页面的锚点 widget。
    // createArtistAnchor: создаёт виджет якоря на страницу исполнителя.
    std::unique_ptr<Wt::WAnchor> createArtistAnchor(db::ObjectPtr<db::Artist> artist, bool setText = true);
    
    // createReleaseLink: 创建指向专辑页面的链接。
    // createReleaseLink: создаёт ссылку на страницу альбома.
    Wt::WLink createReleaseLink(db::ObjectPtr<db::Release> release);
    
    // createReleaseAnchor: 创建指向专辑页面的锚点 widget。
    // createReleaseAnchor: создаёт виджет якоря на страницу альбома.
    std::unique_ptr<Wt::WAnchor> createReleaseAnchor(db::ObjectPtr<db::Release> release, bool setText = true);
    
    // createTrackListAnchor: 创建指向播放列表页面的锚点 widget。
    // createTrackListAnchor: создаёт виджет якоря на страницу плейлиста.
    std::unique_ptr<Wt::WAnchor> createTrackListAnchor(db::ObjectPtr<db::TrackList> trackList, bool setText = true);
} // namespace lms::ui::utils
