#include "engine/core/utils/FileUtils.h"

#include <iostream>

#ifdef _WIN32
    #include <windows.h>
#else
    #include <unistd.h>
#endif

fs::path FileUtils::GetExecutablePath() {
    char buffer[1024];

#ifdef _WIN32
    if (GetModuleFileName(NULL, buffer, sizeof(buffer)) == 0) {
        std::cerr << "Error retrieving executable path." << std::endl;
        return "";
    }
#elif __linux__
    ssize_t len = readlink("/proc/self/exe", buffer, sizeof(buffer) - 1);
    if (len == -1) {
        std::cerr << "Error reading symbolic link to executable." << std::endl;
        return "";
    }
    buffer[len] = '\0';
#elif __APPLE__
    uint32_t size = sizeof(buffer);
    if (_NSGetExecutablePath(buffer, &size) != 0) {
        std::cerr << "Buffer too small; needed size is " << size << std::endl;
        return "";
    }
#endif

    return fs::path(buffer).parent_path();
}

fs::path FileUtils::GetFilePath(const std::string& relativePath) {
    return fs::path(FileUtils::GetExecutablePath()) / relativePath;
}

bool FileUtils::DirectoryExists(const fs::path& path) {
    return fs::exists(path) && fs::is_directory(path);
}

bool FileUtils::FileExists(const fs::path& path) {
    return fs::exists(path);
}

char FileUtils::GetPathSeparator() {
#ifdef _WIN32
    return '\\';
#else
    return '/';
#endif
}
