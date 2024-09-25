#ifndef PROJECTCONFIG_H
#define PROJECTCONFIG_H

#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <filesystem>

namespace fs = std::filesystem;

struct ProjectConfig {
    std::string projectName;
    fs::path projectDirectory;
    std::string currentDirectory;

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
        projectDirectory = fs::path(configJson.value("project_directory", "build"));
        currentDirectory = configJson.value("current_directory", "Assets/");
        currentDirectory == "" ? "Assets/" : currentDirectory;

        isInitialized = true;
        return true;
    }

    bool saveToFile(const fs::path& filePath) const {
        nlohmann::json configJson;
        configJson["project_name"] = projectName;
        configJson["project_directory"] = projectDirectory;
        configJson["current_directory"] = currentDirectory;

        std::ofstream file(filePath);
        if (!file.is_open()) {
            std::cerr << "Error: Could not write to config file: " << filePath.string() << std::endl;
            return false;
        }

        file << configJson.dump(4);
        return true;
    }

    fs::path getFullCurrentDirectory() const {
        return projectDirectory / (currentDirectory == "" ? "Assets/" : currentDirectory);
    }

    void addRecentProject(const fs::path& base_path) {
        fs::path recentProjectsFile = base_path / "recent_projects.json";

        nlohmann::json recentProjectsJson;
        std::ifstream fileIn(recentProjectsFile);

        if (fileIn.is_open()) {
            try {
                fileIn >> recentProjectsJson;
            } catch (nlohmann::json::parse_error& e) {
                std::cerr << "Error: Failed to parse recent projects file: " << e.what() << std::endl;
                recentProjectsJson = nlohmann::json::array();
            }
            fileIn.close();
        }

        if (!recentProjectsJson.is_array()) {
            recentProjectsJson = nlohmann::json::array();
        }

        nlohmann::json currentProjectJson;
        currentProjectJson["project_name"] = projectName;
        currentProjectJson["project_directory"] = projectDirectory;
        currentProjectJson["current_directory"] = currentDirectory;

        for (auto it = recentProjectsJson.begin(); it != recentProjectsJson.end(); ++it) {
            if ((*it)["project_directory"] == projectDirectory.string()) {
                recentProjectsJson.erase(it);
                break;
            }
        }

        recentProjectsJson.insert(recentProjectsJson.begin(), currentProjectJson);

        const size_t maxRecentProjects = 3;
        if (recentProjectsJson.size() > maxRecentProjects) {
            recentProjectsJson.erase(recentProjectsJson.begin() + maxRecentProjects, recentProjectsJson.end());
        }

        std::ofstream fileOut(recentProjectsFile);
        if (!fileOut.is_open()) {
            std::cerr << "Error: Could not open recent projects file: " << recentProjectsFile << std::endl;
            return;
        }

        fileOut << recentProjectsJson.dump(4);
        fileOut.close();
    }

    std::vector<ProjectConfig> getRecentProjects(const fs::path& base_path) {
        fs::path recentProjectsFile = base_path / "recent_projects.json";

        nlohmann::json recentProjectsJson;
        std::ifstream file(recentProjectsFile);
        if (!file.is_open()) {
            std::cerr << "Error: Could not open recent projects file: " << recentProjectsFile << std::endl;
            return {};
        }

        file >> recentProjectsJson;
        file.close();

        std::vector<ProjectConfig> recentProjects;
        for (const auto& projectJson : recentProjectsJson) {
            ProjectConfig project;
            project.projectName = projectJson.value("project_name", "Unnamed Project");
            project.projectDirectory = projectJson.value("project_directory", "build");
            project.currentDirectory = projectJson.value("current_directory", "Assets/");
            project.isInitialized = true;

            recentProjects.push_back(std::move(project));
        }

        return recentProjects;
    }

    bool isConfigInitialized() const {
        return isInitialized;
    }

    ProjectConfig(const ProjectConfig&) = delete;
    void operator=(const ProjectConfig&) = delete;

    ProjectConfig(ProjectConfig&& other) noexcept
        : projectName(std::move(other.projectName)),
          projectDirectory(std::move(other.projectDirectory)),
          currentDirectory(std::move(other.currentDirectory)),
          isInitialized(other.isInitialized) {}

    ProjectConfig& operator=(ProjectConfig&& other) noexcept {
        if (this != &other) {
            projectName = std::move(other.projectName);
            projectDirectory = std::move(other.projectDirectory);
            currentDirectory = std::move(other.currentDirectory);
            isInitialized = other.isInitialized;
        }
        return *this;
    }

private:
    ProjectConfig() = default;
};

#endif // PROJECTCONFIG_H
