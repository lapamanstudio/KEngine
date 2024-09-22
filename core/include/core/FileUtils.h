#ifndef FILE_UTILS_H
#define FILE_UTILS_H

#include <string>
#include <GL/gl.h>

std::string GetExecutablePath();
std::string GetFilePath(const std::string& relativePath);
std::string GetDataFilePath(const std::string& relativePath);
std::string getProjectsBaseFolder();
bool directoryExists(const std::string& path);
char getPathSeparator();

#endif  // FILE_UTILS_H
