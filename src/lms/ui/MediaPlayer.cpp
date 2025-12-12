
#include "MediaPlayer.hpp"

#include <algorithm>

#include <Wt/Json/Object.h>
#include <Wt/Json/Parser.h>
#include <Wt/Json/Serializer.h>
#include <Wt/Json/Value.h>
#include <Wt/WPushButton.h>

#include "core/ILogger.hpp"
#include "core/String.hpp"
#include "database/Session.hpp"
#include "database/Types.hpp"
#include "database/objects/Artist.hpp"
#include "database/objects/Release.hpp"
#include "database/objects/Track.hpp"
#include "database/objects/TrackList.hpp"
#include "database/objects/User.hpp"

#include "LmsApplication.hpp"
#include "Utils.hpp"
#include "resource/ArtworkResource.hpp"
#include "resource/AudioFileResource.hpp"
#include "resource/AudioTranscodingResource.hpp"

namespace lms::ui
{
    namespace
    {
        // settingsToJSString: 将播放器设置序列化为 JSON 字符串，传给前端 JS。
        // settingsToJSString: сериализует настройки плеера в JSON для передачи в JS.
        // 将转码模式和 ReplayGain 设置转换为 JSON 格式，供前端 JavaScript 播放器使用。
        // Преобразует режим транскодирования и настройки ReplayGain в формат JSON для использования JavaScript-плеером на фронтенде.
        std::string settingsToJSString(const MediaPlayer::Settings& settings)
        {
            namespace Json = Wt::Json;

            Json::Object res;

            {
                Json::Object transcoding;
                transcoding["mode"] = static_cast<int>(settings.transcoding.mode);
                transcoding["format"] = static_cast<int>(settings.transcoding.format);
                transcoding["bitrate"] = static_cast<int>(settings.transcoding.bitrate);
                res["transcoding"] = std::move(transcoding);
            }

            {
                Json::Object replayGain;
                replayGain["mode"] = static_cast<int>(settings.replayGain.mode);
                replayGain["preAmpGain"] = settings.replayGain.preAmpGain;
                replayGain["preAmpGainIfNoInfo"] = settings.replayGain.preAmpGainIfNoInfo;
                res["replayGain"] = std::move(replayGain);
            }

            return Json::serialize(res);
        }

        // transcodingModeFromString: 从字符串还原转码模式，非法值返回 std::nullopt。
        // transcodingModeFromString: восстанавливает режим транскодирования из строки, при ошибке возвращает std::nullopt.
        std::optional<MediaPlayer::Settings::Transcoding::Mode> transcodingModeFromString(const std::string& str)
        {
            const auto value{ core::stringUtils::readAs<int>(str) };
            if (!value)
                return std::nullopt;

            MediaPlayer::Settings::Transcoding::Mode mode{ static_cast<MediaPlayer::Settings::Transcoding::Mode>(*value) };
            switch (mode)
            {
            case MediaPlayer::Settings::Transcoding::Mode::Never:
            case MediaPlayer::Settings::Transcoding::Mode::Always:
            case MediaPlayer::Settings::Transcoding::Mode::IfFormatNotSupported:
                return mode;
            }

            return std::nullopt;
        }

        // formatFromString: 从字符串还原输出格式，仅接受定义好的枚举值。
        // formatFromString: восстанавливает формат вывода, принимая только корректные значения enum.
        std::optional<MediaPlayer::Format> formatFromString(const std::string& str)
        {
            const auto value{ core::stringUtils::readAs<int>(str) };
            if (!value)
                return std::nullopt;

            MediaPlayer::Format format{ static_cast<MediaPlayer::Format>(*value) };
            switch (format)
            {
            case MediaPlayer::Format::MP3:
            case MediaPlayer::Format::OGG_OPUS:
            case MediaPlayer::Format::MATROSKA_OPUS:
            case MediaPlayer::Format::OGG_VORBIS:
            case MediaPlayer::Format::WEBM_VORBIS:
                return format;
            }

            return std::nullopt;
        }

        // bitrateFromInt: 检查比特率是否在允许范围内，否则返回 std::nullopt。
        // bitrateFromInt: проверяет, разрешён ли данный битрейт, иначе возвращает std::nullopt.
        std::optional<MediaPlayer::Bitrate> bitrateFromInt(int bitRate)
        {
            if (!db::isAudioBitrateAllowed(bitRate))
                return std::nullopt;

            return bitRate;
        }

        // replayGainModeFromString: 解析重放增益模式字符串。
        // replayGainModeFromString: парсит строку в режим ReplayGain.
        std::optional<MediaPlayer::Settings::ReplayGain::Mode> replayGainModeFromString(const std::string& str)
        {
            const auto value{ core::stringUtils::readAs<int>(str) };
            if (!value)
                return std::nullopt;

            MediaPlayer::Settings::ReplayGain::Mode mode{ static_cast<MediaPlayer::Settings::ReplayGain::Mode>(*value) };
            switch (mode)
            {
            case MediaPlayer::Settings::ReplayGain::Mode::None:
            case MediaPlayer::Settings::ReplayGain::Mode::Auto:
            case MediaPlayer::Settings::ReplayGain::Mode::Track:
            case MediaPlayer::Settings::ReplayGain::Mode::Release:
                return mode;
            }

            return std::nullopt;
        }

