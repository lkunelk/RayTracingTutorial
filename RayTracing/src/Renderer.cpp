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
			m_ImageData[x + y * m_FinalImage->GetWidth()] = PerPixel(coord); // pixel format ABGR
		}
	}
	m_FinalImage->SetData(m_ImageData); // send to GPU
}

uint32_t Renderer::PerPixel(glm::vec2 coord)
{
	uint8_t red = (uint8_t)(coord.x * 255);
	uint8_t green = (uint8_t)(coord.y * 255);

	return 0xff000000 | (green << 8) | red;
}
