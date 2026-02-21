#include <PakRegistry.hpp>
#include <filesystem>
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
    std::filesystem::path p("./paks");
    if (!std::filesystem::exists(p) || !std::filesystem::is_directory(p)) return;
    for (auto& e : std::filesystem::directory_iterator(p)) {
        try {
            if (e.is_regular_file() && e.path().extension() == ".cup") {
                std::string name = toLo(e.path().stem().string());
                m_filePaks[name] = e.path().string();
            } else if (e.is_directory()) {
                std::string name = toLo(e.path().stem().string());
                m_filePaks[name] = e.path().string();
            }
        } catch (...) {}
    }
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
