/*
 * Copyright (C) 2013 Paul Kocialkowski
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
#include <stdint.h>
#include <stdbool.h>
#include <pthread.h>

#include <videodev2.h>
#include <videodev2_exynos_media.h>
#include <videodev2_exynos_camera.h>

#include <Exif.h>

#include <hardware/hardware.h>
#include <hardware/camera.h>

#ifndef _SMDK4210_CAMERA_H_
#define _SMDK4210_CAMERA_H_

#define SMDK4210_CAMERA_MAX_PRESETS_COUNT		2
#define SMDK4210_CAMERA_MAX_V4L2_NODES_COUNT	4
#define SMDK4210_CAMERA_MIN_BUFFERS_COUNT		3
#define SMDK4210_CAMERA_MAX_BUFFERS_COUNT		8

#define SMDK4210_CAMERA_MSG_ENABLED(msg) \
	(smdk4210_camera->messages_enabled & msg)
#define SMDK4210_CAMERA_CALLBACK_DEFINED(cb) \
	(smdk4210_camera->callbacks.cb != NULL)

#define SMDK4210_CAMERA_ALIGN(value) ((value + (0x10000 - 1)) & ~(0x10000 - 1))

/*
 * Structures
 */

struct list_head {
	struct list_head *next;
	struct list_head *prev;
};

enum smdk4210_param_type {
	SMDK4210_PARAM_INT,
	SMDK4210_PARAM_FLOAT,
	SMDK4210_PARAM_STRING,
};

union smdk4210_param_data {
	int integer;
	float floating;
	char *string;
};

struct smdk4210_param {
	struct list_head list;

	char *key;
	union smdk4210_param_data data;
	enum smdk4210_param_type type;
};

struct smdk4210_camera_params {
	char *preview_size_values;
	char *preview_size;
	char *preview_format_values;
	char *preview_format;
	char *preview_frame_rate_values;
	int preview_frame_rate;
	char *preview_fps_range_values;
	char *preview_fps_range;

	char *picture_size_values;
	char *picture_size;
	char *picture_format_values;
	char *picture_format;
	char *jpeg_thumbnail_size_values;
	int jpeg_thumbnail_width;
	int jpeg_thumbnail_height;
	int jpeg_thumbnail_quality;
	int jpeg_quality;

	char *recording_size;
	char *recording_size_values;
	char *recording_format;

	char *focus_mode;
	char *focus_mode_values;
	char *focus_distances;
	char *focus_areas;
	int max_num_focus_areas;

	int zoom_supported;
	int smooth_zoom_supported;
	char *zoom_ratios;
	int zoom;
	int max_zoom;

	char *flash_mode;
	char *flash_mode_values;

	int exposure_compensation;
	float exposure_compensation_step;
	int min_exposure_compensation;
	int max_exposure_compensation;

	char *whitebalance;
	char *whitebalance_values;

	char *scene_mode;
	char *scene_mode_values;

	char *effect;
	char *effect_values;

	char *iso;
	char *iso_values;
};

struct smdk4210_camera_preset {
	char *name;
	int facing;
	int orientation;

	int rotation;
	int hflip;
	int vflip;

	int picture_format;

	float focal_length;
	float horizontal_view_angle;
	float vertical_view_angle;

	int metering;

	struct smdk4210_camera_params params;
};

struct smdk4210_v4l2_node {
	int id;
	char *node;
};

struct exynox_camera_config {
	struct smdk4210_camera_preset *presets;
	int presets_count;

	struct smdk4210_v4l2_node *v4l2_nodes;
	int v4l2_nodes_count;
};

struct smdk4210_camera_callbacks {
	camera_notify_callback notify;
	camera_data_callback data;
	camera_data_timestamp_callback data_timestamp;
	camera_request_memory request_memory;
	void *user;
};

struct smdk4210_camera {
	int v4l2_fds[SMDK4210_CAMERA_MAX_V4L2_NODES_COUNT];

	struct exynox_camera_config *config;
	struct smdk4210_param *params;

	struct smdk4210_camera_callbacks callbacks;
	int messages_enabled;

	gralloc_module_t *gralloc;

	// Picture
	pthread_t picture_thread;
	pthread_mutex_t picture_mutex;
	int picture_thread_running;

