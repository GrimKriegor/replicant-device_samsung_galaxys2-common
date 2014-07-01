/*
 * Copyright (C) 2013 Paul Kocialkowski <contact@paulk.fr>
 *
 * Based on crespo libcamera and exynos4 hal libcamera:
 * Copyright 2008, The Android Open Source Project, Apache License 2.0
 * Copyright 2010, Samsung Electronics Co. LTD, Apache License 2.0
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

#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include <malloc.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

#include <asm/types.h>
#include <jpeg_api.h>

#define LOG_TAG "smdk4210_camera"
#include <utils/Log.h>
#include <utils/Timers.h>

#include "smdk4210_camera.h"

/*
 * Devices configurations
 */

struct smdk4210_camera_preset smdk4210_camera_presets_galaxys2[] = {
	{
		.name = "M5MO",
		.facing = CAMERA_FACING_BACK,
		.orientation = 90,
		.rotation = 0,
		.hflip = 0,
		.vflip = 0,
		.picture_format = V4L2_PIX_FMT_JPEG,
		.focal_length = 4.03f,
		.horizontal_view_angle = 60.5f,
		.vertical_view_angle = 47.1f,
		.metering = METERING_CENTER,
		.params = {
			.preview_size_values = "800x480,720x480,640x480,320x240,176x144",
			.preview_size = "640x480",
			.preview_format_values = "yuv420sp,yuv420p,rgb565",
			.preview_format = "rgb565",
			.preview_frame_rate_values = "30,25,20,15,10,7",
			.preview_frame_rate = 30,
			.preview_fps_range_values = "(7000,30000)",
			.preview_fps_range = "7000,30000",

			.picture_size_values = "3264x2448,3264x1968,2048x1536,2048x1232,1280x960,800x480,640x480",
			.picture_size = "3264x2448",
			.picture_format_values = "jpeg",
			.picture_format = "jpeg",
			.jpeg_thumbnail_size_values = "320x240,400x240",
			.jpeg_thumbnail_width = 320,
			.jpeg_thumbnail_height = 240,
			.jpeg_thumbnail_quality = 100,
			.jpeg_quality = 90,

			.recording_size = "720x480",
			.recording_size_values = "1920x1080,1280x720,720x480,640x480",
			.recording_format = "yuv420sp",

			.focus_mode = "auto",
			.focus_mode_values = "auto,infinity,macro,fixed,facedetect,continuous-video",
			.focus_distances = "0.15,1.20,Infinity",
			.focus_areas = "(0,0,0,0,0)",
			.max_num_focus_areas = 1,

			.zoom_supported = 1,
			.smooth_zoom_supported = 0,
			.zoom_ratios = "100,102,104,109,111,113,119,121,124,131,134,138,146,150,155,159,165,170,182,189,200,213,222,232,243,255,283,300,319,364,400",
			.zoom = 0,
			.max_zoom = 30,

			.flash_mode = "off",
			.flash_mode_values = "off,auto,on,torch",

			.exposure_compensation = 0,
			.exposure_compensation_step = 0.5,
			.min_exposure_compensation = -4,
			.max_exposure_compensation = 4,

			.whitebalance = "auto",
			.whitebalance_values = "auto,incandescent,fluorescent,daylight,cloudy-daylight",

			.scene_mode = "auto",
			.scene_mode_values = "auto,portrait,landscape,night,beach,snow,sunset,fireworks,sports,party,candlelight,dusk-dawn,fall-color,back-light,text",

			.effect = "none",
			.effect_values = "none,mono,negative,sepia,aqua",

			.iso = "auto",
			.iso_values = "auto,ISO50,ISO100,ISO200,ISO400,ISO800",
		},
	},
	{
		.name = "S5K5BAFX",
		.facing = CAMERA_FACING_FRONT,
		.orientation = 270,
		.rotation = 0,
		.hflip = 0,
		.vflip = 0,
		.picture_format = V4L2_PIX_FMT_YUYV,
		.focal_length = 2.73f,
		.horizontal_view_angle = 51.2f,
		.vertical_view_angle = 39.4f,
		.metering = METERING_CENTER,
		.params = {
			.preview_size_values = "640x480,352x288,320x240,176x144",
			.preview_size = "640x480",
			.preview_format_values = "yuv420sp,yuv420p,rgb565",
			.preview_format = "rgb565",
			.preview_frame_rate_values = "30,25,20,15,10,7",
			.preview_frame_rate = 30,
			.preview_fps_range_values = "(7000,30000)",
			.preview_fps_range = "7000,30000",

			.picture_size_values = "1600x1200,640x480",
			.picture_size = "1600x1200",
			.picture_format_values = "jpeg",
			.picture_format = "jpeg",
			.jpeg_thumbnail_size_values = "160x120",
			.jpeg_thumbnail_width = 160,
			.jpeg_thumbnail_height = 120,
			.jpeg_thumbnail_quality = 100,
			.jpeg_quality = 90,

			.recording_size = "640x480",
			.recording_size_values = "720x480,640x480",
			.recording_format = "yuv420sp",

			.focus_mode = "fixed",
			.focus_mode_values = "fixed",
			.focus_distances = "0.20,0.25,Infinity",
			.focus_areas = NULL,
			.max_num_focus_areas = 0,

			.zoom_supported = 0,

			.flash_mode = NULL,
			.flash_mode_values = NULL,

			.exposure_compensation = 0,
			.exposure_compensation_step = 0.5,
			.min_exposure_compensation = -4,
			.max_exposure_compensation = 4,

			.whitebalance = NULL,
			.whitebalance_values = NULL,

			.scene_mode = NULL,
			.scene_mode_values = NULL,

			.effect = NULL,
			.effect_values = NULL,

			.iso = "auto",
			.iso_values = "auto",
		},
	},
};

struct smdk4210_v4l2_node smdk4210_v4l2_nodes_galaxys2[] = {
	{
		.id = 0,
		.node = "/dev/video0",
	},
	{
		.id = 1,
		.node = "/dev/video1",
	},
	{
		.id = 2,
		.node = "/dev/video2",
	},
};

struct exynox_camera_config smdk4210_camera_config_galaxys2 = {
	.presets = (struct smdk4210_camera_preset *) &smdk4210_camera_presets_galaxys2,
	.presets_count = 2,
	.v4l2_nodes = (struct smdk4210_v4l2_node *) &smdk4210_v4l2_nodes_galaxys2,
	.v4l2_nodes_count = 3,
};

/*
 * SMDK4210 Camera
 */

struct exynox_camera_config *smdk4210_camera_config =
	&smdk4210_camera_config_galaxys2;

int smdk4210_camera_init(struct smdk4210_camera *smdk4210_camera, int id)
{
	char firmware_version[7] = { 0 };
	struct smdk4210_v4l2_ext_control control;
	int rc;

	if (smdk4210_camera == NULL || id >= smdk4210_camera->config->presets_count)
		return -EINVAL;

	// Init FIMC1
	rc = smdk4210_v4l2_open(smdk4210_camera, 0);
	if (rc < 0) {
		ALOGE("Unable to open v4l2 device");
		return -1;
	}

	rc = smdk4210_v4l2_querycap_cap(smdk4210_camera, 0);
	if (rc < 0) {
		ALOGE("%s: querycap failed", __func__);
		return -1;
	}

	rc = smdk4210_v4l2_enum_input(smdk4210_camera, 0, id);
	if (rc < 0) {
		ALOGE("%s: enum input failed", __func__);
		return -1;
	}

	rc = smdk4210_v4l2_s_input(smdk4210_camera, 0, id);
	if (rc < 0) {
		ALOGE("%s: s input failed", __func__);
		return -1;
	}

	// Init FIMC2
	rc = smdk4210_v4l2_open(smdk4210_camera, 2);
	if (rc < 0) {
		ALOGE("Unable to open v4l2 device");
		return -1;
	}

	rc = smdk4210_v4l2_querycap_cap(smdk4210_camera, 2);
	if (rc < 0) {
		ALOGE("%s: querycap failed", __func__);
		return -1;
	}

	rc = smdk4210_v4l2_enum_input(smdk4210_camera, 2, id);
	if (rc < 0) {
		ALOGE("%s: enum input failed", __func__);
		return -1;
	}

	rc = smdk4210_v4l2_s_input(smdk4210_camera, 2, id);
	if (rc < 0) {
		ALOGE("%s: s input failed", __func__);
		return -1;
	}

	// Get firmware information
	memset(&control, 0, sizeof(control));
	control.id = V4L2_CID_CAM_SENSOR_FW_VER;
	control.data.string = firmware_version;

	rc = smdk4210_v4l2_g_ext_ctrls(smdk4210_camera, 0, (struct v4l2_ext_control *) &control, 1);
	if (rc < 0) {
		ALOGE("%s: g ext ctrls failed", __func__);
	} else {
		ALOGD("Firmware version: %s", firmware_version);
	}

	// Params
	rc = smdk4210_camera_params_init(smdk4210_camera, id);
	if (rc < 0)
		ALOGE("%s: Unable to init params", __func__);

	// Gralloc
	rc = hw_get_module(GRALLOC_HARDWARE_MODULE_ID, (const struct hw_module_t **) &smdk4210_camera->gralloc);
	if (rc)
		ALOGE("%s: Unable to get gralloc module", __func__);

	return 0;
}

void smdk4210_camera_deinit(struct smdk4210_camera *smdk4210_camera)
{
	int i;
	int id;

	if (smdk4210_camera == NULL || smdk4210_camera->config == NULL)
		return;

	smdk4210_v4l2_close(smdk4210_camera, 0);
	smdk4210_v4l2_close(smdk4210_camera, 2);
}

// Params

