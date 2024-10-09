
#include "bisected.h"

/* global state */
static SDL_Surface *screen = NULL;
static Uint32 tick_delta = 1000;
static Uint32 num_ticks = 0;
static Uint32 render_delta = 1000;
static Uint32 num_frames = 0;
static float framerate = 0;
static float tickrate = 0;
static Uint8 *keys = NULL;
static config_t config;
static SDL_bool buttonstate[16];
static Uint16 mouse_x = 0, mouse_y = 0;
static Sint16 mouse_xrel = 0, mouse_yrel = 0;
static SDL_bool mouse_lock = SDL_FALSE;
static float camera_look_x = 0, camera_look_y = 0, camera_look_z = 0;

static void gl_init_attributes(void)
{
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 5);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 5);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 5);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
}

static void gl_init(GLint width, GLint height)
{
	glShadeModel(GL_SMOOTH);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
}

static void gl_draw_grid(int width)
{
	int i;

	glLineWidth(width);

	glBegin(GL_LINES);

	for (i = GRID_WEST; i <= GRID_EAST; i += GRID_STRIDE)
	{
		if (i % (8 * 8))
			glColor3f(0.3, 0.3, 0.3);
		else if (i == 0)
			glColor3f(0.0, 0.5, 0.5);
		else
			glColor3f(0.5, 0.5, 0.5);
		glVertex3i(i, GRID_NORTH, 0);
		glVertex3i(i, GRID_SOUTH, 0);
	}

	for (i = GRID_NORTH; i <= GRID_SOUTH; i += GRID_STRIDE)
	{
		if (i % (8 * 8))
			glColor3f(0.3, 0.3, 0.3);
		else if (i == 0)
			glColor3f(0.0, 0.5, 0.5);
		else
			glColor3f(0.5, 0.5, 0.5);
		glVertex3i(GRID_WEST, i, 0);
		glVertex3i(GRID_EAST, i, 0);
	}

	glEnd();
}

