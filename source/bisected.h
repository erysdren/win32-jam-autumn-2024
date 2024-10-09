
#pragma once
#ifndef _BISECTED_H_
#define _BISECTED_H_
#ifdef __cplusplus
extern "C" {
#endif

#include "SDL_config.h"
#include "SDL.h"

#ifdef __WIN32__
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include <math.h>
#include <float.h>

#ifndef MAX_PATH
#define MAX_PATH 256
#endif

#ifdef __WIN32__
#define PATH_SEP_CHR '\\'
#define PATH_SEP_STR "\\"
#else
#define PATH_SEP_CHR '/'
#define PATH_SEP_STR "/"
#endif

#ifndef UNUSED
#define UNUSED(x) ((void)(x))
#endif

/*
 * META
 */

#define DEFAULT_CONFIG "bisected.ini"

#define GRID_NORTH (-4096)
#define GRID_EAST (4096)
#define GRID_SOUTH (4096)
#define GRID_WEST (-4096)
#define GRID_STRIDE (8)
#define GRID_MIN_STRIDE (8)
#define GRID_MAX_STRIDE (32)

/*
 * MATHS
 */

#ifndef M_PI
#define M_PI 3.14159265358979323846264338327950288
#endif

#define RAD2DEG(r) ((r) * 180.0f / M_PI)
#define DEG2RAD(d) ((d) * M_PI / 180.0f)

float wrapf(float value, float mod);
float minf(float a, float b);
float maxf(float a, float b);
float clampf(float value, float min, float max);

/*
 * MAIN
 */

void engine_quit(int code);
void engine_error(const char *fmt, ...);
void engine_frame(Uint32 dt);
void engine_main(void);

/*
 * CONFIG
 */

typedef struct config {
	char last_map[MAX_PATH + 1];
	float grid_offset_x;
	float grid_offset_y;
	float grid_scale;
	float camera_x;
	float camera_y;
	float camera_z;
	float camera_pitch;
	float camera_yaw;
	float camera_fov;
	float camera_near;
	float camera_far;
	int window_w;
	int window_h;
	char window_title[64];
	float target_framerate;
} config_t;

void config_reset(config_t *config);
SDL_bool config_load(const char *filename, config_t *config);
SDL_bool config_save(const char *filename, config_t *config);

/*
 * VID
 */

void vid_draw(void);
SDL_Surface *vid_resize(int w, int h);
SDL_Surface *vid_init(config_t *cfg);
void vid_quit(void);

#ifdef __cplusplus
}
#endif
#endif /* _BISECTED_H_ */