	int picture_enabled;
	camera_memory_t *picture_memory;
	int picture_buffer_length;

	// Auto-focus
	pthread_t auto_focus_thread;
	pthread_mutex_t auto_focus_mutex;
	int auto_focus_thread_running;

	int auto_focus_enabled;

	// Preview
	pthread_t preview_thread;
	pthread_mutex_t preview_mutex;
	pthread_mutex_t preview_lock_mutex;
	int preview_thread_running;

	int preview_enabled;
	struct preview_stream_ops *preview_window;
	camera_memory_t *preview_memory;
	int preview_buffers_count;
	int preview_frame_size;
	int preview_params_set;

	// Recording
	pthread_mutex_t recording_mutex;

	int recording_enabled;
	camera_memory_t *recording_memory;
	int recording_buffers_count;

	// Camera params
	int camera_rotation;
	int camera_hflip;
	int camera_vflip;
	int camera_picture_format;
	int camera_focal_length;
	int camera_metering;

	int camera_sensor_mode;

	// Params
	int preview_width;
	int preview_height;
	int preview_format;
	int preview_fps;
	int picture_width;
	int picture_height;
	int picture_format;
	int jpeg_thumbnail_width;
	int jpeg_thumbnail_height;
	int jpeg_thumbnail_quality;
	int jpeg_quality;
	int recording_width;
	int recording_height;
	int recording_format;
	int focus_mode;
	int focus_x;
	int focus_y;
	int zoom;
	int flash_mode;
	int exposure_compensation;
	int whitebalance;
	int scene_mode;
	int effect;
	int iso;
	int metering;
};

struct smdk4210_camera_addrs {
	unsigned int type;
	unsigned int y;
	unsigned int cbcr;
	unsigned int index;
	unsigned int reserved;
};

// This is because the linux header uses anonymous union
struct smdk4210_v4l2_ext_control {
	__u32 id;
	__u32 size;
	__u32 reserved2[1];
	union {
		__s32 value;
		__s64 value64;
		char *string;
	} data;
} __attribute__ ((packed));

enum m5mo_af_status {
	M5MO_AF_STATUS_FAIL = 0,
	M5MO_AF_STATUS_IN_PROGRESS,
	M5MO_AF_STATUS_SUCCESS,
	M5MO_AF_STATUS_1ST_SUCCESS = 4,
};

/*
 * Camera
 */

int smdk4210_camera_params_init(struct smdk4210_camera *smdk4210_camera, int id);
int smdk4210_camera_params_apply(struct smdk4210_camera *smdk4210_camera);

int smdk4210_camera_auto_focus_start(struct smdk4210_camera *smdk4210_camera);
void smdk4210_camera_auto_focus_stop(struct smdk4210_camera *smdk4210_camera);

int smdk4210_camera_picture(struct smdk4210_camera *smdk4210_camera);
int smdk4210_camera_picture_start(struct smdk4210_camera *smdk4210_camera);

int smdk4210_camera_preview(struct smdk4210_camera *smdk4210_camera);
int smdk4210_camera_preview_start(struct smdk4210_camera *smdk4210_camera);
void smdk4210_camera_preview_stop(struct smdk4210_camera *smdk4210_camera);

/*
 * EXIF
 */

int smdk4210_exif_attributes_create_static(struct smdk4210_camera *smdk4210_camera,
	exif_attribute_t *exif_attributes);
int smdk4210_exif_attributes_create_params(struct smdk4210_camera *smdk4210_camera,
	exif_attribute_t *exif_attributes);

int smdk4210_exif_create(struct smdk4210_camera *smdk4210_camera,
	exif_attribute_t *exif_attributes,
	void *jpeg_thumbnail_data, int jpeg_thumbnail_size,
	camera_memory_t **exif_data_memory_p, int *exif_size_p);

/*
 * Param
 */

int smdk4210_param_int_get(struct smdk4210_camera *smdk4210_camera,
	char *key);
float smdk4210_param_float_get(struct smdk4210_camera *smdk4210_camera,
	char *key);
char *smdk4210_param_string_get(struct smdk4210_camera *smdk4210_camera,
	char *key);

int smdk4210_param_int_set(struct smdk4210_camera *smdk4210_camera,
	char *key, int integer);
