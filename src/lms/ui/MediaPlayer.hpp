
#pragma once

#include <optional>

#include <Wt/WAnchor.h>
#include <Wt/WJavaScript.h>
#include <Wt/WPushButton.h>
#include <Wt/WTemplate.h>
#include <Wt/WText.h>

#include "database/Types.hpp"
#include "database/objects/TrackId.hpp"

namespace lms::ui
{
    class AudioFileResource;
    class AudioTranscodingResource;

    // MediaPlayer: 前端音频播放器控件，负责与 JS 播放器交互、加载曲目和维护播放设置。
    // MediaPlayer: виджет аудиоплеера, взаимодействует с JS‑плеером, загружает треки и хранит настройки воспроизведения.
    // 
    // 功能：
    // - 管理音频资源的加载和播放（通过 AudioFileResource 和 AudioTranscodingResource）
    // - 处理转码设置（格式、比特率、何时转码）
    // - 处理 ReplayGain 设置（自动音量调整）
    // - 与前端 JavaScript 播放器通信（播放、暂停、上一首、下一首等）
    // - 处理 Scrobbling（记录播放历史）
    //
    // Функции:
    // - Управляет загрузкой и воспроизведением аудиоресурсов (через AudioFileResource и AudioTranscodingResource)
    // - Обрабатывает настройки транскодирования (формат, битрейт, когда транскодировать)
    // - Обрабатывает настройки ReplayGain (автоматическая регулировка громкости)
    // - Общается с JavaScript-плеером на фронтенде (воспроизведение, пауза, предыдущий, следующий и т.д.)
    // - Обрабатывает Scrobbling (запись истории воспроизведения)
    class MediaPlayer : public Wt::WTemplate
    {
    public:
        using Bitrate = db::Bitrate;
        using Format = db::TranscodingOutputFormat;
        using Gain = float;

        // Settings: 播放器设置结构，包含转码和 ReplayGain 配置。
        // Settings: структура настроек плеера, содержит конфигурацию транскодирования и ReplayGain.
        struct Settings
        {
            // Transcoding: 转码设置，控制何时以及如何转码音频。
            // Transcoding: настройки транскодирования, контролируют когда и как транскодировать аудио.
            struct Transcoding
            {
                enum class Mode
                {
                    Never = 0,                      // 从不转码 / никогда не транскодировать
                    Always = 1,                     // 总是转码 / всегда транскодировать
                    IfFormatNotSupported = 2,       // 仅在格式不支持时转码 / транскодировать только если формат не поддерживается
                };
                static inline constexpr Mode defaultMode{ Mode::IfFormatNotSupported };
                static inline constexpr Format defaultFormat{ Format::OGG_OPUS };
                static inline constexpr Bitrate defaultBitrate{ 128000 };

                Mode mode{ defaultMode };
                Format format{ defaultFormat };
                Bitrate bitrate{ defaultBitrate };
            };

            // ReplayGain: ReplayGain 设置，用于自动音量标准化。
            // ReplayGain: настройки ReplayGain для автоматической нормализации громкости.
            struct ReplayGain
            {
                enum class Mode
                {
                    None = 0,       // 不使用 ReplayGain / не использовать ReplayGain
                    Auto = 1,       // 自动选择（曲目或专辑） / автоматический выбор (трек или альбом)
                    Track = 2,      // 使用曲目级别 ReplayGain / использовать ReplayGain уровня трека
                    Release = 3,    // 使用专辑级别 ReplayGain / использовать ReplayGain уровня альбома
                };

                static inline constexpr Mode defaultMode{ Mode::None };
                static inline constexpr Gain defaultPreAmpGain{};
                static inline constexpr Gain minPreAmpGain{ -15 };
                static inline constexpr Gain maxPreAmpGain{ 15 };

                Mode mode{ defaultMode };
                Gain preAmpGain{ defaultPreAmpGain };              // 前置放大增益 / предусиление
                Gain preAmpGainIfNoInfo{ defaultPreAmpGain };     // 无 ReplayGain 信息时的前置放大增益 / предусиление при отсутствии информации ReplayGain
            };

            Transcoding transcoding;
            ReplayGain replayGain;
        };

        MediaPlayer();
        ~MediaPlayer() override;
        MediaPlayer(const MediaPlayer&) = delete;
        MediaPlayer& operator=(const MediaPlayer&) = delete;

        std::optional<db::TrackId> getTrackLoaded() const { return _trackIdLoaded; }

        void loadTrack(db::TrackId trackId, bool play, float replayGain);
        void stop();

        std::optional<Settings> getSettings() const { return _settings; }
        void setSettings(const Settings& settings);

        void onPlayQueueUpdated(std::size_t trackCount);

        // Signals
        Wt::JSignal<> playPrevious;
        Wt::JSignal<> playNext;
        Wt::Signal<db::TrackId> trackLoaded;
        Wt::Signal<> settingsLoaded;

        Wt::JSignal<db::TrackId::ValueType> scrobbleListenNow;
        Wt::JSignal<db::TrackId::ValueType, unsigned /* ms */> scrobbleListenFinished;

        Wt::JSignal<> playbackEnded;

    private:
        std::unique_ptr<AudioFileResource> _audioFileResource;
        std::unique_ptr<AudioTranscodingResource> _audioTranscodingResource;

        std::optional<db::TrackId> _trackIdLoaded;
        std::optional<Settings> _settings;

        Wt::JSignal<std::string> _settingsLoaded;

        Wt::WText* _title{};
        Wt::WAnchor* _release{};
        Wt::WText* _separator{};
        Wt::WAnchor* _artist{};
        Wt::WPushButton* _playQueue{};
    };

} // namespace lms::ui
