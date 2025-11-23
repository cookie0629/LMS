#include "AudioFileScanOperation.hpp"

#include <filesystem>

#include "audio/IAudioFileInfo.hpp"
#include "core/ILogger.hpp"
#include "core/Path.hpp"
#include "core/Service.hpp"
#include "database/IDb.hpp"
#include "database/objects/Artist.hpp"
#include "database/objects/Directory.hpp"
#include "database/objects/MediaLibrary.hpp"
#include "database/objects/Release.hpp"
#include "database/objects/Track.hpp"
#include "database/Session.hpp"
#include "database/Transaction.hpp"
#include "services/scanner/ScanErrors.hpp"

namespace lms::scanner
{
    AudioFileScanOperation::AudioFileScanOperation(FileToScan&& fileToScan, db::IDb& db, const ScannerSettings& settings)
        : FileScanOperationBase{ std::move(fileToScan), db, settings }
    {
    }

    void AudioFileScanOperation::scan()
    {
        const auto& filePath = getFilePath();

        try
        {
            if (!std::filesystem::exists(filePath))
            {
                addError<FileScanError>(filePath, "File does not exist");
                _scanSuccess = false;
                return;
            }

            // 尝试解析音频文件
            // 简化版：只检查文件是否存在，不实际解析
            // 完整版本需要：
            // 1. 使用音频处理库（如 TagLib）读取文件
            // 2. 提取元数据（标题、艺术家、专辑等）
            // 3. 提取音频属性（时长、比特率、采样率等）
            // 4. 提取嵌入图像
            // 5. 计算文件哈希值

            audio::ParserOptions parserOptions;
            _audioFileInfo = audio::parseAudioFile(filePath, parserOptions);

            // 简化版：即使解析失败也继续（返回空实现）
            // 完整版本需要检查解析结果并提取元数据

            _scanSuccess = true;
        }
        catch (const audio::Exception& e)
        {
            addError<FileScanError>(filePath, std::string("Audio file parsing error: ") + e.what());
            _scanSuccess = false;
        }
        catch (const std::exception& e)
        {
            addError<FileScanError>(filePath, std::string("Scan error: ") + e.what());
            _scanSuccess = false;
        }
    }

    IFileScanOperation::OperationResult AudioFileScanOperation::processResult()
    {
        if (!_scanSuccess)
        {
            return OperationResult::Skipped;
        }

        try
        {
            auto& session = getDb().getTLSSession();
            auto transaction = session.createWriteTransaction();

            const auto& filePath = getFilePath();
            const auto& mediaLibrary = getMediaLibrary();

            // 1. 获取或创建 MediaLibrary 对象
            db::MediaLibrary::pointer library;
            if (mediaLibrary.id.getValue() != 0)
            {
                library = db::MediaLibrary::find(session, mediaLibrary.id);
            }
            if (!library)
            {
                library = db::MediaLibrary::getOrCreate(session, core::pathUtils::getFilename(mediaLibrary.rootPath), mediaLibrary.rootPath);
            }

            // 2. 获取或创建 Directory 对象
            auto dirPath = filePath.parent_path();
            db::Directory::pointer directory;
            
            // 递归创建父目录
            std::vector<std::filesystem::path> dirsToCreate;
            auto currentPath = dirPath;
            while (!currentPath.empty() && currentPath != mediaLibrary.rootPath && currentPath != currentPath.root_path())
            {
                dirsToCreate.push_back(currentPath);
                currentPath = currentPath.parent_path();
            }
            
            // 从根目录开始创建
            db::Directory::pointer parentDir;
            for (auto it = dirsToCreate.rbegin(); it != dirsToCreate.rend(); ++it)
            {
                directory = db::Directory::getOrCreate(session, *it, library, parentDir);
                parentDir = directory;
            }
            
            if (!directory)
            {
                // 如果文件就在根目录下，创建根目录
                directory = db::Directory::getOrCreate(session, mediaLibrary.rootPath, library, {});
            }

            // 3. 检查 Track 是否已存在
            auto existingTrack = db::Track::findByPath(session, filePath);
            
            // 4. 获取文件信息
            std::filesystem::path path = filePath;
            auto fileSize = static_cast<long long>(getFileSize());
            auto lastWriteTime = getLastWriteTime();

            if (existingTrack)
            {
                // 更新现有 Track
                existingTrack.modify()->setFileSize(fileSize);
                existingTrack.modify()->setLastWriteTime(lastWriteTime);
                existingTrack.modify()->setDirectory(directory);
                
                // 如果文件名已更改，更新名称
                auto currentName = existingTrack->getName();
                auto newName = core::pathUtils::getFilename(filePath);
                if (currentName != newName)
                {
                    existingTrack.modify()->setName(newName);
                }

                transaction.commit();
                return OperationResult::Updated;
            }
            else
            {
                // 创建新 Track
                auto track = db::Track::getOrCreate(session, filePath);
                track.modify()->setFileSize(fileSize);
                track.modify()->setLastWriteTime(lastWriteTime);
                track.modify()->setDirectory(directory);
                track.modify()->setName(core::pathUtils::getFilename(filePath));

                transaction.commit();
                return OperationResult::Added;
            }
        }
        catch (const std::exception& e)
        {
            addError<FileScanError>(getFilePath(), std::string("Database error: ") + e.what());
            return OperationResult::Skipped;
        }
    }
} // namespace lms::scanner

