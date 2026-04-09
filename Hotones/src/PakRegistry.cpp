#include <PakRegistry.hpp>
#include <sys/stat.h>
#include <dirent.h>
#include <algorithm>
#include <cctype>

namespace Hotones {

PakRegistry& PakRegistry::Get() {
    static PakRegistry instance;
    return instance;
}

static std::string toLo(const std::string& s) {
    std::string out = s;
    std::transform(out.begin(), out.end(), out.begin(), [](unsigned char c){ return (char)std::tolower(c); });
    return out;
}

void PakRegistry::RegisterBuiltIn(const std::string& name, SceneFactory factory) {
    m_builtins[toLo(name)] = std::move(factory);
}

bool PakRegistry::IsBuiltIn(const std::string& name) const {
    return m_builtins.find(toLo(name)) != m_builtins.end();
}

PakRegistry::SceneFactory PakRegistry::GetBuiltInFactory(const std::string& name) const {
    auto it = m_builtins.find(toLo(name));
    if (it == m_builtins.end()) return SceneFactory();
    return it->second;
}

std::vector<std::string> PakRegistry::GetBuiltInNames() const {
    std::vector<std::string> out;
    out.reserve(m_builtins.size());
    for (const auto& kv : m_builtins) out.push_back(kv.first);
    return out;
}

void PakRegistry::ScanPaksDir() {
    m_filePaks.clear();
    const char* dirPath = "./paks";
    DIR* dir = opendir(dirPath);
    if (!dir) return;
    struct dirent* ent;
    while ((ent = readdir(dir)) != nullptr) {
        std::string name(ent->d_name);
        if (name == "." || name == "..") continue;
        std::string full = std::string(dirPath) + "/" + name;
        struct stat st;
        if (stat(full.c_str(), &st) != 0) continue;
        bool is_dir = S_ISDIR(st.st_mode);
        bool is_file = S_ISREG(st.st_mode);
        if (is_file) {
            // check .cup extension
            size_t dot = name.find_last_of('.');
            std::string ext = (dot == std::string::npos) ? std::string() : name.substr(dot);
            if (ext == ".cup") {
                size_t dpos = name.find_last_of('.');
                std::string stem = (dpos == std::string::npos) ? name : name.substr(0, dpos);
                m_filePaks[toLo(stem)] = full;
            }
        } else if (is_dir) {
            // directory - use directory name as pak name
            m_filePaks[toLo(name)] = full;
        }
    }
    closedir(dir);
}

std::string PakRegistry::GetFilePakPath(const std::string& name) const {
    auto it = m_filePaks.find(toLo(name));
    if (it == m_filePaks.end()) return std::string();
    return it->second;
}

std::vector<std::pair<std::string,std::string>> PakRegistry::GetFilePaks() const {
    std::vector<std::pair<std::string,std::string>> out;
    out.reserve(m_filePaks.size());
    for (const auto& kv : m_filePaks) out.emplace_back(kv.first, kv.second);
    return out;
}

} // namespace Hotones
