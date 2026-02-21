// MainMenu for OverTone built-in pack
#include <ECS/ECS.hpp>
#include <GFX/UIManager.hpp>
#include <PakRegistry.hpp>
#include <GFX/Renderer.hpp>
#include <Scripting/ScriptBehavior.hpp>
/*
    OverTone is a 2.5D built in pack based on similar designs to Habo hotel, but with a more Modern focus.
    primarily designed to be the default pack for the game, and to be a template for future pack creators to build on.
*/

// Main menu game scene
class OverTone : public Hotones::Scripting::ScriptBehavior {
public:
    void Init()   override { /* load assets */ }
    void Update() override { /* game logic  */ }
    void Draw3D() override { /* mesh draws  */ }
    void Draw()   override {
        ClearBackground(BLACK);
        auto& UI = Hotones::GFX::UIManager::Get();
        
    }
};

// Adapter scene that owns an OverTone instance and forwards Scene calls.
class OverTone_SceneAdapter : public Hotones::Scene {
public:
    OverTone_SceneAdapter() = default;
    void Init() override { m_impl.Init(); }
    void Update() override { m_impl.Update(); }
    void Draw3D() override { m_impl.Draw3D(); }
    void Draw() override { m_impl.Draw(); }
    void Unload() override {}

private:
    OverTone m_impl;
};

// Static registration so this built-in registers itself with PakRegistry
namespace {
    struct OverToneRegister {
        OverToneRegister() {
            Hotones::PakRegistry::Get().RegisterBuiltIn("overtone", [](){
                return std::make_unique<OverTone_SceneAdapter>();
            });
        }
    } s_overToneRegister;
}