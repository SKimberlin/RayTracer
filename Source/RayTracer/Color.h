#pragma once
#include <glm/glm.hpp>
typedef glm::vec3 color3_t;
typedef glm::vec4 color4_t;
typedef uint32_t rgba_t;


static color4_t RGBAToColor(const rgba_t& rgba) 
{ 
	color4_t color;
	color.r = ((rgba >> 24) & 0xff) / 255.0f;
	color.g = ((rgba >> 16) & 0xff) / 255.0f;
	color.b = ((rgba >> 8) & 0xff) / 255.0f;
	color.a = ((rgba >> 0) & 0xff) / 255.0f;

	return color;
}
static rgba_t ColorToRGBA(const color4_t color)
{
	rgba_t rgba = 0;
	rgba |= uint8_t(glm::clamp(color.r, 0.0f, 1.0f) * 255) << 24;
	rgba |= uint8_t(glm::clamp(color.g, 0.0f, 1.0f) * 255) << 16;
	rgba |= uint8_t(glm::clamp(color.b, 0.0f, 1.0f) * 255) << 8;
	rgba |= uint8_t(glm::clamp(color.a, 0.0f, 1.0f) * 255) << 0;

	return rgba;
}