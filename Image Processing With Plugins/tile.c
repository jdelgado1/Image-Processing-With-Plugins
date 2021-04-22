/* Shawn Tripathy and Jose Delgado
 * CSF Assignment 04
 * Tile plugin
 */
#include <stdlib.h>
#include <math.h>
#include "image_plugin.h"

struct Arguments {
	int N;
};

const char *get_plugin_name(void) {
	return "tile";
}

const char *get_plugin_desc(void) {
	return "tile source image in an NxN arrangement";
}

void *parse_arguments(int num_args, char *args[]) {

	if (num_args != 1) {
		return NULL;
	}
	struct Arguments *arg = malloc(sizeof(struct Arguments));
	int n = atoi(args[0]);
	if (n <= 0) {
		return NULL;
	}
	arg->N = n;

	return arg;
}

//Get the pixel we want to populate the return image with
static int find_pixel(unsigned x, unsigned y, int n, unsigned width, unsigned height) {
	//find width and height of the first tiles
	int tileWidth = (int) ceil((double) width / n);
	int tileHeight = (int) ceil((double)height / n);

	
	int tilex = x / tileWidth;
	x = x - (tilex * tileWidth);
	int tiley = y / tileHeight;
	y = y - (tiley * tileHeight);

	return y * n * width + x * n;
}

struct Image *transform_image(struct Image *source, void *arg_data) {
	struct Arguments *args = arg_data;

	struct Image *out = img_create(source->width, source->height);
	if (!out) {
		free(args);
		return NULL;
	}

	int n = args->N;

	
	for (unsigned i = 0; i < source->height; i++) {
		for (unsigned j = 0; j < source->width; j++) {
			int index = find_pixel(j, i, n, source->width, source->height);
			out->data[i * source->width + j] = source->data[index];	
		}
	}

	free(args);

	return out;
}