int smdk4210_camera_params_init(struct smdk4210_camera *smdk4210_camera, int id)
{
	int rc;

	if (smdk4210_camera == NULL || id >= smdk4210_camera->config->presets_count)
		return -EINVAL;

	// Camera params
	smdk4210_camera->camera_rotation = smdk4210_camera->config->presets[id].rotation;
	smdk4210_camera->camera_hflip = smdk4210_camera->config->presets[id].hflip;
	smdk4210_camera->camera_vflip = smdk4210_camera->config->presets[id].vflip;
	smdk4210_camera->camera_picture_format = smdk4210_camera->config->presets[id].picture_format;
	smdk4210_camera->camera_focal_length = (int) (smdk4210_camera->config->presets[id].focal_length * 100);
	smdk4210_camera->camera_metering = smdk4210_camera->config->presets[id].metering;

	// Recording preview
	smdk4210_param_string_set(smdk4210_camera, "preferred-preview-size-for-video",
		smdk4210_camera->config->presets[id].params.preview_size);

	// Preview
	smdk4210_param_string_set(smdk4210_camera, "preview-size-values",
		smdk4210_camera->config->presets[id].params.preview_size_values);
	smdk4210_param_string_set(smdk4210_camera, "preview-size",
		smdk4210_camera->config->presets[id].params.preview_size);
	smdk4210_param_string_set(smdk4210_camera, "preview-format-values",
		smdk4210_camera->config->presets[id].params.preview_format_values);
	smdk4210_param_string_set(smdk4210_camera, "preview-format",
		smdk4210_camera->config->presets[id].params.preview_format);
	smdk4210_param_string_set(smdk4210_camera, "preview-frame-rate-values",
		smdk4210_camera->config->presets[id].params.preview_frame_rate_values);
	smdk4210_param_int_set(smdk4210_camera, "preview-frame-rate",
		smdk4210_camera->config->presets[id].params.preview_frame_rate);
	smdk4210_param_string_set(smdk4210_camera, "preview-fps-range-values",
		smdk4210_camera->config->presets[id].params.preview_fps_range_values);
	smdk4210_param_string_set(smdk4210_camera, "preview-fps-range",
		smdk4210_camera->config->presets[id].params.preview_fps_range);

	// Picture
	smdk4210_param_string_set(smdk4210_camera, "picture-size-values",
		smdk4210_camera->config->presets[id].params.picture_size_values);
	smdk4210_param_string_set(smdk4210_camera, "picture-size",
		smdk4210_camera->config->presets[id].params.picture_size);
	smdk4210_param_string_set(smdk4210_camera, "picture-format-values",
		smdk4210_camera->config->presets[id].params.picture_format_values);
	smdk4210_param_string_set(smdk4210_camera, "picture-format",
		smdk4210_camera->config->presets[id].params.picture_format);
	smdk4210_param_string_set(smdk4210_camera, "jpeg-thumbnail-size-values",
		smdk4210_camera->config->presets[id].params.jpeg_thumbnail_size_values);
	smdk4210_param_int_set(smdk4210_camera, "jpeg-thumbnail-width",
		smdk4210_camera->config->presets[id].params.jpeg_thumbnail_width);
	smdk4210_param_int_set(smdk4210_camera, "jpeg-thumbnail-height",
		smdk4210_camera->config->presets[id].params.jpeg_thumbnail_height);
	smdk4210_param_int_set(smdk4210_camera, "jpeg-thumbnail-quality",
		smdk4210_camera->config->presets[id].params.jpeg_thumbnail_quality);
	smdk4210_param_int_set(smdk4210_camera, "jpeg-quality",
		smdk4210_camera->config->presets[id].params.jpeg_quality);

	// Recording
	smdk4210_param_string_set(smdk4210_camera, "video-size",
		smdk4210_camera->config->presets[id].params.recording_size);
	smdk4210_param_string_set(smdk4210_camera, "video-size-values",
		smdk4210_camera->config->presets[id].params.recording_size_values);
	smdk4210_param_string_set(smdk4210_camera, "video-frame-format",
		smdk4210_camera->config->presets[id].params.recording_format);

	// Focus
	smdk4210_param_string_set(smdk4210_camera, "focus-mode",
		smdk4210_camera->config->presets[id].params.focus_mode);
	smdk4210_param_string_set(smdk4210_camera, "focus-mode-values",
		smdk4210_camera->config->presets[id].params.focus_mode_values);
	smdk4210_param_string_set(smdk4210_camera, "focus-distances",
		smdk4210_camera->config->presets[id].params.focus_distances);
	if (smdk4210_camera->config->presets[id].params.max_num_focus_areas > 0) {
		smdk4210_param_string_set(smdk4210_camera, "focus-areas",
			smdk4210_camera->config->presets[id].params.focus_areas);
		smdk4210_param_int_set(smdk4210_camera, "max-num-focus-areas",
			smdk4210_camera->config->presets[id].params.max_num_focus_areas);
	}

	// Zoom
	if (smdk4210_camera->config->presets[id].params.zoom_supported == 1) {
		smdk4210_param_string_set(smdk4210_camera, "zoom-supported", "true");

		if (smdk4210_camera->config->presets[id].params.smooth_zoom_supported == 1)
			smdk4210_param_string_set(smdk4210_camera, "smooth-zoom-supported", "true");

		if (smdk4210_camera->config->presets[id].params.zoom_ratios != NULL)
			smdk4210_param_string_set(smdk4210_camera, "zoom-ratios", smdk4210_camera->config->presets[id].params.zoom_ratios);

		smdk4210_param_int_set(smdk4210_camera, "zoom", smdk4210_camera->config->presets[id].params.zoom);
		smdk4210_param_int_set(smdk4210_camera, "max-zoom", smdk4210_camera->config->presets[id].params.max_zoom);

	} else {
		smdk4210_param_string_set(smdk4210_camera, "zoom-supported", "false");
	}

	// Flash
	smdk4210_param_string_set(smdk4210_camera, "flash-mode",
		smdk4210_camera->config->presets[id].params.flash_mode);
	smdk4210_param_string_set(smdk4210_camera, "flash-mode-values",
		smdk4210_camera->config->presets[id].params.flash_mode_values);

	// Exposure
	smdk4210_param_int_set(smdk4210_camera, "exposure-compensation",
		smdk4210_camera->config->presets[id].params.exposure_compensation);
	smdk4210_param_float_set(smdk4210_camera, "exposure-compensation-step",
		smdk4210_camera->config->presets[id].params.exposure_compensation_step);
	smdk4210_param_int_set(smdk4210_camera, "min-exposure-compensation",
		smdk4210_camera->config->presets[id].params.min_exposure_compensation);
	smdk4210_param_int_set(smdk4210_camera, "max-exposure-compensation",
		smdk4210_camera->config->presets[id].params.max_exposure_compensation);

	// WB
	smdk4210_param_string_set(smdk4210_camera, "whitebalance",
		smdk4210_camera->config->presets[id].params.whitebalance);
	smdk4210_param_string_set(smdk4210_camera, "whitebalance-values",
		smdk4210_camera->config->presets[id].params.whitebalance_values);

	// Scene mode
	smdk4210_param_string_set(smdk4210_camera, "scene-mode",
		smdk4210_camera->config->presets[id].params.scene_mode);
	smdk4210_param_string_set(smdk4210_camera, "scene-mode-values",
		smdk4210_camera->config->presets[id].params.scene_mode_values);

	// Effect
	smdk4210_param_string_set(smdk4210_camera, "effect",
		smdk4210_camera->config->presets[id].params.effect);
	smdk4210_param_string_set(smdk4210_camera, "effect-values",
		smdk4210_camera->config->presets[id].params.effect_values);

	// ISO
	smdk4210_param_string_set(smdk4210_camera, "iso",
		smdk4210_camera->config->presets[id].params.iso);
	smdk4210_param_string_set(smdk4210_camera, "iso-values",
		smdk4210_camera->config->presets[id].params.iso_values);

	// Camera
	smdk4210_param_float_set(smdk4210_camera, "focal-length",
		smdk4210_camera->config->presets[id].focal_length);
	smdk4210_param_float_set(smdk4210_camera, "horizontal-view-angle",
		smdk4210_camera->config->presets[id].horizontal_view_angle);
	smdk4210_param_float_set(smdk4210_camera, "vertical-view-angle",
		smdk4210_camera->config->presets[id].vertical_view_angle);

	rc = smdk4210_camera_params_apply(smdk4210_camera);
	if (rc < 0) {
		ALOGE("%s: Unable to apply params", __func__);
		return -1;
	}

	return 0;
}

