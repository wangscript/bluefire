/*


  Bluefire: Audio Functions

  (c) Copyright 2004-2005 Jacob Hipps

  Bluefire MDL Core: Winchester

  Status: WIP, Level 1
  ** This module is not yet ready for general use.

  Implements audio using OpenAL

  To do:
	Add sound pool support, similar to the object pool

*/


#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../project_w/libpng/png.h"
#include "../project_w/zlib/zlib.h"
#include "bluefire.h"
#include <al.h>
#include <alc.h>
#include <alut.h>


ALuint Source;
ALuint Buffer;
ALCcontext *Context;
ALCdevice *Device;

int bf_init_audio() {
	BEGIN_FUNC("bf_init_audio")

	zlogthis("bf_init_audio: Initializing OpenAL...\n");
	zlogthis("bf_init_audio: Enumerating audio devices...\n\n");

	char *auddevs;
	auddevs = (char*)alcGetString(NULL,ALC_DEVICE_SPECIFIER);

	char devices[16][32];
	int x;
	int xx = 0;
	for(int i = 0; i < 16; i++) {
		for(x = 0; x < 31; x++) {
			devices[i][x] = auddevs[xx];
			xx++;
			if(auddevs[xx - 1] == 0) break;			
		}
		// list terminates at double-null
		if(auddevs[xx - 1] == 0 && auddevs[xx] == 0) break;
	}

	zlogthis("%i devices enumerated.\n", i + 1);
	for(x = 0; x <= i; x++) {
		zlogthis("Device %i: %s\n",x,devices[x]);
	}


	Device = alcOpenDevice("Generic Hardware");
	//Device = alcOpenDevice(NULL);

	if (Device == NULL) {
		zlogthis("bf_init_audio: Unable to initialize audio device!\n");
		MessageBox(NULL,"Unable to initialize audio device!\nMake sure OpenAL is installed correctly!","bluefire->bf_init_audio",0);
		return -1;
	}

	//Create context(s)
	Context = alcCreateContext(Device, NULL);
	//Set active context
	alcMakeContextCurrent(Context);
	alGetError();
	
	zlogthis("bf_init_audio: Audio subsystem information...\n\n"
			 "OpenAL Version: %s\n"
			 "Vendor: %s\n"
			 "Renderer: %s\n"
			 "EAX 2.0 Capable: %s\n"
			 "\n"
			 ,alGetString(AL_VERSION),alGetString(AL_VENDOR),alGetString(AL_RENDERER)
			 ,(alIsExtensionPresent("EAX2.0") ? "Yes" : "No"));
	zlogthis("bf_init_audio: Ok. OpenAL initialized successfully.\n");

	END_FUNC
	return 0;
}


int bf_kill_audio() {
	BEGIN_FUNC("bf_kill_audio")

	alSourceStop(Source);
	zlogthis("bf_kill_audio: Deallocating buffers...\n");
	alDeleteBuffers(1, &Buffer);
    alDeleteSources(1, &Source);

	zlogthis("bf_kill_audio: Killing OpenAL...\n");

	//Get active context
	Context = alcGetCurrentContext();
	//Get device for active context
	Device = alcGetContextsDevice(Context);
	//Disable context
	alcMakeContextCurrent(NULL);
	//Release context(s)
	alcDestroyContext(Context);
	//Close device
	alcCloseDevice(Device);

	zlogthis("bf_kill_audio: Ok. OpenAL closed.\n");

	END_FUNC
	return 0;
}


int bf_al_test(){

// Position of the source sound.
ALfloat SourcePos[] = { 0.0, 0.0, 0.0 };

// Velocity of the source sound.
ALfloat SourceVel[] = { 0.0, 0.0, 0.0 };

// Position of the listener.
ALfloat ListenerPos[] = { 0.0, 0.0, 0.0 };

// Velocity of the listener.
ALfloat ListenerVel[] = { 0.0, 0.0, 0.0 };

// Orientation of the listener. (first 3 elements are "at", second 3 are "up")
ALfloat ListenerOri[] = { 0.0, 0.0, -1.0,  0.0, 1.0, 0.0 };

    // Variables to load into.

    ALenum format;
    ALsizei size;
    ALvoid* data;
    ALsizei freq;
    ALboolean loop;

    // Load wav data into a buffer.
    alGenBuffers(1, &Buffer);
    if (alGetError() != AL_NO_ERROR)
        zlogthis("alGetError: assertion failed! [0]\n");

    alutLoadWAVFile("waterjuice-low_room.wav", &format, &data, &size, &freq, &loop);
	zlogthis("audio info: data = 0x%08x, size = %i, freq = %i, loop = %i\n",data,size,freq,loop);
    alBufferData(Buffer, format, data, size, freq);
    alutUnloadWAV(format, data, size, freq);

	    // Bind buffer with a source.
    alGenSources(1, &Source);

    if (alGetError() != AL_NO_ERROR)
        zlogthis("alGetError: assertion failed! [1]\n");

    alSourcei (Source, AL_BUFFER,   Buffer   );
    alSourcef (Source, AL_PITCH,    1.0f     );
    alSourcef (Source, AL_GAIN,     1.0f     );
    alSourcefv(Source, AL_POSITION, SourcePos);
    alSourcefv(Source, AL_VELOCITY, SourceVel);
    alSourcei (Source, AL_LOOPING,  loop     );

    // Do another error check and return.
	if (alGetError() != AL_NO_ERROR)
		zlogthis("alGetError: assertion failed! [2]\n");

	alListenerfv(AL_POSITION,    ListenerPos);
    alListenerfv(AL_VELOCITY,    ListenerVel);
    alListenerfv(AL_ORIENTATION, ListenerOri);

	alSourcePlay(Source);

	return 0;
}

