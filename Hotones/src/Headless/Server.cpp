#include <server/Server.hpp>
#include <server/NetworkManager.hpp>
#include <Scripting/CupLoader.hpp>
#include <Scripting/CupPackage.hpp>

#include <atomic>
#include <chrono>
#include <csignal>
#include <filesystem>
#include <iostream>
#include <thread>

namespace {
    std::atomic<bool> g_serverRunning{ true };
}

static void SignalHandler(int /*sig*/) {
    g_serverRunning = false;
}

namespace Hotones {

void RunHeadlessServer(uint16_t port, const std::string& pakPath) {
    std::signal(SIGINT,  SignalHandler);
    std::signal(SIGTERM, SignalHandler);

    // -- Optional game pack ---------------------------------------------------
    Hotones::Scripting::CupPackage pak;
    Hotones::Scripting::CupLoader  script;
    bool hasPak = false;

    if (!pakPath.empty()) {
        if (!pak.open(pakPath)) {
            std::cerr << "[Server] Failed to open pack: " << pakPath << "\n";
            return;
        }
        if (!script.init() || !script.loadPak(pak)) {
            std::cerr << "[Server] Failed to initialise pack.\n";
            return;
        }
        hasPak = true;
        std::cout << "[Server] Game pack loaded: " << pakPath;
        if (!script.mainScenePath().empty())
            std::cout << "  (scene: " << script.mainScenePath() << ")";
        std::cout << "\n";
    }

    // -- Network --------------------------------------------------------------
    Net::NetworkManager server;

    if (hasPak) {
        // Advertise the pack's display name in SERVER_INFO_RESP replies.
        // Extract the stem from the provided path without using
        // std::filesystem::path::stem().string() to avoid codecvt linkage.
        auto extract_stem = [](const std::string& p) -> std::string {
            size_t end = p.size();
            while (end > 0 && (p[end-1] == '/' || p[end-1] == '\\')) --end;
            size_t start = p.find_last_of("/\\", end == 0 ? 0 : end-1);
            if (start == std::string::npos) start = 0; else ++start;
            size_t dot = p.find_last_of('.', end == 0 ? 0 : end-1);
            if (dot == std::string::npos || dot < start) dot = end;
            return p.substr(start, dot - start);
        };
        server.SetHostedPakName(extract_stem(pakPath).c_str());
    }

    if (hasPak) {
        // Forward network player events into the Lua pack
        server.OnPlayerJoined = [&script](uint8_t id, const char* name) {
            std::cout << "[Server] ++ Player " << static_cast<int>(id)
                      << " \"" << name << "\" joined\n";
            script.firePlayerJoined(id, name);
        };
        server.OnPlayerLeft = [&script](uint8_t id) {
            std::cout << "[Server] -- Player " << static_cast<int>(id) << " left\n";
            script.firePlayerLeft(id);
        };
        // Give the Lua pack access to live player data via network.*
        script.setNetworkManager(&server);
    } else {
        server.OnPlayerJoined = [](uint8_t id, const char* name) {
            std::cout << "[Server] ++ Player " << static_cast<int>(id)
                      << " \"" << name << "\" joined\n";
        };
        server.OnPlayerLeft = [](uint8_t id) {
            std::cout << "[Server] -- Player " << static_cast<int>(id) << " left\n";
        };
    }

    if (!server.StartServer(port)) {
        std::cerr << "[Server] Failed to start on port " << port << "\n";
        return;
    }

    std::cout << "[Server] Dedicated server running on UDP port " << port << "\n";
    std::cout << "[Server] Press Ctrl+C to shut down.\n";

    // -- Main loop ------------------------------------------------------------
    while (g_serverRunning.load()) {
        server.Update();
        if (hasPak) script.update();
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    std::cout << "\n[Server] Shutting down...\n";
    server.StopServer();
    std::cout << "[Server] Goodbye!\n";
}

} // namespace Hotones
