// 文件资源读取/写入处理声明

#pragma once

#include <filesystem>
#include <fstream>
#include <string>
#include <string_view>

#include "core/IResourceHandler.hpp"

namespace lms::core
{
    class FileResourceHandler final : public IResourceHandler
    {
    public:
        FileResourceHandler(const std::filesystem::path& filePath, std::string_view mimeType);

    private:
        Wt::Http::ResponseContinuation* processRequest(const Wt::Http::Request& request, Wt::Http::Response& response) override;
        void abort() override {};

        static constexpr std::size_t _chunkSize{ 262'144 };

        std::string _mimeType;
        ::uint64_t _beyondLastByte{};
        ::uint64_t _offset{};
        ::uint64_t _fileSize{};
        std::ifstream _ifs;
    };
} // namespace lms::core