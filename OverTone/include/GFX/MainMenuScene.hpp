#pragma once

#include <GFX/Scene.hpp>
#include <GFX/StbTextField.hpp>
#include <Assets/PackScanner.hpp>
#include <server/NetworkManager.hpp>
#include <raylib.h>
#include <string>
#include <vector>
#include <cstdint>

namespace Hotones {

class MainMenuScene : public Scene {
public:
    enum class Action { None, Host, Join, Quit };

    explicit MainMenuScene(Net::NetworkManager* net = nullptr);
    ~MainMenuScene() override = default;

    void Init()   override;
    void Update() override;
    void Draw()   override;
    void Unload() override;

    // Read by main.cpp after IsFinished()
    Action      GetAction()        const { return m_action; }
    std::string GetPlayerName()    const { return m_nameField.text(); }
    std::string GetConnectHost()   const { return m_ipField.text(); }
    uint16_t    GetConnectPort()   const { return m_port; }
    std::string GetPakPath()       const { return m_selectedPakPath; }
    std::string GetServerPakName() const { return m_serverPakName; }

private:
    enum class State { Main, ServerBrowser, Host, BuiltInGames };

    // ── State ─────────────────────────────────────────────────────────────────
    State    m_state  = State::Main;
    Action   m_action = Action::None;
    uint16_t m_port   = 27015;

    // ── Network ───────────────────────────────────────────────────────────────
    Net::NetworkManager* m_net = nullptr;

    // ── StbTextField fields ───────────────────────────────────────────────────
    StbTextField m_nameField;
    StbTextField m_ipField;
    StbTextField m_portField;
    int          m_activeField = -1;   // 0=name, 1=port (Host screen)

    // ── Pack list (Host screen) ───────────────────────────────────────────────
    std::vector<Assets::PackEntry> m_packs;
    int         m_packScroll    = 0;
    int         m_selectedPack  = -1;
    std::string m_selectedPakPath;

    // ── Server browser ────────────────────────────────────────────────────────
    struct ServerEntry {
        std::string host;
        uint16_t    port        = Net::DEFAULT_PORT;
        uint8_t     playerCount = 0;
        uint8_t     maxPlayers  = 0;
        char        pakName[32] = {};
        char        gameVersion[16] = {};
        char        pakVersion[16]  = {};
        bool        responded   = false;
        bool        pinging     = false;
    };
    std::vector<ServerEntry> m_servers;
    int         m_serverScroll   = 0;
    int         m_selectedServer = -1;
    std::string m_serverPakName;
    std::string m_statusMessage;
    float       m_statusTimer = 0.f;

    // Inline "add server" sub-form
    bool         m_showAddServer  = false;
    StbTextField m_addIpField;
    StbTextField m_addPortField;
    int          m_addActiveField = -1;

    // ── Helpers ───────────────────────────────────────────────────────────────
    static bool Button(const char* text, Rectangle rect,
                       Color bg = {55, 35, 85, 255},
                       Color fg = {220, 210, 235, 255});
    static void Label(const char* text, int x, int y,
                      int fs = 16, Color col = {155, 145, 175, 255});

    void DrawMain();
    void DrawServerBrowser();
    void DrawHost();
    void DrawBuiltInGames();
    
    void RefreshPacks();
    void PingAllServers();
    void AddServer(const char* host, uint16_t port);
    void RemoveSelectedServer();
    // If a local pack name matches pakName, sets m_selectedPakPath and returns true
    bool MatchLocalPak(const char* pakName);
};

} // namespace Hotones
