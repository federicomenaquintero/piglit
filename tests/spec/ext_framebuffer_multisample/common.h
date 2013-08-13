/* Copyright © 2012 Intel Corporation
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
 */

/**
 * \file common.h
 * This file declares functions which can be utilized to develop new multisample
 * test cases.
 */

#include "piglit-util-gl-common.h"
#include "piglit-test-pattern.h"
#include "piglit-fbo.h"
#include "math.h"

enum test_type_enum {
	TEST_TYPE_COLOR,
	TEST_TYPE_SRGB,
	TEST_TYPE_STENCIL_DRAW,
	TEST_TYPE_STENCIL_RESOLVE,
	TEST_TYPE_DEPTH_DRAW,
	TEST_TYPE_DEPTH_RESOLVE,
};

/**
 * Fragment shader program we apply to the supersampled color buffer
 * to produce the reference image.  This program manually blends each
 * 16x16 block of samples in the supersampled color buffer down to a
 * single sample in the downsampled buffer.
 */
class DownsampleProg
{
public:
	void compile(int supersample_factor);
	void run(const Fbo *src_fbo, int dest_width, int dest_height,
		 bool srgb);

private:
	GLint prog;
	GLuint vertex_buf;
	GLuint vao;
};

/**
 * Data structure for keeping track of statistics on pixel accuracy.
 *
 * We keep track of the number of pixels tested, and the sum of the
 * squared error, so that we can summarize the RMS error at the
 * conclusion of the test.
 */
class Stats
{
public:
	Stats();

	void record(float error)
	{
		++count;
		sum_squared_error += error * error;
	}

	void summarize();

	bool is_perfect();

	bool is_better_than(double rms_error_threshold);

private:
	int count;
	double sum_squared_error;
};

/**
 * This data structure wraps up all the data we need to keep track of
 * to run the test.
 */
class Test
{
public:
	Test(TestPattern *pattern, ManifestProgram *manifest_program,
	     bool test_resolve, GLbitfield blit_type, bool srgb);
	void init(int num_samples, bool small, bool combine_depth_stencil,
		  int pattern_width, int pattern_height,
		  int supersample_factor);
	bool run();
	void draw_test_image(Fbo *fbo);
	void draw_to_default_framebuffer();
	void draw_reference_image();
	bool measure_accuracy();

	/**
	 * Fbo that we use to just draw test image
	 */
	Fbo test_fbo;

private:
	void resolve(Fbo *fbo, GLbitfield which_buffers);
	void downsample_color(int downsampled_width, int downsampled_height);
	void show(Fbo *src_fbo, int x_offset, int y_offset);
	void draw_pattern(int x_offset, int y_offset, int width, int height);

	/** The test pattern to draw. */
	TestPattern *pattern;

	/**
	 * The program to use to manifest depth or stencil into color,
	 * or NULL if we're just testing color rendering.
	 */
	ManifestProgram *manifest_program;

	/**
	 * True if we are testing the resolve pass, so we should
	 * downsample before manifesting; false if we should manifest
	 * before downsampling.
	 */
	bool test_resolve;

	/**
	 * The buffer under test--this should be compatible with the
	 * "mask" argument of
	 * glBlitFramebuffer--i.e. GL_COLOR_BUFFER_BIT,
	 * GL_STENCIL_BUFFER_BIT, or GL_DEPTH_BUFFER_BIT.
	 */
	GLbitfield blit_type;

	/**
	 * Fbo that we perform MSAA rendering into.
	 */
	Fbo multisample_fbo;

	/**
	 * Single-sampled fbo that we blit into to force the
	 * implementation to resolve MSAA buffers.
	 */
	Fbo resolve_fbo;

	/**
	 * Large fbo that we perform high-resolution ("supersampled")
	 * rendering into.
	 */
	Fbo supersample_fbo;

	/**
	 * Normal-sized fbo that we manually downsample the
	 * supersampled render result into, to create the reference
	 * image.
	 */
	Fbo downsample_fbo;

	int num_samples;
	int pattern_width;
	int pattern_height;
	int supersample_factor;
	bool srgb;
	DownsampleProg downsample_prog;
};

Test *
create_test(test_type_enum test_type, int n_samples, bool small,
	    bool combine_depth_stencil, int pattern_width,
	    int pattern_height, int supersample_factor);

void
visualize_image(float *img, GLenum base_internal_format,
		int image_width, int image_height,
		int draw_buffer_count, bool rhs);
