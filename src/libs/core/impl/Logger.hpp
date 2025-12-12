
#pragma once

#include <filesystem>
#include <iosfwd>
#include <list>
#include <mutex>
#include <unordered_map>

#include "core/ILogger.hpp"

namespace lms::core::logging
{
    class Logger final : public ILogger
    {
    public:
        Logger(Severity minSeverity, const std::filesystem::path& logFilePath);
        ~Logger() override;
        Logger(const Logger&) = delete;
        Logger& operator=(const Logger&) = delete;

    private:
        bool isSeverityActive(Severity severity) const override;
        void processLog(const Log& log) override;
        void processLog(Module module, Severity severity, std::string_view message) override;

        void addOutputStream(std::ostream& os, Severity severity);

        struct OutputStream
        {
            OutputStream(std::ostream& os);

            std::mutex mutex;
            std::ostream& stream;
        };

        std::list<OutputStream> _outputStreams;
        std::unordered_map<Severity, OutputStream*> _severityToOutputStreamMap;
        std::unique_ptr<std::ofstream> _logFileStream;
    };
} // namespace lms::core::logging