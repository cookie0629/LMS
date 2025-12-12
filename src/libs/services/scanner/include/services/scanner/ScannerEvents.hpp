
#pragma once

#include <Wt/WDateTime.h>
#include <Wt/WSignal.h>

#include "ScannerStats.hpp"

namespace lms::scanner
{

    // Events: 扫描服务对外暴露的一组信号，用于 UI 显示进度和结果。
    // Events: набор сигналов сканера, который слушают UI и другие подсистемы для отображения прогресса/результатов.
    struct Events
    {
        Wt::Signal<> scanAborted;

        Wt::Signal<> scanStarted;

        Wt::Signal<ScanStats> scanComplete;

        Wt::Signal<ScanStepStats> scanInProgress;

        Wt::Signal<Wt::WDateTime> scanScheduled;
    };

} // namespace lms::scanner
