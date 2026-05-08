#pragma once
#include <string>

namespace Ho_tones {
namespace Assets {

// Try to find an asset by `relativePath`. Returns true and places
// the resolved absolute path in `outPath` when found.
bool FindAsset(const std::string& relativePath, std::string& outPath);

// Resolve path relative to the running executable directory.
std::string ResolveRelativeToExe(const std::string& relativePath);

// Return the directory containing the running executable.
std::string GetExecutableDir();

} // namespace Assets
} // namespace Ho_tones
