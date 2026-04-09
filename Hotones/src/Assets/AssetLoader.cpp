#include <Assets/AssetLoader.hpp>
#include <string>
#include <sys/stat.h>
#if defined(_WIN32)
#include <windows.h>
#include <direct.h>
#else
#include <unistd.h>
#include <limits.h>
#include <stdlib.h>
#endif
#if defined(_WIN32)
#include <windows.h>
#elif defined(__linux__)
#include <unistd.h>
#include <limits.h>
#endif

namespace Ho_tones {
namespace Assets {

static bool file_exists(const std::string& p) {
    struct stat st;
    return stat(p.c_str(), &st) == 0;
}

static std::string make_absolute(const std::string& p) {
#if defined(_WIN32)
    char buf[MAX_PATH];
    DWORD len = GetFullPathNameA(p.c_str(), MAX_PATH, buf, NULL);
    if (len == 0 || len >= MAX_PATH) return {};
    return std::string(buf);
#else
    char resolved[PATH_MAX];
    if (realpath(p.c_str(), resolved) == nullptr) return {};
    return std::string(resolved);
#endif
}

std::string GetExecutableDir() {
#if defined(_WIN32)
    char buf[MAX_PATH];
    DWORD len = GetModuleFileNameA(NULL, buf, MAX_PATH);
    if (len == 0 || len == MAX_PATH) return {};
    std::string s(buf, (size_t)len);
    size_t pos = s.find_last_of("/\\");
    if (pos == std::string::npos) return s;
    return s.substr(0, pos);
#else
    char buf[PATH_MAX];
    ssize_t len = readlink("/proc/self/exe", buf, sizeof(buf)-1);
    if (len == -1) return {};
    buf[len] = '\0';
    std::string s(buf);
    size_t pos = s.find_last_of('/');
    if (pos == std::string::npos) return s;
    return s.substr(0, pos);
#endif
}

std::string ResolveRelativeToExe(const std::string& relativePath) {
    std::string exeDir = GetExecutableDir();
    if (exeDir.empty()) return {};
    std::string sep = "/";
#if defined(_WIN32)
    sep = "\\";
#endif
    return exeDir + sep + relativePath;
}

bool FindAsset(const std::string& relativePath, std::string& outPath) {
    // Check given path as-is (relative to cwd)
    if (file_exists(relativePath)) { outPath = make_absolute(relativePath); return true; }

    // Check relative to executable directory
    std::string exeDir = GetExecutableDir();
    if (!exeDir.empty()) {
        std::string candidate = exeDir + "/" + relativePath;
        if (file_exists(candidate)) { outPath = make_absolute(candidate); return true; }

        // Also try one level up (app installed next to a data folder)
        size_t pos = exeDir.find_last_of("/\\");
        if (pos != std::string::npos) {
            std::string up = exeDir.substr(0, pos);
            std::string candidate2 = up + "/" + relativePath;
            if (file_exists(candidate2)) { outPath = make_absolute(candidate2); return true; }
        }
    }

    // Check common build/data folders relative to cwd
    std::string buildCandidate = std::string("build") + "/" + relativePath;
    if (file_exists(buildCandidate)) { outPath = make_absolute(buildCandidate); return true; }

    // Not found
    return false;
}

} // namespace Assets
} // namespace Ho_tones
