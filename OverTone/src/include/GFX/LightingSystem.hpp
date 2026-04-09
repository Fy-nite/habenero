#pragma once
#include <raylib.h>
#include <array>

// ─── LightingSystem ───────────────────────────────────────────────────────────
//
// Singleton that owns a Blinn-Phong GLSL shader and manages up to MAX_LIGHTS
// dynamic lights (point, directional, spot).  The shader is applied to any
// raylib Model by calling ApplyToModel(); uniforms are re-uploaded every frame
// via UploadUniforms().
//
// Typical per-frame usage in ScriptedScene::Draw():
//   GFX::LightingSystem::Get().UploadUniforms(camera);
//   BeginMode3D(camera);
//       worldModel.Draw();   // already has the lit shader patched in
//   EndMode3D();
//
// Lua packs control lights through the "lighting" global table.

namespace Hotones::GFX {

constexpr int MAX_LIGHTS = 8;

enum class LightType : int {
    Point       = 0,
    Directional = 1,
    Spot        = 2
};

struct LightDesc {
    LightType type      = LightType::Point;
    bool      enabled   = false;
    Vector3   position  = {0.f, 5.f, 0.f};
    Vector3   direction = {0.f, -1.f, 0.f};  // normalised, world-space
    Vector3   color     = {1.f, 1.f, 1.f};   // linear 0-1
    float     intensity = 1.0f;
    float     range     = 20.0f;              // attenuation radius (point/spot)
    float     innerCos  = 0.9763f;            // cos(12.5°) spot inner cone
    float     outerCos  = 0.8660f;            // cos(30°)   spot outer cone
};

class LightingSystem {
public:
    // ── Singleton ─────────────────────────────────────────────────────────────
    static LightingSystem& Get();

    // ── Lifecycle ─────────────────────────────────────────────────────────────

    // Compile and link the GLSL lighting shader.
    // Must be called after raylib InitWindow().  Idempotent.
    bool Init();

    // Release the shader.  Must be called before CloseWindow().
    void Unload();

    bool IsReady() const { return m_ready; }

    // ── Light management ──────────────────────────────────────────────────────

    // Allocate a light slot.  Returns a 1-based handle (1..MAX_LIGHTS) or 0
    // when all slots are occupied.  The new light is immediately enabled.
    int  AddLight(LightType type, Vector3 pos, Vector3 dir,
                  Vector3 colorLinear, float intensity = 1.f, float range = 20.f);

    // Free the slot so it can be reused.
    void RemoveLight(int handle);

    bool       IsValidHandle(int handle) const;
    LightDesc* GetLight(int handle);        // nullptr if handle is invalid

    // ── Ambient ───────────────────────────────────────────────────────────────

    // colorLinear: per-channel linear colour (0-1).
    // intensity  : scalar multiplier applied on top of the colour.
    void SetAmbient(Vector3 colorLinear, float intensity = 0.15f);

    // ── Per-frame ─────────────────────────────────────────────────────────────

    // Pack all light data into shader uniforms + upload view-position.
    // Call once per frame before drawing any lit geometry.
    void UploadUniforms(Camera camera);

    // ── Model integration ─────────────────────────────────────────────────────

    // Set this system's shader on every material slot in a raylib Model.
    // Call once after loading the model (or whenever the model is reloaded).
    void ApplyToModel(Model& model);

    const Shader& GetShader() const { return m_shader; }

private:
    LightingSystem() = default;

    Shader  m_shader = {};
    bool    m_ready  = false;

    std::array<LightDesc, MAX_LIGHTS> m_lights = {};

    Vector3 m_ambientColor     = {1.f, 1.f, 1.f};
    float   m_ambientIntensity = 0.15f;

    // Cached uniform locations (one per array, pointing at [0])
    struct Locs {
        int viewPos          = -1;
        int ambientColor     = -1;
        int ambientIntensity = -1;
        int lightEnabled     = -1;
        int lightType        = -1;
        int lightPos         = -1;
        int lightDir         = -1;
        int lightColor       = -1;
        int lightIntensity   = -1;
        int lightRange       = -1;
        int lightInnerCos    = -1;
        int lightOuterCos    = -1;
    } m_locs;

    void CacheLocations();
};

} // namespace Hotones::GFX
