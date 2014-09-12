#pragma once
#include "Geometry.h"
#include "Color.h"

using namespace gfx;

class Sphere : public Geometry
{
private:
	float radius;

public:
	Sphere(float cx, float cy, float cz, float init_radius, Color init_color,
			float spec_coeff, float init_refract, bool reflect = true);
	~Sphere(void);

	void init(float cx, float cy, float cz, float init_radius, Color init_color, 
				float spec_coeff, float refract, bool reflect);
				
	virtual bool intersect_ray(Ray &ray, Vec3f &intersect_pt, int &result);
	
	virtual float get_diffuse(Vec3f &light_pos, Vec3f &intersect_pt);
	virtual float get_specular(Ray &view_ray, Vec3f &light_pos, Vec3f &intersect_pt);
	virtual Vec3f get_reflection(Ray &view_ray, Vec3f &intersect_pt);
	virtual Vec3f get_normal(Vec3f &intersect_pt);
};
