
#include "bisected.h"

void config_reset(config_t *config)
{
	SDL_memset(config, 0, sizeof(config_t));
	config->grid_scale = 1;
}

static SDL_bool read_line(SDL_RWops *rw, char *dst, int n)
{
	char c;
	int i = 0;

	while (1)
	{
		if (SDL_RWread(rw, &c, sizeof(char), 1) != 1)
			return SDL_FALSE;

		if (i >= n)
			break;

		if (c == '\n')
			break;

		dst[i++] = c;
	}

	dst[i] = '\0';

	return SDL_TRUE;
}

SDL_bool config_load(const char *filename, config_t *config)
{
	SDL_RWops *rw;
	char line[1024];

	rw = SDL_RWFromFile(filename, "rb");
	if (!rw) return SDL_FALSE;

	while (read_line(rw, line, sizeof(line)))
	{
		printf("%s\n", line);
	}

	/* clean up */
	SDL_RWclose(rw);
	return SDL_TRUE;
}

static void write_string(SDL_RWops *rw, const char *name, const char *value)
{
	int len;
	char str[1024];
	len = SDL_snprintf(str, sizeof(str), "%s=%s\n", name, value);
	SDL_RWwrite(rw, str, len, 1);
}

static void write_float(SDL_RWops *rw, const char *name, float value)
{
	int len;
	char str[1024];
	len = SDL_snprintf(str, sizeof(str), "%s=%g\n", name, value);
	SDL_RWwrite(rw, str, len, 1);
}

static void write_int(SDL_RWops *rw, const char *name, int value)
{
	int len;
	char str[1024];
	len = SDL_snprintf(str, sizeof(str), "%s=%d\n", name, value);
	SDL_RWwrite(rw, str, len, 1);
}

SDL_bool config_save(const char *filename, config_t *config)
{
	SDL_RWops *rw;

	rw = SDL_RWFromFile(filename, "wb");
	if (!rw) return SDL_FALSE;

	write_string(rw, "last_map", config->last_map);
	write_float(rw, "grid_scale", config->grid_scale);
	write_float(rw, "grid_offset_x", config->grid_offset_x);
	write_float(rw, "grid_offset_y", config->grid_offset_y);
	write_float(rw, "camera_x", config->camera_x);
	write_float(rw, "camera_y", config->camera_y);
	write_float(rw, "camera_z", config->camera_z);
	write_float(rw, "camera_pitch", config->camera_pitch);
	write_float(rw, "camera_yaw", config->camera_yaw);
	write_int(rw, "window_w", config->window_w);
	write_int(rw, "window_h", config->window_h);

	/* clean up */
	SDL_RWclose(rw);
	return SDL_TRUE;
}
