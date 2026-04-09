#include <Assets/PackScanner.hpp>
#include <algorithm>
#include <sys/stat.h>
#include <dirent.h>

namespace Hotones::Assets {

std::vector<PackEntry> ScanPacksDir(const std::string& basePath) {
    std::vector<PackEntry> out;
    DIR* dir = opendir(basePath.c_str());
    if (!dir) return out;
    struct dirent* ent;
    while ((ent = readdir(dir)) != nullptr) {
        std::string name(ent->d_name);
        if (name == "." || name == "..") continue;
        std::string full = basePath + "/" + name;
        struct stat st;
        if (stat(full.c_str(), &st) != 0) continue;

        PackEntry pe;
        pe.fullPath = full;
        pe.displayName = name;

        if (S_ISDIR(st.st_mode)) {
            std::string initPath = full + "/init.lua";
            struct stat ist;
            if (stat(initPath.c_str(), &ist) == 0 && S_ISREG(ist.st_mode)) {
                pe.type = PackEntry::Type::Directory;
                out.push_back(pe);
            }
        } else if (S_ISREG(st.st_mode)) {
            size_t dot = name.find_last_of('.');
            std::string ext = (dot == std::string::npos) ? std::string() : name.substr(dot);
            if (ext == ".cup" || ext == ".zip") {
                pe.type = PackEntry::Type::ZipFile;
                out.push_back(pe);
            }
        }
    }
    closedir(dir);

    std::sort(out.begin(), out.end(), [](const PackEntry& a, const PackEntry& b) {
        return a.displayName < b.displayName;
    });

    return out;
}

} // namespace Hotones::Assets
