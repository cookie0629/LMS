// 归档压缩/打包工具类声明

#pragma once

#include <cstddef>
#include <memory>

#include "core/IZipper.hpp"

extern "C"
{
    struct archive;
    struct archive_entry;
}

namespace lms::zip
{
    // ArchiveZipper: 基于 libarchive 的 ZIP 压缩器实现，用于按块写出 ZIP 内容。
    // ArchiveZipper: реализация IZipper на базе libarchive, пишет ZIP‑архив порциями.
    class ArchiveZipper : public IZipper
    {
    public:
        ArchiveZipper(const EntryContainer& entries);
        ~ArchiveZipper() = default;
        ArchiveZipper(const ArchiveZipper&) = delete;
        ArchiveZipper& operator=(const ArchiveZipper&) = delete;

    private:
        std::uint64_t writeSome(std::ostream& output) override;
        bool isComplete() const override;
        void abort() override;

        class ArchiveDeleter
        {
        public:
            void operator()(struct ::archive* arch);
        };
        using ArchivePtr = std::unique_ptr<struct ::archive, ArchiveDeleter>;

        class ArchiveEntryDeleter
        {
        public:
            void operator()(struct ::archive_entry* archEntry);
        };
        using ArchiveEntryPtr = std::unique_ptr<struct ::archive_entry, ArchiveEntryDeleter>;

        void prepareCurrentEntry();
        static ArchiveEntryPtr createArchiveEntry(const Entry& entry);
        bool writeSomeCurrentFileData();
        std::int64_t onWriteCallback(const std::byte* buff, std::size_t size);

        const EntryContainer _entries;
        ArchivePtr _archive;

        static inline constexpr std::size_t _writeBlockSize{ 65536 };
        static inline constexpr std::size_t _readBufferSize{ 65536 };
        std::vector<std::byte> _readBuffer;

        EntryContainer::const_iterator _currentEntry;
        ArchiveEntryPtr _currentArchiveEntry;

        std::uint64_t _currentEntryOffset{};
        std::ostream* _currentOutputStream{};
        std::uint64_t _bytesWrittenInCurrentOutputStream{};
    };

} // namespace lms::zip