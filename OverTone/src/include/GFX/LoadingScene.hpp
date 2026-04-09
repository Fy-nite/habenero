#pragma once

#include <GFX/Scene.hpp>
#include <raylib.h>
#include <functional>
#include <string>

#define LOADING_STAR_COUNT 420

namespace Hotones {

class LoadingScene : public Scene {
public:
    // Optional progress callback returns value in [0,1].  Optional error
    // callback returns a non-empty string when an error occurred.
    LoadingScene(float durationSeconds = 3.0f,
                 std::function<float()> progressCb = nullptr,
                 std::function<std::string()> errorCb = nullptr);
    virtual ~LoadingScene() override = default;

    void Init() override;
    void Update() override;
    void Draw() override;
    void Unload() override;

private:
    Vector3 stars[LOADING_STAR_COUNT];
    Vector2 starsScreenPos[LOADING_STAR_COUNT];
    Color bgColor;
    float speed;
    bool drawLines;
    float elapsed;
    float duration;
    std::function<float()> m_progressCb;
    std::function<std::string()> m_errorCb;
};

} // namespace Hotones
