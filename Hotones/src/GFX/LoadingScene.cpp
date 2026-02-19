#include <GFX/LoadingScene.hpp>
#include <raymath.h>

namespace Hotones {

LoadingScene::LoadingScene(float durationSeconds, std::function<float()> progressCb, std::function<std::string()> errorCb)
    : bgColor(ColorLerp(DARKBLUE, BLACK, 0.69f)), speed(10.0f/9.0f), drawLines(true), elapsed(0.0f), duration(durationSeconds), m_progressCb(progressCb), m_errorCb(errorCb)
{
}

void LoadingScene::Init()
{
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();
    for (int i = 0; i < LOADING_STAR_COUNT; i++)
    {
        stars[i].x = GetRandomValue(-screenWidth*0.5f, screenWidth*0.5f);
        stars[i].y = GetRandomValue(-screenHeight*0.5f, screenHeight*0.5f);
        stars[i].z = 1.0f;
        starsScreenPos[i] = {0,0};
    }
}

void LoadingScene::Update()
{
    float dt = GetFrameTime();
    elapsed += dt;

    // Update speed via mouse wheel (optional)
    // float mouseMove = GetMouseWheelMove();
    // if ((int)mouseMove != 0) speed += 2.0f*mouseMove/9.0f;
    // if (speed < 0.0f) speed = 0.1f;
    // else if (speed > 2.0f) speed = 2.0f;

    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();

    for (int i = 0; i < LOADING_STAR_COUNT; i++)
    {
        stars[i].z -= dt*speed;

        starsScreenPos[i] = (Vector2){
            screenWidth*0.5f + stars[i].x/stars[i].z,
            screenHeight*0.5f + stars[i].y/stars[i].z,
        };

        if ((stars[i].z < 0.0f) || (starsScreenPos[i].x < 0) || (starsScreenPos[i].y < 0.0f) ||
            (starsScreenPos[i].x > screenWidth) || (starsScreenPos[i].y > screenHeight))
        {
            stars[i].x = GetRandomValue(-screenWidth*0.5f, screenWidth*0.5f);
            stars[i].y = GetRandomValue(-screenHeight*0.5f, screenHeight*0.5f);
            stars[i].z = 1.0f;
        }
    }

    // // Toggle mode
    // if (IsKeyPressed(KEY_SPACE)) drawLines = !drawLines;

    // Finish scene after duration
    if (elapsed >= duration) MarkFinished();
}

void LoadingScene::Draw()
{
    ClearBackground(bgColor);

    for (int i = 0; i < LOADING_STAR_COUNT; i++)
    {
        if (drawLines)
        {
            float t = Clamp(stars[i].z + 1.0f/32.0f, 0.0f, 1.0f);
            if ((t - stars[i].z) > 1e-3)
            {
                Vector2 startPos = (Vector2){
                    GetScreenWidth()*0.5f + stars[i].x/t,
                    GetScreenHeight()*0.5f + stars[i].y/t,
                };
                DrawLineV(startPos, starsScreenPos[i], RAYWHITE);
            }
        }
        else
        {
            float radius = Lerp(stars[i].z, 1.0f, 5.0f);
            DrawCircleV(starsScreenPos[i], radius, RAYWHITE);
        }
    }

    DrawText("Loading Game, please wait.", 10, 40, 20, RAYWHITE);
    // DrawText("", 10, 70, 20, RAYWHITE);
    DrawFPS(10, 10);

    // Draw a simple progress indicator — prefer external progressCb if set
    float t = 0.0f;
    if (m_progressCb) {
        t = m_progressCb();
    } else {
        t = elapsed / duration;
    }
    if (t < 0.0f) t = 0.0f; if (t > 1.0f) t = 1.0f;
    DrawRectangle(10, GetScreenHeight() - 30, (int)((GetScreenWidth()-20)*t), 16, GREEN);
    DrawRectangleLines(10, GetScreenHeight() - 30, GetScreenWidth()-20, 16, WHITE);

    // If an error callback is provided and returns non-empty, show it here
    if (m_errorCb) {
        std::string err = m_errorCb();
        if (!err.empty()) {
            DrawTextEx(GetFontDefault(), "Error:", {10, 70}, 18, 0, RED);
            DrawText(err.c_str(), 10, 90, 16, RAYWHITE);
        }
    }
}

void LoadingScene::Unload()
{
    // nothing to free for now
}

} // namespace Hotones
