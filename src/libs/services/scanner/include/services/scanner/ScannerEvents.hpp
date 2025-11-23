#pragma once

#include <Wt/WDateTime.h>
#include <Wt/WSignal.h>

#include "ScannerStats.hpp"

namespace lms::scanner
{
    /**
     * @brief 扫描事件
     */
    struct Events
    {
        // 扫描被中止时调用
        Wt::Signal<> scanAborted;

        // 扫描开始时调用
        Wt::Signal<> scanStarted;

        // 扫描完成时调用（参数表示是否有变更）
        Wt::Signal<ScanStats> scanComplete;

        // 扫描进行中调用
        Wt::Signal<ScanStepStats> scanInProgress;

        // 扫描被调度后调用
        Wt::Signal<Wt::WDateTime> scanScheduled;
    };
} // namespace lms::scanner

