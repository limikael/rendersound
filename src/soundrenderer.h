#ifndef __SOUNDRENDERER_H__
#define __SOUNDRENDERER_H__

typedef struct SOUNDRENDERER SOUNDRENDERER;

SOUNDRENDERER *soundrenderer_create();
void soundrenderer_dispose(SOUNDRENDERER *soundrenderer);
int soundrenderer_render(SOUNDRENDERER *soundrenderer, char *fn);
void soundrenderer_set_output_file_name(SOUNDRENDERER *soundrenderer, char *fn);
void soundrenderer_set_color(SOUNDRENDERER *soundrenderer, unsigned int col);
void soundrenderer_set_height(SOUNDRENDERER *soundrenderer, unsigned int height);
void soundrenderer_set_width(SOUNDRENDERER *soundrenderer, unsigned int width);
void soundrenderer_set_pixels_per_sec(SOUNDRENDERER *soundrenderer, unsigned int pixels_per_sec);
void soundrenderer_set_progress_callback(SOUNDRENDERER *soundrenderer, void (*callback)(int));
char *soundrenderer_get_error(SOUNDRENDERER *soundrenderer);
void soundrenderer_set_super_sample_factor(SOUNDRENDERER *soundrenderer, int factor);

#endif
