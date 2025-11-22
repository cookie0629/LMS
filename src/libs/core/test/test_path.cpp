#include <iostream>
#include <filesystem>
#include "core/Path.hpp"

int main()
{
    std::cout << "==========================================" << std::endl;
    std::cout << "Path 工具类测试" << std::endl;
    std::cout << "==========================================" << std::endl;
    std::cout << std::endl;

    // 测试路径
    std::filesystem::path testPath = std::filesystem::current_path();
    
    std::cout << "当前工作目录: " << testPath << std::endl;
    std::cout << std::endl;

    // 测试 exists()
    std::cout << "测试 exists():" << std::endl;
    bool pathExists = lms::core::pathUtils::exists(testPath);
    std::cout << "  路径存在: " << (pathExists ? "是" : "否") << std::endl;
    std::cout << std::endl;

    // 测试 isDirectory()
    std::cout << "测试 isDirectory():" << std::endl;
    bool isDir = lms::core::pathUtils::isDirectory(testPath);
    std::cout << "  是目录: " << (isDir ? "是" : "否") << std::endl;
    std::cout << std::endl;

    // 测试 getFilename()
    std::cout << "测试 getFilename():" << std::endl;
    std::string filename = lms::core::pathUtils::getFilename(testPath);
    std::cout << "  文件名: " << filename << std::endl;
    std::cout << std::endl;

    // 测试 getExtension()
    std::cout << "测试 getExtension():" << std::endl;
    std::filesystem::path testFile = testPath / "test.mp3";
    std::string extension = lms::core::pathUtils::getExtension(testFile);
    std::cout << "  扩展名: " << extension << std::endl;
    std::cout << std::endl;

    // 测试 combine()
    std::cout << "测试 combine():" << std::endl;
    std::filesystem::path combined = lms::core::pathUtils::combine(testPath, "subdir");
    std::cout << "  拼接结果: " << combined << std::endl;
    std::cout << std::endl;

    // 测试 hasFileAnyExtension()
    std::cout << "测试 hasFileAnyExtension():" << std::endl;
    std::filesystem::path audioFile = testPath / "test.mp3";
    std::array<std::filesystem::path, 3> extensions = {".mp3", ".flac", ".ogg"};
    bool hasExt = lms::core::pathUtils::hasFileAnyExtension(audioFile, extensions);
    std::cout << "  有音频扩展名: " << (hasExt ? "是" : "否") << std::endl;
    std::cout << std::endl;

    // 测试 sanitizeFileStem()
    std::cout << "测试 sanitizeFileStem():" << std::endl;
    std::string dirtyName = "test:file*name?";
    std::string cleanName = lms::core::pathUtils::sanitizeFileStem(dirtyName);
    std::cout << "  原始: " << dirtyName << std::endl;
    std::cout << "  清理后: " << cleanName << std::endl;
    std::cout << std::endl;

    std::cout << "==========================================" << std::endl;
    std::cout << "测试完成！" << std::endl;
    std::cout << "==========================================" << std::endl;

    return 0;
}

