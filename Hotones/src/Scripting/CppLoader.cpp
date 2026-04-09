#include "../include/Scripting/CppLoader.hpp"
#include "../include/Scripting/NativeGame.hpp"
#include <string>
#include <iostream>

#ifdef _WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif

struct Hotones::Scripting::CppLoader::Impl {
#ifdef _WIN32
    HMODULE handle = nullptr;
#else
    void* handle = nullptr;
#endif
    void* instance = nullptr;
    CreateGameFn createFn = nullptr;
    DestroyGameFn destroyFn = nullptr;
    UpdateGameFn updateFn = nullptr;
    DrawGameFn drawFn = nullptr;
};

using namespace Hotones::Scripting;

CppLoader::CppLoader() : m_impl(new Impl()) {}
CppLoader::~CppLoader() { unloadModule(); }

bool CppLoader::loadModule(const std::string& path)
{
    unloadModule();
#ifdef _WIN32
    HMODULE h = LoadLibraryA(path.c_str());
    if (!h) {
        m_lastError = "LoadLibrary failed";
        return false;
    }
    m_impl->handle = h;
    m_impl->createFn = (CreateGameFn)GetProcAddress(h, "CreateGame");
    m_impl->destroyFn = (DestroyGameFn)GetProcAddress(h, "DestroyGame");
    m_impl->updateFn = (UpdateGameFn)GetProcAddress(h, "UpdateGame");
    m_impl->drawFn = (DrawGameFn)GetProcAddress(h, "DrawGame");
#else
    void* h = dlopen(path.c_str(), RTLD_NOW);
    if (!h) {
        m_lastError = dlerror();
        return false;
    }
    m_impl->handle = h;
    m_impl->createFn = (CreateGameFn)dlsym(h, "CreateGame");
    m_impl->destroyFn = (DestroyGameFn)dlsym(h, "DestroyGame");
    m_impl->updateFn = (UpdateGameFn)dlsym(h, "UpdateGame");
    m_impl->drawFn = (DrawGameFn)dlsym(h, "DrawGame");
#endif

    if (!m_impl->createFn || !m_impl->destroyFn || !m_impl->updateFn || !m_impl->drawFn) {
        m_lastError = "Module missing required symbols (CreateGame/DestroyGame/UpdateGame/DrawGame)";
        unloadModule();
        return false;
    }

    m_impl->instance = m_impl->createFn();
    if (!m_impl->instance) {
        m_lastError = "CreateGame returned null";
        unloadModule();
        return false;
    }
    return true;
}

void CppLoader::unloadModule()
{
    if (!m_impl) return;
    if (m_impl->instance && m_impl->destroyFn) {
        m_impl->destroyFn(m_impl->instance);
        m_impl->instance = nullptr;
    }
#ifdef _WIN32
    if (m_impl->handle) {
        FreeLibrary(m_impl->handle);
        m_impl->handle = nullptr;
    }
#else
    if (m_impl->handle) {
        dlclose(m_impl->handle);
        m_impl->handle = nullptr;
    }
#endif
    m_impl->createFn = nullptr;
    m_impl->destroyFn = nullptr;
    m_impl->updateFn = nullptr;
    m_impl->drawFn = nullptr;
}

void CppLoader::update(float dt)
{
    if (m_impl && m_impl->updateFn && m_impl->instance)
        m_impl->updateFn(m_impl->instance, dt);
}

void CppLoader::draw()
{
    if (m_impl && m_impl->drawFn && m_impl->instance)
        m_impl->drawFn(m_impl->instance);
}
