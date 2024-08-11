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
}

void Renderer::Render()
{
    //  render every pixel
    for (uint32_t y = 0; y < m_FinalImage->GetHeight(); y++)
    {
        for (uint32_t x = 0; x < m_FinalImage->GetWidth(); x++)
        {
            glm::vec2 coord = { x / (float)m_FinalImage->GetWidth(), y / (float)m_FinalImage->GetHeight() };
            coord = coord * 2.0f - 1.0f; // -1 -> 1 coordinates. (-1, -1), (-1, 1), (1, -1), (1, 1)

            coord.x *= (float)m_FinalImage->GetWidth() / (float)m_FinalImage->GetHeight(); //keep aspect ratio when image is resized
            glm::vec4 color = PerPixel(coord);
            color = glm::clamp(color, glm::vec4(0.0f), glm::vec4(1.0f));
            m_ImageData[x + y * m_FinalImage->GetWidth()] = Utils::ConvertToRGBA(color);
	    }
    }

	m_FinalImage->SetData(m_ImageData);
}

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
}

// This is where the colors are determined
glm::vec4 Renderer::PerPixel(glm::vec2 coord)
{
    //convert 2D coords into 3D
    glm::vec3 rayOrigin(0.0f, 0.0f, 1.0f);
    glm::vec3 rayDirection(coord.x, coord.y, -1.0f);
    float radius = 0.5f;
    // make a unit vector
    // rayDirection = glm::normalize(rayDirection);

    // Equation for sphere in 3D space
    // (bx^2 + by^2 + bz^2)t^2 + (2(axbx + ayby + azbz))t + (ax^2 + ay^2 + az^2 -r^2) = 0
    // a = ray origin
    // b = ray direction
    // r = radius
    // t = hit distance
    // x, y, z = coordinates

    // Note: a, b, and c variables represent terms in the equation above
    // e.g. a + b + c = 0
    
    // first term (bx^2 + by^2 + bz^2)t^2
    float a = glm::dot(rayDirection, rayDirection); // dot product
    // middle term (2(axbx + ayby + azbz))t
    float b = 2.0f * glm::dot(rayOrigin, rayDirection);
    // last term (ax^2 + ay^2 + az^2 -r^2)
    float c = glm::dot(rayOrigin, rayOrigin) - radius * radius;

    // Use Quadratic discriminant to detect intersection with sphere
    // b^2 - 4ac
    float discriminant = b * b - 4.0f * a * c;

    if (discriminant < 0.0f)
        return glm::vec4(0, 0, 0, 1);

    // Solving Quadratic equation (hit distance)
    // (-b +- sqrt(discriminant)) / 2a
    // float t0 = (-b + glm::sqrt(discriminant)) / (2.0f * a);
    float closestT = (-b - glm::sqrt(discriminant)) / (2.0f * a);

    glm::vec3 hitPoint = rayOrigin + rayDirection * closestT; // t1 will always be closest hit
    glm::vec3 normal = glm::normalize(hitPoint); // Don't need to subtract sphere origin because its (0, 0)

    glm::vec3 lightDir = glm::normalize(glm::vec3(-1, -1, -1));

    float facingLightRatio = glm::max(glm::dot(normal, -lightDir), 0.0f); // == cos(angle) | cos(> 90) == neg, so we max this

    glm::vec3 sphereColor(1, 0, 1);
    sphereColor *= facingLightRatio;

    return glm::vec4(sphereColor, 1.0f);
}
