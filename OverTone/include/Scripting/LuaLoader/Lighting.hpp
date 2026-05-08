#pragma once

struct lua_State;

namespace Hotones::Scripting::LuaLoader {

// Register the "lighting" Lua global table into the given Lua state.
// Provides runtime control over the LightingSystem singleton.
void registerLighting(lua_State* L);

} // namespace Hotones::Scripting::LuaLoader