int smdk4210_camera_params_apply(struct smdk4210_camera *smdk4210_camera)
{
	struct preview_stream_ops *preview_window;
	int gralloc_format;

	char *recording_hint_string;
	char *recording_preview_size_string;

	char *preview_size_string;
	int preview_width = 0;
	int preview_height = 0;
	int preview_changed = 0;
	char *preview_format_string;
	int preview_format;
	int preview_fps;

	char *picture_size_string;
	int picture_width = 0;
	int picture_height = 0;
	char *picture_format_string;
	int picture_format;

	int jpeg_thumbnail_width;
	int jpeg_thumbnail_height;
	int jpeg_thumbnail_quality;
	int jpeg_quality;

	char *video_size_string;
	int recording_width = 0;
	int recording_height = 0;
	char *video_frame_format_string;
	int recording_format;
	int camera_sensor_mode;
	int camera_sensor_output_size;

	char *focus_mode_string;
	int focus_mode = 0;
	char *focus_areas_string;
	int focus_left, focus_top, focus_right, focus_bottom, focus_weigth;
	int focus_x;
	int focus_y;

	char *zoom_supported_string;
	int zoom, max_zoom;

	char *flash_mode_string;
	int flash_mode;

	int exposure_compensation;
	int min_exposure_compensation;
	int max_exposure_compensation;

	char *whitebalance_string;
	int whitebalance;

	char *scene_mode_string;
	int scene_mode;

	char *effect_string;
	int effect;

	char *iso_string;
	int iso;

	int force = 0;

	int w, h;
	char *k;
	int rc;

	if (smdk4210_camera == NULL)
		return -EINVAL;

	if (!smdk4210_camera->preview_params_set) {
		ALOGE("%s: Setting preview params", __func__);
		smdk4210_camera->preview_params_set = 1;
		force = 1;
	}
	// Preview
	preview_size_string = smdk4210_param_string_get(smdk4210_camera, "preview-size");
	if (preview_size_string != NULL) {
		sscanf(preview_size_string, "%dx%d", &preview_width, &preview_height);

		if (preview_width != 0 && preview_width != smdk4210_camera->preview_width) {
			smdk4210_camera->preview_width = preview_width;
			preview_changed = 1;
		} if (preview_height != 0 && preview_height != smdk4210_camera->preview_height) {
			smdk4210_camera->preview_height = preview_height;
			preview_changed = 1;
		}
	}

	preview_format_string = smdk4210_param_string_get(smdk4210_camera, "preview-format");
	if (preview_format_string != NULL) {
		if (strcmp(preview_format_string, "yuv420sp") == 0) {
			preview_format = V4L2_PIX_FMT_NV21;
		} else if (strcmp(preview_format_string, "yuv420p") == 0) {
			preview_format = V4L2_PIX_FMT_YUV420;
		} else if (strcmp(preview_format_string, "rgb565") == 0) {
			preview_format = V4L2_PIX_FMT_RGB565;
		} else if (strcmp(preview_format_string, "rgb8888") == 0) {
			preview_format = V4L2_PIX_FMT_RGB32;
		} else {
			ALOGE("%s: Unsupported preview format: %s", __func__, preview_format_string);
			preview_format = V4L2_PIX_FMT_NV21;
		}

		if (preview_format != smdk4210_camera->preview_format)
			smdk4210_camera->preview_format = preview_format;
	}

	preview_fps = smdk4210_param_int_get(smdk4210_camera, "preview-frame-rate");
	if (preview_fps > 0)
		smdk4210_camera->preview_fps = preview_fps;
	else
		smdk4210_camera->preview_fps = 0;

	// Picture
	picture_size_string = smdk4210_param_string_get(smdk4210_camera, "picture-size");
	if (picture_size_string != NULL) {
		sscanf(picture_size_string, "%dx%d", &picture_width, &picture_height);

		if (picture_width != 0 && picture_width != smdk4210_camera->picture_width)
			smdk4210_camera->picture_width = picture_width;
		if (picture_height != 0 && picture_height != smdk4210_camera->picture_height)
			smdk4210_camera->picture_height = picture_height;
	}

	picture_format_string = smdk4210_param_string_get(smdk4210_camera, "picture-format");
	if (picture_format_string != NULL) {
		if (strcmp(picture_format_string, "jpeg") == 0) {
			picture_format = V4L2_PIX_FMT_JPEG;
		} else {
			ALOGE("%s: Unsupported picture format: %s", __func__, picture_format_string);
			picture_format = V4L2_PIX_FMT_JPEG;
		}

		if (picture_format != smdk4210_camera->picture_format)
			smdk4210_camera->picture_format = picture_format;
	}

	jpeg_thumbnail_width = smdk4210_param_int_get(smdk4210_camera, "jpeg-thumbnail-width");
	if (jpeg_thumbnail_width > 0)
		smdk4210_camera->jpeg_thumbnail_width = jpeg_thumbnail_width;

	jpeg_thumbnail_height = smdk4210_param_int_get(smdk4210_camera, "jpeg-thumbnail-height");
	if (jpeg_thumbnail_height > 0)
		smdk4210_camera->jpeg_thumbnail_height = jpeg_thumbnail_height;

	jpeg_thumbnail_quality = smdk4210_param_int_get(smdk4210_camera, "jpeg-thumbnail-quality");
	if (jpeg_thumbnail_quality > 0)
		smdk4210_camera->jpeg_thumbnail_quality = jpeg_thumbnail_quality;

	jpeg_quality = smdk4210_param_int_get(smdk4210_camera, "jpeg-quality");
	if (jpeg_quality <= 100 && jpeg_quality >= 0 && (jpeg_quality != smdk4210_camera->jpeg_quality || force)) {
		smdk4210_camera->jpeg_quality = jpeg_quality;
		rc = smdk4210_v4l2_s_ctrl(smdk4210_camera, 0, V4L2_CID_CAM_JPEG_QUALITY, jpeg_quality);
		if (rc < 0)
			ALOGE("%s: s ctrl failed!", __func__);
	}

	// Recording
	video_size_string = smdk4210_param_string_get(smdk4210_camera, "video-size");
	if (video_size_string == NULL)
		video_size_string = smdk4210_param_string_get(smdk4210_camera, "preview-size");

	if (video_size_string != NULL) {
		sscanf(video_size_string, "%dx%d", &recording_width, &recording_height);

		if (recording_width != 0 && recording_width != smdk4210_camera->recording_width)
			smdk4210_camera->recording_width = recording_width;
		if (recording_height != 0 && recording_height != smdk4210_camera->recording_height)
			smdk4210_camera->recording_height = recording_height;
	}

	video_frame_format_string = smdk4210_param_string_get(smdk4210_camera, "video-frame-format");
	if (video_frame_format_string != NULL) {
		if (strcmp(video_frame_format_string, "yuv420sp") == 0) {
			recording_format = V4L2_PIX_FMT_NV12;
		} else if (strcmp(video_frame_format_string, "yuv420p") == 0) {
			recording_format = V4L2_PIX_FMT_YUV420;
		} else if (strcmp(video_frame_format_string, "rgb565") == 0) {
			recording_format = V4L2_PIX_FMT_RGB565;
		} else if (strcmp(video_frame_format_string, "rgb8888") == 0) {
			recording_format = V4L2_PIX_FMT_RGB32;
		} else {
			ALOGE("%s: Unsupported recording format: %s", __func__, video_frame_format_string);
			recording_format = V4L2_PIX_FMT_NV12;
		}

		if (recording_format != smdk4210_camera->recording_format)
			smdk4210_camera->recording_format = recording_format;
	}

	recording_hint_string = smdk4210_param_string_get(smdk4210_camera, "recording-hint");
	if (recording_hint_string != NULL && strcmp(recording_hint_string, "true") == 0) {
		camera_sensor_mode = SENSOR_MOVIE;

		k = smdk4210_param_string_get(smdk4210_camera, "preview-size-values");
		while (recording_width != 0 && recording_height != 0) {
			if (k == NULL)
				break;

			sscanf(k, "%dx%d", &w, &h);

			// Look for same aspect ratio
			if ((recording_width * h) / recording_height == w) {
				preview_width = w;
				preview_height = h;
				break;
			}

			k = strchr(k, ',');
			if (k == NULL)
				break;

			k++;
		}

		if (preview_width != 0 && preview_width != smdk4210_camera->preview_width)
			smdk4210_camera->preview_width = preview_width;
		if (preview_height != 0 && preview_height != smdk4210_camera->preview_height)
			smdk4210_camera->preview_height = preview_height;

		camera_sensor_output_size = ((recording_width & 0xffff) << 16) | (recording_height & 0xffff);
		rc = smdk4210_v4l2_s_ctrl(smdk4210_camera, 0, V4L2_CID_CAMERA_SENSOR_OUTPUT_SIZE,
			camera_sensor_output_size);
		if (rc < 0)
			ALOGE("%s: s ctrl failed!", __func__);
	} else {
		camera_sensor_mode = SENSOR_CAMERA;
	}

	// Switching modes
	if (camera_sensor_mode != smdk4210_camera->camera_sensor_mode) {
		smdk4210_camera->camera_sensor_mode = camera_sensor_mode;
		rc = smdk4210_v4l2_s_ctrl(smdk4210_camera, 0, V4L2_CID_CAMERA_SENSOR_MODE, camera_sensor_mode);
		if (rc < 0)
			ALOGE("%s: s ctrl failed!", __func__);
	}

	// Focus
	focus_areas_string = smdk4210_param_string_get(smdk4210_camera, "focus-areas");
	if (focus_areas_string != NULL) {
		focus_left = focus_top = focus_right = focus_bottom = focus_weigth = 0;

		rc = sscanf(focus_areas_string, "(%d,%d,%d,%d,%d)",
			&focus_left, &focus_top, &focus_right, &focus_bottom, &focus_weigth);
		if (rc != 5) {
			ALOGE("%s: sscanf failed!", __func__);
		} else if (focus_left != 0 && focus_top != 0 && focus_right != 0 && focus_bottom != 0) {
			focus_x = (((focus_left + focus_right) / 2) + 1000) * preview_width / 2000;
			focus_y =  (((focus_top + focus_bottom) / 2) + 1000) * preview_height / 2000;

			if (focus_x != smdk4210_camera->focus_x || force) {
				smdk4210_camera->focus_x = focus_x;

				rc = smdk4210_v4l2_s_ctrl(smdk4210_camera, 0, V4L2_CID_CAMERA_OBJECT_POSITION_X, focus_x);
				if (rc < 0)
					ALOGE("%s: s ctrl failed!", __func__);
			}

			if (focus_y != smdk4210_camera->focus_y || force) {
				smdk4210_camera->focus_y = focus_y;

				rc = smdk4210_v4l2_s_ctrl(smdk4210_camera, 0, V4L2_CID_CAMERA_OBJECT_POSITION_Y, focus_y);
				if (rc < 0)
					ALOGE("%s: s ctrl failed!", __func__);
			}

			focus_mode = FOCUS_MODE_TOUCH;
		}
	}

	focus_mode_string = smdk4210_param_string_get(smdk4210_camera, "focus-mode");
	if (focus_mode_string != NULL) {
		if (focus_mode == 0) {
			if (strcmp(focus_mode_string, "auto") == 0)
				focus_mode = FOCUS_MODE_AUTO;
			else if (strcmp(focus_mode_string, "infinity") == 0)
				focus_mode = FOCUS_MODE_INFINITY;
			else if (strcmp(focus_mode_string, "macro") == 0)
				focus_mode = FOCUS_MODE_MACRO;
			else if (strcmp(focus_mode_string, "fixed") == 0)
				focus_mode = FOCUS_MODE_FIXED;
			else if (strcmp(focus_mode_string, "facedetect") == 0)
				focus_mode = FOCUS_MODE_FACEDETECT;
			else if (strcmp(focus_mode_string, "continuous-video") == 0)
				focus_mode = FOCUS_MODE_CONTINOUS;
			else if (strcmp(focus_mode_string, "continuous-picture") == 0)
				focus_mode = FOCUS_MODE_CONTINOUS;
			else
				focus_mode = FOCUS_MODE_AUTO;
		}

		if (focus_mode != smdk4210_camera->focus_mode || force) {
			rc = smdk4210_v4l2_s_ctrl(smdk4210_camera, 0, V4L2_CID_CAMERA_FOCUS_MODE, focus_mode);
			if (rc < 0)
				ALOGE("%s: s ctrl failed!", __func__);
		}

		if (focus_mode == FOCUS_MODE_TOUCH) {
			rc = smdk4210_v4l2_s_ctrl(smdk4210_camera, 0, V4L2_CID_CAMERA_TOUCH_AF_START_STOP, 1);
			if (rc < 0)
				ALOGE("%s: s ctrl failed!", __func__);
		} else if (smdk4210_camera->focus_mode == FOCUS_MODE_TOUCH) {
			rc = smdk4210_v4l2_s_ctrl(smdk4210_camera, 0, V4L2_CID_CAMERA_TOUCH_AF_START_STOP, 0);
			if (rc < 0)
				ALOGE("%s: s ctrl failed!", __func__);
		}

		smdk4210_camera->focus_mode = focus_mode;
	}

	// Zoom
	zoom_supported_string = smdk4210_param_string_get(smdk4210_camera, "zoom-supported");
	if (zoom_supported_string != NULL && strcmp(zoom_supported_string, "true") == 0) {
		zoom = smdk4210_param_int_get(smdk4210_camera, "zoom");
		max_zoom = smdk4210_param_int_get(smdk4210_camera, "max-zoom");
		if (zoom <= max_zoom && zoom >= 0 && (zoom != smdk4210_camera->zoom || force)) {
			smdk4210_camera->zoom = zoom;
			rc = smdk4210_v4l2_s_ctrl(smdk4210_camera, 0, V4L2_CID_CAMERA_ZOOM, zoom);
			if (rc < 0)
				ALOGE("%s: s ctrl failed!", __func__);
		}

	}

	// Flash
	flash_mode_string = smdk4210_param_string_get(smdk4210_camera, "flash-mode");
	if (flash_mode_string != NULL) {
		if (strcmp(flash_mode_string, "off") == 0)
			flash_mode = FLASH_MODE_OFF;
		else if (strcmp(flash_mode_string, "auto") == 0)
			flash_mode = FLASH_MODE_AUTO;
		else if (strcmp(flash_mode_string, "on") == 0)
			flash_mode = FLASH_MODE_ON;
		else if (strcmp(flash_mode_string, "torch") == 0)
			flash_mode = FLASH_MODE_TORCH;
		else
			flash_mode = FLASH_MODE_AUTO;

		if (flash_mode != smdk4210_camera->flash_mode || force) {
			smdk4210_camera->flash_mode = flash_mode;
			rc = smdk4210_v4l2_s_ctrl(smdk4210_camera, 0, V4L2_CID_CAMERA_FLASH_MODE, flash_mode);
			if (rc < 0)
				ALOGE("%s: s ctrl failed!", __func__);
		}
	}

	// Exposure
	exposure_compensation = smdk4210_param_int_get(smdk4210_camera, "exposure-compensation");
	min_exposure_compensation = smdk4210_param_int_get(smdk4210_camera, "min-exposure-compensation");
	max_exposure_compensation = smdk4210_param_int_get(smdk4210_camera, "max-exposure-compensation");

	if (exposure_compensation <= max_exposure_compensation && exposure_compensation >= min_exposure_compensation &&
		(exposure_compensation != smdk4210_camera->exposure_compensation || force)) {
		smdk4210_camera->exposure_compensation = exposure_compensation;
		rc = smdk4210_v4l2_s_ctrl(smdk4210_camera, 0, V4L2_CID_CAMERA_BRIGHTNESS, exposure_compensation);
		if (rc < 0)
			ALOGE("%s: s ctrl failed!", __func__);
	}

	// WB
	whitebalance_string = smdk4210_param_string_get(smdk4210_camera, "whitebalance");
	if (whitebalance_string != NULL) {
		if (strcmp(whitebalance_string, "auto") == 0)
			whitebalance = WHITE_BALANCE_AUTO;
		else if (strcmp(whitebalance_string, "incandescent") == 0)
			whitebalance = WHITE_BALANCE_TUNGSTEN;
		else if (strcmp(whitebalance_string, "fluorescent") == 0)
			whitebalance = WHITE_BALANCE_FLUORESCENT;
		else if (strcmp(whitebalance_string, "daylight") == 0)
			whitebalance = WHITE_BALANCE_SUNNY;
		else if (strcmp(whitebalance_string, "cloudy-daylight") == 0)
			whitebalance = WHITE_BALANCE_CLOUDY;
		else
			whitebalance = WHITE_BALANCE_AUTO;

		if (whitebalance != smdk4210_camera->whitebalance || force) {
			smdk4210_camera->whitebalance = whitebalance;
			rc = smdk4210_v4l2_s_ctrl(smdk4210_camera, 0, V4L2_CID_CAMERA_WHITE_BALANCE, whitebalance);
			if (rc < 0)
				ALOGE("%s: s ctrl failed!", __func__);
		}
	}

	// Scene mode
	scene_mode_string = smdk4210_param_string_get(smdk4210_camera, "scene-mode");
	if (scene_mode_string != NULL) {
		if (strcmp(scene_mode_string, "auto") == 0)
			scene_mode = SCENE_MODE_NONE;
		else if (strcmp(scene_mode_string, "portrait") == 0)
			scene_mode = SCENE_MODE_PORTRAIT;
		else if (strcmp(scene_mode_string, "landscape") == 0)
			scene_mode = SCENE_MODE_LANDSCAPE;
		else if (strcmp(scene_mode_string, "night") == 0)
			scene_mode = SCENE_MODE_NIGHTSHOT;
		else if (strcmp(scene_mode_string, "beach") == 0)
			scene_mode = SCENE_MODE_BEACH_SNOW;
		else if (strcmp(scene_mode_string, "snow") == 0)
			scene_mode = SCENE_MODE_BEACH_SNOW;
		else if (strcmp(scene_mode_string, "sunset") == 0)
			scene_mode = SCENE_MODE_SUNSET;
		else if (strcmp(scene_mode_string, "fireworks") == 0)
			scene_mode = SCENE_MODE_FIREWORKS;
		else if (strcmp(scene_mode_string, "sports") == 0)
			scene_mode = SCENE_MODE_SPORTS;
		else if (strcmp(scene_mode_string, "party") == 0)
			scene_mode = SCENE_MODE_PARTY_INDOOR;
		else if (strcmp(scene_mode_string, "candlelight") == 0)
			scene_mode = SCENE_MODE_CANDLE_LIGHT;
		else if (strcmp(scene_mode_string, "dusk-dawn") == 0)
			scene_mode = SCENE_MODE_DUSK_DAWN;
		else if (strcmp(scene_mode_string, "fall-color") == 0)
			scene_mode = SCENE_MODE_FALL_COLOR;
		else if (strcmp(scene_mode_string, "back-light") == 0)
			scene_mode = SCENE_MODE_BACK_LIGHT;
		else if (strcmp(scene_mode_string, "text") == 0)
			scene_mode = SCENE_MODE_TEXT;
		else
			scene_mode = SCENE_MODE_NONE;

		if (scene_mode != smdk4210_camera->scene_mode || force) {
			smdk4210_camera->scene_mode = scene_mode;
			rc = smdk4210_v4l2_s_ctrl(smdk4210_camera, 0, V4L2_CID_CAMERA_SCENE_MODE, scene_mode);
			if (rc < 0)
				ALOGE("%s: s ctrl failed!", __func__);
		}
	}

	// Effect
	effect_string = smdk4210_param_string_get(smdk4210_camera, "effect");
	if (effect_string != NULL) {
		if (strcmp(effect_string, "auto") == 0)
			effect = IMAGE_EFFECT_NONE;
		else if (strcmp(effect_string, "mono") == 0)
			effect = IMAGE_EFFECT_BNW;
		else if (strcmp(effect_string, "negative") == 0)
			effect = IMAGE_EFFECT_NEGATIVE;
		else if (strcmp(effect_string, "sepia") == 0)
			effect = IMAGE_EFFECT_SEPIA;
		else if (strcmp(effect_string, "aqua") == 0)
			effect = IMAGE_EFFECT_AQUA;
		else
			effect = IMAGE_EFFECT_NONE;

		if (effect != smdk4210_camera->effect || force) {
			smdk4210_camera->effect = effect;
			rc = smdk4210_v4l2_s_ctrl(smdk4210_camera, 0, V4L2_CID_CAMERA_EFFECT, effect);
			if (rc < 0)
				ALOGE("%s: s ctrl failed!", __func__);
		}
	}

	// ISO
	iso_string = smdk4210_param_string_get(smdk4210_camera, "iso");
	if (iso_string != NULL) {
		if (strcmp(iso_string, "auto") == 0)
			iso = ISO_AUTO;
		else if (strcmp(iso_string, "ISO50") == 0)
			iso = ISO_50;
		else if (strcmp(iso_string, "ISO100") == 0)
			iso = ISO_100;
		else if (strcmp(iso_string, "ISO200") == 0)
			iso = ISO_200;
		else if (strcmp(iso_string, "ISO400") == 0)
			iso = ISO_400;
		else if (strcmp(iso_string, "ISO800") == 0)
			iso = ISO_800;
		else
			iso = ISO_AUTO;

		if (iso != smdk4210_camera->iso || force) {
			smdk4210_camera->iso = iso;
			rc = smdk4210_v4l2_s_ctrl(smdk4210_camera, 0, V4L2_CID_CAMERA_ISO, iso);
			if (rc < 0)
				ALOGE("%s: s ctrl failed!", __func__);
		}
	}

	ALOGD("%s: Preview size: %dx%d, picture size: %dx%d, recording size: %dx%d",
		__func__, preview_width, preview_height, picture_width, picture_height,
		recording_width, recording_height);

	if (preview_changed && smdk4210_camera->preview_thread_running) {
		preview_window = smdk4210_camera->preview_window;

		smdk4210_camera_preview_stop(smdk4210_camera);

		gralloc_format = smdk4210_gralloc_format(smdk4210_camera->preview_format);

		rc = preview_window->set_buffers_geometry(preview_window, smdk4210_camera->preview_width, smdk4210_camera->preview_height, gralloc_format);
		if (rc)
			ALOGE("%s: Unable to set buffers geometry", __func__);

		smdk4210_camera->preview_window = preview_window;

		smdk4210_camera_preview_start(smdk4210_camera);
	}

	return 0;
}

