#include <Assets/PackScanner.hpp>
#include <filesystem>
#include <algorithm>

namespace Hotones::Assets {

std::vector<PackEntry> ScanPacksDir(const std::string& basePath) {
    std::vector<PackEntry> out;
    namespace fs = std::filesystem;

    std::error_code ec;
    if (!fs::exists(basePath, ec) || !fs::is_directory(basePath, ec))
        return out;

    for (const auto& e : fs::directory_iterator(basePath, ec)) {
        if (ec) break;

        PackEntry pe;
        pe.fullPath    = e.path().string();
        pe.displayName = e.path().filename().string();

        if (e.is_directory(ec)) {
            if (fs::exists(e.path() / "init.lua", ec)) {
                pe.type = PackEntry::Type::Directory;
                out.push_back(pe);
            }
        } else if (e.is_regular_file(ec)) {
            auto ext = e.path().extension().string();
            if (ext == ".cup" || ext == ".zip") {
                pe.type = PackEntry::Type::ZipFile;
                out.push_back(pe);
            }
        }
    }

    std::sort(out.begin(), out.end(), [](const PackEntry& a, const PackEntry& b) {
        return a.displayName < b.displayName;
    });

    return out;
}

} // namespace Hotones::Assets
