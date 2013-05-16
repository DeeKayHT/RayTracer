#pragma once
#include "gfx/vec3.h"

using namespace gfx;

class Ray
{
public:
	Vec3f origin;
	Vec3f direction;

public:
	Ray(Vec3f init_origin, Vec3f init_direction);
	~Ray(void);

	void init(Vec3f init_origin, Vec3f init_direction);
};
