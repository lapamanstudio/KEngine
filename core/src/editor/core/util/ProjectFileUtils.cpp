#include "editor/core/utils/ProjectFileUtils.h"

#include <iostream>

#ifdef _WIN32
    #include <windows.h>
#else
    #include <unistd.h>
#endif

fs::path ProjectFileUtils::GetDataFilePath(const std::string& relativePath) {
    return fs::path(FileUtils::GetExecutablePath()) / "datafiles" / relativePath;
}

fs::path ProjectFileUtils::GetProjectsBaseFolder() {
    const char* homeDir = nullptr;

#ifdef _WIN32
    homeDir = std::getenv("USERPROFILE");
#elif defined(__linux__) || defined(__APPLE__)
    homeDir = std::getenv("HOME");
#endif

    if (homeDir) {
        fs::path projectsPath = fs::path(homeDir) / "KEngine projects";
        return projectsPath;
    } else {
        std::cerr << "Failed to get home directory environment variable." << std::endl;
        return ".";
    }
}
