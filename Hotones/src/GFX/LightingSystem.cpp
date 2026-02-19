// LightingSystem.cpp — custom Blinn-Phong lighting shader + runtime light manager
//
// The GLSL shaders are embedded as raw string literals so no external shader
// files are needed.  Uniforms are uploaded every frame via UploadUniforms().

#include <GFX/LightingSystem.hpp>
#include <raylib.h>
#include <cstdio>

// ─── Embedded GLSL — vertex shader ───────────────────────────────────────────
//
// Attribute / uniform names must match raylib's built-in conventions so that
// DrawMesh() automatically binds MVP matrix, model matrix, diffuse colour, and
// the texture sampler.

static const char* kLitVS = R"(
#version 330

in vec3 vertexPosition;
in vec2 vertexTexCoord;
in vec3 vertexNormal;
in vec4 vertexColor;

uniform mat4 mvp;
uniform mat4 matModel;
uniform mat4 matNormal;

out vec3 fragPos;
out vec2 fragTexCoord;
out vec4 fragColor;
out vec3 fragNormal;

void main()
{
    fragPos      = vec3(matModel * vec4(vertexPosition, 1.0));
    fragTexCoord = vertexTexCoord;
    fragColor    = vertexColor;
    fragNormal   = normalize(mat3(matNormal) * vertexNormal);
    gl_Position  = mvp * vec4(vertexPosition, 1.0);
}
)";

// ─── Embedded GLSL — fragment shader ─────────────────────────────────────────
//
// Supports point, directional and spot lights via parallel uniform arrays.
// Lighting model: Blinn-Phong with squared-distance attenuation.

static const char* kLitFS = R"(
#version 330

#define MAX_LIGHTS 8

in vec3 fragPos;
in vec2 fragTexCoord;
in vec4 fragColor;
in vec3 fragNormal;

uniform sampler2D texture0;
uniform vec4      colDiffuse;

uniform vec3  viewPos;
uniform vec3  ambientColor;
uniform float ambientIntensity;

// Parallel arrays — one element per light slot (always MAX_LIGHTS elements)
uniform int   lightEnabled[MAX_LIGHTS];
uniform int   lightType[MAX_LIGHTS];       // 0=point  1=directional  2=spot
uniform vec3  lightPos[MAX_LIGHTS];
uniform vec3  lightDir[MAX_LIGHTS];        // normalised world-space direction
uniform vec3  lightColor[MAX_LIGHTS];      // linear 0-1
uniform float lightIntensity[MAX_LIGHTS];
uniform float lightRange[MAX_LIGHTS];
uniform float lightInnerCos[MAX_LIGHTS];   // cos(innerAngle)
uniform float lightOuterCos[MAX_LIGHTS];   // cos(outerAngle)

out vec4 finalColor;

// ── Point light ──────────────────────────────────────────────────────────────
vec3 CalcPointLight(int i, vec3 N, vec3 V)
{
    vec3  L   = normalize(lightPos[i] - fragPos);
    float d   = length(lightPos[i] - fragPos);
    float att = clamp(1.0 - (d / lightRange[i]) * (d / lightRange[i]), 0.0, 1.0);
    float diff = max(dot(N, L), 0.0);
    vec3  H    = normalize(L + V);
    float spec = pow(max(dot(N, H), 0.0), 32.0) * 0.3;
    return lightColor[i] * lightIntensity[i] * att * (diff + spec);
}

// ── Directional light ─────────────────────────────────────────────────────────
vec3 CalcDirLight(int i, vec3 N, vec3 V)
{
    vec3  L    = normalize(-lightDir[i]);
    float diff = max(dot(N, L), 0.0);
    vec3  H    = normalize(L + V);
    float spec = pow(max(dot(N, H), 0.0), 32.0) * 0.3;
    return lightColor[i] * lightIntensity[i] * (diff + spec);
}

