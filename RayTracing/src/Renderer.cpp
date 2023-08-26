#include "Renderer.h"

#include "Walnut/Random.h"

void Renderer::OnResize(uint32_t width, uint32_t height)
{
	if (m_FinalImage)
	{
		if (m_FinalImage->GetWidth() == width && m_FinalImage->GetHeight() == height)
			return;

		m_FinalImage->Resize(width, height);
	}
	else
	{
		m_FinalImage = std::make_shared<Walnut::Image>(width, height, Walnut::ImageFormat::RGBA);
	}

	delete[] m_ImageData; // delete null object is ok
	m_ImageData = new uint32_t[width * height];
}

void Renderer::Render()
{
	for (uint32_t y = 0; y < m_FinalImage->GetHeight(); y++) // start with y on outside to be friendly to cache, makes things contiguous?
	{
		for (uint32_t x = 0; x < m_FinalImage->GetWidth(); x++)
		{
			glm::vec2 coord = {(float)x / (float)m_FinalImage->GetWidth(), (float)y / (float)m_FinalImage->GetHeight()};
			coord = coord * 2.0f - 1.0f; // range 1 to -1
			m_ImageData[x + y * m_FinalImage->GetWidth()] = PerPixel(coord); // pixel format ABGR
		}
	}
	m_FinalImage->SetData(m_ImageData); // send to GPU
}

uint32_t Renderer::PerPixel(glm::vec2 coord)
{
	// this will eventually run on shader for speed up
	uint8_t red = (uint8_t)((coord.x + 1.0f) / 2.0f * 255);
	uint8_t green = (uint8_t)((coord.y + 1.0f) / 2.0f * 255);

	glm::vec3 rayOrigin(0.0f, 0.0f, 2.0f);
	glm::vec3 rayDirection(coord.x, coord.y, -1);
	float radius = 0.5f;


	// (dir_x^2 + dir_y^2)t^2 + 2(orig_x*dir_x + origin_y*dir_y)t + (origin_x^2 + origin_y^2 - radius^2) = 0
	// solving for t (distance along the ray)
	// origin (ray origin), dir (ray direction), radius (circle radius) uknown variables
	// coord is ray direction

	float a = glm::dot(rayDirection, rayDirection);
	float b = 2.0f * glm::dot(rayDirection, rayOrigin);
	float c = glm::dot(rayOrigin, rayOrigin) - radius * radius;

	// quadratic discriminant: tells us if ray intersects sphere
	// b^2 - 4ac

	float discriminant = b * b - 4.0f * a * c;

	if (discriminant >= 0)
		return 0xff000000 | (green << 8) | red;
	else
		return 0x00000000;
}
