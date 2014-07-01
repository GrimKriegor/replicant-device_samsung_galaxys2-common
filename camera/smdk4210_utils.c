/*
 * Copyright (C) 2013-2014 Paul Kocialkowski <contact@paulk.fr>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdlib.h>

#define LOG_TAG "smdk4210_camera"
#include <utils/Log.h>

#include "smdk4210_camera.h"

int smdk4210_camera_buffer_length(int width, int height, int format)
{
	float bpp;
	int buffer_length;

	switch (format) {
		case V4L2_PIX_FMT_RGB32:
			bpp = 4.0f;
			buffer_length = (int) ((float) width * (float) height * bpp);
			break;
		case V4L2_PIX_FMT_RGB565:
		case V4L2_PIX_FMT_YUYV:
		case V4L2_PIX_FMT_UYVY:
		case V4L2_PIX_FMT_VYUY:
		case V4L2_PIX_FMT_YVYU:
		case V4L2_PIX_FMT_YUV422P:
		case V4L2_PIX_FMT_NV16:
		case V4L2_PIX_FMT_NV61:
			bpp = 2.0f;
			buffer_length = (int) ((float) width * (float) height * bpp);
			break;
		case V4L2_PIX_FMT_NV12:
		case V4L2_PIX_FMT_NV12T:
		case V4L2_PIX_FMT_YUV420:
		case V4L2_PIX_FMT_YVU420:
			bpp = 1.5f;
			buffer_length = SMDK4210_CAMERA_ALIGN(width * height);
			buffer_length += SMDK4210_CAMERA_ALIGN(width * height / 2);
			break;
		case V4L2_PIX_FMT_NV21:
			bpp = 1.5f;
			buffer_length = (int) ((float) width * (float) height * bpp);
			break;
		case V4L2_PIX_FMT_JPEG:
		case V4L2_PIX_FMT_INTERLEAVED:
		default:
			buffer_length = -1;
			bpp = 0;
			break;
	}

	return buffer_length;
}

int smdk4210_gralloc_format(int format)
{
	switch (format) {
		case V4L2_PIX_FMT_NV21:
			return HAL_PIXEL_FORMAT_YCrCb_420_SP;
		case V4L2_PIX_FMT_YUV420:
			return HAL_PIXEL_FORMAT_YV12;
		case V4L2_PIX_FMT_RGB565:
			return HAL_PIXEL_FORMAT_RGB_565;
		case V4L2_PIX_FMT_RGB32:
			return HAL_PIXEL_FORMAT_RGBX_8888;
		default:
			return HAL_PIXEL_FORMAT_YCrCb_420_SP;
	}
}

int smdk4210_scale_yuv422(void *src, int src_width, int src_height, void *dst,
	int dst_width, int dst_height)
{
	unsigned char *src_p, *dst_p;
	int stride_x, stride_y;
	int step_x, step_y;
	int start_y;
	int x, y;

	step_x = src_width / dst_width;
	step_y = src_height / dst_height;

	stride_x = step_x * 2;
	stride_y = src_width * 2;

	dst_p = (unsigned char *) dst;

	for (y = 0; y < dst_height; y++) {
		start_y = stride_y * step_y * y;

		for (x = 0; x < dst_width; x += 2) {
			src_p = (unsigned char *) src + start_y + stride_x * x;

			*dst_p++ = *src_p++;
			*dst_p++ = *src_p++;
			*dst_p++ = *src_p++;
			*dst_p++ = *src_p++;
		}
	}

	return 0;
}
