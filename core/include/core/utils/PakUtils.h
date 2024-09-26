#ifndef PAK_UTILS_H
#define PAK_UTILS_H

#include "core/utils/FileUtils.h"

#include <string>
#include <vector>
#include <cstdint>

class PakUtils {
public:
    static void BundleAssets(const fs::path& pakPath, const std::vector<std::string>& assets);

    struct FileEntry {
        std::string fileName;
        uint32_t fileSize;
        uint32_t fileOffset;
    };
};

#endif // PAK_UTILS_H
