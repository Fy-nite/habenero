#pragma once

#include <string>
#include <memory>

namespace Hotones::Scripting {

class CppLoader {
public:
    CppLoader();
    ~CppLoader();

    // Load a native module (full path). Returns true on success.
    bool loadModule(const std::string& path);

    // Unload module.
    void unloadModule();

    // Callbacks
    void update(float dt);
    void draw();

    const std::string& GetLastError() const { return m_lastError; }

private:
    struct Impl;
    std::unique_ptr<Impl> m_impl;
    std::string m_lastError;
};

} // namespace
