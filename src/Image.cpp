// --------------------------------------------------------
// Image.cpp
// (c) 2015 - Daniel Kim
// --------------------------------------------------------


// --------------------------------------------------------
// Includes
// --------------------------------------------------------

#include <fstream>
#include "Image.h"


// --------------------------------------------------------
// Defines
// --------------------------------------------------------

#define BYTES_PER_PIXEL	3


// --------------------------------------------------------

Image::Image(int width, int height)
{
	this->width = width;
	this->height = height;
	image_buffer = NULL;
}

Image::~Image()
{
	free(image_buffer);
}

bool Image::init_buffer()
{
	size_t buffer_size = width * height * BYTES_PER_PIXEL;

	image_buffer = (uint8_t*)malloc(buffer_size);
	if (image_buffer == NULL) {
		return false;
	}
	memset(image_buffer, 0, buffer_size);
	return true;
}

bool Image::write_to_file(char* filename, ImageFormat format)
{
	switch (format)
	{
		case IMG_FORMAT_TGA:
			return write_TGA(filename);

		default:
			// Format not supported!
			return false;
	}
}

bool Image::write_TGA(char* filename)
{
	if (filename == NULL) {
		return false;
	}
	if (image_buffer == NULL) {
		return false;
	}

	std::ofstream imageFile;
	imageFile.open(filename, std::ios_base::binary);
	if (!imageFile.is_open()) {
		return false;
	}

	// TGA Format Description: http://paulbourke.net/dataformats/tga/
	// Addition of the TGA header
	imageFile.put(0);
	imageFile.put(0);
	imageFile.put(2);	// image contains uncompressed RGB data

	imageFile.put(0);
	imageFile.put(0);
	imageFile.put(0);
	imageFile.put(0);
	imageFile.put(0);

	imageFile.put(0);
	imageFile.put(0);	// x-origin (2-bytes)
	imageFile.put(0);
	imageFile.put(0);	// y-origin (2-bytes)

	// In order to handle writing data in the correct order for different architectures,
	// each dimension must be written with the lower-byte order first.
	imageFile.put((unsigned char)(width & 0x00FF));
	imageFile.put((unsigned char)((width & 0xFF00) / 256));
	imageFile.put((unsigned char)(height & 0x00FF));
	imageFile.put((unsigned char)((height & 0xFF00) / 256));

	imageFile.put(24);	// image is a 24-bit RGB bitmap
	imageFile.put(0);

	// Write pixel data by rows into BGR from the buffer's RGB format
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			int index = (x * BYTES_PER_PIXEL) + (y * BYTES_PER_PIXEL * width);
			imageFile.put( image_buffer[index + 2] );
			imageFile.put( image_buffer[index + 1] );
			imageFile.put( image_buffer[index + 0] );
		}

	}

	imageFile.close();
	return true;
}

void Image::set_pixel(int x, int y, gfx::Vec3f color)
{
	if (image_buffer == NULL) {
		return;
	}

	int pixel_index = (x * BYTES_PER_PIXEL) + (y * BYTES_PER_PIXEL * width);
	image_buffer[pixel_index + 0] = (uint8_t)(color[0] * 255.0f);
	image_buffer[pixel_index + 1] = (uint8_t)(color[1] * 255.0f);
	image_buffer[pixel_index + 2] = (uint8_t)(color[2] * 255.0f);
}