// Picture

int smdk4210_camera_picture(struct smdk4210_camera *smdk4210_camera)
{
	camera_memory_t *data_memory = NULL;
	camera_memory_t *exif_data_memory = NULL;
	camera_memory_t *picture_data_memory = NULL;
	camera_memory_t *raw_thumbnail_data_memory = NULL;
	camera_memory_t *jpeg_thumbnail_data_memory = NULL;

	void *jpeg_data = NULL;
	int jpeg_size = 0;
	void *raw_thumbnail_data = NULL;
	int raw_thumbnail_size = 0;
	void *jpeg_thumbnail_data = NULL;
	int jpeg_thumbnail_size = 0;

	int camera_picture_format;
	int picture_width;
	int picture_height;
	int picture_format;

	int jpeg_thumbnail_width;
	int jpeg_thumbnail_height;
	int jpeg_thumbnail_quality;
	int jpeg_quality;

	int data_size;

	int offset = 0;
	void *jpeg_main_data = NULL;
	int jpeg_main_size = 0;
	void *jpeg_thumb_data = NULL;
	int jpeg_thumb_size = 0;

	int jpeg_fd;
	struct jpeg_enc_param jpeg_enc_params;
	enum jpeg_frame_format jpeg_in_format;
	enum jpeg_stream_format jpeg_out_format;
	enum jpeg_ret_type jpeg_result;
	void *jpeg_in_buffer;
	int jpeg_in_size;
	void *jpeg_out_buffer;
	int jpeg_out_size;

	exif_attribute_t exif_attributes;
	int exif_size = 0;

	int index;
	int rc;

	if (smdk4210_camera == NULL)
		return -EINVAL;

	picture_width = smdk4210_camera->picture_width;
	picture_height = smdk4210_camera->picture_height;
	picture_format = smdk4210_camera->picture_format;
	camera_picture_format = smdk4210_camera->camera_picture_format;
	jpeg_thumbnail_width = smdk4210_camera->jpeg_thumbnail_width;
	jpeg_thumbnail_height = smdk4210_camera->jpeg_thumbnail_height;
	jpeg_thumbnail_quality = smdk4210_camera->jpeg_thumbnail_quality;
	jpeg_quality = smdk4210_camera->jpeg_quality;

	if (camera_picture_format == 0)
		camera_picture_format = picture_format;

	// V4L2

	rc = smdk4210_v4l2_poll(smdk4210_camera, 0);
	if (rc < 0) {
		ALOGE("%s: poll failed!", __func__);
		return -1;
	} else if (rc == 0) {
		ALOGE("%s: poll timeout!", __func__);
		return -1;
	}

	rc = smdk4210_v4l2_streamoff_cap(smdk4210_camera, 0);
	if (rc < 0) {
		ALOGE("%s: streamoff failed!", __func__);
		return -1;
	}

	index = smdk4210_v4l2_dqbuf_cap(smdk4210_camera, 0);
	if (index < 0) {
		ALOGE("%s: dqbuf failed!", __func__);
		return -1;
	}

	// This assumes that the output format is JPEG

	if (camera_picture_format == V4L2_PIX_FMT_JPEG) {
		rc = smdk4210_v4l2_g_ctrl(smdk4210_camera, 0, V4L2_CID_CAM_JPEG_MAIN_SIZE,
			&jpeg_main_size);
		if (rc < 0) {
			ALOGE("%s: g ctrl failed!", __func__);
			return -1;
		}

		rc = smdk4210_v4l2_g_ctrl(smdk4210_camera, 0, V4L2_CID_CAM_JPEG_MAIN_OFFSET,
			&offset);
		if (rc < 0) {
			ALOGE("%s: g ctrl failed!", __func__);
			return -1;
		}

		jpeg_main_data = (void *) ((int) smdk4210_camera->picture_memory->data + offset);

		rc = smdk4210_v4l2_g_ctrl(smdk4210_camera, 0, V4L2_CID_CAM_JPEG_THUMB_SIZE,
			&jpeg_thumb_size);
		if (rc < 0) {
			ALOGE("%s: g ctrl failed!", __func__);
			return -1;
		}

		rc = smdk4210_v4l2_g_ctrl(smdk4210_camera, 0, V4L2_CID_CAM_JPEG_THUMB_OFFSET,
			&offset);
		if (rc < 0) {
			ALOGE("%s: g ctrl failed!", __func__);
			return -1;
		}

		jpeg_thumb_data = (void *) ((int) smdk4210_camera->picture_memory->data + offset);
	}

	// Thumbnail

	if (camera_picture_format == V4L2_PIX_FMT_JPEG && jpeg_thumb_data != NULL && jpeg_thumb_size >= 0) {
		jpeg_thumbnail_data = jpeg_thumb_data;
		jpeg_thumbnail_size = jpeg_thumb_size;
	} else {
		raw_thumbnail_size = smdk4210_camera_buffer_length(jpeg_thumbnail_width, jpeg_thumbnail_height, camera_picture_format);

		if (jpeg_thumbnail_width != picture_width || jpeg_thumbnail_height != picture_height) {
			if (smdk4210_camera->callbacks.request_memory != NULL) {
				raw_thumbnail_data_memory =
					smdk4210_camera->callbacks.request_memory(-1,
						raw_thumbnail_size, 1, 0);
				if (raw_thumbnail_data_memory == NULL) {
					ALOGE("%s: raw thumbnail memory request failed!", __func__);
					goto error;
				}
			} else {
				ALOGE("%s: No memory request function!", __func__);
				goto error;
			}

			switch (camera_picture_format) {
				case V4L2_PIX_FMT_YUYV:
				case V4L2_PIX_FMT_UYVY:
				case V4L2_PIX_FMT_YUV422P:
				default:
					rc = smdk4210_scale_yuv422(smdk4210_camera->picture_memory->data, picture_width, picture_height, raw_thumbnail_data_memory->data, jpeg_thumbnail_width, jpeg_thumbnail_height);
					break;
			}

			if (rc < 0) {
				ALOGE("%s: Resizing picture failed!", __func__);
				goto error;
			}

			raw_thumbnail_data = raw_thumbnail_data_memory->data;
		} else {
			raw_thumbnail_data = smdk4210_camera->picture_memory->data;
		}

		jpeg_fd = api_jpeg_encode_init();
		if (jpeg_fd < 0) {
			ALOGE("%s: Failed to init JPEG", __func__);
			goto error;
		}

		switch (camera_picture_format) {
			case V4L2_PIX_FMT_RGB565:
				jpeg_in_format = RGB_565;
				jpeg_out_format = JPEG_420;
				break;
			case V4L2_PIX_FMT_NV12:
			case V4L2_PIX_FMT_NV21:
			case V4L2_PIX_FMT_NV12T:
			case V4L2_PIX_FMT_YUV420:
				jpeg_in_format = YUV_420;
				jpeg_out_format = JPEG_420;
				break;
			case V4L2_PIX_FMT_YUYV:
			case V4L2_PIX_FMT_UYVY:
			case V4L2_PIX_FMT_YUV422P:
			default:
				jpeg_in_format = YUV_422;
				jpeg_out_format = JPEG_422;
				break;
		}

		jpeg_in_size = raw_thumbnail_size;

		memset(&jpeg_enc_params, 0, sizeof(jpeg_enc_params));

		jpeg_enc_params.width = jpeg_thumbnail_width;
		jpeg_enc_params.height = jpeg_thumbnail_height;
		jpeg_enc_params.in_fmt = jpeg_in_format;
		jpeg_enc_params.out_fmt = jpeg_out_format;

		if (jpeg_thumbnail_quality >= 90)
			jpeg_enc_params.quality = QUALITY_LEVEL_1;
		else if (jpeg_thumbnail_quality >= 80)
			jpeg_enc_params.quality = QUALITY_LEVEL_2;
		else if (jpeg_thumbnail_quality >= 70)
			jpeg_enc_params.quality = QUALITY_LEVEL_3;
		else
			jpeg_enc_params.quality = QUALITY_LEVEL_4;

		api_jpeg_set_encode_param(&jpeg_enc_params);

		jpeg_in_buffer = api_jpeg_get_encode_in_buf(jpeg_fd, jpeg_in_size);
		if (jpeg_in_buffer == NULL) {
			ALOGE("%s: Failed to get JPEG in buffer", __func__);
			api_jpeg_encode_deinit(jpeg_fd);
			goto error;
		}

		jpeg_out_buffer = api_jpeg_get_encode_out_buf(jpeg_fd);
		if (jpeg_out_buffer == NULL) {
			ALOGE("%s: Failed to get JPEG out buffer", __func__);
			api_jpeg_encode_deinit(jpeg_fd);
			goto error;
		}

		memcpy(jpeg_in_buffer, raw_thumbnail_data, jpeg_in_size);

		jpeg_result = api_jpeg_encode_exe(jpeg_fd, &jpeg_enc_params);
		if (jpeg_result != JPEG_ENCODE_OK) {
			ALOGE("%s: Failed to encode JPEG", __func__);
			api_jpeg_encode_deinit(jpeg_fd);
			goto error;
		}

		jpeg_out_size = jpeg_enc_params.size;
		if (jpeg_out_size <= 0) {
			ALOGE("%s: Failed to get JPEG out size", __func__);
			api_jpeg_encode_deinit(jpeg_fd);
			goto error;
		}

		if (smdk4210_camera->callbacks.request_memory != NULL) {
			jpeg_thumbnail_data_memory =
				smdk4210_camera->callbacks.request_memory(-1,
					jpeg_out_size, 1, 0);
			if (jpeg_thumbnail_data_memory == NULL) {
				ALOGE("%s: thumbnail memory request failed!", __func__);
				api_jpeg_encode_deinit(jpeg_fd);
				goto error;
			}
		} else {
			ALOGE("%s: No memory request function!", __func__);
			api_jpeg_encode_deinit(jpeg_fd);
			goto error;
		}

		memcpy(jpeg_thumbnail_data_memory->data, jpeg_out_buffer, jpeg_out_size);

		jpeg_thumbnail_data = jpeg_thumbnail_data_memory->data;
		jpeg_thumbnail_size = jpeg_out_size;

		api_jpeg_encode_deinit(jpeg_fd);
	}

	// Picture

	if (camera_picture_format == V4L2_PIX_FMT_JPEG && jpeg_main_data != NULL && jpeg_main_size >= 0) {
		jpeg_data = jpeg_main_data;
		jpeg_size = jpeg_main_size;
	} else {
		jpeg_fd = api_jpeg_encode_init();
		if (jpeg_fd < 0) {
			ALOGE("%s: Failed to init JPEG", __func__);
			goto error;
		}

		switch (camera_picture_format) {
			case V4L2_PIX_FMT_RGB565:
				jpeg_in_format = RGB_565;
				jpeg_out_format = JPEG_420;
				break;
			case V4L2_PIX_FMT_NV12:
			case V4L2_PIX_FMT_NV21:
			case V4L2_PIX_FMT_NV12T:
			case V4L2_PIX_FMT_YUV420:
				jpeg_in_format = YUV_420;
				jpeg_out_format = JPEG_420;
				break;
			case V4L2_PIX_FMT_YUYV:
			case V4L2_PIX_FMT_UYVY:
			case V4L2_PIX_FMT_YUV422P:
			default:
				jpeg_in_format = YUV_422;
				jpeg_out_format = JPEG_422;
				break;
		}

		jpeg_in_size = smdk4210_camera_buffer_length(picture_width, picture_height, camera_picture_format);

		memset(&jpeg_enc_params, 0, sizeof(jpeg_enc_params));

		jpeg_enc_params.width = picture_width;
		jpeg_enc_params.height = picture_height;
		jpeg_enc_params.in_fmt = jpeg_in_format;
		jpeg_enc_params.out_fmt = jpeg_out_format;

		if (jpeg_quality >= 90)
			jpeg_enc_params.quality = QUALITY_LEVEL_1;
		else if (jpeg_quality >= 80)
			jpeg_enc_params.quality = QUALITY_LEVEL_2;
		else if (jpeg_quality >= 70)
			jpeg_enc_params.quality = QUALITY_LEVEL_3;
		else
			jpeg_enc_params.quality = QUALITY_LEVEL_4;

		api_jpeg_set_encode_param(&jpeg_enc_params);

		jpeg_in_buffer = api_jpeg_get_encode_in_buf(jpeg_fd, jpeg_in_size);
		if (jpeg_in_buffer == NULL) {
			ALOGE("%s: Failed to get JPEG in buffer", __func__);
			api_jpeg_encode_deinit(jpeg_fd);
			goto error;
		}

		jpeg_out_buffer = api_jpeg_get_encode_out_buf(jpeg_fd);
		if (jpeg_out_buffer == NULL) {
			ALOGE("%s: Failed to get JPEG out buffer", __func__);
			api_jpeg_encode_deinit(jpeg_fd);
			goto error;
		}

		memcpy(jpeg_in_buffer, smdk4210_camera->picture_memory->data, jpeg_in_size);

		jpeg_result = api_jpeg_encode_exe(jpeg_fd, &jpeg_enc_params);
		if (jpeg_result != JPEG_ENCODE_OK) {
			ALOGE("%s: Failed to encode JPEG", __func__);
			api_jpeg_encode_deinit(jpeg_fd);
			goto error;
		}

		jpeg_out_size = jpeg_enc_params.size;
		if (jpeg_out_size <= 0) {
			ALOGE("%s: Failed to get JPEG out size", __func__);
			api_jpeg_encode_deinit(jpeg_fd);
			goto error;
		}

		if (smdk4210_camera->callbacks.request_memory != NULL) {
			picture_data_memory =
				smdk4210_camera->callbacks.request_memory(-1,
					jpeg_out_size, 1, 0);
			if (picture_data_memory == NULL) {
				ALOGE("%s: picture memory request failed!", __func__);
				api_jpeg_encode_deinit(jpeg_fd);
				goto error;
			}
		} else {
			ALOGE("%s: No memory request function!", __func__);
			api_jpeg_encode_deinit(jpeg_fd);
			goto error;
		}

		memcpy(picture_data_memory->data, jpeg_out_buffer, jpeg_out_size);

		jpeg_data = picture_data_memory->data;
		jpeg_size = jpeg_out_size;

		api_jpeg_encode_deinit(jpeg_fd);
	}

	// EXIF

	memset(&exif_attributes, 0, sizeof(exif_attributes));
	smdk4210_exif_attributes_create_static(smdk4210_camera, &exif_attributes);
	smdk4210_exif_attributes_create_params(smdk4210_camera, &exif_attributes);

	rc = smdk4210_exif_create(smdk4210_camera, &exif_attributes,
		jpeg_thumbnail_data, jpeg_thumbnail_size,
		&exif_data_memory, &exif_size);
	if (rc < 0 || exif_data_memory == NULL || exif_size <= 0) {
		ALOGE("%s: EXIF create failed!", __func__);
		goto error;
	}

	data_size = exif_size + jpeg_size;

	if (smdk4210_camera->callbacks.request_memory != NULL) {
		data_memory =
			smdk4210_camera->callbacks.request_memory(-1,
				data_size, 1, 0);
		if (data_memory == NULL) {
			ALOGE("%s: data memory request failed!", __func__);
			goto error;
		}
	} else {
		ALOGE("%s: No memory request function!", __func__);
		goto error;
	}

	// Copy the first two bytes of the JPEG picture
	memcpy(data_memory->data, jpeg_data, 2);

	// Copy the EXIF data
	memcpy((void *) ((int) data_memory->data + 2), exif_data_memory->data,
		exif_size);

	// Copy the JPEG picture
	memcpy((void *) ((int) data_memory->data + 2 + exif_size),
		(void *) ((int) jpeg_data + 2), jpeg_size - 2);

	// Callbacks

	if (SMDK4210_CAMERA_MSG_ENABLED(CAMERA_MSG_SHUTTER) && SMDK4210_CAMERA_CALLBACK_DEFINED(notify))
		smdk4210_camera->callbacks.notify(CAMERA_MSG_SHUTTER, 0, 0,
			smdk4210_camera->callbacks.user);

	if (SMDK4210_CAMERA_MSG_ENABLED(CAMERA_MSG_COMPRESSED_IMAGE) && SMDK4210_CAMERA_CALLBACK_DEFINED(data) &&
		data_memory != NULL)
		smdk4210_camera->callbacks.data(CAMERA_MSG_COMPRESSED_IMAGE,
			data_memory, 0, NULL, smdk4210_camera->callbacks.user);

	rc = 0;
	goto complete;

error:
	rc = -1;

complete:
	if (jpeg_thumbnail_data_memory != NULL && jpeg_thumbnail_data_memory->release != NULL)
		jpeg_thumbnail_data_memory->release(jpeg_thumbnail_data_memory);

	if (raw_thumbnail_data_memory != NULL && raw_thumbnail_data_memory->release != NULL)
		raw_thumbnail_data_memory->release(raw_thumbnail_data_memory);

	if (picture_data_memory != NULL && picture_data_memory->release != NULL)
		picture_data_memory->release(picture_data_memory);

	if (exif_data_memory != NULL && exif_data_memory->release != NULL)
		exif_data_memory->release(exif_data_memory);

	if (data_memory != NULL && data_memory->release != NULL)
		data_memory->release(data_memory);

	return rc;
}

