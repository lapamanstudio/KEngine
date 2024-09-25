#ifndef PAK_UTILS_H
#define PAK_UTILS_H

#include <string>
#include <vector>
#include <cstdint>

class PakUtils {
public:
    static void BundleAssets(const std::string& pakPath, const std::vector<std::string>& assets);

    struct FileEntry {
        std::string fileName;
        uint32_t fileSize;
        uint32_t fileOffset;
    };
};

#endif // PAK_UTILS_H
