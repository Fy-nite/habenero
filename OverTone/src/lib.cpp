
#include "Draw3D/Draw3D.hpp"
#include "GFX/Player.hpp"
#include "Input/Input.hpp"
#include <Habenero.hpp>
#include <cstdio>
#include <cstdlib>
#include <list>
#include <raylib.h>
#include <tuple>
extern "C" {
#ifdef _WIN32
#define EXPORT __declspec(dllexport)
#else
#define EXPORT
#endif

using namespace Hotones;
const std::list<std::tuple<Vector3, Vector3>> boxes = {
    *new std::tuple<Vector3, Vector3>(*new Vector3(0, 0, 0),
                                      *new Vector3(5, 0.1, 5))};
EXPORT core::ModuleState *CreateGame() {
  core::ModuleState *s = new core::ModuleState();
  return s;
}

EXPORT void DestroyGame(void *instance) {
  if (!instance)
    return;
  core::ModuleState *s = static_cast<core::ModuleState *>(instance);
  delete s;
}

EXPORT void UpdateGame(void *instance, float dt) {
  if (!instance)
    return;
  core::ModuleState *s = static_cast<core::ModuleState *>(instance);
  s->time += dt;
  s->frames += 1;

}

EXPORT void DrawGame(core::ModuleState *instance) {
  if (!instance)
    return;


  Hotones::Draw3D::Axes();
  for (auto box : boxes) {

    Hotones::Draw3D::Box(std::get<0>(box).x, std::get<0>(box).y,
                         std::get<0>(box).z, std::get<1>(box).x,
                         std::get<1>(box).y, std::get<1>(box).z);
  }
  // 	std::printf("[NativeGame] time=%.3f frames=%d\n", s->time, s->frames);
  // 	std::fflush(stdout);
}
}
