#include <Assets/AssetLoader.hpp>
#include <filesystem>
#include <string>
#if defined(_WIN32)
#include <windows.h>
#elif defined(__linux__)
#include <unistd.h>
#include <limits.h>
#endif

namespace fs = std::filesystem;

namespace Ho_tones {
namespace Assets {

std::string GetExecutableDir() {
#if defined(_WIN32)
    char buf[MAX_PATH];
    DWORD len = GetModuleFileNameA(NULL, buf, MAX_PATH);
    if (len == 0 || len == MAX_PATH) return {};
    fs::path p(buf);
    return p.parent_path().string();
#elif defined(__linux__)
    char buf[PATH_MAX];
    ssize_t len = readlink("/proc/self/exe", buf, sizeof(buf)-1);
    if (len == -1) return {};
    buf[len] = '\0';
    fs::path p(buf);
    return p.parent_path().string();
#else
    // Fallback: current path
    return fs::current_path().string();
#endif
}

std::string ResolveRelativeToExe(const std::string& relativePath) {
    std::string exeDir = GetExecutableDir();
    if (exeDir.empty()) return {};
    fs::path combined = fs::path(exeDir) / relativePath;
    return combined.string();
}

bool FindAsset(const std::string& relativePath, std::string& outPath) {
    // Check given path as-is (relative to cwd)
    fs::path p1 = relativePath;
    if (fs::exists(p1)) { outPath = fs::absolute(p1).string(); return true; }

    // Check relative to executable directory
    std::string exeDir = GetExecutableDir();
    if (!exeDir.empty()) {
        fs::path p2 = fs::path(exeDir) / relativePath;
        if (fs::exists(p2)) { outPath = fs::absolute(p2).string(); return true; }

        // Also try one level up (app installed next to a data folder)
        fs::path p3 = fs::path(exeDir).parent_path() / relativePath;
        if (fs::exists(p3)) { outPath = fs::absolute(p3).string(); return true; }
    }

    // Check common build/data folders relative to cwd
    fs::path p4 = fs::path("build") / relativePath;
    if (fs::exists(p4)) { outPath = fs::absolute(p4).string(); return true; }

    // Not found
    return false;
}

} // namespace Assets
} // namespace Ho_tones
