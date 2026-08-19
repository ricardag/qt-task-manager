#pragma once

#include <filesystem>
#include <optional>
#include <string>
#include <vector>

namespace taskmanager::util {

// Reads a whole file into a string. Returns nullopt instead of throwing
// when the file is missing or unreadable, since /proc entries can vanish
// between being listed and being read (the process exited).
std::optional<std::string> readFile(const std::filesystem::path& path);

// Splits file content into lines, dropping the trailing empty line if any.
std::vector<std::string> splitLines(const std::string& content);

// Splits a single line on runs of whitespace, discarding empty tokens.
std::vector<std::string> splitWhitespace(const std::string& line);

} // namespace taskmanager::util
