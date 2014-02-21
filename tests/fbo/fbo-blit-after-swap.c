/*
 * Copyright 2013 Suse, Inc. 
 * Copyright © 2011 Henri Verbeet <hverbeet@gmail.com>
 * Copyright 2011 Red Hat, Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 *
 */

/** @file fbo-blit-after-swap.c
 *
 * Test a glBlitFrameBuffer() with a smaller-than-the-window region after doing a buffer swap
 */

#include "piglit-util-gl-common.h"

PIGLIT_GL_TEST_CONFIG_BEGIN

	config.supports_gl_compat_version = 10;

	config.window_visual = PIGLIT_GL_VISUAL_DOUBLE | PIGLIT_GL_VISUAL_RGB;
	config.requires_displayed_window = true;

PIGLIT_GL_TEST_CONFIG_END

static bool use_swap_buffers = true;

static const GLfloat red[]     = {1.0f, 0.0f, 0.0f, 1.0f};
static const GLfloat green[]   = {0.0f, 1.0f, 0.0f, 1.0f};
static const GLfloat blue[]    = {0.0f, 0.0f, 1.0f, 1.0f};
static const GLfloat magenta[] = {1.0f, 0.0f, 1.0f, 1.0f};
static const GLfloat black[]   = {0.0f, 0.0f, 0.0f, 1.0f};

static void
clear_to_color4fv(const GLfloat *color)
{
	glClearColor(color[0], color[1], color[2], color[3]);
	glClear(GL_COLOR_BUFFER_BIT);
}

static void
draw_rect_with_color4fv(const GLfloat *color, int x, int y, int w, int h)
{
	glColor4fv(color);
	piglit_draw_rect(x, y, w, h);
}

static void
setup_front_buffer(void)
{
	glDrawBuffer(GL_BACK);
	clear_to_color4fv(blue);

	if (use_swap_buffers)
		piglit_swap_buffers();
	else {
		glDrawBuffer(GL_FRONT);
		glReadBuffer(GL_BACK);
		glBlitFramebufferEXT(0, 0, piglit_width, piglit_height,
				     0, 0, piglit_width, piglit_height,
				     GL_COLOR_BUFFER_BIT, GL_NEAREST);
	}
}

static void
setup_back_buffer(void)
{
	int w = piglit_width;
	int h = piglit_height;

	glDrawBuffer(GL_BACK);

	clear_to_color4fv(green);
	draw_rect_with_color4fv(red, w / 4, h / 4, w / 2, h / 2);
}

static void
blit_from_back_to_front(int x0, int y0, int x1, int y1)
{
	glDrawBuffer(GL_FRONT);
	glReadBuffer(GL_BACK);
	glBlitFramebufferEXT(x0, y0, x1, y1,
			     x0, y0, x1, y1,
			     GL_COLOR_BUFFER_BIT, GL_NEAREST);
}

static bool
test_corner_and_center_colors(const GLfloat *corner, const GLfloat *center)
{
	int w = piglit_width;
	int h = piglit_height;
	bool pass = true;

	glReadBuffer(GL_FRONT);
	pass = pass && piglit_probe_pixel_rgb(0,     0,     corner);
	pass = pass && piglit_probe_pixel_rgb(w - 1, 0,     corner);
	pass = pass && piglit_probe_pixel_rgb(0,     h - 1, corner);
	pass = pass && piglit_probe_pixel_rgb(w - 1, h - 1, corner);
	pass = pass && piglit_probe_pixel_rgb(w / 2, h / 2, center);

	return pass;
}

enum piglit_result piglit_display(void)
{
	int w = piglit_width;
	int h = piglit_height;
	bool pass = true;

	piglit_ortho_projection(w, h, GL_FALSE);

	glDrawBuffer(GL_BACK);

	/* First, blue background, green square in the middle */

	clear_to_color4fv(blue);
	draw_rect_with_color4fv(green, w / 4, h /4, w / 2, h / 2);
	piglit_swap_buffers();
	glFlush();

	pass = pass && test_corner_and_center_colors(blue, green);

	sleep (1);

	/* Second, blue background, red square in the middle */

	clear_to_color4fv(blue);
	draw_rect_with_color4fv(red, w / 4, h /4, w / 2, h / 2);

	/* Blit just the magenta square */
	blit_from_back_to_front(w / 4, h / 4, w / 4 + w / 2, h / 4 + h / 2);
	glFlush();

	pass = pass && test_corner_and_center_colors(blue, red);
	
	return pass ? PIGLIT_PASS : PIGLIT_FAIL;
}

void piglit_init(int argc, char **argv)
{
	int i;

	for (i = 1; i < argc; i++) {
		if (!strcmp(argv[i], "no-swap"))
			use_swap_buffers = false;
	}

	piglit_require_extension("GL_EXT_framebuffer_object");
	piglit_require_extension("GL_EXT_framebuffer_blit");
}
