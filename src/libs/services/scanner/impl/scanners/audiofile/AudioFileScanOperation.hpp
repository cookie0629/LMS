#pragma once

#include <optional>

#include "../FileScanOperationBase.hpp"

namespace lms::scanner
{
    /**
     * @brief 音频文件扫描操作（简化版）
     * 
     * 注意：完整版本需要音频处理库（如 TagLib）支持
     */
    class AudioFileScanOperation : public FileScanOperationBase
    {
    public:
        AudioFileScanOperation(FileToScan&& fileToScan, db::IDb& db, const ScannerSettings& settings);
        ~AudioFileScanOperation() override = default;
        AudioFileScanOperation(const AudioFileScanOperation&) = delete;
        AudioFileScanOperation& operator=(const AudioFileScanOperation&) = delete;

    private:
        std::string_view getName() const override { return "ScanAudioFile"; }
        void scan() override;
        OperationResult processResult() override;

        // 简化版：暂时不存储元数据
        // 完整版本需要存储：音频属性、轨道信息、图像等
        bool _scanSuccess{ false };
    };
} // namespace lms::scanner

