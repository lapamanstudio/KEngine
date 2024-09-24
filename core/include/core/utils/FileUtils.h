#ifndef FILE_UTILS_H
#define FILE_UTILS_H

#include <string>
#include <GL/gl.h>

class FileUtils {
public:
    static std::string GetExecutablePath();
    static std::string GetFilePath(const std::string& relativePath);
    static std::string GetDataFilePath(const std::string& relativePath);
    static std::string GetProjectsBaseFolder();
    static bool DirectoryExists(const std::string& path);
    static bool FileExists(const std::string& path);
    static char GetPathSeparator();
};

#endif  // FILE_UTILS_H