int smdk4210_param_float_set(struct smdk4210_camera *smdk4210_camera,
	char *key, float floating);
int smdk4210_param_string_set(struct smdk4210_camera *smdk4210_camera,
	char *key, char *string);

char *smdk4210_params_string_get(struct smdk4210_camera *smdk4210_camera);
int smdk4210_params_string_set(struct smdk4210_camera *smdk4210_camera, char *string);

/*
 * Utils
 */

int smdk4210_camera_buffer_length(int width, int height, int format);
int smdk4210_gralloc_format(int format);
int smdk4210_scale_yuv422(void *src, int src_width, int src_height, void *dst,
	int dst_width, int dst_height);

/*
 * V4L2
 */

// Utils
int smdk4210_v4l2_find_index(struct smdk4210_camera *smdk4210_camera, int smdk4210_v4l2_id);
int smdk4210_v4l2_find_fd(struct smdk4210_camera *smdk4210_camera, int smdk4210_v4l2_id);

// File ops
int smdk4210_v4l2_open(struct smdk4210_camera *smdk4210_camera, int id);
void smdk4210_v4l2_close(struct smdk4210_camera *smdk4210_camera, int id);
int smdk4210_v4l2_ioctl(struct smdk4210_camera *smdk4210_camera, int id, int request, void *data);
int smdk4210_v4l2_poll(struct smdk4210_camera *smdk4210_camera, int smdk4210_v4l2_id);

// VIDIOC
int smdk4210_v4l2_qbuf(struct smdk4210_camera *smdk4210_camera, int smdk4210_v4l2_id,
	int type, int memory, int index);
int smdk4210_v4l2_qbuf_cap(struct smdk4210_camera *smdk4210_camera, int smdk4210_v4l2_id,
	int index);
int smdk4210_v4l2_qbuf_out(struct smdk4210_camera *smdk4210_camera, int smdk4210_v4l2_id,
	int index);
int smdk4210_v4l2_dqbuf(struct smdk4210_camera *smdk4210_camera, int smdk4210_v4l2_id,
	int type, int memory);
int smdk4210_v4l2_dqbuf_cap(struct smdk4210_camera *smdk4210_camera, int smdk4210_v4l2_id);
int smdk4210_v4l2_dqbuf_out(struct smdk4210_camera *smdk4210_camera, int smdk4210_v4l2_id);
int smdk4210_v4l2_reqbufs(struct smdk4210_camera *smdk4210_camera, int smdk4210_v4l2_id,
	int type, int memory, int count);
int smdk4210_v4l2_reqbufs_cap(struct smdk4210_camera *smdk4210_camera, int smdk4210_v4l2_id,
	int count);
int smdk4210_v4l2_reqbufs_out(struct smdk4210_camera *smdk4210_camera, int smdk4210_v4l2_id,
	int count);
int smdk4210_v4l2_querybuf(struct smdk4210_camera *smdk4210_camera, int smdk4210_v4l2_id,
	int type, int memory, int index);
int smdk4210_v4l2_querybuf_cap(struct smdk4210_camera *smdk4210_camera, int smdk4210_v4l2_id,
	int index);
int smdk4210_v4l2_querybuf_out(struct smdk4210_camera *smdk4210_camera, int smdk4210_v4l2_id,
	int index);
int smdk4210_v4l2_querycap(struct smdk4210_camera *smdk4210_camera, int smdk4210_v4l2_id,
	int flags);
int smdk4210_v4l2_querycap_cap(struct smdk4210_camera *smdk4210_camera, int smdk4210_v4l2_id);
int smdk4210_v4l2_querycap_out(struct smdk4210_camera *smdk4210_camera, int smdk4210_v4l2_id);
int smdk4210_v4l2_streamon(struct smdk4210_camera *smdk4210_camera, int smdk4210_v4l2_id,
	int type);
int smdk4210_v4l2_streamon_cap(struct smdk4210_camera *smdk4210_camera, int smdk4210_v4l2_id);
int smdk4210_v4l2_streamon_out(struct smdk4210_camera *smdk4210_camera, int smdk4210_v4l2_id);
int smdk4210_v4l2_streamoff(struct smdk4210_camera *smdk4210_camera, int smdk4210_v4l2_id,
	int type);