void *smdk4210_camera_picture_thread(void *data)
{
	struct smdk4210_camera *smdk4210_camera;
	int rc;
	int i;

	if (data == NULL)
		return NULL;

	smdk4210_camera = (struct smdk4210_camera *) data;

	ALOGE("%s: Starting thread", __func__);
	smdk4210_camera->picture_thread_running = 1;

	if (smdk4210_camera->picture_enabled == 1) {
		pthread_mutex_lock(&smdk4210_camera->picture_mutex);

		rc = smdk4210_camera_picture(smdk4210_camera);
		if (rc < 0) {
			ALOGE("%s: picture failed!", __func__);
			smdk4210_camera->picture_enabled = 0;
		}

		if (smdk4210_camera->picture_memory != NULL && smdk4210_camera->picture_memory->release != NULL) {
			smdk4210_camera->picture_memory->release(smdk4210_camera->picture_memory);
			smdk4210_camera->picture_memory = NULL;
		}

		pthread_mutex_unlock(&smdk4210_camera->picture_mutex);
	}

	smdk4210_camera->picture_thread_running = 0;
	smdk4210_camera->picture_enabled = 0;

	ALOGE("%s: Exiting thread", __func__);

	return NULL;
}

int smdk4210_camera_picture_start(struct smdk4210_camera *smdk4210_camera)
{
	pthread_attr_t thread_attr;

	int width, height, format, camera_format;

	int fd;
	int rc;

	if (smdk4210_camera == NULL)
		return -EINVAL;

	// Stop preview thread
	smdk4210_camera_preview_stop(smdk4210_camera);

	width = smdk4210_camera->picture_width;
	height = smdk4210_camera->picture_height;
	format = smdk4210_camera->picture_format;
	camera_format = smdk4210_camera->camera_picture_format;

	// V4L2

	if (camera_format == 0)
		camera_format = format;

	rc = smdk4210_v4l2_enum_fmt_cap(smdk4210_camera, 0, camera_format);
	if (rc < 0) {
		ALOGE("%s: enum fmt failed!", __func__);
		return -1;
	}

	rc = smdk4210_v4l2_s_fmt_pix_cap(smdk4210_camera, 0, width, height, camera_format, V4L2_PIX_FMT_MODE_CAPTURE);
	if (rc < 0) {
		ALOGE("%s: s fmt failed!", __func__);
		return -1;
	}

	// Only use 1 buffer
	rc = smdk4210_v4l2_reqbufs_cap(smdk4210_camera, 0, 1);
	if (rc < 0) {
		ALOGE("%s: reqbufs failed!", __func__);
		return -1;
	}

	rc = smdk4210_v4l2_querybuf_cap(smdk4210_camera, 0, 0);
	if (rc < 0) {
		ALOGE("%s: querybuf failed!", __func__);
		return -1;
	}

	smdk4210_camera->picture_buffer_length = rc;

	if (smdk4210_camera->callbacks.request_memory != NULL) {
		fd = smdk4210_v4l2_find_fd(smdk4210_camera, 0);
		if (fd < 0) {
			ALOGE("%s: Unable to find v4l2 fd", __func__);
			return -1;
		}

		if (smdk4210_camera->picture_memory != NULL && smdk4210_camera->picture_memory->release != NULL)
			smdk4210_camera->picture_memory->release(smdk4210_camera->picture_memory);

		smdk4210_camera->picture_memory =
			smdk4210_camera->callbacks.request_memory(fd,
				smdk4210_camera->picture_buffer_length, 1, 0);
		if (smdk4210_camera->picture_memory == NULL) {
			ALOGE("%s: memory request failed!", __func__);
			return -1;
		}
	} else {
		ALOGE("%s: No memory request function!", __func__);
		return -1;
	}

	rc = smdk4210_v4l2_qbuf_cap(smdk4210_camera, 0, 0);
	if (rc < 0) {
		ALOGE("%s: qbuf failed!", __func__);
		return -1;
	}

	rc = smdk4210_v4l2_streamon_cap(smdk4210_camera, 0);
	if (rc < 0) {
		ALOGE("%s: streamon failed!", __func__);
		return -1;
	}

	rc = smdk4210_v4l2_s_ctrl(smdk4210_camera, 0, V4L2_CID_CAMERA_CAPTURE, 0);
	if (rc < 0) {
		ALOGE("%s: s ctrl failed!", __func__);
		return -1;
	}

	// Thread

	if (smdk4210_camera->picture_thread_running) {
		ALOGE("Picture thread is already running!");
		return -1;
	}

	pthread_mutex_init(&smdk4210_camera->picture_mutex, NULL);

	pthread_attr_init(&thread_attr);
	pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_DETACHED);

	smdk4210_camera->picture_enabled = 1;

	rc = pthread_create(&smdk4210_camera->picture_thread, &thread_attr,
		smdk4210_camera_picture_thread, (void *) smdk4210_camera);
	if (rc < 0) {
		ALOGE("%s: Unable to create thread", __func__);
		return -1;
	}

	return 0;
}

void smdk4210_camera_picture_stop(struct smdk4210_camera *smdk4210_camera)
{
	int rc;
	int i;

	if (smdk4210_camera == NULL)
		return;

	if (!smdk4210_camera->picture_enabled) {
		ALOGE("Picture was already stopped!");
		return;
	}

	pthread_mutex_lock(&smdk4210_camera->picture_mutex);

	// Disable picture to make the thread end
	smdk4210_camera->picture_enabled = 0;

	pthread_mutex_unlock(&smdk4210_camera->picture_mutex);

	// Wait for the thread to end
	for (i = 0; i < 10; i++) {
		if (!smdk4210_camera->picture_thread_running)
			break;

		usleep(500);
	}

	pthread_mutex_destroy(&smdk4210_camera->picture_mutex);
}

// Auto-focus

void *smdk4210_camera_auto_focus_thread(void *data)
{
	struct smdk4210_camera *smdk4210_camera;
	int auto_focus_status = -1;
	int auto_focus_result = 0;
	int rc;
	int i;

	if (data == NULL)
		return NULL;

	smdk4210_camera = (struct smdk4210_camera *) data;

	ALOGE("%s: Starting thread", __func__);
	smdk4210_camera->auto_focus_thread_running = 1;

	rc = smdk4210_v4l2_s_ctrl(smdk4210_camera, 0, V4L2_CID_CAMERA_SET_AUTO_FOCUS, AUTO_FOCUS_ON);
	if (rc < 0) {
		ALOGE("%s: s ctrl failed!", __func__);
		auto_focus_result = 0;
		goto thread_exit;
	}

	while (smdk4210_camera->auto_focus_enabled == 1) {
		pthread_mutex_lock(&smdk4210_camera->auto_focus_mutex);

		rc = smdk4210_v4l2_g_ctrl(smdk4210_camera, 0, V4L2_CID_CAMERA_AUTO_FOCUS_RESULT, &auto_focus_status);
		if (rc < 0) {
			ALOGE("%s: g ctrl failed!", __func__);
			auto_focus_result = 0;
			pthread_mutex_unlock(&smdk4210_camera->auto_focus_mutex);
			goto thread_exit;
		}

		if (auto_focus_status & M5MO_AF_STATUS_IN_PROGRESS) {
			usleep(10000);
		} else if (auto_focus_status == M5MO_AF_STATUS_SUCCESS || auto_focus_status == M5MO_AF_STATUS_1ST_SUCCESS) {
			auto_focus_result = 1;
			pthread_mutex_unlock(&smdk4210_camera->auto_focus_mutex);
			goto thread_exit;
		} else {
			auto_focus_result = 0;
			pthread_mutex_unlock(&smdk4210_camera->auto_focus_mutex);
			goto thread_exit;
		}

		pthread_mutex_unlock(&smdk4210_camera->auto_focus_mutex);
	}

thread_exit:
	rc = smdk4210_v4l2_s_ctrl(smdk4210_camera, 0, V4L2_CID_CAMERA_SET_AUTO_FOCUS, AUTO_FOCUS_OFF);
	if (rc < 0)
		ALOGE("%s: s ctrl failed!", __func__);

	if (SMDK4210_CAMERA_MSG_ENABLED(CAMERA_MSG_FOCUS) && SMDK4210_CAMERA_CALLBACK_DEFINED(notify))
		smdk4210_camera->callbacks.notify(CAMERA_MSG_FOCUS,
			(int32_t) auto_focus_result, 0, smdk4210_camera->callbacks.user);

	smdk4210_camera->auto_focus_thread_running = 0;
	smdk4210_camera->auto_focus_enabled = 0;

	ALOGE("%s: Exiting thread", __func__);

	return NULL;
}

