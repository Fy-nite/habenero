#pragma once

#ifdef _WIN32
#define NATIVE_GAME_API __declspec(dllexport)
#else
#define NATIVE_GAME_API
#endif

extern "C" {
    // Minimal required exported functions from a native game module.
    // CreateGame returns an opaque pointer to module state.
    typedef void* (*CreateGameFn)();
    typedef void  (*DestroyGameFn)(void* instance);
    typedef void  (*UpdateGameFn)(void* instance, float dt);
    typedef void  (*DrawGameFn)(void* instance);
}
