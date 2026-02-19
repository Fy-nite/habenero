// Lighting.cpp — Lua bindings for LightingSystem
//
// Exposes the "lighting" global table to Lua scripts:
//
//   lighting.POINT        = 0
//   lighting.DIRECTIONAL  = 1
//   lighting.SPOT         = 2
//
//   lighting.setAmbient(r, g, b [, intensity])
//   handle = lighting.add(type, x, y, z, r, g, b [, intensity, range])
//   lighting.remove(handle)
//   lighting.setPos(handle, x, y, z)
//   lighting.setDir(handle, x, y, z)
//   lighting.setColor(handle, r, g, b)
//   lighting.setIntensity(handle, value)
//   lighting.setRange(handle, value)
//   lighting.setEnabled(handle, bool)
//
// Colour parameters (r, g, b) use the same 0-255 integer convention as the
// rest of the engine's Lua API.  They are converted to linear 0-1 internally.

#include <lua.hpp>
#include "../../include/Scripting/LuaLoader/Lighting.hpp"
#include "../../include/GFX/LightingSystem.hpp"
#include <cmath>

namespace Hotones::Scripting::LuaLoader {

// Helper: convert 0-255 integer to linear 0-1 float
static inline float c255(lua_State* L, int idx, lua_Number def = 255.0)
{
    return (float)luaL_optnumber(L, idx, def) / 255.f;
}

// lighting.setAmbient(r, g, b [, intensity])
static int l_setAmbient(lua_State* L)
{
    float r = c255(L, 1, 255);
    float g = c255(L, 2, 255);
    float b = c255(L, 3, 255);
    float i = (float)luaL_optnumber(L, 4, 0.15);
    GFX::LightingSystem::Get().SetAmbient({r, g, b}, i);
    return 0;
}

// handle = lighting.add(type, x, y, z, r, g, b [, intensity, range])
static int l_add(lua_State* L)
{
    auto  type      = static_cast<GFX::LightType>((int)luaL_checkinteger(L, 1));
    float x         = (float)luaL_checknumber(L, 2);
    float y         = (float)luaL_checknumber(L, 3);
    float z         = (float)luaL_checknumber(L, 4);
    float r         = c255(L, 5, 255);
    float g         = c255(L, 6, 255);
    float b         = c255(L, 7, 255);
    float intensity = (float)luaL_optnumber(L, 8, 1.0);
    float range     = (float)luaL_optnumber(L, 9, 20.0);

    int handle = GFX::LightingSystem::Get().AddLight(
        type, {x, y, z}, {0.f, -1.f, 0.f}, {r, g, b}, intensity, range);

    lua_pushinteger(L, handle);
    return 1;
}

// lighting.remove(handle)
static int l_remove(lua_State* L)
{
    int h = (int)luaL_checkinteger(L, 1);
    GFX::LightingSystem::Get().RemoveLight(h);
    return 0;
}

// lighting.setPos(handle, x, y, z)
static int l_setPos(lua_State* L)
{
    int   h = (int)luaL_checkinteger(L, 1);
    float x = (float)luaL_checknumber(L, 2);
    float y = (float)luaL_checknumber(L, 3);
    float z = (float)luaL_checknumber(L, 4);
    auto* light = GFX::LightingSystem::Get().GetLight(h);
    if (light) light->position = {x, y, z};
    return 0;
}

// lighting.setDir(handle, x, y, z)
// Sets the direction vector for directional and spot lights.
static int l_setDir(lua_State* L)
{
    int   h = (int)luaL_checkinteger(L, 1);
    float x = (float)luaL_checknumber(L, 2);
    float y = (float)luaL_checknumber(L, 3);
    float z = (float)luaL_checknumber(L, 4);
    auto* light = GFX::LightingSystem::Get().GetLight(h);
    if (light) light->direction = {x, y, z};
    return 0;
}

// lighting.setColor(handle, r, g, b)
static int l_setColor(lua_State* L)
{
    int   h = (int)luaL_checkinteger(L, 1);
    float r = c255(L, 2, 255);
    float g = c255(L, 3, 255);
    float b = c255(L, 4, 255);
    auto* light = GFX::LightingSystem::Get().GetLight(h);
    if (light) light->color = {r, g, b};
    return 0;
}

// lighting.setIntensity(handle, value)
static int l_setIntensity(lua_State* L)
{
    int   h = (int)luaL_checkinteger(L, 1);
    float v = (float)luaL_checknumber(L, 2);
    auto* light = GFX::LightingSystem::Get().GetLight(h);
    if (light) light->intensity = v;
    return 0;
}

// lighting.setRange(handle, value)
static int l_setRange(lua_State* L)
{
    int   h = (int)luaL_checkinteger(L, 1);
    float v = (float)luaL_checknumber(L, 2);
    auto* light = GFX::LightingSystem::Get().GetLight(h);
    if (light) light->range = v;
    return 0;
}

// lighting.setEnabled(handle, bool)
static int l_setEnabled(lua_State* L)
{
    int  h = (int)luaL_checkinteger(L, 1);
    bool v = lua_toboolean(L, 2) != 0;
    auto* light = GFX::LightingSystem::Get().GetLight(h);
    if (light) light->enabled = v;
    return 0;
}

// lighting.setSpotAngles(handle, innerDeg, outerDeg)
// Convenience: set spot cone angles in degrees.
static int l_setSpotAngles(lua_State* L)
{
    int   h     = (int)luaL_checkinteger(L, 1);
    float inner = (float)luaL_checknumber(L, 2);
    float outer = (float)luaL_checknumber(L, 3);
    auto* light = GFX::LightingSystem::Get().GetLight(h);
    if (light) {
        light->innerCos = cosf(inner * DEG2RAD);
        light->outerCos = cosf(outer * DEG2RAD);
    }
    return 0;
}

void registerLighting(lua_State* L)
{
    static const luaL_Reg funcs[] = {
        {"setAmbient",    l_setAmbient},
        {"add",           l_add},
        {"remove",        l_remove},
        {"setPos",        l_setPos},
        {"setDir",        l_setDir},
        {"setColor",      l_setColor},
        {"setIntensity",  l_setIntensity},
        {"setRange",      l_setRange},
        {"setEnabled",    l_setEnabled},
        {"setSpotAngles", l_setSpotAngles},
        {nullptr, nullptr}
    };

    luaL_newlib(L, funcs);

    // Light-type constants
    lua_pushinteger(L, static_cast<int>(GFX::LightType::Point));       lua_setfield(L, -2, "POINT");
    lua_pushinteger(L, static_cast<int>(GFX::LightType::Directional)); lua_setfield(L, -2, "DIRECTIONAL");
    lua_pushinteger(L, static_cast<int>(GFX::LightType::Spot));        lua_setfield(L, -2, "SPOT");

    lua_setglobal(L, "lighting");
}

} // namespace Hotones::Scripting::LuaLoader