int smdk4210_camera_auto_focus_start(struct smdk4210_camera *smdk4210_camera)
{
	pthread_attr_t thread_attr;
	int rc;

	if (smdk4210_camera == NULL)
		return -EINVAL;

	// Thread

	if (smdk4210_camera->auto_focus_thread_running) {
		ALOGE("Auto-focus thread is already running!");
		return -1;
	}

	pthread_mutex_init(&smdk4210_camera->auto_focus_mutex, NULL);

	pthread_attr_init(&thread_attr);
	pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_DETACHED);

	smdk4210_camera->auto_focus_enabled = 1;

	rc = pthread_create(&smdk4210_camera->auto_focus_thread, &thread_attr,
		smdk4210_camera_auto_focus_thread, (void *) smdk4210_camera);
	if (rc < 0) {
		ALOGE("%s: Unable to create thread", __func__);
		return -1;
	}

	return 0;
}

void smdk4210_camera_auto_focus_stop(struct smdk4210_camera *smdk4210_camera)
{
	int rc;
	int i;

	if (smdk4210_camera == NULL)
		return;

	if (!smdk4210_camera->auto_focus_enabled) {
		ALOGE("Auto-focus was already stopped!");
		return;
	}

	pthread_mutex_lock(&smdk4210_camera->auto_focus_mutex);

	// Disable auto-focus to make the thread end
	smdk4210_camera->auto_focus_enabled = 0;

	pthread_mutex_unlock(&smdk4210_camera->auto_focus_mutex);

	// Wait for the thread to end
	for (i = 0; i < 10; i++) {
		if (!smdk4210_camera->auto_focus_thread_running)
			break;

		usleep(500);
	}

	pthread_mutex_destroy(&smdk4210_camera->auto_focus_mutex);
}

// Preview

int smdk4210_camera_preview(struct smdk4210_camera *smdk4210_camera)
{
	buffer_handle_t *buffer;
	int stride;

	int width, height;

	char *preview_format_string;
	int frame_size, offset;
	void *preview_data;
	void *window_data;

	unsigned int recording_y_addr;
	unsigned int recording_cbcr_addr;
	nsecs_t timestamp;
	struct smdk4210_camera_addrs *addrs;
	struct timespec ts;

	int index;
	int rc;
	int i;

	if (smdk4210_camera == NULL || smdk4210_camera->preview_memory == NULL ||
		smdk4210_camera->preview_window == NULL)
		return -EINVAL;

	timestamp = systemTime(1);

	// V4L2

	rc = smdk4210_v4l2_poll(smdk4210_camera, 0);
	if (rc < 0) {
		ALOGE("%s: poll failed!", __func__);
		return -1;
	} else if (rc == 0) {
		ALOGE("%s: poll timeout!", __func__);
		return -1;
	}

	index = smdk4210_v4l2_dqbuf_cap(smdk4210_camera, 0);
	if (index < 0 || index >= smdk4210_camera->preview_buffers_count) {
		ALOGE("%s: dqbuf failed!", __func__);
		return -1;
	}

	rc = smdk4210_v4l2_qbuf_cap(smdk4210_camera, 0, index);
	if (rc < 0) {
		ALOGE("%s: qbuf failed!", __func__);
		return -1;
	}

	// Preview window

	width = smdk4210_camera->preview_width;
	height = smdk4210_camera->preview_height;

	smdk4210_camera->preview_window->dequeue_buffer(smdk4210_camera->preview_window,
		&buffer, &stride);
	smdk4210_camera->gralloc->lock(smdk4210_camera->gralloc, *buffer, GRALLOC_USAGE_SW_WRITE_OFTEN,
		0, 0, width, height, &window_data);

	if (window_data == NULL) {
		ALOGE("%s: gralloc lock failed!", __func__);
		return -1;
	}

	frame_size = smdk4210_camera->preview_frame_size;
	offset = index * frame_size;

	preview_data = (void *) ((int) smdk4210_camera->preview_memory->data + offset);
	memcpy(window_data, preview_data, frame_size);

	smdk4210_camera->gralloc->unlock(smdk4210_camera->gralloc, *buffer);
	smdk4210_camera->preview_window->enqueue_buffer(smdk4210_camera->preview_window,
		buffer);

	if (SMDK4210_CAMERA_MSG_ENABLED(CAMERA_MSG_PREVIEW_FRAME) && SMDK4210_CAMERA_CALLBACK_DEFINED(data)) {
		smdk4210_camera->callbacks.data(CAMERA_MSG_PREVIEW_FRAME,
			smdk4210_camera->preview_memory, index, NULL, smdk4210_camera->callbacks.user);
	}

	// Recording

	if (smdk4210_camera->recording_enabled && smdk4210_camera->recording_memory != NULL) {
		pthread_mutex_lock(&smdk4210_camera->recording_mutex);

		// V4L2

		rc = smdk4210_v4l2_poll(smdk4210_camera, 2);
		if (rc < 0) {
			ALOGE("%s: poll failed!", __func__);
			goto error_recording;
		} else if (rc == 0) {
			ALOGE("%s: poll timeout!", __func__);
			goto error_recording;
		}

		index = smdk4210_v4l2_dqbuf_cap(smdk4210_camera, 2);
		if (index < 0) {
			ALOGE("%s: dqbuf failed!", __func__);
			goto error_recording;
		}

		recording_y_addr = smdk4210_v4l2_s_ctrl(smdk4210_camera, 2, V4L2_CID_PADDR_Y, index);
		if (recording_y_addr == 0xffffffff) {
			ALOGE("%s: s ctrl failed!", __func__);
			goto error_recording;
		}

		recording_cbcr_addr = smdk4210_v4l2_s_ctrl(smdk4210_camera, 2, V4L2_CID_PADDR_CBCR, index);
		if (recording_cbcr_addr == 0xffffffff) {
			ALOGE("%s: s ctrl failed!", __func__);
			goto error_recording;
		}

		addrs = (struct smdk4210_camera_addrs *) smdk4210_camera->recording_memory->data;

		addrs[index].type = 0; // kMetadataBufferTypeCameraSource
		addrs[index].y = recording_y_addr;
		addrs[index].cbcr = recording_cbcr_addr;
		addrs[index].index = index;
		addrs[index].reserved = 0;

		pthread_mutex_unlock(&smdk4210_camera->recording_mutex);

		if (SMDK4210_CAMERA_MSG_ENABLED(CAMERA_MSG_VIDEO_FRAME) && SMDK4210_CAMERA_CALLBACK_DEFINED(data_timestamp)) {
			smdk4210_camera->callbacks.data_timestamp(timestamp, CAMERA_MSG_VIDEO_FRAME,
				smdk4210_camera->recording_memory, index, smdk4210_camera->callbacks.user);
		} else {
			rc = smdk4210_v4l2_qbuf_cap(smdk4210_camera, 2, index);
			if (rc < 0) {
				ALOGE("%s: qbuf failed!", __func__);
				return -1;
			}
		}
	}

	return 0;

error_recording:
	pthread_mutex_lock(&smdk4210_camera->recording_mutex);

	return -1;
}

void *smdk4210_camera_preview_thread(void *data)
{
	struct smdk4210_camera *smdk4210_camera;
	int rc;

	if (data == NULL)
		return NULL;

	smdk4210_camera = (struct smdk4210_camera *) data;

	ALOGE("%s: Starting thread", __func__);
	smdk4210_camera->preview_thread_running = 1;

	if (smdk4210_camera->preview_window == NULL) {
		// Lock preview lock mutex
		pthread_mutex_lock(&smdk4210_camera->preview_lock_mutex);
	}

	while (smdk4210_camera->preview_enabled == 1) {
		pthread_mutex_lock(&smdk4210_camera->preview_mutex);

		rc = smdk4210_camera_preview(smdk4210_camera);
		if (rc < 0) {
			ALOGE("%s: preview failed!", __func__);
			smdk4210_camera->preview_enabled = 0;
		}

		pthread_mutex_unlock(&smdk4210_camera->preview_mutex);
	}

	smdk4210_camera->preview_thread_running = 0;
	ALOGE("%s: Exiting thread", __func__);

	return NULL;
}

int smdk4210_camera_preview_start(struct smdk4210_camera *smdk4210_camera)
{
	struct v4l2_streamparm streamparm;
	int width, height, format;
	int fps, frame_size;
	int fd;

	pthread_attr_t thread_attr;

	int rc;
	int i;

	if (smdk4210_camera == NULL)
		return -EINVAL;

	if (smdk4210_camera->preview_enabled) {
		ALOGE("Preview was already started!");
		return 0;
	}

	// V4L2

	format = smdk4210_camera->preview_format;

	rc = smdk4210_v4l2_enum_fmt_cap(smdk4210_camera, 0, format);
	if (rc < 0) {
		ALOGE("%s: enum fmt failed!", __func__);
		return -1;
	}

	width = smdk4210_camera->preview_width;
	height = smdk4210_camera->preview_height;

	rc = smdk4210_v4l2_s_fmt_pix_cap(smdk4210_camera, 0, width, height, format, V4L2_PIX_FMT_MODE_PREVIEW);
	if (rc < 0) {
		ALOGE("%s: s fmt failed!", __func__);
		return -1;
	}

	rc = smdk4210_v4l2_s_ctrl(smdk4210_camera, 0, V4L2_CID_CACHEABLE, 1);
	if (rc < 0) {
		ALOGE("%s: s ctrl failed!", __func__);
		return -1;
	}

	for (i = SMDK4210_CAMERA_MAX_BUFFERS_COUNT; i >= SMDK4210_CAMERA_MIN_BUFFERS_COUNT; i--) {
		rc = smdk4210_v4l2_reqbufs_cap(smdk4210_camera, 0, i);
		if (rc >= 0)
			break;
	}

	if (rc < 0) {
		ALOGE("%s: reqbufs failed!", __func__);
		return -1;
	}

	smdk4210_camera->preview_buffers_count = rc;
	ALOGD("Found %d preview buffers available!", smdk4210_camera->preview_buffers_count);

	fps = smdk4210_camera->preview_fps;
	memset(&streamparm, 0, sizeof(streamparm));
	streamparm.parm.capture.timeperframe.numerator = 1;
	streamparm.parm.capture.timeperframe.denominator = fps;

	rc = smdk4210_v4l2_s_parm_cap(smdk4210_camera, 0, &streamparm);
	if (rc < 0) {
		ALOGE("%s: s parm failed!", __func__);
		return -1;
	}

	frame_size = smdk4210_camera_buffer_length(width, height, format);
	for (i = 0; i < smdk4210_camera->preview_buffers_count; i++) {
		rc = smdk4210_v4l2_querybuf_cap(smdk4210_camera, 0, i);
		if (rc < 0) {
			ALOGE("%s: querybuf failed!", __func__);
			return -1;
		}

		if (rc < frame_size) {
			ALOGE("%s: problematic frame size: %d/%d", __func__, rc, frame_size);
			return -1;
		}
	}

	frame_size = rc;
	smdk4210_camera->preview_frame_size = frame_size;

	if (smdk4210_camera->callbacks.request_memory != NULL) {
		fd = smdk4210_v4l2_find_fd(smdk4210_camera, 0);
		if (fd < 0) {
			ALOGE("%s: Unable to find v4l2 fd", __func__);
			return -1;
		}

		if (smdk4210_camera->preview_memory != NULL && smdk4210_camera->preview_memory->release != NULL)
			smdk4210_camera->preview_memory->release(smdk4210_camera->preview_memory);

		smdk4210_camera->preview_memory =
			smdk4210_camera->callbacks.request_memory(fd,
				frame_size, smdk4210_camera->preview_buffers_count, 0);
		if (smdk4210_camera->preview_memory == NULL) {
			ALOGE("%s: memory request failed!", __func__);
			return -1;
		}
	} else {
		ALOGE("%s: No memory request function!", __func__);
		return -1;
	}

	for (i = 0; i < smdk4210_camera->preview_buffers_count; i++) {
		rc = smdk4210_v4l2_qbuf_cap(smdk4210_camera, 0, i);
		if (rc < 0) {
			ALOGE("%s: qbuf failed!", __func__);
			return -1;
		}
	}

	rc = smdk4210_v4l2_s_ctrl(smdk4210_camera, 0, V4L2_CID_ROTATION,
		smdk4210_camera->camera_rotation);
	if (rc < 0) {
		ALOGE("%s: s ctrl failed!", __func__);
		return -1;
	}

	rc = smdk4210_v4l2_s_ctrl(smdk4210_camera, 0, V4L2_CID_HFLIP,
		smdk4210_camera->camera_hflip);
	if (rc < 0) {
		ALOGE("%s: s ctrl failed!", __func__);
		return -1;
	}

	rc = smdk4210_v4l2_s_ctrl(smdk4210_camera, 0, V4L2_CID_VFLIP,
		smdk4210_camera->camera_vflip);
	if (rc < 0) {
		ALOGE("%s: s ctrl failed!", __func__);
		return -1;
	}

	rc = smdk4210_v4l2_streamon_cap(smdk4210_camera, 0);
	if (rc < 0) {
		ALOGE("%s: streamon failed!", __func__);
		return -1;
	}

	// Thread

	pthread_mutex_init(&smdk4210_camera->preview_mutex, NULL);
	pthread_mutex_init(&smdk4210_camera->preview_lock_mutex, NULL);

	// Lock preview lock
	pthread_mutex_lock(&smdk4210_camera->preview_lock_mutex);

	pthread_attr_init(&thread_attr);
	pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_DETACHED);

	smdk4210_camera->preview_enabled = 1;

	rc = pthread_create(&smdk4210_camera->preview_thread, &thread_attr,
		smdk4210_camera_preview_thread, (void *) smdk4210_camera);
	if (rc < 0) {
		ALOGE("%s: Unable to create thread", __func__);
		return -1;
	}

	return 0;
}

