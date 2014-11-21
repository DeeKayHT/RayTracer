#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <vector>
#include <iostream>
#include <fstream>
#include <assert.h>
#include <time.h>

#include "Geometry.h"
#include "Ray.h"
#include "Plane.h"
#include "Sphere.h"
#include "Light.h"


// ---------------------------------------------------------------------
// Defines
// ---------------------------------------------------------------------

#define WIDTH	640
#define HEIGHT	480

// viewing angle of camera (in degrees)
#define FIELD_OF_VIEW	75

#define AREA_LIGHT_SAMPLES 4	//4x4 grid on area light

// Depth of Field Values
#define DOF_NUM_RAYS	32
#define DOF_STRENGTH	2.0f
#define DOF_FOCAL_DIST	50.0f

// Cross-platform function declarations
#if defined(_MSC_VER)
#define snprintf _snprintf
#endif	// ifdef (_MSC_VER)


// ---------------------------------------------------------------------

using namespace gfx;
using namespace std;

vector<Geometry *> geometry_list;
vector<Light *> light_list;

void Raytrace(Ray &ray, Vec3f &pix_color, float reflect, float refract, float r_index)
{
	Vec3f intersect_pt, temp_intersect_pt;
	
	int result = 0;
	float dist = 0.0f;
	float smallest_dist = 20000.0f;
	int closest_obj_indx = -1;

	for (unsigned int obj = 0; obj < geometry_list.size(); obj++) {
		if (geometry_list[obj]->intersect_ray(ray, temp_intersect_pt, result)) {
			dist = abs(norm(temp_intersect_pt - ray.origin));
			if (dist > 0.0f && dist < smallest_dist) {
				smallest_dist = dist;
				intersect_pt = temp_intersect_pt;
				closest_obj_indx = obj;
			}
		}
	}

	if (closest_obj_indx >= 0) {
		Geometry* closest_obj = geometry_list[closest_obj_indx];

		for (unsigned int light_idx = 0; light_idx < light_list.size(); light_idx++) {
			// Is it under shadow?
			float shade = 1.0f;
			if (light_list[light_idx]->type == LIGHT_BOX) {
				// First check under the box light source
				LightBox* lb = (LightBox*)(light_list[light_idx]);
				float delta = lb->size * (2.0f/AREA_LIGHT_SAMPLES);
				for (int grid1 = 0; grid1 < AREA_LIGHT_SAMPLES; grid1++) {
					for (int grid2 = 0; grid2 < AREA_LIGHT_SAMPLES; grid2++) {
						float gridx = ((-AREA_LIGHT_SAMPLES)/2 + grid1) * delta;
						float gridz = ((-AREA_LIGHT_SAMPLES)/2 + grid2) * delta;
						float delta_x = delta * ((float)rand()/(float)RAND_MAX+float(1));
						float delta_z = delta * ((float)rand()/(float)RAND_MAX+float(1));

						Vec3f grid_pos(gridx + delta_x + lb->position[0], lb->position[1], gridz + delta_z + lb->position[2]);
						Vec3f light_vec = grid_pos - intersect_pt;
						float light_dist = norm(light_vec);
						normalize(light_vec);
						Ray light_ray(intersect_pt, light_vec);
						for (unsigned int obj = 0; obj < geometry_list.size(); obj++) {
							int result2 = 0;
							if (geometry_list[obj]->intersect_ray(light_ray, temp_intersect_pt, result2)) {
								Vec3f td1 = intersect_pt - temp_intersect_pt;
								float td2 = norm(td1);
								if (td2 < light_dist && obj != closest_obj_indx) {
									shade -= 1.0f / (float)(AREA_LIGHT_SAMPLES * AREA_LIGHT_SAMPLES);

									// Due to the lack of radiosity, clamp the shadow factor to prevent a full black shadow
									if (shade < 0.4f) {
										shade = 0.4f;
									}
								}
							}
						}
					}
				}

			} else {
				// Check for a point light source
				Vec3f light_vec = light_list[light_idx]->position - intersect_pt;
				float light_dist = norm(light_vec);
				normalize(light_vec);
				Ray light_ray(intersect_pt, light_vec);
				for (unsigned int obj = 0; obj < geometry_list.size(); obj++) {
					int result2 = 0;
					if (geometry_list[obj]->intersect_ray(light_ray, temp_intersect_pt, result2)) {
						Vec3f td1 = intersect_pt - temp_intersect_pt;
						float td2 = norm(td1);
						
						if (td2 < light_dist && obj != closest_obj_indx) {
							// Since there is a lack of radiosity, set shadow factor to 0.4 to prevent a full black shadow
							shade = 0.4f;
						}
					}
				}
			}

			// Calculate Diffuse
			float diffuse = closest_obj->get_diffuse(light_list[light_idx]->position, intersect_pt);
			if (! (closest_obj->checkerboard)) {
				pix_color[0] += light_list[light_idx]->color[0] * closest_obj->color[0] * diffuse * shade;
				pix_color[1] += light_list[light_idx]->color[1] * closest_obj->color[1] * diffuse * shade;
				pix_color[2] += light_list[light_idx]->color[2] * closest_obj->color[2] * diffuse * shade;

			} else {
				if ((fmodf(abs(intersect_pt[0]), 100.0f) < 50.0f && fmodf(abs(intersect_pt[1]), 100.0f) < 50.0f) ||
				(fmodf(abs(intersect_pt[0]), 100.0f) >= 50.0f && fmodf(abs(intersect_pt[1]), 100.0f) >= 50.0f)) {
					pix_color[0] += light_list[light_idx]->color[0] * 0.8f * diffuse * shade;
					pix_color[1] += light_list[light_idx]->color[1] * 0.8f * diffuse * shade;
					pix_color[2] += light_list[light_idx]->color[2] * 0.8f * diffuse * shade;
				} else if ((fmodf(abs(intersect_pt[0]), 100.0f) < 50.0f && fmodf(abs(intersect_pt[1]), 100.0f) >= 50.0f) ||
					(fmodf(abs(intersect_pt[0]), 100.0f) >= 50.0f && fmodf(abs(intersect_pt[1]), 100.0f) < 50.0f)) {
					pix_color[0] += light_list[light_idx]->color[0] * 0.1f * diffuse * shade;
					pix_color[1] += light_list[light_idx]->color[1] * 0.1f * diffuse * shade;
					pix_color[2] += light_list[light_idx]->color[2] * 0.1f * diffuse * shade;
				}
			}

			// Calculate Specular
			if (result != -1) {
				float specular = closest_obj->get_specular(ray, light_list[light_idx]->position, intersect_pt);
				pix_color += light_list[light_idx]->color * specular * shade;
			}
		}

		// Calculate Reflection
		if (closest_obj->reflect_coeff > 0.0f && reflect > 0.10f && closest_obj->is_reflecting) {
			Vec3f R = closest_obj->get_reflection(ray, intersect_pt);
			Vec3f reflect_color(0.0f);
			Ray reflect_ray(intersect_pt, R);
			Raytrace(reflect_ray, reflect_color, reflect*closest_obj->reflect_coeff, refract, r_index);

			pix_color[0] += reflect_color[0] * closest_obj->reflect_coeff * closest_obj->color[0];
			pix_color[1] += reflect_color[1] * closest_obj->reflect_coeff * closest_obj->color[1];
			pix_color[2] += reflect_color[2] * closest_obj->reflect_coeff * closest_obj->color[2];
		}

		// Calculate Refraction
		if (closest_obj->refract_index > 0.0f && refract > 0.10f) {
			float n = 0.0f;
			if (result == 1) {
				n = 1.0f / closest_obj->refract_index;
			} else if (result == -1) {
				n = closest_obj->refract_index / 1.0f;
			}
			Vec3f N = (closest_obj->get_normal(intersect_pt)) * (float)result;
			float theta1 = -(N * ray.direction);
			float theta2 = sqrtf(1 - (n*n) * (1 - (theta1 * theta1)));
			if (theta2 < 1.0f)
			{
				Vec3f T(0.0f);
				if (theta1 >= 0) {
					T = (n * ray.direction) + (n * theta1 - theta2) * N;
				} else {
					T = (n * ray.direction) + (n * theta1 + theta2) * N;
				}
				normalize(T);
				Ray refract_ray(intersect_pt, T);
				Vec3f refract_color(0.0f);
				Raytrace( refract_ray, refract_color, reflect, refract * closest_obj->refract_coeff, closest_obj->refract_index);
				pix_color += refract_color;
			}
		}
	}
}