static void gl_draw_2d(void)
{
	int i;

	glViewport(screen->w / 2, 0, screen->w / 2, screen->h);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluOrtho2D(0, screen->w / 2, screen->h, 0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(config.grid_offset_x, config.grid_offset_y, 0);
	glScalef(config.grid_scale, config.grid_scale, 0);

	gl_draw_grid(1);
}

static void gl_draw_3d(void)
{
	int i;
	GLfloat w = (GLfloat)(screen->w / 2) / (GLfloat)screen->h;
	GLfloat h = (GLfloat)screen->h / (GLfloat)(screen->w / 2);

	glViewport(0, 0, screen->w / 2, screen->h);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(FOV * h, w, ZNEAR, ZFAR);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	config.camera_x = 128;
	config.camera_y = 128;
	config.camera_z = 128;

	camera_look_x = cos(DEG2RAD(config.camera_yaw)) * cos(DEG2RAD(config.camera_pitch));
	camera_look_y = sin(DEG2RAD(config.camera_yaw)) * cos(DEG2RAD(config.camera_pitch));
	camera_look_z = sin(DEG2RAD(config.camera_pitch));

	gluLookAt(
		config.camera_x, config.camera_y, config.camera_z,
		config.camera_x + camera_look_x,
		config.camera_y + camera_look_y,
		config.camera_z + camera_look_z,
		0, 0, 1
	);

	gl_draw_grid(2);
}

void engine_quit(int code)
{
	/* save config */
	config.window_w = screen->w;
	config.window_h = screen->h;
	config_save(DEFAULT_CONFIG, &config);

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

void engine_tick(void)
{
	/* handle movement in views and lock mouse */
	if (buttonstate[SDL_BUTTON_RIGHT])
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
			config.camera_yaw -= mouse_xrel;
			config.camera_pitch -= mouse_yrel;

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

void engine_render(void)
{
	/* clear screen */
	glClearColor(0.1, 0.1, 0.1, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/* draw 2d view */
	gl_draw_2d();

	/* draw 3d view */
	gl_draw_3d();

	/* do double buffering */
	SDL_GL_SwapBuffers();
}

void engine_events(void)
{
	SDL_Event event;

	keys = SDL_GetKeyState(NULL);

	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
			case SDL_QUIT:
			{
				engine_quit(0);
			}

			case SDL_MOUSEBUTTONDOWN:
			{
				buttonstate[event.button.button] = SDL_TRUE;
				break;
			}

			case SDL_MOUSEBUTTONUP:
			{
				buttonstate[event.button.button] = SDL_FALSE;
				break;
			}

			case SDL_MOUSEMOTION:
			{
				if (!mouse_lock)
				{
					mouse_x = event.motion.x;
					mouse_y = event.motion.y;
				}

				mouse_xrel = event.motion.xrel;
				mouse_yrel = event.motion.yrel;
				break;
			}

			case SDL_VIDEOEXPOSE:
			{
				engine_render();
				break;
			}

			case SDL_VIDEORESIZE:
			{
				int w, h, bpp, flags;

				w = event.resize.w;
				h = event.resize.h;
				bpp = screen->format->BitsPerPixel;
				flags = SDL_OPENGL | SDL_RESIZABLE;

				if ((screen = SDL_SetVideoMode(w, h, bpp, flags)) == NULL)
					engine_error("SDL: %s", SDL_GetError());

				break;
			}
		}
	}
}

void engine_frame(Uint32 dt)
{
	SDL_bool do_tick, do_render;

	/* let the system handle anything it needs to */

	engine_events();

	/* add dt to time counters */

	tick_delta += dt;
	render_delta += dt;

	/* check if we should do a tick or render frame */

	if (tick_delta > (1000 / TICKRATE))
		do_tick = SDL_TRUE;
	else
		do_tick = SDL_FALSE;

	if (render_delta > (1000 / FRAMERATE))
		do_render = SDL_TRUE;
	else
		do_render = SDL_FALSE;

	/* do the frames */

	if (do_tick)
	{
		engine_tick();
		tick_delta = 0;
		num_ticks++;
	}

	if (do_render)
	{
		engine_render();
		render_delta = 0;
		num_frames++;
	}
}

void engine_main(void)
{
	Uint32 start, now, then;
	const SDL_VideoInfo *info;
	int w, h, flags, bpp;

	/* init sdl */
	if (SDL_Init(SDL_INIT_VIDEO) != 0)
		engine_error("SDL: %s", SDL_GetError());

	/* clear buttonstate */
	SDL_memset(buttonstate, 0, sizeof(buttonstate));

	/* load config */
	config_reset(&config);
	config_load(DEFAULT_CONFIG, &config);

	/* query available video mode */
	info = SDL_GetVideoInfo();

	/* setup GL attributes */
	gl_init_attributes();

	/* setup window attributes */
	if (config.window_w <= 0)
		w = info->current_w;
	else if (info->current_w < config.window_w)
		w = info->current_w;
	else
		w = config.window_w;

	if (config.window_h <= 0)
		h = info->current_h;
	else if (info->current_h < config.window_h)
		h = info->current_h;
	else
		h = config.window_h;

	bpp = info->vfmt->BitsPerPixel;
	flags = SDL_OPENGL | SDL_RESIZABLE;

	/* create window */
	if ((screen = SDL_SetVideoMode(w, h, bpp, flags)) == NULL)
		engine_error("SDL: %s", SDL_GetError());

	/* set window title */
	SDL_WM_SetCaption(TITLE, NULL);

	/* setup gl stuff */
	gl_init(w, h);

	/* start counting time and run main loop */
	start = then = SDL_GetTicks();
	while (1)
	{
		/* calculate dt and run engine frame */
		now = SDL_GetTicks();
		engine_frame(now - then);
		then = now;

		/* calculate framerate and tickrate for the sickos who care */
		framerate = ((float)num_frames / (float)(SDL_GetTicks() - start)) * 1000.0f;
		tickrate = ((float)num_ticks / (float)(SDL_GetTicks() - start)) * 1000.0f;
	}
}

int main(int argc, char **argv)
{
	engine_main();
	return 0;
}