void smdk4210_camera_preview_stop(struct smdk4210_camera *smdk4210_camera)
{
	int rc;
	int i;

	if (smdk4210_camera == NULL)
		return;

	if (!smdk4210_camera->preview_enabled) {
		ALOGE("Preview was already stopped!");
		return;
	}

	smdk4210_camera->preview_enabled = 0;

	// Unlock preview lock
	pthread_mutex_unlock(&smdk4210_camera->preview_lock_mutex);

	pthread_mutex_lock(&smdk4210_camera->preview_mutex);

	// Wait for the thread to end
	for (i = 0; i < 10; i++) {
		if (!smdk4210_camera->preview_thread_running)
			break;

		usleep(1000);
	}

	rc = smdk4210_v4l2_streamoff_cap(smdk4210_camera, 0);
	if (rc < 0) {
		ALOGE("%s: streamoff failed!", __func__);
	}

	smdk4210_camera->preview_params_set = 0;

	if (smdk4210_camera->preview_memory != NULL && smdk4210_camera->preview_memory->release != NULL) {
		smdk4210_camera->preview_memory->release(smdk4210_camera->preview_memory);
		smdk4210_camera->preview_memory = NULL;
	}

	smdk4210_camera->preview_window = NULL;

	pthread_mutex_unlock(&smdk4210_camera->preview_mutex);

	pthread_mutex_destroy(&smdk4210_camera->preview_lock_mutex);
	pthread_mutex_destroy(&smdk4210_camera->preview_mutex);
}

// Recording

void smdk4210_camera_recording_frame_release(struct smdk4210_camera *smdk4210_camera, void *data)
{
	struct smdk4210_camera_addrs *addrs;
	int rc;

	if (smdk4210_camera == NULL || data == NULL)
		return;

	addrs = (struct smdk4210_camera_addrs *) data;
	if (addrs->index >= (unsigned int) smdk4210_camera->recording_buffers_count)
		return;

	pthread_mutex_lock(&smdk4210_camera->recording_mutex);

	rc = smdk4210_v4l2_qbuf_cap(smdk4210_camera, 2, addrs->index);
	if (rc < 0) {
		ALOGE("%s: qbuf failed!", __func__);
		goto error;
	}

error:
	pthread_mutex_unlock(&smdk4210_camera->recording_mutex);
}

int smdk4210_camera_recording_start(struct smdk4210_camera *smdk4210_camera)
{
	int width, height, format;
	int fd;

	int rc;
	int i;

	if (smdk4210_camera == NULL)
		return -EINVAL;

	if (smdk4210_camera->recording_enabled) {
		ALOGE("Recording was already started!");
		return 0;
	}

	pthread_mutex_lock(&smdk4210_camera->preview_mutex);

	// V4L2

	format = smdk4210_camera->recording_format;

	rc = smdk4210_v4l2_enum_fmt_cap(smdk4210_camera, 2, format);
	if (rc < 0) {
		ALOGE("%s: enum fmt failed!", __func__);
		goto error;
	}

	width = smdk4210_camera->recording_width;
	height = smdk4210_camera->recording_height;

	rc = smdk4210_v4l2_s_fmt_pix_cap(smdk4210_camera, 2, width, height, format, V4L2_PIX_FMT_MODE_CAPTURE);
	if (rc < 0) {
		ALOGE("%s: s fmt failed!", __func__);
		goto error;
	}

	for (i = SMDK4210_CAMERA_MAX_BUFFERS_COUNT; i >= SMDK4210_CAMERA_MIN_BUFFERS_COUNT; i--) {
		rc = smdk4210_v4l2_reqbufs_cap(smdk4210_camera, 2, i);
		if (rc >= 0)
			break;
	}

	if (rc < 0) {
		ALOGE("%s: reqbufs failed!", __func__);
		goto error;
	}

	smdk4210_camera->recording_buffers_count = rc;
	ALOGD("Found %d recording buffers available!", smdk4210_camera->recording_buffers_count);

	for (i = 0; i < smdk4210_camera->recording_buffers_count; i++) {
		rc = smdk4210_v4l2_querybuf_cap(smdk4210_camera, 2, i);
		if (rc < 0) {
			ALOGE("%s: querybuf failed!", __func__);
			goto error;
		}
	}

	if (smdk4210_camera->callbacks.request_memory != NULL) {
		if (smdk4210_camera->recording_memory != NULL && smdk4210_camera->recording_memory->release != NULL)
			smdk4210_camera->recording_memory->release(smdk4210_camera->recording_memory);

		smdk4210_camera->recording_memory =
			smdk4210_camera->callbacks.request_memory(-1, sizeof(struct smdk4210_camera_addrs),
				smdk4210_camera->recording_buffers_count, 0);
		if (smdk4210_camera->recording_memory == NULL) {
			ALOGE("%s: memory request failed!", __func__);
			goto error;
		}
	} else {
		ALOGE("%s: No memory request function!", __func__);
		goto error;
	}

	for (i = 0; i < smdk4210_camera->recording_buffers_count; i++) {
		rc = smdk4210_v4l2_qbuf_cap(smdk4210_camera, 2, i);
		if (rc < 0) {
			ALOGE("%s: qbuf failed!", __func__);
			goto error;
		}
	}

	rc = smdk4210_v4l2_s_ctrl(smdk4210_camera, 2, V4L2_CID_ROTATION,
		smdk4210_camera->camera_rotation);
	if (rc < 0) {
		ALOGE("%s: s ctrl failed!", __func__);
		goto error;
	}

	rc = smdk4210_v4l2_s_ctrl(smdk4210_camera, 2, V4L2_CID_HFLIP,
		smdk4210_camera->camera_hflip);
	if (rc < 0) {
		ALOGE("%s: s ctrl failed!", __func__);
		goto error;
	}

	rc = smdk4210_v4l2_s_ctrl(smdk4210_camera, 2, V4L2_CID_VFLIP,
		smdk4210_camera->camera_vflip);
	if (rc < 0) {
		ALOGE("%s: s ctrl failed!", __func__);
		goto error;
	}

	rc = smdk4210_v4l2_streamon_cap(smdk4210_camera, 2);
	if (rc < 0) {
		ALOGE("%s: streamon failed!", __func__);
		goto error;
	}

	pthread_mutex_init(&smdk4210_camera->recording_mutex, NULL);

	smdk4210_camera->recording_enabled = 1;

	pthread_mutex_unlock(&smdk4210_camera->preview_mutex);

	return 0;
error:
	pthread_mutex_unlock(&smdk4210_camera->preview_mutex);

	return -1;
}

void smdk4210_camera_recording_stop(struct smdk4210_camera *smdk4210_camera)
{
	int rc;

	if (smdk4210_camera == NULL)
		return;

	if (!smdk4210_camera->recording_enabled) {
		ALOGE("Recording was already stopped!");
		return;
	}

	smdk4210_camera->recording_enabled = 0;

	pthread_mutex_lock(&smdk4210_camera->preview_mutex);

	rc = smdk4210_v4l2_streamoff_cap(smdk4210_camera, 2);
	if (rc < 0) {
		ALOGE("%s: streamoff failed!", __func__);
	}

	if (smdk4210_camera->recording_memory != NULL && smdk4210_camera->recording_memory->release != NULL) {
		smdk4210_camera->recording_memory->release(smdk4210_camera->recording_memory);
		smdk4210_camera->recording_memory = NULL;
	}

	pthread_mutex_unlock(&smdk4210_camera->preview_mutex);

	pthread_mutex_destroy(&smdk4210_camera->recording_mutex);
}

/*
 * SMDK4210 Camera OPS
 */

int smdk4210_camera_set_preview_window(struct camera_device *device,
	struct preview_stream_ops *w)
{
	struct smdk4210_camera *smdk4210_camera;

	int width, height, format, gralloc_format;

	buffer_handle_t *buffer;
	int stride;
	void *addr = NULL;

	int rc;

	ALOGD("%s(%p, %p)", __func__, device, w);

	if (device == NULL || device->priv == NULL)
		return -EINVAL;

	smdk4210_camera = (struct smdk4210_camera *) device->priv;

	if (w == NULL)
		return 0;

	smdk4210_camera->preview_window = w;

	if (w->set_buffer_count == NULL || w->set_usage == NULL || w->set_buffers_geometry == NULL)
		return -EINVAL;

	if (smdk4210_camera->preview_buffers_count <= 0) {
		ALOGE("%s: Invalid preview buffers count", __func__);
		smdk4210_camera->preview_buffers_count = SMDK4210_CAMERA_MAX_BUFFERS_COUNT;
	}

	rc = w->set_buffer_count(w, smdk4210_camera->preview_buffers_count);
	if (rc) {
		ALOGE("%s: Unable to set buffer count (%d)", __func__,
			smdk4210_camera->preview_buffers_count);
		return -1;
	}

	rc = w->set_usage(w, GRALLOC_USAGE_SW_WRITE_OFTEN);
	if (rc) {
		ALOGE("%s: Unable to set usage", __func__);
		return -1;
	}

	width = smdk4210_camera->preview_width;
	height = smdk4210_camera->preview_height;
	format = smdk4210_camera->preview_format;

	gralloc_format = smdk4210_gralloc_format(format);

	rc = w->set_buffers_geometry(w, width, height, gralloc_format);
	if (rc) {
		ALOGE("%s: Unable to set buffers geometry", __func__);
		return -1;
	}

	// Unlock preview lock
	pthread_mutex_unlock(&smdk4210_camera->preview_lock_mutex);

	return 0;
}

void smdk4210_camera_set_callbacks(struct camera_device *device,
	camera_notify_callback notify_cb,
	camera_data_callback data_cb,
	camera_data_timestamp_callback data_cb_timestamp,
	camera_request_memory get_memory,
	void *user)
{
	struct smdk4210_camera *smdk4210_camera;

	ALOGD("%s(%p, %p)", __func__, device, user);

	if (device == NULL || device->priv == NULL)
		return;

	smdk4210_camera = (struct smdk4210_camera *) device->priv;

	smdk4210_camera->callbacks.notify = notify_cb;
	smdk4210_camera->callbacks.data = data_cb;
	smdk4210_camera->callbacks.data_timestamp = data_cb_timestamp;
	smdk4210_camera->callbacks.request_memory = get_memory;
	smdk4210_camera->callbacks.user = user;
}

void smdk4210_camera_enable_msg_type(struct camera_device *device,
	int32_t msg_type)
{
	struct smdk4210_camera *smdk4210_camera;

	ALOGD("%s(%p, %d)", __func__, device, msg_type);

	if (device == NULL || device->priv == NULL)
		return;

	smdk4210_camera = (struct smdk4210_camera *) device->priv;

	smdk4210_camera->messages_enabled |= msg_type;
}

void smdk4210_camera_disable_msg_type(struct camera_device *device,
	int32_t msg_type)
{
	struct smdk4210_camera *smdk4210_camera;

	ALOGD("%s(%p, %d)", __func__, device, msg_type);

	if (device == NULL || device->priv == NULL)
		return;

	smdk4210_camera = (struct smdk4210_camera *) device->priv;

	smdk4210_camera->messages_enabled &= ~msg_type;
}

