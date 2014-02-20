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

static const GLfloat red[]   = {1.0f, 0.0f, 0.0f, 1.0f};
static const GLfloat green[] = {0.0f, 1.0f, 0.0f, 1.0f};
static const GLfloat blue[]  = {0.0f, 0.0f, 1.0f, 1.0f};
static const GLfloat black[] = {0.0f, 0.0f, 0.0f, 1.0f};

static void
setup_front_buffer(void)
{
	glDrawBuffer(GL_BACK);

	glClearColor(blue[0], blue[1], blue[2], blue[3]);
	glClear(GL_COLOR_BUFFER_BIT);

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

	/* Clear to green */
	glClearColor(green[0], green[1], green[2], green[3]);
	glClear(GL_COLOR_BUFFER_BIT);

	/* Paint a red square in the middle of the back buffer */
	glColor4fv(red);
	piglit_draw_rect(w / 4, h / 4, w / 2, h / 2);
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

enum piglit_result piglit_display(void)
{
	int w = piglit_width;
	int h = piglit_height;
	bool pass = true;

	piglit_ortho_projection(w, h, GL_FALSE);

	setup_front_buffer();
	setup_back_buffer();

	/* Copy just the red square from the back buffer to the blue front buffer */
	blit_from_back_to_front(w / 4, h / 4, 3 * w / 4, 3 * h / 4);

	glFlush();

	/* Now see how we did... */

	glReadBuffer(GL_FRONT);

	/* the middle should be red */
	pass = pass && piglit_probe_pixel_rgb(w / 2, h / 2, red);

	/* the corners should be blue */
	pass = pass && piglit_probe_pixel_rgb(0, 0, blue);
	pass = pass && piglit_probe_pixel_rgb(w - 1, 0, blue);
	pass = pass && piglit_probe_pixel_rgb(0, h - 1, blue);
	pass = pass && piglit_probe_pixel_rgb(w - 1, h - 1, blue);

	/* Blit some green goodness */

	blit_from_back_to_front(0, 0, w / 4, h / 4);
	glFlush();

	pass = pass && piglit_probe_pixel_rgb(0, 0, green);

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
