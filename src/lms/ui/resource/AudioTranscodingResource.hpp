
#pragma once

#include <Wt/Dbo/ptr.h>
#include <Wt/WResource.h>

#include "database/objects/TrackId.hpp"

namespace lms::db
{
    class User;
}

namespace lms::ui
{
    // AudioTranscodingResource: 音频转码资源处理器，提供实时转码的音频流。
    // AudioTranscodingResource: обработчик ресурсов транскодирования аудио, предоставляет потоковую передачу аудио в реальном времени.
    //
    // 功能：
    // - 根据用户设置对音频文件进行实时转码
    // - 支持多种输出格式（OGG Opus、MP3、AAC 等）
    // - 支持可配置的比特率
    // - 流式传输转码后的音频数据（分块传输）
    // - URL 依赖于用户，因为转码设置是用户特定的
    //
    // Функции:
    // - Выполняет транскодирование аудиофайлов в реальном времени согласно настройкам пользователя
    // - Поддерживает различные форматы вывода (OGG Opus, MP3, AAC и т.д.)
    // - Поддерживает настраиваемый битрейт
    // - Передаёт транскодированные аудиоданные потоком (по частям)
    // - URL зависит от пользователя, так как настройки транскодирования специфичны для пользователя
    class AudioTranscodingResource : public Wt::WResource
    {
    public:
        ~AudioTranscodingResource();

        // Url depends on the user since settings are used in parameters
        // URL 依赖于用户，因为转码设置（格式、比特率等）包含在参数中
        // URL зависит от пользователя, так как настройки транскодирования (формат, битрейт и т.д.) включены в параметры
        std::string getUrl(db::TrackId trackId) const;

        // handleRequest: 处理转码请求，启动转码进程并流式传输数据。
        // handleRequest: обрабатывает запрос транскодирования, запускает процесс транскодирования и передаёт данные потоком.
        void handleRequest(const Wt::Http::Request& request, Wt::Http::Response& response);

    private:
        static constexpr std::size_t _chunkSize{ 262144 };  // 每次传输的数据块大小（256 KB）/ размер блока данных за передачу (256 КБ)
    };
} // namespace lms::ui
