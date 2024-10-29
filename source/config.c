
#include "bisected.h"

void config_reset(config_t *config)
{
	SDL_memset(config, 0, sizeof(config_t));
	config->grid_offset_x = 0;
	config->grid_offset_y = 0;
	config->grid_scale = 1;
	config->camera_x = 128;
	config->camera_y = 128;
	config->camera_z = 128;
	config->camera_pitch = 0;
	config->camera_yaw = 0;
	config->camera_fov = 90.0f;
	config->camera_near = 0.1f;
	config->camera_far = 10000.0f;
	config->window_w = 640;
	config->window_h = 480;
	SDL_snprintf(config->window_title, sizeof(config->window_title), "BiSectEd");
	config->target_framerate = 60.0f;
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
	int n = 1;

	rw = SDL_RWFromFile(filename, "rb");
	if (!rw) return SDL_FALSE;

	while (read_line(rw, line, sizeof(line)))
	{
		char *mid = strchr(line, '=');
		char *left, *right;
		if (!mid)
		{
			fprintf(stderr, "Warning: line %d of %s is malformed\n", n, filename);
			continue;
		}

		left = line;
		*mid = '\0';
		right = mid + 1;

		if (SDL_strcmp(left, "last_map") == 0)
			SDL_strlcpy(config->last_map, right, sizeof(config->last_map));
		else if (SDL_strcmp(left, "grid_scale") == 0)
			config->grid_scale = SDL_atof(right);
		else if (SDL_strcmp(left, "grid_offset_x") == 0)
			config->grid_offset_x = SDL_atof(right);
		else if (SDL_strcmp(left, "grid_offset_y") == 0)
			config->grid_offset_y = SDL_atof(right);
		else if (SDL_strcmp(left, "camera_x") == 0)
			config->camera_x = SDL_atof(right);
		else if (SDL_strcmp(left, "camera_y") == 0)
			config->camera_y = SDL_atof(right);
		else if (SDL_strcmp(left, "camera_z") == 0)
			config->camera_z = SDL_atof(right);
		else if (SDL_strcmp(left, "camera_pitch") == 0)
			config->camera_pitch = SDL_atof(right);
		else if (SDL_strcmp(left, "camera_yaw") == 0)
			config->camera_yaw = SDL_atof(right);
		else if (SDL_strcmp(left, "camera_fov") == 0)
			config->camera_fov = SDL_atof(right);
		else if (SDL_strcmp(left, "camera_near") == 0)
			config->camera_near = SDL_atof(right);
		else if (SDL_strcmp(left, "camera_far") == 0)
			config->camera_far = SDL_atof(right);
		else if (SDL_strcmp(left, "window_title") == 0)
			SDL_strlcpy(config->window_title, right, sizeof(config->window_title));
		else if (SDL_strcmp(left, "window_w") == 0)
			config->window_w = SDL_atoi(right);
		else if (SDL_strcmp(left, "window_h") == 0)
			config->window_h = SDL_atoi(right);
		else if (SDL_strcmp(left, "target_framerate") == 0)
			config->target_framerate = SDL_atof(right);

		n++;
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
	write_float(rw, "camera_fov", config->camera_fov);
	write_float(rw, "camera_near", config->camera_near);
	write_float(rw, "camera_far", config->camera_far);
	write_string(rw, "window_title", config->window_title);
	write_int(rw, "window_w", config->window_w);
	write_int(rw, "window_h", config->window_h);
	write_float(rw, "target_framerate", config->target_framerate);

	/* clean up */
	SDL_RWclose(rw);
	return SDL_TRUE;
}
