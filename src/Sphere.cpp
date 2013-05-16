#include "Sphere.h"

Sphere::Sphere(float cx, float cy, float cz, float init_radius, COLORREF init_color, 
			   float spec_coeff, float init_refract, bool reflect)
{
	init(cx, cy, cz, init_radius, init_color, spec_coeff, init_refract, reflect);
}

Sphere::~Sphere(void)
{
}

// Initialize the sphere's position, radius, color, specular and refraction coefficients, and whether it should reflect
//
void Sphere::init(float cx, float cy, float cz, float init_radius, COLORREF init_color,
				  float init_spec, float refract, bool reflect)
{
	position[0] = cx;
	position[1] = cy;
	position[2] = cz;

	radius = init_radius;

	color[0] = ((float)GetRValue(init_color)) / 255.0f;
	color[1] = ((float)GetGValue(init_color)) / 255.0f;
	color[2] = ((float)GetBValue(init_color)) / 255.0f;

	diffuse_coeff = 1.0f;
	specular_coeff = init_spec;
	reflect_coeff = 0.8f;
	refract_coeff = 0.8f;
	refract_index = refract;

	is_reflecting = reflect;
	
	// debug flag to render the sphere with a checkerboard pattern
	checkerboard = false;
}

// Checks if the ray intersects the sphere, returns true if it intersects, false otherwise
// Output:	intersect_pt - the point where the ray intersects the sphere
//			result - 
//
bool Sphere::intersect_ray(Ray &ray, Vec3f &intersect_pt, int &result)
{
	Vec3f dist = position - ray.origin; 
	float B = ray.direction * dist;
	float D = (B * B) - (dist * dist) + (radius * radius);
	
	if (D < 0.f) {
		return false;
	}
	
	float t0 = B - sqrt(D); 
	float t1 = B + sqrt(D);
	
	float t = 20000.f;
	Vec3f intersect_vec;
	bool retvalue = false; 
	if ((t0 > 0.01f) && (t0 < t)) 
	{
		t = t0;
		intersect_vec[0] = ray.direction[0] * t;
		intersect_vec[1] = ray.direction[1] * t;
		intersect_vec[2] = ray.direction[2] * t;

		intersect_pt[0] = ray.origin[0] + intersect_vec[0];
		intersect_pt[1] = ray.origin[1] + intersect_vec[1];
		intersect_pt[2] = ray.origin[2] + intersect_vec[2];
		result = 1;
		retvalue = true; 
	} 
	
	if ((t1 > 0.01f) && (t1 < t)) 
	{
		t = t1;
		intersect_vec[0] = ray.direction[0] * t;
		intersect_vec[1] = ray.direction[1] * t;
		intersect_vec[2] = ray.direction[2] * t;

		intersect_pt[0] = ray.origin[0] + intersect_vec[0];
		intersect_pt[1] = ray.origin[1] + intersect_vec[1];
		intersect_pt[2] = ray.origin[2] + intersect_vec[2];
		result = -1;
		retvalue = true; 
	}

   return retvalue; 

	/* Code below taken from Quesa3D sphere intersect code
	// Prepare to intersect
	//
	// First calculate the vector from the sphere to the ray origin, its
	// length squared, the projection of this vector onto the ray direction,
	// and the squared radius of the sphere.
	Vec3f dst = ray.origin - position;
	float t;
	float d = dst * ray.direction;
	float dst_squared = dst * dst;
	float r_squared = radius * radius;

	// if sphere is behind ray origin, then they do not intersect
	//if (d < 0.0f && dst_squared > r_squared) {
	if (d < 0.0f ) {
		return false;
	}

	// Calculate the squared distance from the sphere position to the projection.
	// If it's greater than the radius then they don't intersect.
	float m2 = (dst_squared - (d * d));
	if (m2 > r_squared) {
		return false;
	}

	// Calculate the distance along the ray to the intersection point
	float q = sqrt(r_squared - m2);
	if (dst_squared > r_squared) {
		t = d - q;
	} else {
		t = d + q;
	}

	Vec3f intersect_vec;
	intersect_vec[0] = ray.direction[0] * t;
	intersect_vec[1] = ray.direction[1] * t;
	intersect_vec[2] = ray.direction[2] * t;

	intersect_pt[0] = ray.origin[0] + intersect_vec[0];
	intersect_pt[1] = ray.origin[1] + intersect_vec[1];
	intersect_pt[2] = ray.origin[2] + intersect_vec[2];

	return true;*/
}

// Returns the diffuse component of the light at the plane's intersection point
//
float Sphere::get_diffuse(Vec3f &light_pos, Vec3f &intersect_pt)
{
	Vec3f light_norm = get_normal(intersect_pt);
	Vec3f light_vec = light_pos - intersect_pt;
	normalize(light_vec);
	
	float angle = light_vec * light_norm;
	if (angle < 0.0f) {
		return 0.0f;
	}
	
	float diffuse = angle * diffuse_coeff;
	return diffuse;
}

// Returns the specular component of the light at the plane's intersection point
//
float Sphere::get_specular(Ray &view_ray, Vec3f &light_pos, Vec3f &intersect_pt)
{
	float spec = 0.0f;

	Vec3f light_norm = get_normal(intersect_pt);
	Vec3f light_vec = light_pos - intersect_pt;
	normalize(light_vec);
	Vec3f R = light_vec - 2.0f * (light_vec * light_norm) * light_norm;
	
	float dot = max(R * view_ray.direction, 0.0f);
	if (dot > 0.0f) {
		spec = powf(dot, 20) * specular_coeff; 
	}
	
	return spec;
}

Vec3f Sphere::get_reflection(Ray &view_ray, Vec3f &intersect_pt)
{
	Vec3f normal = intersect_pt - position;
	normalize(normal);
	
	// reflected_ray = incoming_ray - 2 * N * (incoming_ray . N)
	Vec3f R = view_ray.direction - 2.0f * (view_ray.direction * normal) * normal;
	normalize(R);
	return R;
}

Vec3f Sphere::get_normal(Vec3f &intersect_pt)
{
	Vec3f normal = intersect_pt - position;
	normalize(normal);
	return normal;
}