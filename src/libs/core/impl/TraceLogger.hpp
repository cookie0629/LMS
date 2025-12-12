// 链路/跟踪日志记录声明

#pragma once

#include <array>
#include <deque>
#include <map>
#include <mutex>
#include <shared_mutex>
#include <thread>
#include <unordered_map>
#include <vector>

#include "core/ITraceLogger.hpp"

namespace lms::core::tracing
{
    class TraceLogger : public ITraceLogger
    {
    public:
        TraceLogger(Level minLevel, std::size_t bufferSizeinMBytes);

        void onThreadPreDestroy();

    private:
        bool isLevelActive(Level level) const override;
        void write(const CompleteEvent& event) override;
        void dumpCurrentBuffer(std::ostream& os) override;
        void setThreadName(std::thread::id id, std::string_view threadName) override;
        ArgHashType registerArg(LiteralString argType, std::string_view argValue) override;
        void setMetadata(std::string_view metadata, std::string_view value) override;

        std::size_t getRegisteredArgCount() const;

        static ArgHashType computeArgHash(LiteralString type, std::string_view value);
        static std::uint32_t toTraceThreadId(std::thread::id threadId);

        static constexpr std::size_t BufferSize{ 64 * 1024 };

        // Same as ComplteEvent, but compacted
        struct CompleteEventEntry
        {
            clock::time_point start;
            clock::duration duration;
            const char* name;
            const char* category;
            ArgHashType arg;
        };
        static constexpr ArgHashType invalidHash{ 0 };

        struct alignas(64) Buffer
        {
            static constexpr std::size_t CompleteEventCount{ BufferSize / sizeof(CompleteEventEntry) };

            std::thread::id threadId;
            std::array<CompleteEventEntry, CompleteEventCount> durationEvents;
            std::atomic<std::size_t> currentDurationIndex{};
        };

        Buffer* acquireBuffer();
        void releaseBuffer(Buffer* buffer);

        const Level _minLevel;
        const clock::time_point _start;
        const std::thread::id _creatorThreadId;

        std::vector<Buffer> _buffers; // allocated once during construction

        mutable std::shared_mutex _argMutex;
        struct ArgEntry
        {
            LiteralString type;
            std::string value;
        };
        using ArgEntryMap = std::unordered_map<ArgHashType, ArgEntry>;
        ArgEntryMap _argEntries; // collisions not handled

        std::mutex _threadNameMutex;
        std::unordered_map<std::thread::id, std::string> _threadNames;

        std::mutex _metadataMutex;
        std::map<std::string, std::string> _metadata;

        std::mutex _mutex;
        std::deque<Buffer*> _freeBuffers;

        static thread_local Buffer* _currentBuffer;
    };
} // namespace lms::core::tracing