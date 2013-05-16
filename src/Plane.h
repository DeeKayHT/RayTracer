#pragma once
#include "Geometry.h"

using namespace gfx;

class Plane : public Geometry
{
private:
	Vec3f normal;

public:
	Plane(Vec3f init_norm, Vec3f init_pos, COLORREF init_color, bool reflect = true);
	~Plane(void);

	void init(Vec3f init_norm, Vec3f init_pos, COLORREF init_color, bool reflect);
	
	virtual bool intersect_ray(Ray &ray, Vec3f &intersect_pt, int &result);
	
	virtual float get_diffuse(Vec3f &light_pos, Vec3f &intersect_pt);
	virtual float get_specular(Ray &view_ray, Vec3f &light_pos, Vec3f &intersect_pt);
	virtual Vec3f get_reflection(Ray &view_ray, Vec3f &intersect_pt);
	virtual Vec3f get_normal(Vec3f &intersect_pt);
};