        // replayGainPreAmpGainFromString: 解析并钳制前级增益值到合法范围。
        // replayGainPreAmpGainFromString: парсит и зажимает предусиление в допустимый диапазон.
        std::optional<float> replayGainPreAmpGainFromString(const std::string& str)
        {
            const auto value{ core::stringUtils::readAs<double>(str) };
            if (!value)
                return std::nullopt;

            return std::clamp(*value, (double)MediaPlayer::Settings::ReplayGain::minPreAmpGain, (double)MediaPlayer::Settings::ReplayGain::maxPreAmpGain);
        }

        // settingsfromJSString: 从 JS 传回的 JSON 字符串重建 Settings 结构。
        // settingsfromJSString: восстанавливает структуру Settings из JSON‑строки, пришедшей из JS.
        MediaPlayer::Settings settingsfromJSString(const std::string& strSettings)
        {
            using Settings = MediaPlayer::Settings;
            namespace Json = Wt::Json;
            Json::Object parsedSettings;

            Json::parse(strSettings, parsedSettings);

            MediaPlayer::Settings settings;

            {
                const Json::Value transcodingValue{ parsedSettings.get("transcoding") };
                if (transcodingValue.type() == Json::Type::Object)
                {
                    const Json::Object transcoding{ transcodingValue };
                    settings.transcoding.mode = transcodingModeFromString(transcoding.get("mode").toString().orIfNull("")).value_or(Settings::Transcoding::defaultMode);
                    settings.transcoding.format = formatFromString(transcoding.get("format").toString().orIfNull("")).value_or(Settings::Transcoding::defaultFormat);
                    settings.transcoding.bitrate = bitrateFromInt(transcoding.get("bitrate").toNumber().orIfNull(0)).value_or(Settings::Transcoding::defaultBitrate);
                }
            }
            {
                const Json::Value replayGainValue{ parsedSettings.get("replayGain") };
                if (replayGainValue.type() == Json::Type::Object)
                {
                    const Json::Object replayGain{ replayGainValue };
                    settings.replayGain.mode = replayGainModeFromString(replayGain.get("mode").toString().orIfNull("")).value_or(Settings::ReplayGain::defaultMode);
                    settings.replayGain.preAmpGain = replayGainPreAmpGainFromString(replayGain.get("preAmpGain").toString().orIfNull("")).value_or(Settings::ReplayGain::defaultPreAmpGain);
                    settings.replayGain.preAmpGainIfNoInfo = replayGainPreAmpGainFromString(replayGain.get("preAmpGainIfNoInfo").toString().orIfNull("")).value_or(Settings::ReplayGain::defaultPreAmpGain);
                }
            }

            return settings;
        }
    } // namespace

    MediaPlayer::MediaPlayer()
        : Wt::WTemplate{ Wt::WString::tr("Lms.MediaPlayer.template") }
        , playPrevious{ this, "playPrevious" }
        , playNext{ this, "playNext" }
        , scrobbleListenNow{ this, "scrobbleListenNow" }
        , scrobbleListenFinished{ this, "scrobbleListenFinished" }
        , playbackEnded{ this, "playbackEnded" }
        , _settingsLoaded{ this, "settingsLoaded" }
    {
        addFunction("tr", &Wt::WTemplate::Functions::tr);

        _audioTranscodingResource = std::make_unique<AudioTranscodingResource>();
        _audioFileResource = std::make_unique<AudioFileResource>();

        _title = bindNew<Wt::WText>("title");
        _artist = bindNew<Wt::WAnchor>("artist");
        _release = bindNew<Wt::WAnchor>("release");
        _separator = bindNew<Wt::WText>("separator");
        _playQueue = bindNew<Wt::WPushButton>("playqueue-btn", Wt::WString::tr("Lms.MediaPlayer.template.playqueue-btn").arg(0), Wt::TextFormat::XHTML);
        _playQueue->setLink(Wt::WLink{ Wt::LinkType::InternalPath, "/playqueue" });
        _playQueue->setToolTip(tr("Lms.PlayQueue.playqueue"));

        _settingsLoaded.connect([this](const std::string& settings) {
            LMS_LOG(UI, DEBUG, "Settings loaded! '" << settings << "'");

            _settings = settingsfromJSString(settings);

            settingsLoaded.emit();
        });

        {
            Settings defaultSettings;

            std::ostringstream oss;
            oss << jsRef() + ".mediaplayer = new LMSMediaPlayer("
                << jsRef()
                << ", defaultSettings = " << settingsToJSString(defaultSettings)
                << ")";

            LMS_LOG(UI, DEBUG, "Running js = '" << oss.str() << "'");
            doJavaScript(oss.str());
        }
    }

    MediaPlayer::~MediaPlayer() = default;

