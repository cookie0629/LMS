
#pragma once

#include <memory>

#include "core/IResourceHandler.hpp"
#include "core/IZipper.hpp"

namespace lms::zip
{
    // createZipperResourceHandler: 将 IZipper 适配为 HTTP 资源处理器，支持通过 HTTP 下载 ZIP 流。
    // createZipperResourceHandler: адаптирует IZipper к IResourceHandler для HTTP‑выдачи ZIP‑архива.
    std::unique_ptr<IResourceHandler> createZipperResourceHandler(std::unique_ptr<IZipper> zipper);
}
