/****************************************************************************
 * Gamecube Audio
 *
 * Audio Layer for Snes9x 1.4.3
 *
 * NOTES:
 * This is the Snes9x Audio driver stripped down to the bear minimum
 * I've chosen 32Khz as this gives back a few valuable clock cycles.
 *
 * softdev - November 2005
 ****************************************************************************/

#include <gccore.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "snes9x.h"
#include "memmap.h"
#include "debug.h"
#include "ppu.h"
#include "snapshot.h"
#include "gfx.h"
#include "display.h"
#include "apu.h"
#include "soundux.h"

#define AUDIOBUFFER 2048
uint8  SoundBuffer[2][AUDIOBUFFER]  __attribute__((__aligned__(32)));
uint8  tempbuffer[AUDIOBUFFER];
int whichab = 0;

/****************************************************************************
 * Audio Callback
 *
 * Once started, should never stop :)
 ****************************************************************************/
static void AudioSwitchBuffers()
{
    uint32 *src;
    uint32 *dst;
    int count;

    //AUDIO_StopDMA();
    AUDIO_InitDMA((u32)SoundBuffer[whichab], AUDIOBUFFER);
    DCFlushRange(&SoundBuffer[whichab], AUDIOBUFFER);
    AUDIO_StartDMA();

    whichab ^= 1;
    memset(&SoundBuffer[whichab], 0, AUDIOBUFFER);
    so.samples_mixed_so_far = so.play_position = 0;

    S9xMixSamples((unsigned char *)&tempbuffer, AUDIOBUFFER >> 2);

    count = AUDIOBUFFER >> 3;
    src = (uint32 *)&tempbuffer;
    dst = (uint32 *)&SoundBuffer[whichab];

    while ( count )
    {
        *dst++ = *src;
        *dst++ = *src++;
        count--;
    }
}

/****************************************************************************
 * AudioGo
 *
 * Kick off the audio processing, before kicking in S9xMainLoop
 ****************************************************************************/
void AudioGo()
{
    AudioSwitchBuffers();
}

/****************************************************************************
 * InitialiseSound 
 *
 * libogc sound setup
 ****************************************************************************/
void InitialiseSound()
{
    AUDIO_Init(NULL);       /*** Start audio subsystem ***/    
    AUDIO_SetDSPSampleRate(AI_SAMPLERATE_48KHZ); /*** Set default samplerate to 48khz ***/    
    AUDIO_RegisterDMACallback( AudioSwitchBuffers );/*** and the DMA Callback ***/
    DCFlushRange((char *)&SoundBuffer[0], AUDIOBUFFER);
}

/****************************************************************************
 * Gamecube Audio Setup
 *
 * This is taken pretty much as is from the unix version
 ****************************************************************************/
bool SetupSound()
{
    InitialiseSound();

    so.playback_rate = 24000;
    so.sixteen_bit = TRUE;
    so.stereo = TRUE;

    S9xSetPlaybackRate( so.playback_rate );

    so.encoded = FALSE;
    so.buffer_size = AUDIOBUFFER;

    return TRUE;
}

/****************************************************************************
 * Does nothing ... really just a placeholder
 ****************************************************************************/
void S9xGenerateSound()
{
    return;
}