int smdk4210_camera_msg_type_enabled(struct camera_device *device,
	int32_t msg_type)
{
	struct smdk4210_camera *smdk4210_camera;

	ALOGD("%s(%p, %d)", __func__, device, msg_type);

	if (device == NULL || device->priv == NULL)
		return -EINVAL;

	smdk4210_camera = (struct smdk4210_camera *) device->priv;

	return smdk4210_camera->messages_enabled & msg_type;
}

int smdk4210_camera_start_preview(struct camera_device *device)
{
	struct smdk4210_camera *smdk4210_camera;

	ALOGD("%s(%p)", __func__, device);

	if (device == NULL || device->priv == NULL)
		return -EINVAL;

	smdk4210_camera = (struct smdk4210_camera *) device->priv;

	return smdk4210_camera_preview_start(smdk4210_camera);
}

void smdk4210_camera_stop_preview(struct camera_device *device)
{
	struct smdk4210_camera *smdk4210_camera;

	ALOGD("%s(%p)", __func__, device);

	if (device == NULL || device->priv == NULL)
		return;

	smdk4210_camera = (struct smdk4210_camera *) device->priv;

	smdk4210_camera_preview_stop(smdk4210_camera);
}

int smdk4210_camera_preview_enabled(struct camera_device *device)
{
	struct smdk4210_camera *smdk4210_camera;

	ALOGD("%s(%p)", __func__, device);

	if (device == NULL || device->priv == NULL)
		return -EINVAL;

	smdk4210_camera = (struct smdk4210_camera *) device->priv;

	return smdk4210_camera->preview_enabled;
}

int smdk4210_camera_store_meta_data_in_buffers(struct camera_device *device,
	int enable)
{
	ALOGD("%s(%p, %d)", __func__, device, enable);

	if (!enable) {
		ALOGE("%s: Cannot disable meta-data in buffers!", __func__);
		return -1;
	}

	return 0;
}

int smdk4210_camera_start_recording(struct camera_device *device)
{
	struct smdk4210_camera *smdk4210_camera;

	ALOGD("%s(%p)", __func__, device);

	smdk4210_camera = (struct smdk4210_camera *) device->priv;

	return smdk4210_camera_recording_start(smdk4210_camera);
}

void smdk4210_camera_stop_recording(struct camera_device *device)
{
	struct smdk4210_camera *smdk4210_camera;

	ALOGD("%s(%p)", __func__, device);

	smdk4210_camera = (struct smdk4210_camera *) device->priv;

	smdk4210_camera_recording_stop(smdk4210_camera);
}

int smdk4210_camera_recording_enabled(struct camera_device *device)
{
	struct smdk4210_camera *smdk4210_camera;

	ALOGD("%s(%p)", __func__, device);

	if (device == NULL || device->priv == NULL)
		return -EINVAL;

	smdk4210_camera = (struct smdk4210_camera *) device->priv;

	return smdk4210_camera->recording_enabled;
}

void smdk4210_camera_release_recording_frame(struct camera_device *device,
	const void *opaque)
{
	struct smdk4210_camera *smdk4210_camera;

	ALOGV("%s(%p, %p)", __func__, device, opaque);

	if (device == NULL || device->priv == NULL)
		return;

	smdk4210_camera = (struct smdk4210_camera *) device->priv;

	smdk4210_camera_recording_frame_release(smdk4210_camera, (void *) opaque);
}

int smdk4210_camera_auto_focus(struct camera_device *device)
{
	struct smdk4210_camera *smdk4210_camera;

	ALOGD("%s(%p)", __func__, device);

	if (device == NULL || device->priv == NULL)
		return -EINVAL;

	smdk4210_camera = (struct smdk4210_camera *) device->priv;

	return smdk4210_camera_auto_focus_start(smdk4210_camera);
}

int smdk4210_camera_cancel_auto_focus(struct camera_device *device)
{
	struct smdk4210_camera *smdk4210_camera;

	ALOGD("%s(%p)", __func__, device);

	if (device == NULL || device->priv == NULL)
		return -EINVAL;

	smdk4210_camera = (struct smdk4210_camera *) device->priv;

	smdk4210_camera_auto_focus_stop(smdk4210_camera);

	return 0;
}

int smdk4210_camera_take_picture(struct camera_device *device)
{
	struct smdk4210_camera *smdk4210_camera;

	ALOGD("%s(%p)", __func__, device);

	if (device == NULL || device->priv == NULL)
		return -EINVAL;

	smdk4210_camera = (struct smdk4210_camera *) device->priv;

	return smdk4210_camera_picture_start(smdk4210_camera);
}

int smdk4210_camera_cancel_picture(struct camera_device *device)
{
	struct smdk4210_camera *smdk4210_camera;

	ALOGD("%s(%p)", __func__, device);

	if (device == NULL || device->priv == NULL)
		return -EINVAL;

	smdk4210_camera = (struct smdk4210_camera *) device->priv;

	smdk4210_camera_picture_stop(smdk4210_camera);

	return 0;
}

int smdk4210_camera_set_parameters(struct camera_device *device,
	const char *params)
{
	struct smdk4210_camera *smdk4210_camera;
	int rc;

	ALOGD("%s(%p, %s)", __func__, device, params);

	if (device == NULL || device->priv == NULL || params == NULL)
		return -EINVAL;

	smdk4210_camera = (struct smdk4210_camera *) device->priv;

	rc = smdk4210_params_string_set(smdk4210_camera, (char *) params);
	if (rc < 0) {
		ALOGE("%s: Unable to set params string", __func__);
		return -1;
	}

	rc = smdk4210_camera_params_apply(smdk4210_camera);
	if (rc < 0) {
		ALOGE("%s: Unable to apply params", __func__);
		return -1;
	}

	return 0;
}

char *smdk4210_camera_get_parameters(struct camera_device *device)
{
	struct smdk4210_camera *smdk4210_camera;
	char *params;

	ALOGD("%s(%p)", __func__, device);

	if (device == NULL || device->priv == NULL)
		return NULL;

	smdk4210_camera = (struct smdk4210_camera *) device->priv;

	params = smdk4210_params_string_get(smdk4210_camera);
	if (params == NULL) {
		ALOGE("%s: Couldn't find any param", __func__);
		return strdup("");
	}

	return params;
}

void smdk4210_camera_put_parameters(struct camera_device *device, char *params)
{
	ALOGD("%s(%p)", __func__, device);

	if (params != NULL)
		free(params);
}

int smdk4210_camera_send_command(struct camera_device *device,
	int32_t cmd, int32_t arg1, int32_t arg2)
{
	ALOGD("%s(%p, %d, %d, %d)", __func__, device, cmd, arg1, arg2);

	return 0;
}

void smdk4210_camera_release(struct camera_device *device)
{
	struct smdk4210_camera *smdk4210_camera;

	ALOGD("%s(%p)", __func__, device);

	if (device == NULL || device->priv == NULL)
		return;

	smdk4210_camera = (struct smdk4210_camera *) device->priv;

	if (smdk4210_camera->preview_memory != NULL && smdk4210_camera->preview_memory->release != NULL) {
		smdk4210_camera->preview_memory->release(smdk4210_camera->preview_memory);
		smdk4210_camera->preview_memory = NULL;
	}

	if (smdk4210_camera->picture_memory != NULL && smdk4210_camera->picture_memory->release != NULL) {
		smdk4210_camera->picture_memory->release(smdk4210_camera->picture_memory);
		smdk4210_camera->picture_memory = NULL;
	}

	smdk4210_camera_deinit(smdk4210_camera);
}

int smdk4210_camera_dump(struct camera_device *device, int fd)
{
	ALOGD("%s(%p, %d)", __func__, device, fd);

	return 0;
}

/*
 * Interface
 */

struct camera_device_ops smdk4210_camera_ops = {
	.set_preview_window = smdk4210_camera_set_preview_window,
	.set_callbacks = smdk4210_camera_set_callbacks,
	.enable_msg_type = smdk4210_camera_enable_msg_type,
	.disable_msg_type = smdk4210_camera_disable_msg_type,
	.msg_type_enabled = smdk4210_camera_msg_type_enabled,
	.start_preview = smdk4210_camera_start_preview,
	.stop_preview = smdk4210_camera_stop_preview,
	.preview_enabled = smdk4210_camera_preview_enabled,
	.store_meta_data_in_buffers = smdk4210_camera_store_meta_data_in_buffers,
	.start_recording = smdk4210_camera_start_recording,
	.stop_recording = smdk4210_camera_stop_recording,
	.recording_enabled = smdk4210_camera_recording_enabled,
	.release_recording_frame = smdk4210_camera_release_recording_frame,
	.auto_focus = smdk4210_camera_auto_focus,
	.cancel_auto_focus = smdk4210_camera_cancel_auto_focus,
	.take_picture = smdk4210_camera_take_picture,
	.cancel_picture = smdk4210_camera_cancel_picture,
	.set_parameters = smdk4210_camera_set_parameters,
	.get_parameters = smdk4210_camera_get_parameters,
	.put_parameters = smdk4210_camera_put_parameters,
	.send_command = smdk4210_camera_send_command,
	.release = smdk4210_camera_release,
	.dump = smdk4210_camera_dump,
};

int smdk4210_camera_close(hw_device_t *device)
{
	struct camera_device *camera_device;
	struct smdk4210_camera *smdk4210_camera;

	ALOGD("%s(%p)", __func__, device);

	if (device == NULL)
		return -EINVAL;

	camera_device = (struct camera_device *) device;

	if (camera_device->priv != NULL) {
		free(camera_device->priv);
	}

	free(camera_device);

	return 0;
}

int smdk4210_camera_open(const struct hw_module_t* module, const char *camera_id,
	struct hw_device_t** device)
{
	struct camera_device *camera_device = NULL;
	struct smdk4210_camera *smdk4210_camera = NULL;
	int id;
	int rc;

	ALOGD("%s(%p, %s, %p)", __func__, module, camera_id, device);

	if (module == NULL || camera_id == NULL || device == NULL)
		return -EINVAL;

	id = atoi(camera_id);
	if (id < 0)
		return -EINVAL;

	smdk4210_camera = calloc(1, sizeof(struct smdk4210_camera));
	smdk4210_camera->config = smdk4210_camera_config;

	if (smdk4210_camera->config->presets_count > SMDK4210_CAMERA_MAX_PRESETS_COUNT ||
		smdk4210_camera->config->v4l2_nodes_count > SMDK4210_CAMERA_MAX_V4L2_NODES_COUNT)
		goto error_preset;

	if (id >= smdk4210_camera->config->presets_count)
		goto error_preset;

	rc = smdk4210_camera_init(smdk4210_camera, id);
	if (rc < 0) {
		ALOGE("%s: Unable to init camera", __func__);
		goto error;
	}

	camera_device = calloc(1, sizeof(struct camera_device));
	camera_device->common.tag = HARDWARE_DEVICE_TAG;
	camera_device->common.version = 0;
	camera_device->common.module = (struct hw_module_t *) module;
	camera_device->common.close = smdk4210_camera_close;

	camera_device->ops = &smdk4210_camera_ops;
	camera_device->priv = smdk4210_camera;

	*device = (struct hw_device_t *) &(camera_device->common);

	return 0;

error:
	smdk4210_camera_deinit(smdk4210_camera);

error_device:
	if (camera_device != NULL)
		free(camera_device);

error_preset:
	if (smdk4210_camera != NULL)
		free(smdk4210_camera);

	return -1;
}

int smdk4210_camera_get_number_of_cameras(void)
{
	ALOGD("%s()", __func__);

	if (smdk4210_camera_config == NULL || smdk4210_camera_config->presets == NULL) {
		ALOGE("%s: Unable to find proper camera config", __func__);
		return -1;
	}

	return smdk4210_camera_config->presets_count;
}

int smdk4210_camera_get_camera_info(int id, struct camera_info *info)
{
	ALOGD("%s(%d, %p)", __func__, id, info);

	if (id < 0 || info == NULL)
		return -EINVAL;

	if (smdk4210_camera_config == NULL || smdk4210_camera_config->presets == NULL) {
		ALOGE("%s: Unable to find proper camera config", __func__);
		return -1;
	}

	if (id >= smdk4210_camera_config->presets_count)
		return -EINVAL;

	ALOGD("Selected camera: %s", smdk4210_camera_config->presets[id].name);

	info->facing = smdk4210_camera_config->presets[id].facing;
	info->orientation = smdk4210_camera_config->presets[id].orientation;

	return 0;
}

struct hw_module_methods_t smdk4210_camera_module_methods = {
	.open = smdk4210_camera_open,
};

struct camera_module HAL_MODULE_INFO_SYM = {
	.common = {
		.tag = HARDWARE_MODULE_TAG,
		.hal_api_version = HARDWARE_HAL_API_VERSION,
		.module_api_version = CAMERA_MODULE_API_VERSION_1_0,
		.id = CAMERA_HARDWARE_MODULE_ID,
		.name = "SMDK4210 Camera",
		.author = "Paul Kocialkowski",
		.methods = &smdk4210_camera_module_methods,
	},
	.get_number_of_cameras = smdk4210_camera_get_number_of_cameras,
	.get_camera_info = smdk4210_camera_get_camera_info,
};
