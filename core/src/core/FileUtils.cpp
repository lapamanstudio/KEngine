#include "core/FileUtils.h"
#include "graphics/utils/stb_image.h"

#include <iostream>
#include <filesystem>
#include <GL/gl.h>

#ifdef _WIN32
    #include <windows.h>
#else
    #include <unistd.h>
#endif

std::string GetExecutablePath() {
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

    return std::filesystem::path(buffer).parent_path().string();
}

std::string GetFilePath(const std::string& relativePath) {
    std::filesystem::path filePath = std::filesystem::path(GetExecutablePath()) / relativePath;
    return filePath.string();
}

std::string GetDataFilePath(const std::string& relativePath) {
    std::filesystem::path dataFilePath = std::filesystem::path(GetExecutablePath()) / "datafiles" / relativePath;
    return dataFilePath.string();
}

std::string getProjectsBaseFolder() {
    const char* homeDir = nullptr;

#ifdef _WIN32
    homeDir = std::getenv("USERPROFILE");
#elif defined(__linux__) || defined(__APPLE__)
    homeDir = std::getenv("HOME");
#endif

    if (homeDir) {
        std::filesystem::path projectsPath = std::filesystem::path(homeDir) / "kengine projects";
        return projectsPath.string();
    } else {
        std::cerr << "Failed to get home directory environment variable." << std::endl;
        return ".";
    }
}

bool directoryExists(const std::string& path) {
    return std::filesystem::exists(path) && std::filesystem::is_directory(path);
}

// getPathSeparator
char getPathSeparator() {
#ifdef _WIN32
    return '\\';
#else
    return '/';
#endif
}