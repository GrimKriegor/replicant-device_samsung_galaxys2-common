/*
 * Copyright (C) 2013-2014 Paul Kocialkowski
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

#define LOG_TAG "Audio-RIL-Interface"
#include <cutils/log.h>
#include <system/audio.h>

#include <audio_ril_interface.h>

#include <samsung-ril-socket.h>
#include <srs-client.h>

int galaxys2_mic_mute(void *pdata, int mute)
{
	return 0;
}

int galaxys2_voice_volume(void *pdata, audio_devices_t device, float volume)
{
	struct srs_client *client = NULL;
	struct srs_snd_call_volume_data call_volume;
	int rc;

	ALOGD("%s(%d, %f)", __func__, device, volume);

	if (pdata == NULL)
		return -1;

	client = (struct srs_client *) pdata;

	switch((int) device) {
		case AUDIO_DEVICE_OUT_EARPIECE:
			call_volume.type = SRS_SND_TYPE_VOICE;
			break;
		case AUDIO_DEVICE_OUT_SPEAKER:
			call_volume.type = SRS_SND_TYPE_SPEAKER;
			break;
		case AUDIO_DEVICE_OUT_WIRED_HEADSET:
		case AUDIO_DEVICE_OUT_WIRED_HEADPHONE:
			call_volume.type = SRS_SND_TYPE_HEADSET;
			break;
		case AUDIO_DEVICE_OUT_BLUETOOTH_SCO:
		case AUDIO_DEVICE_OUT_BLUETOOTH_SCO_HEADSET:
		case AUDIO_DEVICE_OUT_BLUETOOTH_SCO_CARKIT:
		case AUDIO_DEVICE_OUT_BLUETOOTH_A2DP:
		case AUDIO_DEVICE_OUT_BLUETOOTH_A2DP_HEADPHONES:
		case AUDIO_DEVICE_OUT_BLUETOOTH_A2DP_SPEAKER:
			call_volume.type = SRS_SND_TYPE_BTVOICE;
			break;
		default:
			call_volume.type = SRS_SND_TYPE_VOICE;
			break;
	}

	call_volume.volume = (int) (5 * volume);

	rc = srs_client_send(client, SRS_SND_SET_CALL_VOLUME, &call_volume, sizeof(call_volume));
	if (rc < 0)
		return -1;

	return 0;
}

int galaxys2_route(void *pdata, audio_devices_t device)
{
	struct srs_client *client;
	struct srs_snd_call_audio_path_data call_audio_path;
	int rc;

	ALOGD("%s(%d)", __func__, device);

	if (pdata == NULL)
		return -1;

	client = (struct srs_client *) pdata;

	switch((int) device) {
		case AUDIO_DEVICE_OUT_EARPIECE:
			call_audio_path.path = SRS_SND_PATH_HANDSET;
			break;
		case AUDIO_DEVICE_OUT_SPEAKER:
			call_audio_path.path = SRS_SND_PATH_SPEAKER;
			break;
		case AUDIO_DEVICE_OUT_WIRED_HEADSET:
			call_audio_path.path = SRS_SND_PATH_HEADSET;
			break;
		case AUDIO_DEVICE_OUT_WIRED_HEADPHONE:
			call_audio_path.path = SRS_SND_PATH_HEADPHONE;
			break;
		// FIXME: Bluetooth values/path relation
		case AUDIO_DEVICE_OUT_BLUETOOTH_SCO:
		case AUDIO_DEVICE_OUT_BLUETOOTH_SCO_HEADSET:
		case AUDIO_DEVICE_OUT_BLUETOOTH_SCO_CARKIT:
			call_audio_path.path = SRS_SND_PATH_BLUETOOTH;
		case AUDIO_DEVICE_OUT_BLUETOOTH_A2DP:
		case AUDIO_DEVICE_OUT_BLUETOOTH_A2DP_HEADPHONES:
		case AUDIO_DEVICE_OUT_BLUETOOTH_A2DP_SPEAKER:
			call_audio_path.path = SRS_SND_PATH_BLUETOOTH_NO_NR;
			break;
		default:
			call_audio_path.path = SRS_SND_PATH_HANDSET;
			break;
	}

	rc = srs_client_send(client, SRS_SND_SET_CALL_AUDIO_PATH, &call_audio_path, sizeof(call_audio_path));
	if (rc < 0)
		return -1;

	return 0;
}

/*
 * Interface
 */

struct audio_ril_interface galaxys2_interface = {
	NULL,
	galaxys2_mic_mute,
	galaxys2_voice_volume,
	galaxys2_route
};

struct audio_ril_interface *audio_ril_interface_open(void)
{
	struct srs_client *client = NULL;
	int rc;

	ALOGD("%s()", __func__);

	client = srs_client_create();
	if (client == NULL) {
		ALOGE("%s: Failed to create SRS client", __func__);
		return NULL;
	}

	rc = srs_client_open(client);
	if (rc < 0) {
		ALOGE("%s: Failed to open SRS client", __func__);
		return NULL;
	}

	rc = srs_ping(client);
	if (rc < 0)
		ALOGE("%s: Failed to ping SRS", __func__);

	galaxys2_interface.pdata = (void *) client;

	return &galaxys2_interface;
}

void audio_ril_interface_close(struct audio_ril_interface *interface)
{
	struct srs_client *client = NULL;
	int rc;

	ALOGD("%s(%p)", __func__, interface);

	if (interface == NULL || interface->pdata == NULL)
		return;

	client = (struct srs_client *) interface->pdata;

	rc = srs_client_close(client);
	if (rc < 0)
		ALOGE("%s: Failed to close SRS client", __func__);

	rc = srs_client_destroy(client);
	if (rc < 0)
		ALOGE("%s: Failed to destroy SRS client", __func__);
}