int main(int argc, const char* argv[])
{
	// get today's date (MM_DD_YYYY)
	time_t raw_time;
	const int DATE_BUFFER_SIZE = 12;
	char date[DATE_BUFFER_SIZE];
	if ( time(&raw_time) != -1 ) {
		tm local_time;

		if ( localtime_s(&local_time, &raw_time) == 0 ) {
			strftime(date, DATE_BUFFER_SIZE, "%m_%d_%Y", &local_time);
		}
	}

	const int FILENAME_BUFFER_SIZE = 32;
	char filename[FILENAME_BUFFER_SIZE];
	snprintf(filename, FILENAME_BUFFER_SIZE, "render_%s.tga", date);

	// save render to a file (render_MM_DD_YYYY.tga)
	ofstream imageFile;
	imageFile.open(filename, ios_base::binary);

	// TODO: Check for existing files and increment filename
	if ( !imageFile ) {
		return false;
	}
	
	// TGA Format Description: http://paulbourke.net/dataformats/tga/
	// Addition of the TGA header
	imageFile.put(0);
	imageFile.put(0);
	imageFile.put(2);        /* RGB not compressed */

	imageFile.put(0);
	imageFile.put(0);
	imageFile.put(0);
	imageFile.put(0);
	imageFile.put(0);

	imageFile.put(0);
	imageFile.put(0); /* origin X */ 
	imageFile.put(0);
	imageFile.put(0); /* origin Y */

	imageFile.put((unsigned char)(WIDTH & 0x00FF)).put((unsigned char)((WIDTH & 0xFF00) / 256));
	imageFile.put((unsigned char)(HEIGHT & 0x00FF)).put((unsigned char)((HEIGHT & 0xFF00) / 256));
	imageFile.put(24);       /* 24 bit bitmap */ 
	imageFile.put(0); 

	// initialize the scene
	// initialize the floor plane
	Vec3f p1_norm(0.0f, 1.0f, 0.0f);
	Vec3f p1_pos(0.0f, -40.0f, 0.0f);
	Plane plane1(p1_norm, p1_pos, Color(100, 200, 100), false);
	geometry_list.push_back(&plane1);

	// initialize the background plane
	Vec3f p2_norm(0.0f, 0.0f, -1.0f);
	Vec3f p2_pos(0.0f, 0.0f, 220.0f);
	Plane plane2(p2_norm, p2_pos, Color(100, 100, 100), false);
	geometry_list.push_back(&plane2);

	Sphere sphere1(-70.0f, 0.0f, 100.0f, 20.0f, Color(200, 100, 100), 0.8f, 0.0f);
	Sphere sphere2(0.0f, 0.0f, 50.0f, 20.0f, Color(200, 200, 100), 0.8f, 0.0f);
	Sphere sphere3(70.0f, 0.0f, 100.0f, 20.0f, Color(100, 100, 200), 0.8f, 0.0f);
	Sphere sphere4(40.0f, 0.0f, 50.0f, 5.0f, Color(0, 0, 0), 0.8f, 1.5f, false);
	geometry_list.push_back(&sphere1);
	geometry_list.push_back(&sphere2);
	geometry_list.push_back(&sphere3);
	geometry_list.push_back(&sphere4);
	
	Light light1(0.0f, 40.0f, 30.0f, Color(255, 255, 255));
	LightBox lightbox(0.0f, 50.0f, 10.0f, 10.0f, Color(255,255,255));
	//light_list.push_back(&light1);
	light_list.push_back(&lightbox);

	srand( (unsigned int)time(NULL) );

	float percent_threshold = 10.0f;

	// begin ray tracing
	for (int y = 0; y < HEIGHT; y++) {
		for (int x = 0; x < WIDTH; x++) {
			// Show percentage of render complete by increments of 10%
			int pix_num = y * WIDTH + (x + 1);
			float percent_complete = 100.0f * ((float)pix_num / (float)(WIDTH * HEIGHT));
			if (percent_complete > percent_threshold) {
				printf("%.f%% complete\n", percent_complete);
				percent_threshold += 10.0f;
			}

			float z = (WIDTH / 2.0f) / atan(FIELD_OF_VIEW / 2.0f);
			Vec3f ray_start(0.0f);
			Vec3f unit_direction((float)x - (WIDTH / 2.0f), (float)y - (HEIGHT / 2.0f), z);

			Vec3f pix_color(0.0f);
			Vec3f dof_accum(0.0f);

			Vec3f dof_dir;
			normalize(unit_direction);
			Ray camera_ray(ray_start, unit_direction);
			Raytrace(camera_ray, pix_color, 1.0f, 1.0, 1.0f);
			
			// Depth of Field (DOF) added by Craig Grupp
			dof_dir = camera_ray.direction;
			dof_dir *= DOF_FOCAL_DIST;

			dof_accum += pix_color;

			for(int dof=0; dof<DOF_NUM_RAYS; dof++)
			{
				float dof_x = ((float)rand()/(float)RAND_MAX+float(1));
				dof_x *= DOF_STRENGTH;

				float dof_y = ((float)rand()/(float)RAND_MAX+float(1));
				dof_y *= DOF_STRENGTH;

				ray_start = Vec3f(dof_x, dof_y, 0);
				unit_direction = dof_dir - ray_start;
				normalize(unit_direction);
				camera_ray = Ray(ray_start, unit_direction);
				Raytrace(camera_ray, pix_color, 1.0f, 1.0, 1.0f);
				dof_accum += pix_color;
			}
			pix_color /= DOF_NUM_RAYS;

			imageFile.put((unsigned char)min(pix_color[2]*255.0f, 255.0f));
			imageFile.put((unsigned char)min(pix_color[1]*255.0f, 255.0f));
			imageFile.put((unsigned char)min(pix_color[0]*255.0f, 255.0f));
		}
	}

	imageFile.close();

	return 0;
}

