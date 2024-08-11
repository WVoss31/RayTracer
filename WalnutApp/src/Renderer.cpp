#include "Renderer.h"
#include "Walnut/Random.h"

namespace Utils {
    static uint32_t ConvertToRGBA(const glm::vec4& color)
    {
        uint8_t r = (u_int8_t)(color.r * 255.0f);
        uint8_t g = (u_int8_t)(color.g * 255.0f);
        uint8_t b = (u_int8_t)(color.b * 255.0f);
        uint8_t a = (u_int8_t)(color.a * 255.0f);

        uint32_t result = (a << 24) | (b << 16) | (g << 8) | r;
        return result;
    }
} // Utils

void Renderer::Render(const Scene& scene, const Camera& camera)
{
    m_ActiveCamera = &camera;
    m_ActiveScene = &scene;

    //  render every pixel
    for (uint32_t y = 0; y < m_FinalImage->GetHeight(); y++)
    {
        for (uint32_t x = 0; x < m_FinalImage->GetWidth(); x++)
        {
            glm::vec4 color = PerPixel(x, y);
            color = glm::clamp(color, glm::vec4(0.0f), glm::vec4(1.0f));
            m_ImageData[x + y * m_FinalImage->GetWidth()] = Utils::ConvertToRGBA(color);
	    }
    }

	m_FinalImage->SetData(m_ImageData);

} // Render

void Renderer::OnResize(uint32_t width, uint32_t height)
{
    if (m_FinalImage)
    {
        // No resize necessary
        if (m_FinalImage->GetWidth() == width && m_FinalImage->GetHeight() == height)
            return;

        m_FinalImage->Resize(width, height);
    }
    else
    {
        m_FinalImage = std::make_shared<Walnut::Image>(width, height, Walnut::ImageFormat::RGBA);
    }

    delete[] m_ImageData;
	m_ImageData = new uint32_t[width * height];

} // OnResize

// This is where the colors are determined
Renderer::HitPayload Renderer::TraceRay(const Ray& ray)
{
    // Equation for sphere in 3D space
    // (bx^2 + by^2 + bz^2)t^2 + (2(axbx + ayby + azbz))t + (ax^2 + ay^2 + az^2 -r^2) = 0
    // a = ray origin
    // b = ray direction
    // r = radius
    // t = hit distance
    // x, y, z = coordinates

    // Note: a, b, and c variables represent terms in the equation above
    // e.g. a + b + c = 0-
    
    int closestSphere = -1;
    float hitDistance = std::numeric_limits<float>::max();

    for (size_t i = 0; i < m_ActiveScene->Spheres.size(); i++)
    {
        const Sphere& sphere = m_ActiveScene->Spheres[i];
        glm::vec3 origin = ray.Origin - sphere.Position;

        // first term (bx^2 + by^2 + bz^2)t^2
        float a = glm::dot(ray.Direction, ray.Direction); // dot product
        // middle term (2(axbx + ayby + azbz))t
        float b = 2.0f * glm::dot(origin, ray.Direction);
        // last term (ax^2 + ay^2 + az^2 -r^2)
        float c = glm::dot(origin, origin) - sphere.Radius * sphere.Radius;

        // Use Quadratic discriminant to detect intersection with sphere
        // b^2 - 4ac
        float discriminant = b * b - 4.0f * a * c;

        if (discriminant < 0.0f)
            continue;

        // Solving Quadratic equation (hit distance)
        // (-b +- sqrt(discriminant)) / 2a
        // float t0 = (-b + glm::sqrt(discriminant)) / (2.0f * a);  CURRENTLY UNUSED
        float closestT = (-b - glm::sqrt(discriminant)) / (2.0f * a);
        if (closestT > 0.0f && closestT < hitDistance)
        {
            hitDistance = closestT;
            closestSphere = (int)i;
        }
    }

    if (closestSphere < 0)
        return Miss(ray);
    
    return ClosestHit(ray, hitDistance, closestSphere);

} // Trace Ray

Renderer::HitPayload Renderer::ClosestHit(const Ray &ray, float hitDistance, int objectIndex)
{
    Renderer::HitPayload payload;
    payload.HitDistance = hitDistance;
    payload.ObjectIndex = objectIndex;

    const Sphere& closestSphere = m_ActiveScene->Spheres[objectIndex];
    glm::vec3 origin = ray.Origin - closestSphere.Position;
    payload.WorldPosition = origin + ray.Direction * hitDistance;
    payload.WorldNormal = glm::normalize(payload.WorldPosition); // Don't need to subtract sphere origin because its (0, 0)

    payload.WorldPosition += closestSphere.Position; // needed because we substract it for the origin

    return payload;
} // Closest Hit

Renderer::HitPayload Renderer::Miss(const Ray &ray)
{
    Renderer::HitPayload payload;
    payload.HitDistance = -1.0f;
    
    return payload;
} // Miss

glm::vec4 Renderer::PerPixel(u_int32_t x, u_int32_t y)
{
    Ray ray;
    ray.Origin = m_ActiveCamera->GetPosition();
    ray.Direction = m_ActiveCamera->GetRayDirections()[x + y * m_FinalImage->GetWidth()];

    glm::vec3 color(0.0f);
    float muliplier = 1.0f;
    
    int bounces = 2;
    for (int i = 0; i < bounces; i++)
    {
        Renderer::HitPayload payload = TraceRay(ray);
        if (payload.HitDistance < 0.0f)
        {
            glm::vec3 skyColor = glm::vec3(0.0f, 0.0f, 0.0f);
            color += skyColor * muliplier;
            break;
        }

        glm::vec3 lightDir = glm::normalize(glm::vec3(-1, -1, -1));

        float lightIntensity = glm::max(glm::dot(payload.WorldNormal, -lightDir), 0.0f); // == cos(angle) | cos(> 90) == neg, so we max this

        const Sphere& sphere = m_ActiveScene->Spheres[payload.ObjectIndex];

        glm::vec3 sphereColor = sphere.Albedo;
        sphereColor *= lightIntensity;
        color += sphereColor * muliplier;

        muliplier *= 0.7f;

        ray.Origin = payload.WorldPosition + payload.WorldNormal * 0.0001f;
        ray.Direction = glm::reflect(ray.Direction, payload.WorldNormal);
    }

    return glm::vec4(color, 1.0f);
} // PerPixel
