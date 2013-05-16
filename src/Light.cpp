#include "Light.h"

Light::Light()
{
}

Light::~Light(void)
{
}

Light::Light(float cx, float cy, float cz, COLORREF init_color)
{
	init(cx, cy, cz, init_color);
}


// Initialize a light (defaults as point light)
//
void Light::init(float cx, float cy, float cz, COLORREF init_color)
{
	position[0] = cx;
	position[1] = cy;
	position[2] = cz;

	color[0] = ((float)GetRValue(init_color)) / 255.0f;
	color[1] = ((float)GetGValue(init_color)) / 255.0f;
	color[2] = ((float)GetBValue(init_color)) / 255.0f;

	type = LIGHT_POINT;
}


LightBox::LightBox(float cx, float cy, float cz, float size, COLORREF init_color)
{
	init(cx, cy, cz, size, init_color);
}

LightBox::~LightBox(void)
{
}

// Initialize a 2D box light (aligned with x-z plane)
//
void LightBox::init(float cx, float cy, float cz, float init_size, COLORREF init_color)
{
	position[0] = cx;
	position[1] = cy;
	position[2] = cz;

	color[0] = ((float)GetRValue(init_color)) / 255.0f;
	color[1] = ((float)GetGValue(init_color)) / 255.0f;
	color[2] = ((float)GetBValue(init_color)) / 255.0f;

	size = init_size;

	type = LIGHT_BOX;
}