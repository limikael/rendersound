#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

#include "soundrenderer.h"

/**
 * Show help text.
 */
void printhelp() {
	char *help=
		"Usage: rendersound [options] <input> <output>\n"
		"Renders the waveform of a sound file as a png image.\n"
		"\n"
		"Options:\n"
		"\n"
		"  -h              Show this help.\n"
		"  -y size         Height in pixels of output.\n"
		"  -x size         Width in pixels of output.\n"
		"  -s scale        Set scale as pixels/sec.\n"
		"                  This will cause -x to be ignored.\n"
		"  -c color        Color expressed as rgb hex, e.g. 80ff80.\n"
		"  -f factor       Set super sampling factor for anti aliasing.\n"
		"                  Default is 4.\n"
		"  -p              Show progress.\n"
		"\n";

	printf("%s",help);
	exit(1);
}

/**
 * Report progress.
 */
void progress(int percent) {
	printf("\rProgress: %d%%",percent);
	fflush(stdout);
}

/**
 * Main.
 */
int main(int argc, char **argv) {
	SOUNDRENDERER *soundrenderer;
	char c;
	unsigned int u;
	int show_progress=0;
	int res;

	soundrenderer=soundrenderer_create();

	while ((c=getopt(argc,argv,"hc:x:y:s:pf:")) != -1) {
		switch (c) {
			case 'c':
				sscanf(optarg,"%x",&u);
				soundrenderer_set_color(soundrenderer,u);
				break;

			case 'x':
				soundrenderer_set_width(soundrenderer,atoi(optarg));
				break;

			case 'y':
				soundrenderer_set_height(soundrenderer,atoi(optarg));
				break;

			case 's':
				soundrenderer_set_pixels_per_sec(soundrenderer,atoi(optarg));
				break;

			case 'f':
				soundrenderer_set_super_sample_factor(soundrenderer,atoi(optarg));
				break;

			case 'h':
				printhelp();
				exit(1);
				break;

			case 'p':
				soundrenderer_set_progress_callback(soundrenderer,progress);
				show_progress=1;
				break;
		}
	}

	if (argc-optind!=2) {
		printf("Usage: rendersound [options] <infile> <outfile>\nTry rendersound -h for help.\n");
		exit(1);
	}

	soundrenderer_set_output_file_name(soundrenderer,argv[optind+1]);
	res=soundrenderer_render(soundrenderer,argv[optind]);

	if (!res) {
		printf("%s\n",soundrenderer_get_error(soundrenderer));
		exit(1);
	}

	if (show_progress) {
		progress(100);
		printf("\n");
	}

	return 0;
}
