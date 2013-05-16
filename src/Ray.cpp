#include "Ray.h"

Ray::Ray(Vec3f init_origin, Vec3f init_direction)
{
	init(init_origin, init_direction);
}

Ray::~Ray(void)
{
}

void Ray::init(Vec3f init_origin, Vec3f init_direction)
{
	origin = init_origin;
	direction = init_direction;
}