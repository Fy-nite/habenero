#pragma once

#include <GFX/Scene.hpp>
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace Hotones {

class PakRegistry {
public:
    using SceneFactory = std::function<std::unique_ptr<Scene>()>;

    static PakRegistry& Get();

    void RegisterBuiltIn(const std::string& name, SceneFactory factory);
    bool IsBuiltIn(const std::string& name) const;
    SceneFactory GetBuiltInFactory(const std::string& name) const;
    std::vector<std::string> GetBuiltInNames() const;

    // Scan ./paks for .cup files or pack folders and register them as file-based packs
    void ScanPaksDir();
    // Get path for a file-based pack by name (returns empty if none)
    std::string GetFilePakPath(const std::string& name) const;
    std::vector<std::pair<std::string,std::string>> GetFilePaks() const;

private:
    std::unordered_map<std::string, SceneFactory> m_builtins;
    std::unordered_map<std::string, std::string> m_filePaks; // name->path
};

} // namespace Hotones
