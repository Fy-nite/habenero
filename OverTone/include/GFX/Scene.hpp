#pragma once

#include <raylib.h>
#include <memory>

namespace Hotones {

class Scene {
public:
    virtual ~Scene() = default;
    virtual void Init() {}
    virtual void Update() = 0;  // update logic (called before Draw)
    virtual void Draw3D() {}    // 3-D pass — called INSIDE BeginMode3D / EndMode3D
    virtual void Draw() = 0;    // 2-D / HUD pass — called OUTSIDE 3D mode
    virtual void Unload() {}

    // Return the camera this scene wants to use for the 3-D pass.
    // Return nullptr (default) to skip the Draw3D() call for this scene
    // (e.g. GameScene manages its own BeginMode3D inside Draw()).
    virtual Camera3D* GetCamera() { return nullptr; }

    bool IsFinished() const { return finished; }
protected:
    void MarkFinished() { finished = true; }

private:
    bool finished = false;
};

} // namespace Hotones
