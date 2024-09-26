#ifndef FILE_UTILS_H
#define FILE_UTILS_H

#include <string>
#include <filesystem>

namespace fs = std::filesystem;

class FileUtils {
public:
    static fs::path GetExecutablePath();
    static fs::path GetFilePath(const std::string& relativePath);
    static fs::path GetDataFilePath(const std::string& relativePath);
    static fs::path GetProjectsBaseFolder();
    static bool DirectoryExists(const fs::path& path);
    static bool FileExists(const fs::path& path);
    static char GetPathSeparator();
};

#endif  // FILE_UTILS_H
