#pragma once

#include <memory>

#include "services/scanner/ScannerStats.hpp"
#include "../ScannerSettings.hpp"

namespace lms::db
{
    class IDb;
    class Session;
} // namespace lms::db

namespace lms::scanner
{
    /**
     * @brief 扫描步骤基类
     */
    class ScanStepBase
    {
    public:
        ScanStepBase(db::IDb& db, const ScannerSettings& settings);
        virtual ~ScanStepBase() = default;
        ScanStepBase(const ScanStepBase&) = delete;
        ScanStepBase& operator=(const ScanStepBase&) = delete;

        /**
         * @brief 执行扫描步骤
         * @param stats 扫描统计信息（会被更新）
         * @return true 如果成功，false 如果失败
         */
        virtual bool execute(ScanStats& stats) = 0;

        /**
         * @brief 获取步骤名称
         */
        virtual ScanStep getStep() const = 0;

    protected:
        db::IDb& getDb() { return _db; }
        const ScannerSettings& getScannerSettings() const { return _settings; }

    private:
        db::IDb& _db;
        const ScannerSettings& _settings;
    };
} // namespace lms::scanner

