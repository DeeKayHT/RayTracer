#include "Plane.h"

Plane::Plane(Vec3f init_norm, Vec3f init_pos, Color init_color, bool reflect)
{
	init(init_norm, init_pos, init_color, reflect);
}

Plane::~Plane(void)
{
}

// Initialize the plane's normal vector, position, color and whether it reflects
//
void Plane::init(Vec3f init_norm, Vec3f init_pos, Color init_color, bool reflect)
{
	normal = init_norm;
	position = init_pos;

	color[0] = (float)init_color.r / 255.0f;
	color[1] = (float)init_color.g / 255.0f;
	color[2] = (float)init_color.b / 255.0f;

	// default values for light coefficient calculations
	diffuse_coeff = 1.0f;
	specular_coeff = 1.0;
	reflect_coeff = 0.8f;
	refract_coeff = 0.0f;
	refract_index = 0.0f;

	is_reflecting = reflect;
	
	// debug flag - if true, applies a checkerboard pattern on the plane
	checkerboard = false;
}

// Checks if the ray intersects the plane, returns true if it intersects, false otherwise
// Output:	intersect_pt - the point where the ray intersects the plane
//			result - 
//
bool Plane::intersect_ray(Ray &ray, Vec3f &intersect_pt, int &result)
{
	/*
	Plane - ray intersection equation (solve for t)
	ray(t) = p + v*t (p = origin, v = direction)
	plane: n . (q - x) = 0 (n = normal, q = point on plane)

	n . (q - r(t)) = 0
	n . (q - p - v*t) = 0
	n . (q - p) = (n . v) * t
	t = [ n . (q - p) ] / (n . v) 
	*/

	// check if ray is parallel to plane
	if (normal * ray.direction == 0.0f) {
		return false;
	}
	
	float t = (normal * (position - ray.origin)) / (normal * ray.direction);
	if (t > 10000.f || t < 0.01f) {
		return false;
	}
	result = 1;

	Vec3f intersect_vec;
	intersect_vec[0] = ray.direction[0] * t;
	intersect_vec[1] = ray.direction[1] * t;
	intersect_vec[2] = ray.direction[2] * t;

	intersect_pt[0] = ray.origin[0] + intersect_vec[0];
	intersect_pt[1] = ray.origin[1] + intersect_vec[1];
	intersect_pt[2] = ray.origin[2] + intersect_vec[2];

    return true; 
}

// Returns the diffuse compopnent of the light at the plane's intersection point
//
float Plane::get_diffuse(Vec3f &light_pos, Vec3f &intersect_pt)
{
	float diffuse = 0.0f;
	
	Vec3f light_vec = light_pos - intersect_pt;
	normalize(light_vec);
	
	float angle = light_vec * normal;
	if (angle < 0.0f) {
		return 0.0f;
	}
	
	diffuse = angle * diffuse_coeff;
	return diffuse;
}

// Returns the specular component of the light at the plane's intersection point
//
float Plane::get_specular(Ray &view_ray, Vec3f &light_pos, Vec3f &intersect_pt)
{
	float spec = 0.0f;

	Vec3f light_vec = light_pos - intersect_pt;
	normalize(light_vec);
	Vec3f R = light_vec - 2.0f * (light_vec * normal) * normal;
	
	float dot = view_ray.direction * R;
	if (dot > 0.0f) {
		spec = 10 * powf(dot, 20) * specular_coeff; 
	}

	//HACK: Return 0.0f to turn off specular shading
	return 0.0f;
}

// Returns the reflected ray of the incoming ray hitting the plane's intersection point
//
Vec3f Plane::get_reflection(Ray &view_ray, Vec3f &intersect_pt)
{
	// reflected_ray = incoming_ray - 2 * N * (incoming_ray . N)
	Vec3f R = view_ray.direction - 2.0f * (view_ray.direction * normal) * normal;
	return R;
}

Vec3f Plane::get_normal(Vec3f &intersect_pt)
{
	return normal;
}