
#include "bisected.h"

/* global state */
static SDL_Surface *screen = NULL;
static Uint32 num_frames = 0;
static float framerate = 0;
static Uint8 *keys = NULL;
static config_t config;
static Uint8 mouse_button_state = 0;
static Uint16 mouse_x = 0, mouse_y = 0;
static Sint16 mouse_xrel = 0, mouse_yrel = 0;
static SDL_bool mouse_lock = SDL_FALSE;

void engine_quit(int code)
{
	/* save config */
	config_save(DEFAULT_CONFIG, &config);

	/* shutdown everything and exit */
	vid_quit();
	SDL_Quit();
	exit(code);
}

void engine_error(const char *fmt, ...)
{
	char error[1024];
	va_list ap;

	va_start(ap, fmt);
	SDL_vsnprintf(error, sizeof(error), fmt, ap);
	va_end(ap);

	fprintf(stderr, "ERROR: %s\n", error);

	engine_quit(1);
}

static void handle_inputs(void)
{
	/* handle grid zoom */
	if (mouse_x > screen->w / 2)
	{
		if (mouse_button_state & SDL_BUTTON(SDL_BUTTON_WHEELUP))
		{
			config.grid_scale += 1;
		}
		else if (mouse_button_state & SDL_BUTTON(SDL_BUTTON_WHEELDOWN))
		{
			config.grid_scale -= 1;
		}
	}

	/* handle movement in views and lock mouse */
	if (mouse_button_state & SDL_BUTTON(SDL_BUTTON_RIGHT))
	{
		mouse_lock = SDL_TRUE;
		SDL_ShowCursor(SDL_FALSE);
		SDL_WarpMouse(mouse_x, mouse_y);

		if (mouse_x > screen->w / 2)
		{
			config.grid_offset_x -= mouse_xrel;
			config.grid_offset_y -= mouse_yrel;
		}
		else
		{
			config.camera_yaw += mouse_xrel;
			config.camera_pitch += mouse_yrel;

			config.camera_pitch = clampf(config.camera_pitch, -70, 70);
			config.camera_yaw = wrapf(config.camera_yaw, 360.0f);
		}
	}
	else
	{
		mouse_lock = SDL_FALSE;
		SDL_ShowCursor(SDL_TRUE);
	}
}

void engine_frame(Uint32 dt)
{
	static Uint32 draw_delta = 1000;
	SDL_Event event;
	SDL_bool do_draw = SDL_FALSE;

	/* handle sdl events */
	keys = SDL_GetKeyState(NULL);
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
			case SDL_QUIT:
				engine_quit(0);

			case SDL_MOUSEBUTTONDOWN:
				if (event.button.button == SDL_BUTTON_WHEELUP && mouse_x > screen->w / 2)
					config.grid_scale += 1;
				break;

			case SDL_MOUSEBUTTONUP:
				if (event.button.button == SDL_BUTTON_WHEELDOWN && mouse_x > screen->w / 2)
					config.grid_scale -= 1;
				break;

#if 0
			case SDL_MOUSEBUTTONDOWN:
				buttonstate[event.button.button] = SDL_TRUE;
				break;

			case SDL_MOUSEBUTTONUP:
				buttonstate[event.button.button] = SDL_FALSE;
				break;
#endif

			case SDL_MOUSEMOTION:
				if (!mouse_lock)
				{
					mouse_x = event.motion.x;
					mouse_y = event.motion.y;
				}
				mouse_xrel = event.motion.xrel;
				mouse_yrel = event.motion.yrel;
				mouse_button_state = event.motion.state;
				break;

			case SDL_VIDEOEXPOSE:
				do_draw = SDL_TRUE;
				break;

			case SDL_VIDEORESIZE:
				screen = vid_resize(event.resize.w, event.resize.h);
				config.window_w = screen->w;
				config.window_h = screen->h;
				break;
		}
	}

	/* handle inputs */
	handle_inputs();

	/* add dt to time counter */
	draw_delta += dt;

	/* check if we need to draw a frame */
	if (draw_delta > (1000 / config.target_framerate))
		do_draw = SDL_TRUE;

	/* do the frame */
	if (do_draw)
	{
		vid_draw();
		draw_delta = 0;
		num_frames++;
	}
}

void engine_main(void)
{
	Uint32 start, now, then;

	/* load config */
	config_reset(&config);
	config_load(DEFAULT_CONFIG, &config);

	/* init video */
	screen = vid_init(&config);

	/* start counting time and run main loop */
	start = then = SDL_GetTicks();
	while (1)
	{
		/* calculate dt and run engine frame */
		now = SDL_GetTicks();
		engine_frame(now - then);
		then = now;

		/* calculate framerate for the sickos who care */
		framerate = ((float)num_frames / (float)(SDL_GetTicks() - start)) * 1000.0f;
	}
}

int main(int argc, char **argv)
{
	engine_main();
	return 0;
}
