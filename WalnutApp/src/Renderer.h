#pragma once

#include <memory>

#include "Walnut/Image.h"
#include "glm/glm.hpp"

#include "Camera.h"
#include "Ray.h"
#include "Scene.h"

class Renderer
{
private:
    std::shared_ptr<Walnut::Image> m_FinalImage;
    uint32_t* m_ImageData = nullptr;
    uint32_t m_FrameIndex = 1;
    const Scene* m_ActiveScene = nullptr;
    const Camera* m_ActiveCamera = nullptr;
    glm::vec4* m_AccumulationData = nullptr;
    std::vector<uint16_t> m_ImageHorizontalIter, m_ImageVerticalIter;

    struct HitPayload
    {
        float HitDistance;
        glm::vec3 WorldPosition;
        glm::vec3 WorldNormal;

        int ObjectIndex;
    };

    glm::vec4 PerPixel(uint32_t x, uint32_t y); //RayGen Shader

    HitPayload TraceRay(const Ray& ray);
    HitPayload ClosestHit(const Ray& ray, float hitDistance, int objectIndex) const;

    HitPayload Miss(const Ray& ray);
public:
    Renderer() = default;

    struct Settings
    {
        bool Accumulate = true;
    };

    void Render(const Scene& scene, const Camera& camera);
    void OnResize(uint32_t width, uint32_t height);

    void ResetFrameIndex() { m_FrameIndex = 1; }
    Settings& GetSettings() { return m_Settings; }
    std::shared_ptr<Walnut::Image> GetFinalImage() const { return m_FinalImage; }
private:
    Settings m_Settings;

};

// #include "Renderer.cpp"
