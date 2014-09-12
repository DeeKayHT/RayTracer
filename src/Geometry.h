#pragma once
#include "Ray.h"

class Geometry
{
public:
	Vec3f position;
	Vec3f color;

	float diffuse_coeff;
	float specular_coeff;
	float reflect_coeff;
	float refract_coeff;
	float refract_index;

	bool is_reflecting;
	
	// debug flag - adds a checkerboard pattern to the Geometry object
	bool checkerboard;

public:
	Geometry(void);
	~Geometry(void);

	virtual bool intersect_ray(Ray &ray, Vec3f &intersection_pt, int &result) { return false; }
	virtual float get_diffuse(Vec3f &light_pos, Vec3f &intersect_pt) { return 0.0f; }
	virtual float get_specular(Ray &view_ray, Vec3f &light_pos, Vec3f &intersect_pt) { return 0.0f; }
	virtual Vec3f get_reflection(Ray &view_ray, Vec3f &intersect_pt) { return Vec3f (0.0f); }
	virtual Vec3f get_normal(Vec3f &intersect_pt) { return Vec3f (0.0f); }
};
