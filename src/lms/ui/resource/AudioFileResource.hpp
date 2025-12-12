
#pragma once

#include <Wt/WResource.h>

#include "database/objects/TrackId.hpp"

namespace lms::ui
{
    // AudioFileResource: 音频文件资源处理器，提供原始音频文件的流式传输。
    // AudioFileResource: обработчик ресурсов аудиофайлов, предоставляет потоковую передачу исходных аудиофайлов.
    //
    // 功能：
    // - 处理音频文件的 HTTP 请求
    // - 支持范围请求（HTTP Range），实现音频文件的断点续传和跳转
    // - 直接从文件系统读取音频文件并流式传输给客户端
    // - 不进行转码，返回原始格式的音频文件
    //
    // Функции:
    // - Обрабатывает HTTP-запросы аудиофайлов
    // - Поддерживает запросы диапазонов (HTTP Range) для возобновления передачи и перехода к позиции
    // - Читает аудиофайлы напрямую из файловой системы и передаёт их клиенту потоком
    // - Не выполняет транскодирование, возвращает аудиофайлы в исходном формате
    class AudioFileResource : public Wt::WResource
    {
    public:
        ~AudioFileResource() override;

        // getUrl: 获取指定曲目的音频文件 URL。
        // getUrl: получает URL аудиофайла указанного трека.
        std::string getUrl(db::TrackId trackId) const;

    private:
        // handleRequest: 处理音频文件的 HTTP 请求，支持范围请求以实现音频播放的跳转功能。
        // handleRequest: обрабатывает HTTP-запрос аудиофайла, поддерживает запросы диапазонов для перехода к позиции воспроизведения.
        void handleRequest(const Wt::Http::Request& request,
                           Wt::Http::Response& response) override;
    };
} // namespace lms::ui
