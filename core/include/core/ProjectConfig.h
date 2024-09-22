#ifndef PROJECTCONFIG_H

#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>
#include <string>

struct ProjectConfig {
    std::string projectName;
    std::string projectDirectory;

    bool isInitialized = false;

    static ProjectConfig& getInstance() {
        static ProjectConfig instance;
        return instance;
    }

    bool loadFromFile(const std::string& filePath) {
        std::ifstream file(filePath);
        if (!file.is_open()) {
            std::cerr << "Error: Could not open config file: " << filePath << std::endl;
            return false;
        }
        nlohmann::json configJson;
        file >> configJson;

        projectName = configJson.value("project_name", "Unnamed Project");
        projectDirectory = configJson.value("project_directory", "build");

        isInitialized = true;
        return true;
    }

    bool saveToFile(const std::string& filePath) const {
        nlohmann::json configJson;
        configJson["project_name"] = projectName;
        configJson["project_directory"] = projectDirectory;

        std::ofstream file(filePath);
        if (!file.is_open()) {
            std::cerr << "Error: Could not write to config file: " << filePath << std::endl;
            return false;
        }

        file << configJson.dump(4);
        return true;
    }

    bool isConfigInitialized() const {
        return isInitialized;
    }

    ProjectConfig(const ProjectConfig&) = delete;
    void operator=(const ProjectConfig&) = delete;

private:
    ProjectConfig() = default;
};

#endif // PROJECTCONFIG_H
