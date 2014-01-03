/*
 * Copyright (C) 2013 Paul Kocialkowski <contact@paulk.fr>
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
#include <errno.h>
#include <malloc.h>
#include <ctype.h>

#define LOG_TAG "smdk4210_param"
#include <utils/Log.h>

#include "smdk4210_camera.h"

int list_head_insert(struct list_head *list, struct list_head *prev,
	struct list_head *next)
{
	if (list == NULL)
		return -EINVAL;

	list->prev = prev;
	list->next = next;

	if(prev != NULL)
		prev->next = list;
	if(next != NULL)
		next->prev = list;

	return 0;
}

void list_head_remove(struct list_head *list)
{
	if(list == NULL)
		return;

	if(list->next != NULL)
		list->next->prev = list->prev;
	if(list->prev != NULL)
		list->prev->next = list->next;
}

int smdk4210_param_register(struct smdk4210_camera *smdk4210_camera, char *key,
	union smdk4210_param_data data, enum smdk4210_param_type type)
{
	struct list_head *list_end;
	struct list_head *list;
	struct smdk4210_param *param;

	if (smdk4210_camera == NULL || key == NULL)
		return -EINVAL;

	param = (struct smdk4210_param *) calloc(1, sizeof(struct smdk4210_param));
	if (param == NULL)
		return -ENOMEM;

	param->key = strdup(key);
	switch (type) {
		case SMDK4210_PARAM_INT:
			param->data.integer = data.integer;
			break;
		case SMDK4210_PARAM_FLOAT:
			param->data.floating = data.floating;
			break;
		case SMDK4210_PARAM_STRING:
			param->data.string = strdup(data.string);
			break;
		default:
			ALOGE("%s: Invalid type", __func__);
			goto error;
	}
	param->type = type;

	list_end = (struct list_head *) smdk4210_camera->params;
	while (list_end != NULL && list_end->next != NULL)
		list_end = list_end->next;

	list = (struct list_head *) param;
	list_head_insert(list, list_end, NULL);

	if (smdk4210_camera->params == NULL)
		smdk4210_camera->params = param;

	return 0;

error:
	if (param != NULL) {
		if (param->key != NULL)
			free(param->key);

		free(param);
	}

	return -1;
}

void smdk4210_param_unregister(struct smdk4210_camera *smdk4210_camera,
	struct smdk4210_param *param)
{
	struct list_head *list;

	if (smdk4210_camera == NULL || param == NULL)
		return;

	list = (struct list_head *) smdk4210_camera->params;
	while (list != NULL) {
		if ((void *) list == (void *) param) {
			list_head_remove(list);

			if ((void *) list == (void *) smdk4210_camera->params)
				smdk4210_camera->params = (struct smdk4210_param *) list->next;

			if (param->type == SMDK4210_PARAM_STRING && param->data.string != NULL)
				free(param->data.string);

			memset(param, 0, sizeof(struct smdk4210_param));
			free(param);

			break;
		}

list_continue:
		list = list->next;
	}
}

struct smdk4210_param *smdk4210_param_find_key(struct smdk4210_camera *smdk4210_camera,
	char *key)
{
	struct smdk4210_param *param;
	struct list_head *list;

	if (smdk4210_camera == NULL || key == NULL)
		return NULL;

	list = (struct list_head *) smdk4210_camera->params;
	while (list != NULL) {
		param = (struct smdk4210_param *) list;
		if (param->key == NULL)
			goto list_continue;

		if (strcmp(param->key, key) == 0)
			return param;

list_continue:
		list = list->next;
	}

	return NULL;
}

int smdk4210_param_data_set(struct smdk4210_camera *smdk4210_camera, char *key,
	union smdk4210_param_data data, enum smdk4210_param_type type)
{
	struct smdk4210_param *param;

	if (smdk4210_camera == NULL || key == NULL)
		return -EINVAL;

	if (strchr(key, '=') || strchr(key, ';'))
		return -EINVAL;

	if (type == SMDK4210_PARAM_STRING && data.string != NULL &&
		(strchr(data.string, '=') || strchr(data.string, ';')))
		return -EINVAL;

	param = smdk4210_param_find_key(smdk4210_camera, key);
	if (param == NULL) {
		// The key isn't in the list yet
		smdk4210_param_register(smdk4210_camera, key, data, type);
		return 0;
	}

	if (param->type != type)
		ALOGE("%s: Mismatching types for key %s", __func__, key);

	if (param->type == SMDK4210_PARAM_STRING && param->data.string != NULL)
		free(param->data.string);

	switch (type) {
		case SMDK4210_PARAM_INT:
			param->data.integer = data.integer;
			break;
		case SMDK4210_PARAM_FLOAT:
			param->data.floating = data.floating;
			break;
		case SMDK4210_PARAM_STRING:
			param->data.string = strdup(data.string);
			break;
		default:
			ALOGE("%s: Invalid type", __func__);
			return -1;
	}
	param->type = type;

	return 0;
}

int smdk4210_param_data_get(struct smdk4210_camera *smdk4210_camera, char *key,
	union smdk4210_param_data *data, enum smdk4210_param_type type)
{
	struct smdk4210_param *param;

	if (smdk4210_camera == NULL || key == NULL || data == NULL)
		return -EINVAL;

	param = smdk4210_param_find_key(smdk4210_camera, key);
	if (param == NULL || param->type != type)
		return -1;

	memcpy(data, &param->data, sizeof(param->data));

	return 0;
}

int smdk4210_param_int_get(struct smdk4210_camera *smdk4210_camera,
	char *key)
{
	union smdk4210_param_data data;
	int rc;

	if (smdk4210_camera == NULL || key == NULL)
		return -EINVAL;

	rc = smdk4210_param_data_get(smdk4210_camera, key, &data, SMDK4210_PARAM_INT);
	if (rc < 0) {
		ALOGE("%s: Unable to get data for key %s", __func__, key);
		return -1;
	}

	return data.integer;
}

float smdk4210_param_float_get(struct smdk4210_camera *smdk4210_camera,
	char *key)
{
	union smdk4210_param_data data;
	int rc;

	if (smdk4210_camera == NULL || key == NULL)
		return -EINVAL;

	rc = smdk4210_param_data_get(smdk4210_camera, key, &data, SMDK4210_PARAM_FLOAT);
	if (rc < 0) {
		ALOGE("%s: Unable to get data for key %s", __func__, key);
		return -1;
	}

	return data.floating;
}

char *smdk4210_param_string_get(struct smdk4210_camera *smdk4210_camera,
	char *key)
{
	union smdk4210_param_data data;
	int rc;

	if (smdk4210_camera == NULL || key == NULL)
		return NULL;

	rc = smdk4210_param_data_get(smdk4210_camera, key, &data, SMDK4210_PARAM_STRING);
	if (rc < 0) {
		ALOGE("%s: Unable to get data for key %s", __func__, key);
		return NULL;
	}

	return data.string;
}

int smdk4210_param_int_set(struct smdk4210_camera *smdk4210_camera,
	char *key, int integer)
{
	union smdk4210_param_data data;
	int rc;

	if (smdk4210_camera == NULL || key == NULL)
		return -EINVAL;

	data.integer = integer;

	rc = smdk4210_param_data_set(smdk4210_camera, key, data, SMDK4210_PARAM_INT);
	if (rc < 0) {
		ALOGE("%s: Unable to set data for key %s", __func__, key);
		return -1;
	}

	return 0;
}

int smdk4210_param_float_set(struct smdk4210_camera *smdk4210_camera,
	char *key, float floating)
{
	union smdk4210_param_data data;
	int rc;

	if (smdk4210_camera == NULL || key == NULL)
		return -EINVAL;

	data.floating = floating;

	rc = smdk4210_param_data_set(smdk4210_camera, key, data, SMDK4210_PARAM_FLOAT);
	if (rc < 0) {
		ALOGE("%s: Unable to set data for key %s", __func__, key);
		return -1;
	}

	return 0;
}

int smdk4210_param_string_set(struct smdk4210_camera *smdk4210_camera,
	char *key, char *string)
{
	union smdk4210_param_data data;
	int rc;

	if (smdk4210_camera == NULL || key == NULL || string == NULL)
		return -EINVAL;

	data.string = string;

	rc = smdk4210_param_data_set(smdk4210_camera, key, data, SMDK4210_PARAM_STRING);
	if (rc < 0) {
		ALOGE("%s: Unable to set data for key %s", __func__, key);
		return -1;
	}

	return 0;
}

char *smdk4210_params_string_get(struct smdk4210_camera *smdk4210_camera)
{
	struct smdk4210_param *param;
	struct list_head *list;
	char *string = NULL;
	char *s = NULL;
	int length = 0;
	int l = 0;

	if (smdk4210_camera == NULL)
		return NULL;

	list = (struct list_head *) smdk4210_camera->params;
	while (list != NULL) {
		param = (struct smdk4210_param *) list;
		if (param->key == NULL)
			goto list_continue_length;

		length += strlen(param->key);
		length++;

		switch (param->type) {
			case SMDK4210_PARAM_INT:
			case SMDK4210_PARAM_FLOAT:
				length += 16;
				break;
			case SMDK4210_PARAM_STRING:
				length += strlen(param->data.string);
				break;
			default:
				ALOGE("%s: Invalid type", __func__);
				return NULL;
		}

		length++;

list_continue_length:
		list = list->next;
	}

	if (length == 0)
		return NULL;

	string = calloc(1, length);
	s = string;

	list = (struct list_head *) smdk4210_camera->params;
	while (list != NULL) {
		param = (struct smdk4210_param *) list;
		if (param->key == NULL)
			goto list_continue;

		l = sprintf(s, "%s=", param->key);
		s += l;

		switch (param->type) {
			case SMDK4210_PARAM_INT:
				l = snprintf(s, 16, "%d", param->data.integer);
				s += l;
				break;
			case SMDK4210_PARAM_FLOAT:
				l = snprintf(s, 16, "%g", param->data.floating);
				s += l;
				break;
			case SMDK4210_PARAM_STRING:
				l = sprintf(s, "%s", param->data.string);
				s += l;
				break;
			default:
				ALOGE("%s: Invalid type", __func__);
				return NULL;
		}

		if (list->next != NULL) {
			*s = ';';
			s++;
		} else {
			*s = '\0';
			break;
		}

list_continue:
		list = list->next;
	}

	return string;
}

int smdk4210_params_string_set(struct smdk4210_camera *smdk4210_camera, char *string)
{
	union smdk4210_param_data data;
	enum smdk4210_param_type type;

	char *d = NULL;
	char *s = NULL;
	char *k = NULL;
	char *v = NULL;

	char *key;
	char *value;

	int rc;
	int i;

	if (smdk4210_camera == NULL || string == NULL)
		return -1;

	d = strdup(string);
	s = d;

	while (1) {
		k = strchr(s, '=');
		if (k == NULL)
			break;
		*k = '\0';
		key = s;

		v = strchr(k+1, ';');
		if (v != NULL)
			*v = '\0';
		value = k+1;

		k = value;
		if (isdigit(k[0]) || k[0] == '-') {
			type = SMDK4210_PARAM_INT;

			for (i=1 ; k[i] != '\0' ; i++) {
				if (k[i] == '.') {
					type = SMDK4210_PARAM_FLOAT;
				} else if (!isdigit(k[i])) {
					type = SMDK4210_PARAM_STRING;
					break;
				}
			}
		} else {
			type = SMDK4210_PARAM_STRING;
		}

		switch (type) {
			case SMDK4210_PARAM_INT:
				data.integer = atoi(value);
				break;
			case SMDK4210_PARAM_FLOAT:
				data.floating = atof(value);
				break;
			case SMDK4210_PARAM_STRING:
				data.string = value;
				break;
			default:
				ALOGE("%s: Invalid type", __func__);
				goto error;
		}

		rc = smdk4210_param_data_set(smdk4210_camera, key, data, type);
		if (rc < 0) {
			ALOGE("%s: Unable to set data for key %s", __func__, key);
			goto error;
		}

		if (v == NULL)
			break;

		s = v+1;
	}

	if (d != NULL)
		free(d);

	return 0;

error:
	if (d != NULL)
		free(d);

	return -1;
}
