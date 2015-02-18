// --------------------------------------------------------
// Image.h
// (c) 2015 - Daniel Kim
// --------------------------------------------------------

#pragma once


// --------------------------------------------------------
// Includes
// --------------------------------------------------------

#include <stdint.h>
#include "gfx/vec3.h"


// --------------------------------------------------------
// Enumerations
// --------------------------------------------------------

enum ImageFormat
{
	IMG_FORMAT_TGA,
	NUM_IMG_FORMATS
};


// --------------------------------------------------------

class Image
{
private:
	int width;
	int height;
	uint8_t* image_buffer;

	bool write_TGA(char* filename);

public:
	Image(int width, int height);
	~Image();

	bool init_buffer();
	bool write_to_file(char* filename, ImageFormat format);
	void set_pixel(int x, int y, gfx::Vec3f color);
};
