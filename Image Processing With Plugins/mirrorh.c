/* Shawn Tripathy and Jose Delgado
 * CSF Assignment 04
 * mirror horizontally plugin
 */
#include <stdlib.h>
#include "image_plugin.h"

struct Arguments {
	int dummy; // no args so put dummy as placeholder
};

const char *get_plugin_name(void) {
	return "mirrorh";
}

const char *get_plugin_desc(void) {
	return "mirror image horizontally"; 
}

void *parse_arguments(int num_args, char *args[]) {
	(void) args;  //avoid warning

	if (num_args != 0) {
		return NULL;
	}
	return calloc(1, sizeof(struct Arguments));
}

//reverse the order of the array
void reverseArray(uint32_t* row, int end) {
	int start = 0;
	while (start < end) {
		uint32_t temp = row[start];
		row[start] = row[end];
		row[end] = temp;
		start++;
		end--;
	}
	
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
		//check if we have collected a full rows worth of pixels
		if (i > 0 && (i + 1) % source->width == 0) {
			//reverse each row when its populated and put it in out
			reverseArray(row, source->width - 1);
			unsigned temp = i + 1 - source->width;
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