// ── Spot light ────────────────────────────────────────────────────────────────
vec3 CalcSpotLight(int i, vec3 N, vec3 V)
{
    vec3  L    = normalize(lightPos[i] - fragPos);
    float d    = length(lightPos[i] - fragPos);
    float att  = clamp(1.0 - (d / lightRange[i]) * (d / lightRange[i]), 0.0, 1.0);
    float cosA = dot(L, normalize(-lightDir[i]));
    float eps  = lightInnerCos[i] - lightOuterCos[i];
    float spot = clamp((cosA - lightOuterCos[i]) / eps, 0.0, 1.0);
    float diff = max(dot(N, L), 0.0);
    vec3  H    = normalize(L + V);
    float spec = pow(max(dot(N, H), 0.0), 32.0) * 0.3;
    return lightColor[i] * lightIntensity[i] * att * spot * (diff + spec);
}

void main()
{
    vec4 texColor  = texture(texture0, fragTexCoord);
    vec4 baseColor = texColor * colDiffuse * fragColor;

    vec3 N = normalize(fragNormal);
    vec3 V = normalize(viewPos - fragPos);

    vec3 result = ambientColor * ambientIntensity;

    for (int i = 0; i < MAX_LIGHTS; i++) {
        if (lightEnabled[i] == 0) continue;
        if      (lightType[i] == 0) result += CalcPointLight(i, N, V);
        else if (lightType[i] == 1) result += CalcDirLight(i, N, V);
        else if (lightType[i] == 2) result += CalcSpotLight(i, N, V);
    }

    finalColor   = vec4(result, 1.0) * baseColor;
    finalColor.a = baseColor.a;
}
)";

// ─── Implementation ───────────────────────────────────────────────────────────

