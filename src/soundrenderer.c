#include <gd.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
//#include <sndfile.h>
#include <stdbool.h>

#include "soundrenderer.h"
#include "ffsndin.h"
#include "ffsndutil.h"

struct SOUNDRENDERER {
	FFSNDIN *ffsndin;

	int width,height;
	int unscaled_width,unscaled_height;
	char *output_file_name;
	int chunksize;
	unsigned int color;
	void (*progress_callback)(int);
	int pixels_per_sec;
	int super_sample_factor;

	char *error_str;
};

/**
 * Create a wav renderer.
 */
SOUNDRENDERER *soundrenderer_create() {
	SOUNDRENDERER *rend;

	rend=malloc(sizeof(SOUNDRENDERER));

	if (!rend)
		return NULL;

	rend->height=50;
	rend->width=500;
	rend->output_file_name="out.png";
	rend->chunksize=100;
	rend->color=0xff0000;
	rend->pixels_per_sec=0;
	rend->progress_callback=NULL;
	rend->super_sample_factor=4;

	return rend;
}

/**
 * Set super sampling factor.
 */
void soundrenderer_set_super_sample_factor(SOUNDRENDERER *soundrenderer, int factor) {
	soundrenderer->super_sample_factor=factor;
}

/**
 * Set progress callback.
 */
void soundrenderer_set_progress_callback(SOUNDRENDERER *rend, void (*callback)(int)) {
	rend->progress_callback=callback;
}

/**
 * Convert pixel to sample.
 */
int pixel_to_sample(SOUNDRENDERER* rend, int pix) {
	return floor(pix*(long long)ffsndin_get_num_frames(rend->ffsndin)/rend->unscaled_width);
}

/**
 * Dispose the soundrenderer.
 */
void soundrenderer_dispose(SOUNDRENDERER *rend) {
	ffsndin_close(rend->ffsndin);
//	sf_close(rend->sndfile);
	free(rend);
}

/**
 * Set output filename.
 */
void soundrenderer_set_output_file_name(SOUNDRENDERER *rend, char *fn) {
	rend->output_file_name=fn;
}

/**
 * Read peak values for a number of samples.
 */
void read_peaks(SOUNDRENDERER *rend, int samples, float *min, float *max) {
	int i;
	float vals[samples*ffsndin_get_num_channels(rend->ffsndin)];

	if (!samples) {
		*min=0;
		*max=0;
		return;
	}

	*min=1;
	*max=-1;

	ffsndin_read(rend->ffsndin,vals,samples);

	for (i=0; i<samples*ffsndin_get_num_channels(rend->ffsndin); i++) {
		if (vals[i]<*min)
			*min=vals[i];

		if (vals[i]>*max)
			*max=vals[i];
	}
}

/**
 * Set color to use.
 */
void soundrenderer_set_color(SOUNDRENDERER *rend, unsigned int col) {
	rend->color=col;
}

/**
 * Set width.
 */
void soundrenderer_set_width(SOUNDRENDERER *rend, unsigned int width) {
	rend->width=width;
}

/**
 * Set pixels per sec.
 */
void soundrenderer_set_pixels_per_sec(SOUNDRENDERER *rend, unsigned int pixels_per_sec) {
	rend->pixels_per_sec=pixels_per_sec;
}

/**
 * Set height.
 */
void soundrenderer_set_height(SOUNDRENDERER *rend, unsigned int height) {
	rend->height=height;
}

/**
 * Allocate rgb.
 */
int allocate_rgb_color(gdImagePtr image, unsigned int rgb) {
	int r=0xff&(rgb>>16);
	int g=0xff&(rgb>>8);
	int b=0xff&(rgb>>0);

	return gdImageColorAllocate(image,r,g,b);
}

/**
 * Get error.
 */
char *soundrenderer_get_error(SOUNDRENDERER *rend) {
	return rend->error_str;
}

/**
 * Render.
 */
int soundrenderer_render(SOUNDRENDERER *rend, char *fn) {
	int pixel, position, next;
	int y1, y2;
	float min,max;
	int col,transparentcol;
	gdImagePtr image;
	int percent, oldpercent;
	int oldtime, t;
	FILE *out=fopen(rend->output_file_name,"wb");

	TRACE("running render..");

	rend->error_str=NULL;
	rend->ffsndin=ffsndin_open(fn);

	if (!rend->ffsndin) {
		rend->error_str="Unable to open input file.";
		return 0;
	}

	if (!out) {
		rend->error_str="Unable to open output file.";
		return 0;
	}

	if (rend->pixels_per_sec) {
		int rate=ffsndin_get_samplerate(rend->ffsndin);
		long frames=ffsndin_get_num_frames(rend->ffsndin);
		rend->width=frames*rend->pixels_per_sec/rate;
	}

/*	TRACE("frames:   %d",pragmasound_get_num_frames(rend->pragmasound));
	TRACE("channels: %d",pragmasound_get_num_channels(rend->pragmasound));
	TRACE("rate:     %d",pragmasound_get_samplerate(rend->pragmasound));*/

	rend->unscaled_width=rend->width*rend->super_sample_factor;
	rend->unscaled_height=rend->height*rend->super_sample_factor;

	image=gdImageCreateTrueColor(rend->unscaled_width,rend->unscaled_height);
	gdImageAlphaBlending(image,false);
	transparentcol=gdImageColorAllocateAlpha(image,255,0,0,127);
	gdImageFilledRectangle(image,0,0,rend->unscaled_width,rend->unscaled_height,transparentcol);
	gdImageAlphaBlending(image,true);

	col=allocate_rgb_color(image,rend->color);
	position=0;

	oldpercent=0;
	oldtime=time(NULL);

	TRACE("WIDTH: %d",rend->width);

	for (pixel=0; pixel<rend->unscaled_width; pixel++) {
		percent=100*pixel/rend->unscaled_width;
		t=time(NULL);
		if (percent!=oldpercent /*&& t!=oldtime*/) {
			if (rend->progress_callback)
				rend->progress_callback(percent);
			oldpercent=percent;
			oldtime=t;
		}
		next=pixel_to_sample(rend,pixel+1);
		read_peaks(rend,next-position,&min,&max);
		position=next;

		y1=round(rend->unscaled_height/2.0+rend->unscaled_height*min/2.0);
		y2=round(rend->unscaled_height/2.0+rend->unscaled_height*max/2.0);

		gdImageLine(image,pixel,y1,pixel,y2,col);
	}

	ffsndin_close(rend->ffsndin);

	gdImagePtr scaled;
	scaled=gdImageCreateTrueColor(rend->width,rend->height);

	gdImageAlphaBlending(scaled,false);
	transparentcol=gdImageColorAllocateAlpha(scaled,255,0,0,127);
	gdImageFilledRectangle(scaled,0,0,rend->width,rend->height,transparentcol);
	gdImageAlphaBlending(scaled,true);

	gdImageCopyResampled(scaled,image, 0,0, 0,0,
		rend->width,rend->height,
		rend->unscaled_width,rend->unscaled_height);

	gdImageAlphaBlending(scaled,false);
	gdImageSaveAlpha(scaled,true);
	gdImagePng(scaled,out);
	fclose(out);

	return 1;
}
