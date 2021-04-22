/* Shawn Tripathy and Jose Delgado
 * CSF Assignment 04
 * Mirrorv plugin
 */
#include <stdlib.h>
#include "image_plugin.h"

struct Arguments {
	int dummy; //no args so dummy variable
};

const char *get_plugin_name(void) {
	return "mirrorv";
}

const char *get_plugin_desc(void) {
	return "mirror image vertically";
}

void *parse_arguments(int num_args, char *args[]) {
	(void) args; //avoid warning

	if (num_args != 0) {
		return NULL;
	}
	return calloc(1, sizeof(struct Arguments));
}

struct Image *transform_image(struct Image *source, void *arg_data) {
	struct Arguments *args = arg_data;

	struct Image *out = img_create(source->width, source->height);
	if (!out) {
		free(args);
		return NULL;
	}

	uint32_t* row = malloc(sizeof(uint32_t) * source->width);
	unsigned num_pixels = source->width * source->height;
	for (unsigned i = 0; i < num_pixels; i++) {
		row[i % source->width] = source->data[i];
		//check row is fully populated
		if (i > 0 && (i + 1) % source->width == 0) {
			unsigned num_rows = (i / source->width) + 1;
			unsigned temp = num_pixels - (num_rows * source->width);
			//place row in opposite row
			for (unsigned j = 0; j < source->width; j++) {
				out->data[temp] = row[j];
				temp++;
			}
		}
	}

	free(args);
	free(row);
	return out;
}