namespace Hotones::GFX {

LightingSystem& LightingSystem::Get()
{
    static LightingSystem s_instance;
    return s_instance;
}

bool LightingSystem::Init()
{
    if (m_ready) return true;

    m_shader = LoadShaderFromMemory(kLitVS, kLitFS);
    if (m_shader.id == 0) {
        TraceLog(LOG_ERROR, "LightingSystem: failed to compile lighting shader");
        return false;
    }

    CacheLocations();
    m_ready = true;
    TraceLog(LOG_INFO, "LightingSystem: ready (MAX_LIGHTS=%d)", MAX_LIGHTS);
    return true;
}

void LightingSystem::Unload()
{
    if (!m_ready) return;
    UnloadShader(m_shader);
    m_shader = {};
    m_ready  = false;
}

void LightingSystem::CacheLocations()
{
    m_locs.viewPos          = GetShaderLocation(m_shader, "viewPos");
    m_locs.ambientColor     = GetShaderLocation(m_shader, "ambientColor");
    m_locs.ambientIntensity = GetShaderLocation(m_shader, "ambientIntensity");

    // Uniform arrays: query the base location via the [0] element
    m_locs.lightEnabled   = GetShaderLocation(m_shader, "lightEnabled[0]");
    m_locs.lightType      = GetShaderLocation(m_shader, "lightType[0]");
    m_locs.lightPos       = GetShaderLocation(m_shader, "lightPos[0]");
    m_locs.lightDir       = GetShaderLocation(m_shader, "lightDir[0]");
    m_locs.lightColor     = GetShaderLocation(m_shader, "lightColor[0]");
    m_locs.lightIntensity = GetShaderLocation(m_shader, "lightIntensity[0]");
    m_locs.lightRange     = GetShaderLocation(m_shader, "lightRange[0]");
    m_locs.lightInnerCos  = GetShaderLocation(m_shader, "lightInnerCos[0]");
    m_locs.lightOuterCos  = GetShaderLocation(m_shader, "lightOuterCos[0]");
}

int LightingSystem::AddLight(LightType type, Vector3 pos, Vector3 dir,
                              Vector3 colorLinear, float intensity, float range)
{
    for (int i = 0; i < MAX_LIGHTS; i++) {
        if (!m_lights[i].enabled) {
            m_lights[i].type      = type;
            m_lights[i].enabled   = true;
            m_lights[i].position  = pos;
            m_lights[i].direction = dir;
            m_lights[i].color     = colorLinear;
            m_lights[i].intensity = intensity;
            m_lights[i].range     = range;
            return i + 1;   // 1-based handle
        }
    }
    TraceLog(LOG_WARNING, "LightingSystem::AddLight: all %d slots occupied", MAX_LIGHTS);
    return 0;
}

void LightingSystem::RemoveLight(int handle)
{
    if (!IsValidHandle(handle)) return;
    m_lights[handle - 1] = LightDesc{};  // resets enabled = false
}

bool LightingSystem::IsValidHandle(int handle) const
{
    return handle >= 1 && handle <= MAX_LIGHTS;
}

LightDesc* LightingSystem::GetLight(int handle)
{
    if (!IsValidHandle(handle)) return nullptr;
    return &m_lights[handle - 1];
}

void LightingSystem::SetAmbient(Vector3 colorLinear, float intensity)
{
    m_ambientColor     = colorLinear;
    m_ambientIntensity = intensity;
}

void LightingSystem::UploadUniforms(Camera camera)
{
    if (!m_ready) return;

    // View position
    SetShaderValue(m_shader, m_locs.viewPos, &camera.position, SHADER_UNIFORM_VEC3);

    // Ambient
    SetShaderValue(m_shader, m_locs.ambientColor,     &m_ambientColor,     SHADER_UNIFORM_VEC3);
    SetShaderValue(m_shader, m_locs.ambientIntensity, &m_ambientIntensity, SHADER_UNIFORM_FLOAT);

    // Pack per-light data into flat arrays for batch upload
    int   enabled[MAX_LIGHTS]   = {};
    int   type[MAX_LIGHTS]      = {};
    float pos[MAX_LIGHTS * 3]   = {};
    float dir[MAX_LIGHTS * 3]   = {};
    float color[MAX_LIGHTS * 3] = {};
    float intensity[MAX_LIGHTS] = {};
    float range[MAX_LIGHTS]     = {};
    float innerCos[MAX_LIGHTS]  = {};
    float outerCos[MAX_LIGHTS]  = {};

    for (int i = 0; i < MAX_LIGHTS; i++) {
        const LightDesc& l = m_lights[i];
        enabled[i]   = l.enabled ? 1 : 0;
        type[i]      = static_cast<int>(l.type);
        pos[i*3+0]   = l.position.x;
        pos[i*3+1]   = l.position.y;
        pos[i*3+2]   = l.position.z;
        dir[i*3+0]   = l.direction.x;
        dir[i*3+1]   = l.direction.y;
        dir[i*3+2]   = l.direction.z;
        color[i*3+0] = l.color.x;
        color[i*3+1] = l.color.y;
        color[i*3+2] = l.color.z;
        intensity[i] = l.intensity;
        range[i]     = l.range;
        innerCos[i]  = l.innerCos;
        outerCos[i]  = l.outerCos;
    }

    SetShaderValueV(m_shader, m_locs.lightEnabled,   enabled,   SHADER_UNIFORM_INT,   MAX_LIGHTS);
    SetShaderValueV(m_shader, m_locs.lightType,      type,      SHADER_UNIFORM_INT,   MAX_LIGHTS);
    SetShaderValueV(m_shader, m_locs.lightPos,       pos,       SHADER_UNIFORM_VEC3,  MAX_LIGHTS);
    SetShaderValueV(m_shader, m_locs.lightDir,       dir,       SHADER_UNIFORM_VEC3,  MAX_LIGHTS);
    SetShaderValueV(m_shader, m_locs.lightColor,     color,     SHADER_UNIFORM_VEC3,  MAX_LIGHTS);
    SetShaderValueV(m_shader, m_locs.lightIntensity, intensity, SHADER_UNIFORM_FLOAT, MAX_LIGHTS);
    SetShaderValueV(m_shader, m_locs.lightRange,     range,     SHADER_UNIFORM_FLOAT, MAX_LIGHTS);
    SetShaderValueV(m_shader, m_locs.lightInnerCos,  innerCos,  SHADER_UNIFORM_FLOAT, MAX_LIGHTS);
    SetShaderValueV(m_shader, m_locs.lightOuterCos,  outerCos,  SHADER_UNIFORM_FLOAT, MAX_LIGHTS);
}

void LightingSystem::ApplyToModel(Model& model)
{
    if (!m_ready) return;
    for (int i = 0; i < model.materialCount; i++)
        model.materials[i].shader = m_shader;
}

} // namespace Hotones::GFX