    void MediaPlayer::loadTrack(db::TrackId trackId, bool play, float replayGain)
    {
        LMS_LOG(UI, DEBUG, "Playing track ID = " << trackId.toString());

        std::ostringstream oss;
        {
            auto transaction{ LmsApp->getDbSession().createReadTransaction() };

            const auto track{ db::Track::find(LmsApp->getDbSession(), trackId) };
            if (!track)
                return;

            const std::string transcodingResource{ _audioTranscodingResource->getUrl(trackId) };
            const std::string nativeResource{ _audioFileResource->getUrl(trackId) };

            const auto artists{ track->getArtists({ db::TrackArtistLinkType::Artist }) };

            oss
                << "var params = {"
                << " trackId :\"" << trackId.toString() << "\","
                << " nativeResource: \"" << nativeResource << "\","
                << " transcodingResource: \"" << transcodingResource << "\","
                << " duration: " << std::chrono::duration_cast<std::chrono::duration<float>>(track->getDuration()).count() << ","
                << " replayGain: " << replayGain << ","
                << " title: \"" << core::stringUtils::jsEscape(track->getName()) << "\","
                << " artist: \"" << (!artists.empty() ? core::stringUtils::jsEscape(track->getArtistDisplayName()) : "") << "\","
                << " release: \"" << (track->getRelease() ? core::stringUtils::jsEscape(track->getRelease()->getName()) : "") << "\",";

            db::ArtworkId artworkId{ track->getPreferredMediaArtworkId() };
            if (!artworkId.isValid())
                artworkId = track->getPreferredArtworkId();
            if (artworkId.isValid())
            {
                // Potential issue: If the image is really not here, we fall back on a default svg image: the provided type does not match, so we don't put type here
                // Another solution would be to test the image presence and put type accordingly
                oss << " artwork: ["
                    << "   { src: \"" << LmsApp->getArtworkResource()->getArtworkUrl(artworkId, ArtworkResource::DefaultArtworkType::Track, ArtworkResource::Size::Small) << "\", sizes: \"128x128\" },"
                    << "   { src: \"" << LmsApp->getArtworkResource()->getArtworkUrl(artworkId, ArtworkResource::DefaultArtworkType::Track, ArtworkResource::Size::Large) << "\", sizes: \"512x512\" },"
                    << " ]";
            }
            else
            {
                oss << " artwork: ["
                    << "   { src: \"" << LmsApp->getArtworkResource()->getDefaultArtworkUrl(ArtworkResource::DefaultArtworkType::Track) << "\", type: \"image/svg+xml\" },"
                    << " ]";
            }
            oss << "};";

            // Update 'sizes' above to match this:
            static_assert(static_cast<std::underlying_type_t<ArtworkResource::Size>>(ArtworkResource::Size::Small) == 128);
            static_assert(static_cast<std::underlying_type_t<ArtworkResource::Size>>(ArtworkResource::Size::Large) == 512);
            oss << jsRef() + ".mediaplayer.loadTrack(params, " << (play ? "true" : "false") << ")"; // true to autoplay

            _title->setTextFormat(Wt::TextFormat::Plain);
            _title->setText(Wt::WString::fromUTF8(track->getName()));

            bool needSeparator{ true };

            if (!artists.empty())
            {
                _artist->setTextFormat(Wt::TextFormat::Plain);
                _artist->setText(Wt::WString::fromUTF8(artists.front()->getName()));
                _artist->setLink(utils::createArtistLink(artists.front()));
            }
            else
            {
                _artist->setText("");
                _artist->setLink({});
                needSeparator = false;
            }

            if (const db::Release::pointer release{ track->getRelease() })
            {
                _release->setTextFormat(Wt::TextFormat::Plain);
                _release->setText(Wt::WString::fromUTF8(std::string{ release->getName() }));
                _release->setLink(utils::createReleaseLink(release));
            }
            else
            {
                _release->setText("");
                _release->setLink({});
                needSeparator = false;
            }

            if (needSeparator)
                _separator->setText(" — ");
            else
                _separator->setText("");
        }

        LMS_LOG(UI, DEBUG, "Running js = '" << oss.str() << "'");
        doJavaScript(oss.str());

        _trackIdLoaded = trackId;
        trackLoaded.emit(*_trackIdLoaded);
    }

    void MediaPlayer::stop()
    {
        doJavaScript(jsRef() + ".mediaplayer.stop()");
    }

    void MediaPlayer::setSettings(const Settings& settings)
    {
        _settings = settings;

        {
            std::ostringstream oss;
            oss << jsRef() + ".mediaplayer.setSettings(settings = " << settingsToJSString(settings) << ")";

            LMS_LOG(UI, DEBUG, "Running js = '" << oss.str() << "'");
            doJavaScript(oss.str());
        }
    }

    void MediaPlayer::onPlayQueueUpdated(std::size_t trackCount)
    {
        _playQueue->setText(Wt::WString::tr("Lms.MediaPlayer.template.playqueue-btn").arg(trackCount));
    }

} // namespace lms::ui
