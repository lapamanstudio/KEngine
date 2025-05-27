#pragma once

#include <string>
#include <filesystem>

namespace fs = std::filesystem;

class FileUtils {
public:
    static fs::path GetExecutablePath();
    static fs::path GetFilePath(const std::string& relativePath);
    static bool DirectoryExists(const fs::path& path);
    static bool FileExists(const fs::path& path);
    static char GetPathSeparator();
};
