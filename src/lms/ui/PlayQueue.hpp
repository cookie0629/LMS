
#pragma once

#include <optional>

#include <Wt/WCheckBox.h>
#include <Wt/WContainerWidget.h>
#include <Wt/WTemplate.h>
#include <Wt/WText.h>

#include "database/Object.hpp"
#include "database/objects/TrackId.hpp"
#include "database/objects/TrackListId.hpp"

#include "common/Template.hpp"

namespace lms::db
{
    class Track;
    class TrackList;
    class TrackListEntry;
} // namespace lms::db

namespace lms::ui
{

    class InfiniteScrollingContainer;

    // PlayQueue: 播放队列视图，负责管理当前排队的曲目、与播放器交互以及"电台"模式。
    // PlayQueue: представление очереди воспроизведения, управляет треками, взаимодействует с плеером и режимом радио.
    //
    // 功能：
    // - 管理播放队列（添加、删除、重排曲目）
    // - 支持多种播放模式（立即播放、下一首播放、随机播放、追加到末尾等）
    // - 支持"电台"模式（自动添加推荐曲目）
    // - 支持"重复全部"模式（循环播放）
    // - 与 MediaPlayer 交互，通知播放器加载曲目
    //
    // Функции:
    // - Управляет очередью воспроизведения (добавление, удаление, перестановка треков)
    // - Поддерживает различные режимы воспроизведения (немедленное воспроизведение, воспроизведение следующим, случайное воспроизведение, добавление в конец и т.д.)
    // - Поддерживает режим "радио" (автоматическое добавление рекомендуемых треков)
    // - Поддерживает режим "повторять всё" (циклическое воспроизведение)
    // - Взаимодействует с MediaPlayer, уведомляет плеер о загрузке треков
    class PlayQueue : public Template
    {
    public:
        PlayQueue();

        // play: 立即播放指定曲目列表，清空当前队列。
        // play: немедленно воспроизводит указанный список треков, очищает текущую очередь.
        void play(const std::vector<db::TrackId>& trackIds);
        
        // playNext: 将曲目列表插入到当前播放位置之后。
        // playNext: вставляет список треков после текущей позиции воспроизведения.
        void playNext(const std::vector<db::TrackId>& trackIds);
        
        // playShuffled: 随机播放指定曲目列表，清空当前队列。
        // playShuffled: случайно воспроизводит указанный список треков, очищает текущую очередь.
        void playShuffled(const std::vector<db::TrackId>& trackIds);
        
        // playOrAddLast: 队列空则播放，否则追加到末尾。
        // playOrAddLast: если очередь пуста – играть, иначе добавить в конец.
        void playOrAddLast(const std::vector<db::TrackId>& trackIds);
        
        // playAtIndex: 在指定索引位置插入曲目列表并开始播放。
        // playAtIndex: вставляет список треков по указанному индексу и начинает воспроизведение.
        void playAtIndex(const std::vector<db::TrackId>& trackIds, std::size_t index);

        void playNext();

        void playPrevious();

        Wt::Signal<db::TrackId, bool /*play*/, float /* replayGain */> trackSelected;

        Wt::Signal<> trackUnselected;

        Wt::Signal<std::size_t> trackCountChanged;

        void onPlaybackEnded();

        std::size_t getCapacity() const { return _capacity; }
        std::size_t getCount();

    private:
        void initTrackLists();

        void notifyAddedTracks(std::size_t nbAddedTracks) const;
        db::ObjectPtr<db::TrackList> getQueue() const;
        bool isFull() const;

        void clearTracks();
        void enqueueTracks(const std::vector<db::TrackId>& trackIds);
        std::vector<db::TrackId> getAndClearNextTracks();
        void addSome();
        void addEntry(const db::ObjectPtr<db::TrackListEntry>& entry);
        void enqueueRadioTracksIfNeeded();
        void enqueueRadioTracks();
        void updateInfo();
        void updateCurrentTrack(bool selected);
        bool isRepeatAllSet() const;
        bool isRadioModeSet() const;

        void loadTrack(std::size_t pos, bool play);
        void stop();

        std::optional<float> getReplayGain(std::size_t pos, const db::ObjectPtr<db::Track>& track) const;

        const std::size_t _capacity;
        static inline constexpr std::size_t _batchSize{ 12 };

        bool _mediaPlayerSettingsLoaded{};
        db::TrackListId _queueId{};
        InfiniteScrollingContainer* _entriesContainer{};
        Wt::WText* _nbTracks{};
        Wt::WText* _duration{};
        Wt::WCheckBox* _repeatBtn{};
        Wt::WCheckBox* _radioBtn{};
        std::optional<std::size_t> _trackPos; // current track position, if set
        bool _isTrackSelected{};
    };

} // namespace lms::ui
