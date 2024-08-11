#pragma once

#include <memory>

#include "Walnut/Image.h"
#include "glm/glm.hpp"



class Renderer
{
private:
    std::shared_ptr<Walnut::Image> m_FinalImage;
    uint32_t* m_ImageData = nullptr;

    glm::vec4 PerPixel(glm::vec2 coord);
public:
    Renderer() = default;
    ~Renderer() = default;

    void Render();
    void OnResize(uint32_t width, uint32_t height);

    std::shared_ptr<Walnut::Image> GetFinalImage() const { return m_FinalImage; }
};

// #include "Renderer.cpp"
