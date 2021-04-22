/* Shawn Tripathy and Jose Delgado
 * CSF Assignment 04
 * Expose plugin
 */
#include <stdlib.h>
#include "image_plugin.h"

struct Arguments {
	double exposure;
};

const char *get_plugin_name(void) {
	return "expose";
}

const char *get_plugin_desc(void) {
	return "adjust the intensity of all pixels";
}

void *parse_arguments(int num_args, char *args[]) {
	if (num_args != 1) {
		return NULL;
	}
	struct Arguments *arg = malloc(sizeof(struct Arguments));
	double exp = atof(args[0]);
	if (exp < 0) {
		return NULL;
	}
	arg->exposure = exp;
	
	return arg;
}

static uint32_t chng_exp(uint32_t pix, double exp) {
	uint8_t r, g, b, a;

	img_unpack_pixel(pix, &r, &g, &b, &a);
	uint32_t r1 = r * exp;
	uint32_t g1 = g * exp;
	uint32_t b1 = b * exp;
	//Check if rgb values go over 255
	if (r1 > 255) {
		r = 255;
	}
	else {
		r = r1;
	}
	if (g1 > 255) {
		g = 255;
	} 
	else {
		g = g1;
	}
	if (b1 > 255) {
		b = 255;
	} 
	else {
		b = b1;
	}

	return img_pack_pixel(r, g, b, a);
}

struct Image *transform_image(struct Image *source, void *arg_data) {
	struct Arguments *args = arg_data;
	struct Image *out = img_create(source->width, source->height);
	if (!out) {
		free(args);
		return NULL;
	}

	unsigned num_pixels = source->width * source->height;
	for (unsigned i = 0; i < num_pixels; i++) {

		out->data[i] = chng_exp(source->data[i], args->exposure);

	}

	free(args);

	return out;
}
