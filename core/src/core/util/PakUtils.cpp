#include "core/utils/PakUtils.h"
#include "core/ProjectConfig.h"

#include <iostream>
#include <fstream>

void PakUtils::BundleAssets(const fs::path& pakPath, const std::vector<std::string>& assets) {
    std::ofstream pakFile(pakPath, std::ios::binary);
    std::vector<FileEntry> fileEntries;

    uint32_t indexOffset = 0;

    // Write each asset
    for (const auto& asset : assets) {
        std::ifstream inputFile(ProjectConfig::getInstance().projectDirectory / asset, std::ios::binary | std::ios::ate);

        if (!inputFile.is_open()) {
            std::cerr << "Error opening file: " << asset << std::endl;
            continue;
        }

        uint32_t fileSize = inputFile.tellg();
        inputFile.seekg(0, std::ios::beg);

        // Save entry
        FileEntry entry;
        entry.fileName = asset;
        entry.fileSize = fileSize;
        entry.fileOffset = pakFile.tellp();

        fileEntries.push_back(entry);

        std::vector<char> buffer(fileSize);
        inputFile.read(buffer.data(), fileSize);
        pakFile.write(buffer.data(), fileSize);

        inputFile.close();
    }

    // Offset to the file index
    indexOffset = pakFile.tellp();

    uint32_t fileCount = fileEntries.size();
    pakFile.write(reinterpret_cast<const char*>(&fileCount), sizeof(fileCount));

    // Write the file index
    for(const auto& entry : fileEntries) {
        uint32_t filenameLength = entry.fileName.size();
        pakFile.write(reinterpret_cast<const char*>(&filenameLength), sizeof(filenameLength));
        pakFile.write(entry.fileName.c_str(), filenameLength);
        pakFile.write(reinterpret_cast<const char*>(&entry.fileSize), sizeof(entry.fileSize));
        pakFile.write(reinterpret_cast<const char*>(&entry.fileOffset), sizeof(entry.fileOffset));
    }

    // Write the index offset
    pakFile.write(reinterpret_cast<const char*>(&indexOffset), sizeof(indexOffset));

    pakFile.close();
}
