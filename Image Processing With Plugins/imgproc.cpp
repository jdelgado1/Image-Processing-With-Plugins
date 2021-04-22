/* Shawn Tripathy and Jose Delgado
 * CSF Assignment 04
 * imgproc.cpp - driver
 */
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <dlfcn.h>
#include <sys/types.h>
#include <dirent.h>
#include <vector>
#include "pnglite.h"
#include "image.h"

using std::cout;
using std::cerr;
using std::endl;
using std::string;
using std::stoi;
using std::vector;

struct Plugin {
	void *handle;
	const char *(*get_plugin_name)(void);
	const char *(*get_plugin_desc)(void);
	void *(*parse_arguments)(int num_args, char *args[]);
	struct Image *(*transform_image)(struct Image *source, void *arg_data);
};

int main(int argc, char* argv[]) {
	if (argc == 1) {
		cout << "Usage: imgproc <command> [<command args...>]" << endl;
		cout << "Commands are:" << endl;
		cout << "  list" << endl;
		cout << "  exec <plugin> <input img> <output img> [<plugin args...>]" << endl;
		return 0;
	}
       		
	const char* plugin_dir = getenv("PLUGIN_DIR");
	if (plugin_dir == NULL) {
		plugin_dir = "./plugins";	
	}

	DIR *directoryStream;
	directoryStream = opendir(plugin_dir);

	if (directoryStream == NULL) {
		cerr << "Error: can't open directory" << endl;
		return 1;
	}
	
	struct dirent *directoryEntry;

	vector<Plugin> vector(5);

	int numPlugs = 0;
	//Readin directory entries
	while ((directoryEntry = readdir(directoryStream)) != NULL) {
		if ((strcmp(directoryEntry->d_name, ".") != 0) && (strcmp(directoryEntry->d_name, "..") != 0)) {
			
			//found the plugin shared library
			string front = plugin_dir; 
			char * name1 = directoryEntry->d_name;
			string name2 = name1;
			string frontName = front + "/" + name2;
			const char * finalName = frontName.c_str();
			void *handle = dlopen(finalName, RTLD_LAZY || RTLD_GLOBAL);

			if (handle == NULL) {
				cerr << "Error: can't load shared library" << endl;
				return 1;
			}
			
			void *symbol1 = dlsym(handle, "get_plugin_name");
			void *symbol2 = dlsym(handle, "get_plugin_desc");
			void *symbol3 = dlsym(handle, "parse_arguments");
			void *symbol4 = dlsym(handle, "transform_image");

			if (symbol1 == NULL || symbol2 == NULL || symbol3 == NULL || symbol4 == NULL) {
				cerr << "Error: can't load needed function" << endl;
			}

			Plugin thisPlugin;
			thisPlugin.handle = handle;
			*(void**) (&thisPlugin.get_plugin_name) = symbol1;
			*(void**) (&thisPlugin.get_plugin_desc) = symbol2;
			*(void**) (&thisPlugin.parse_arguments) = symbol3;
			*(void**) (&thisPlugin.transform_image) = symbol4;
			vector.at(numPlugs) = (thisPlugin);
			numPlugs++;
		}
	}
	//close the directory stream
	int closed = closedir(directoryStream);
	if (closed != 0) {
		cerr << "Error: cannot close directory stream" << endl;
		return 1;
	}

	string command(argv[1]);
	string sourceImg(argv[3]);
	string destImg(argv[4]);
	//execute 'list' command
	if (command == "list") {
		cout << "Loaded " << numPlugs << " plugin(s)" << endl;
		for (int i = 0; i < numPlugs; i++) {
			cout << vector.at(i).get_plugin_name() << ": " << vector.at(i).get_plugin_desc() << endl;
		}
		return 0;
	}

	int numArgs = argc - 5;

	//execute 'exec' commands
	if (command == "exec") {
		int i = 0;
		for (i = 0; i < numPlugs; i++) {
			const char *plugName = (vector.at(i).get_plugin_name());
			if (strcmp(plugName, argv[2]) == 0) {

				Image *theImage = img_read_png(argv[3]);
				//get arguments
				void *parsedArgs = vector.at(i).parse_arguments(numArgs, argv + 5);
				//function returns null for invalid arguments
				if (parsedArgs == NULL) {
					cerr << "Error: invalid arguments" << endl;
					return 1;
				}
				Image *finishedImage = vector.at(i).transform_image(theImage, parsedArgs);
				img_write_png(finishedImage, argv[4]);
				
				//free images
				img_destroy(theImage);
				img_destroy(finishedImage);
				return 0;
			}
		}
		if (i == numPlugs) {
			cerr << "Error: an image processing plugin can't be loaded" << endl;
			return 1;
		}
	}
	else {
		cerr << "Error: Invalid command" << endl;
		return 1;
	}

	return 0;
}
