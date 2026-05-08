#pragma once

#include <GFX/Scene.hpp>

namespace Hotones::Scripting {

/**
 * C++ mirror of the Lua MainClass protocol, as a full Scene subclass.
 *
 * Extend this instead of Scene when you want all five lifecycle hooks with
 * sensible no-op defaults — so you only override what you actually need.
 * Because ScriptBehavior IS-A Scene it can be registered with SceneManager
 * and loaded exactly like any other scene.
 *
 *   Lua                 C++ (this class)    When called
 *   -----------------   ----------------    ----------------------
 *   YourClass:Init()    Init()              Once, after the scene loads.
 *   YourClass:Update()  Update()            Every tick / frame.
 *   YourClass:draw3D()  Draw3D()            Every frame, INSIDE BeginMode3D.
 *   YourClass:Draw()    Draw()              Every frame, OUTSIDE 3D mode.
 *   (no Lua equiv.)     Unload()            Once, when the scene is unloaded.
 *
 * Usage
 * -----
 *
 *   class MyScene : public Hotones::Scripting::ScriptBehavior {
 *   public:
 *       void Init()   override { /* load assets *\/ }
 *       void Update() override { /* game logic  *\/ }
 *       void Draw3D() override { /* mesh draws  *\/ }
 *       void Draw()   override { /* HUD draws   *\/ }
 *   };
 *
 *   // Register and load via SceneManager:
 *   SceneManager::Get().Add("MyScene", [] { return std::make_unique<MyScene>(); });
 *   SceneManager::Get().SwitchTo("MyScene");
 *
 * MarkFinished() (inherited from Scene) can be called inside any method to
 * signal to the SceneManager that this scene is done.
 */
class ScriptBehavior : public Hotones::Scene {
public:
    // ── Camera ────────────────────────────────────────────────────────────
    // Exposed so subclasses (and the main loop) can read/modify the 3-D camera.
    // Initialised to a sensible perspective default; override GetCamera() if you
    // want to supply a different camera object entirely.
    Camera3D camera = []{
        Camera3D c{};
        c.position   = { 0.0f, 2.0f, 10.0f };
        c.target     = { 0.0f, 0.0f,  0.0f };
        c.up         = { 0.0f, 1.0f,  0.0f };
        c.fovy       = 60.0f;
        c.projection = CAMERA_PERSPECTIVE;
        return c;
    }();

    // Returns this scene's camera so the main loop can call BeginMode3D before
    // dispatching Draw3D().  Override to return nullptr to opt out of the
    // automatic 3-D pass (e.g. if you manage BeginMode3D yourself in Draw()).
    Camera3D* GetCamera() override { return &camera; }

    // ── Lifecycle hooks (all no-ops by default) ───────────────────────────

    /** Called once immediately after the scene is loaded. */
    void Init()   override {}

    /** Called every game tick (server: ~100 Hz, client: once per frame). */
    void Update() override {}

    /** Called every frame **inside** BeginMode3D / EndMode3D.
     *  Use 3-D mesh / model draw calls here. */
    void Draw3D() override {}

    /** Called every frame **outside** 3D mode.
     *  Use 2-D / HUD (render.*) draw calls here. */
    void Draw()   override {}

    /** Called once when the scene is being unloaded. */
    void Unload() override {}
};

} // namespace Hotones::Scripting
