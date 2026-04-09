
#include <cstdio>
#include <cstdlib>

extern "C" {
#ifdef _WIN32
#define EXPORT __declspec(dllexport)
#else
#define EXPORT
#endif

	struct ModuleState {
		float time = 0.0f;
		int frames = 0;
	};

	EXPORT void* CreateGame()
	{
		ModuleState* s = new ModuleState();
		return static_cast<void*>(s);
	}

	EXPORT void DestroyGame(void* instance)
	{
		if (!instance) return;
		ModuleState* s = static_cast<ModuleState*>(instance);
		delete s;
	}

	EXPORT void UpdateGame(void* instance, float dt)
	{
		if (!instance) return;
		ModuleState* s = static_cast<ModuleState*>(instance);
		s->time += dt;
		s->frames += 1;
	}

	EXPORT void DrawGame(void* instance)
	{
		if (!instance) return;
		ModuleState* s = static_cast<ModuleState*>(instance);
		// Minimal demonstration output; engines can capture stdout or replace with engine logging
		std::printf("[NativeGame] time=%.3f frames=%d\n", s->time, s->frames);
		std::fflush(stdout);
	}

}