int smdk4210_v4l2_streamoff_cap(struct smdk4210_camera *smdk4210_camera, int smdk4210_v4l2_id);
int smdk4210_v4l2_streamoff_out(struct smdk4210_camera *smdk4210_camera, int smdk4210_v4l2_id);
int smdk4210_v4l2_g_fmt(struct smdk4210_camera *smdk4210_camera, int smdk4210_v4l2_id,
	int type, int *width, int *height, int *fmt);
int smdk4210_v4l2_g_fmt_cap(struct smdk4210_camera *smdk4210_camera, int smdk4210_v4l2_id,
	int *width, int *height, int *fmt);
int smdk4210_v4l2_g_fmt_out(struct smdk4210_camera *smdk4210_camera, int smdk4210_v4l2_id,
	int *width, int *height, int *fmt);
int smdk4210_v4l2_s_fmt_pix(struct smdk4210_camera *smdk4210_camera, int smdk4210_v4l2_id,
	int type, int width, int height, int fmt, int priv);
int smdk4210_v4l2_s_fmt_pix_cap(struct smdk4210_camera *smdk4210_camera, int smdk4210_v4l2_id,
	int width, int height, int fmt, int priv);
int smdk4210_v4l2_s_fmt_pix_out(struct smdk4210_camera *smdk4210_camera, int smdk4210_v4l2_id,
	int width, int height, int fmt, int priv);
int smdk4210_v4l2_s_fmt_win(struct smdk4210_camera *smdk4210_camera, int smdk4210_v4l2_id,
	int left, int top, int width, int height);
int smdk4210_v4l2_enum_fmt(struct smdk4210_camera *smdk4210_camera, int smdk4210_v4l2_id,
	int type, int fmt);
int smdk4210_v4l2_enum_fmt_cap(struct smdk4210_camera *smdk4210_camera, int smdk4210_v4l2_id,
	int fmt);
int smdk4210_v4l2_enum_fmt_out(struct smdk4210_camera *smdk4210_camera, int smdk4210_v4l2_id,
	int fmt);
int smdk4210_v4l2_enum_input(struct smdk4210_camera *smdk4210_camera, int smdk4210_v4l2_id,
	int id);
int smdk4210_v4l2_s_input(struct smdk4210_camera *smdk4210_camera, int smdk4210_v4l2_id,
	int id);
int smdk4210_v4l2_g_ext_ctrls(struct smdk4210_camera *smdk4210_camera, int smdk4210_v4l2_id,
	struct v4l2_ext_control *control, int count);
int smdk4210_v4l2_g_ctrl(struct smdk4210_camera *smdk4210_camera, int smdk4210_v4l2_id,
	int id, int *value);
int smdk4210_v4l2_s_ctrl(struct smdk4210_camera *smdk4210_camera, int smdk4210_v4l2_id,
	int id, int value);
int smdk4210_v4l2_s_parm(struct smdk4210_camera *smdk4210_camera, int smdk4210_v4l2_id,
	int type, struct v4l2_streamparm *streamparm);
int smdk4210_v4l2_s_parm_cap(struct smdk4210_camera *smdk4210_camera, int smdk4210_v4l2_id,
	struct v4l2_streamparm *streamparm);
int smdk4210_v4l2_s_parm_out(struct smdk4210_camera *smdk4210_camera, int smdk4210_v4l2_id,
	struct v4l2_streamparm *streamparm);
int smdk4210_v4l2_s_crop(struct smdk4210_camera *smdk4210_camera, int smdk4210_v4l2_id,
	int type, int left, int top, int width, int height);
int smdk4210_v4l2_s_crop_cap(struct smdk4210_camera *smdk4210_camera, int smdk4210_v4l2_id,
	int left, int top, int width, int height);
int smdk4210_v4l2_s_crop_out(struct smdk4210_camera *smdk4210_camera, int smdk4210_v4l2_id,
	int left, int top, int width, int height);
int smdk4210_v4l2_g_fbuf(struct smdk4210_camera *smdk4210_camera, int smdk4210_v4l2_id,
	void **base, int *width, int *height, int *fmt);
int smdk4210_v4l2_s_fbuf(struct smdk4210_camera *smdk4210_camera, int smdk4210_v4l2_id,
	void *base, int width, int height, int fmt);

#endif
