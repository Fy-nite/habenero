#pragma once
#include <string>
#include <vector>

namespace Hotones::Assets {

struct PackEntry {
    enum class Type { Directory, ZipFile };
    std::string displayName; ///< filename/dirname shown in UI
    std::string fullPath;    ///< path passed to CupPackage::open()
    Type        type = Type::Directory;
};

/// Scan basePath for:
///   - Sub-directories that contain an "init.lua"  (unzipped packs)
///   - Regular files with extension ".cup" or ".zip"  (zipped packs)
/// Returns entries sorted ascending by displayName.
std::vector<PackEntry> ScanPacksDir(const std::string& basePath);

} // namespace Hotones::Assets
