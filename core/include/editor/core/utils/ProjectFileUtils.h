#pragma once

#include "editor/core/utils/ProjectFileUtils.h"
#include "engine/core/utils/FileUtils.h"

#include <string>
#include <filesystem>

namespace fs = std::filesystem;

class ProjectFileUtils {
public:
    static fs::path GetExecutablePath();
    static fs::path GetFilePath(const std::string& relativePath);
    static fs::path GetDataFilePath(const std::string& relativePath);
    static fs::path GetProjectsBaseFolder();
    static bool DirectoryExists(const fs::path& path);
    static bool FileExists(const fs::path& path);
    static char GetPathSeparator();
};
