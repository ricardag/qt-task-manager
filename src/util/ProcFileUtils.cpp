#include "ProcFileUtils.h"

#include <cctype>
#include <fstream>
#include <sstream>

namespace taskmanager::util {

std::optional<std::string> readFile(const std::filesystem::path& path) {
    std::ifstream stream(path, std::ios::in | std::ios::binary);
    if (!stream) {
        return std::nullopt;
    }
    std::ostringstream buffer;
    buffer << stream.rdbuf();
    if (stream.bad()) {
        return std::nullopt;
    }
    return buffer.str();
}

std::vector<std::string> splitLines(const std::string& content) {
    std::vector<std::string> lines;
    std::istringstream stream(content);
    std::string line;
    while (std::getline(stream, line)) {
        lines.push_back(line);
    }
    return lines;
}

std::vector<std::string> splitWhitespace(const std::string& line) {
    std::vector<std::string> tokens;
    std::string current;
    for (char c : line) {
        if (std::isspace(static_cast<unsigned char>(c))) {
            if (!current.empty()) {
                tokens.push_back(current);
                current.clear();
            }
        } else {
            current.push_back(c);
        }
    }
    if (!current.empty()) {
        tokens.push_back(current);
    }
    return tokens;
}

} // namespace taskmanager::util
