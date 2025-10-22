#include <winpixel.h>

/*
 * Value          Meaning
 * SND_ASYNC      The sound is played asynchronously and the function
 *                returns immediately after beginning the sound.
 *                To terminate an asynchronously played sound, call
 *                sndPlaySound with lpszSound set to NULL.
 * SND_LOOP       The sound plays repeatedly until sndPlaySound is
 *                called again with the lpszSound parameter set to NULL.
 *                You must also specify the SND_ASYNC flag to loop sounds.
 * SND_MEMORY     The parameter specified by lpszSound points to an
 *                image of a waveform sound in memory. The data passed
 *                must be trusted by the application.
 * SND_NODEFAULT  If the sound cannot be found, the function returns
 *                silently without playing the default sound.
 * SND_NOSTOP     If a sound is currently playing in the same process,
 *                the function immediately returns FALSE, without playing
 *                the requested sound.
 * SND_SENTRY     Note  Requires Windows Vista or later. If this flag
 *                is set, the function triggers a SoundSentry event
 *                when the sound is played. For more information, see PlaySound.
 * SND_SYNC       The sound is played synchronously and the function
 *                does not return until the sound ends.
 * SND_SYSTEM     Note  Requires Windows Vista or later. If this flag
 *                is set, the sound is assigned to the audio session
 *                for system notification sounds. For more information,
 *                see PlaySound.
 */

/*
 * Winmm.dll
 * Returns TRUE if successful or FALSE otherwise.
 */
int WINPIXELCALL wpx_sound_play (const char *file, int flag) {

	return sndPlaySound(file, flag);
}

int WINPIXELCALL wpx_music_open (const char *filep, const char *sound_id) {

	MCIERROR error;
	char comm[WPXMID];
	char filepath[WPXMID];

	GetShortPathName(filep, filepath, WPXMID);
	snprintf(comm, WPXMID, "open \"%s\" alias %s wait", filepath, sound_id); //build the MCI command string
	error = mciSendString(comm, NULL, 0, NULL); //send the MCI command 

	if (error != MMSYSERR_NOERROR)
		return 0;
	else {
		snprintf(comm, WPXMID, "set %s time format ms", sound_id);
		error = mciSendString(comm, NULL, 0, NULL);
	}
	return 1;
}

int WINPIXELCALL wpx_music_play (const char *sound_id) {

	MCIERROR error;
	char comm[WPXMID];

	snprintf(comm, WPXMID, "play %s", sound_id);
	error = mciSendString(comm, NULL, 0, NULL);

	if (error != MMSYSERR_NOERROR) {
		snprintf(comm, WPXMID, "close %s",sound_id);
		error = mciSendString(comm, NULL, 0, NULL);
		return 0; 
	}

	return 1;
}

int WINPIXELCALL wpx_music_pause (const char *sound_id) {

	char comm[WPXMID];
	char buf[WPXMID];

	snprintf(comm, WPXMID, "pause %s", sound_id);

	if (mciSendString(comm, buf, WPXMID, NULL) != MMSYSERR_NOERROR)
		return 0;
	else
		return 1;
}

/* retomar o arquivo de som */
int WINPIXELCALL wpx_music_resume (const char *sound_id) {

	char buf[WPXMID];
	snprintf(buf, WPXMID, "resume %s", sound_id);
	if (mciSendString(buf, NULL, WPXMID, NULL) != MMSYSERR_NOERROR)
		return 0;
	else
		return 1;
}

/* setar audio (0 - 1000) */
int WINPIXELCALL wpx_music_volume (const char *sound_id, int volume) {

	char buf[WPXMID];
	snprintf(buf, WPXMID, "setaudio %s volume to %d", sound_id, volume);
	if (volume >= 0 && volume <= 1000) {
		if (mciSendString(buf, NULL, WPXMID, NULL) != MMSYSERR_NOERROR)
			return 0;
	}

	return 1;
}

/* controlar volume */
int WINPIXELCALL wpx_music_balance (const char *sound_id, int balance) {

	char buf[WPXMID];
	if (balance >= 0 && balance <= 1000) {
		snprintf(buf, WPXMID, "setaudio %s left volume to %d", sound_id, (1000 - balance));
		mciSendString(buf, NULL, 0, NULL);
		snprintf(buf, WPXMID, "setaudio %s right volume to %d", sound_id, balance);
		mciSendString(buf, NULL, 0, NULL);
		return 1;
	}

	return 0;
}

/* obter o comprimento de som (em milissegundos) */
long WINPIXELCALL wpx_music_length (const char *sound_id) {

	MCIERROR error; 
	char buff[WPXMID];
	char comm[WPXMID];

	snprintf(comm, WPXMID, "status %s length", sound_id);

	error = mciSendString(comm, buff, WPXMID, NULL);
	if (error != MMSYSERR_NOERROR)
		return 0;
	else
		return atol(buff);
}

/* obter a posição de reprodução atual de som (em milissegundos) */
long WINPIXELCALL wpx_music_position (const char *sound_id) {

	MCIERROR error; 
	char comm[WPXMID];
	char buff[WPXMID];

	snprintf(comm, WPXMID, "status %s position", sound_id);
	error = mciSendString(comm, buff, WPXMID, NULL);

	if (error != MMSYSERR_NOERROR)
		return 0;
	else
		return atol(buff);   
}

/* definir a posição de reprodução de som */
int WINPIXELCALL wpx_music_seek (const char *sound_id, long position) {

	char comm[WPXMID];

	snprintf(comm, WPXMID, "seek %s to %ld", sound_id, position);
	if (mciSendString(comm, NULL, 0, NULL) != MMSYSERR_NOERROR)
		return 0;
	else
		return 1;
}

/* fechar o som aberto */
int WINPIXELCALL wpx_music_close (const char *sound_id) {

	MCIERROR error;
	char comm[WPXMID];

	snprintf(comm, WPXMID, "close %s", sound_id);
	error = mciSendString(comm, NULL, 0, NULL);

	if (error != MMSYSERR_NOERROR)
		return 0;
	else
		return 1;
}