#pragma once

#include "engine/core/utils/FileUtils.h"

#include <string>
#include <filesystem>

namespace fs = std::filesystem;

class ProjectFileUtils {
public:
    static fs::path GetDataFilePath(const std::string& relativePath);
    static fs::path GetProjectsBaseFolder();
};
