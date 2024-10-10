
#include "bisected.h"

#include "SDL_opengl.h"

static config_t *config = NULL;
static SDL_Surface *screen = NULL;
static float camera_look_x = 0, camera_look_y = 0, camera_look_z = 0;

static void draw_grid(int line_width)
{
	int i;

	glLineWidth(line_width);

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

static void draw_2d_view(void)
{
	glViewport(screen->w / 2, 0, screen->w / 2, screen->h);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluOrtho2D(0, screen->w / 2, screen->h, 0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(config->grid_offset_x, config->grid_offset_y, 0);
	glScalef(config->grid_scale, config->grid_scale, 0);

	draw_grid(1);
}

static void draw_3d_view(void)
{
	float vfov;
	float aspect_x = (float)(screen->w / 2) / (float)(screen->h);
	float aspect_y = (float)(screen->h) / (float)(screen->w / 2);

	glViewport(0, 0, screen->w / 2, screen->h);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	vfov = 2 * atan(tan(DEG2RAD(config->camera_fov) / 2) * aspect_y);
	gluPerspective(RAD2DEG(vfov), aspect_x, config->camera_near, config->camera_far);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	camera_look_x = cos(DEG2RAD(config->camera_yaw)) * cos(DEG2RAD(config->camera_pitch));
	camera_look_y = sin(DEG2RAD(config->camera_yaw)) * cos(DEG2RAD(config->camera_pitch));
	camera_look_z = sin(DEG2RAD(config->camera_pitch));

	gluLookAt(
		config->camera_x, config->camera_y, config->camera_z,
		config->camera_x + camera_look_x,
		config->camera_y + camera_look_y,
		config->camera_z + camera_look_z,
		0, 0, 1
	);

	draw_grid(2);
}

void vid_draw(void)
{
	/* clear screen */
	glClearColor(0.1, 0.1, 0.1, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/* draw views */
	draw_2d_view();
	draw_3d_view();

	/* do double buffering */
	SDL_GL_SwapBuffers();
}

static void init_gl_attributes(void)
{
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 5);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 5);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 5);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
}

static void init_gl(void)
{
	glShadeModel(GL_SMOOTH);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
}

SDL_Surface *vid_resize(int w, int h)
{
	int bpp, flags;

	bpp = screen->format->BitsPerPixel;
	flags = SDL_OPENGL | SDL_RESIZABLE;

	if ((screen = SDL_SetVideoMode(w, h, bpp, flags)) == NULL)
		engine_error("SDL: %s", SDL_GetError());

	return screen;
}

SDL_Surface *vid_init(config_t *cfg)
{
	int w, h, bpp, flags;
	const SDL_VideoInfo *info;

	/* init sdl video subsystem */
	if (!SDL_WasInit(SDL_INIT_VIDEO))
		if (SDL_Init(SDL_INIT_VIDEO) != 0)
			engine_error("SDL: %s", SDL_GetError());

	/* copy config pointer */
	config = cfg;

	/* get current video mode */
	info = SDL_GetVideoInfo();

	/* setup gl attributes */
	init_gl_attributes();

	/* create window */
	w = config->window_w > info->current_w ? info->current_w : config->window_w;
	h = config->window_h > info->current_h ? info->current_h : config->window_h;
	bpp = info->vfmt->BitsPerPixel;
	flags = SDL_OPENGL | SDL_RESIZABLE;
	if ((screen = SDL_SetVideoMode(w, h, bpp, flags)) == NULL)
		engine_error("SDL: %s", SDL_GetError());

	/* set window title */
	SDL_WM_SetCaption(config->window_title, NULL);

	/* setup gl state */
	init_gl();

	/* return screen */
	return screen;
}

void vid_quit(void)
{

}
