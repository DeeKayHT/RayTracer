#pragma once
#include "Color.h"
#include "gfx/vec3.h"

using namespace gfx;

enum Light_type {
	LIGHT_POINT,
	LIGHT_BOX,
	
	NUM_LIGHT_TYPES
};

class Light
{
public:
	Vec3f position;
	Vec3f color;
	Light_type type;

public:
	Light();
	~Light(void);
	
	Light(float cx, float cy, float cz, Color init_color);

	void init(float cx, float cy, float cz, Color init_color);
};

class LightBox : public Light
{
public:
	// Size of the box light acts like horizontal/vertical radius
	float size;

public:
	LightBox(float cx, float cy, float cz, float size, Color init_color);
	~LightBox(void);

	void init(float cx, float cy, float cz, float init_size, Color init_color);
};