#pragma once

#include <GFX/Scene.hpp>
#include <GFX/UIManager.hpp>
#include <string>

namespace Hotones {

class BuiltInScene : public Scene {
public:
    explicit BuiltInScene(std::string name) : m_name(std::move(name)) {}
    void Init() override {}
    void Update() override;
    void Draw3D() override {}
    void Draw() override;
    void Unload() override {}
private:
    std::string m_name;
};

} // namespace Hotones
