/****************************************************************************
 * Snes9x 1.51 Nintendo Wii/Gamecube Port
 *
 * softdev July 2006
 * crunchy2 May 2007
 * Michniewski 2008
 * Tantric September 2008
 *
 * s9xsupport.cpp
 *
 * This file contains the supporting functions defined in porting.html, with
 * others taken from unix/x11.cpp and unix/unix.cpp
 ***************************************************************************/

#include <gccore.h>
#include <ogcsys.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "snes9x.h"
#include "memmap.h"
#include "debug.h"
#include "cpuexec.h"
#include "ppu.h"
#include "apu.h"
#include "display.h"
#include "gfx.h"
#include "soundux.h"
#include "spc700.h"
#include "spc7110.h"
//#include "controls.h"

#include "snes9xGX.h"
#include "video.h"
#include "audio.h"
#include "input.h"

extern u32 FrameTimer;

long long prev;
long long now;
unsigned int timediffallowed;

extern "C" {

long long gettime();
u32 diff_usec(long long start,long long end);

}


/****************************************************************************
 * ReadJoypad
 *
 * Calls GetJoys in gcpads
 ****************************************************************************/
unsigned int S9xReadJoypad (int which1)
{
    if ( Settings.MultiPlayer5 )
        return 0x80000000 | GetJoys( which1 );
    else
        return (which1 < 2) ? 0x80000000 | GetJoys( which1 ) : 0 ;
}

/*** Miscellaneous Functions ***/
void
S9xMessage (int /*type */ , int /*number */ , const char *message)
{
#define MAX_MESSAGE_LEN (36 * 3)

  static char buffer[MAX_MESSAGE_LEN + 1];
  strncpy (buffer, message, MAX_MESSAGE_LEN);
  buffer[MAX_MESSAGE_LEN] = 0;
  S9xSetInfoString (buffer);
}

void
S9xExit ()
{
	/*** Nintendo Gamecube will NEVER get here ... unless
	      something major went wrong !!

	      In which case, I'll settle for a reboot first -;)
	***/
}

/****************************************************************************
 * GetFilename - PlaceHolder
 ****************************************************************************/
const char *S9xGetFilename (const char *e)
{
    return "0";
}


/*** Memory based functions ***/
void
S9xAutoSaveSRAM ()
{

}


/****************************************************************************
 * OpenSoundDevice
 *
 * Main initialisation for NGC sound system
 ***************************************************************************/
bool8
S9xOpenSoundDevice (int mode, bool8 stereo, int buffer_size)
{
  so.stereo = TRUE;
  so.playback_rate = 32000;
  so.sixteen_bit = TRUE;
  so.encoded = 0;
  so.buffer_size = 4096;
  so.sound_switch = 255;
  S9xSetPlaybackRate (so.playback_rate);

  InitGCAudio ();
  return TRUE;
}

/*** Deprecated function. NGC uses threaded sound ***/
void
S9xGenerateSound ()
{
}

/* eke-eke */
void S9xInitSync()
{
  FrameTimer = 0;
  prev = gettime();
}

/*** Synchronisation ***/
extern int timerstyle;

void S9xSyncSpeed ()
{
    uint32 skipFrms = Settings.SkipFrames;

    if ( Settings.TurboMode )
        skipFrms = Settings.TurboSkipFrames;

    if ( timerstyle == 0 ) /* use NGC vertical sync (VSYNC) with NTSC roms */
    {
        while (FrameTimer == 0)
        {
            usleep (50);
        }

        if (FrameTimer > skipFrms)
            FrameTimer = skipFrms;

        if ((FrameTimer > 1) && (IPPU.SkippedFrames < skipFrms))
        {
            IPPU.SkippedFrames++;
            IPPU.RenderThisFrame = FALSE;
        }
        else
        {
            IPPU.SkippedFrames = 0;
            IPPU.RenderThisFrame = TRUE;
        }
	}
    else  /* use internal timer for PAL roms */
    {
        timediffallowed = Settings.TurboMode ? 0 : Settings.FrameTime;
        now = gettime();

        if (diff_usec(prev, now) > timediffallowed)
        {
            /*while ( diff_usec((prev, now) <  timediffallowed * 2) {
                now = gettime();
            }*/

            /* Timer has already expired */
            if (IPPU.SkippedFrames < skipFrms)
            {
                IPPU.SkippedFrames++;
                IPPU.RenderThisFrame = FALSE;
            }
            else
            {
                IPPU.SkippedFrames = 0;
                IPPU.RenderThisFrame = TRUE;
            }
        }
        else
        {
            /*** Ahead - so hold up ***/
            while (diff_usec(prev, now) <  timediffallowed) now=gettime();
            IPPU.RenderThisFrame = TRUE;
            IPPU.SkippedFrames = 0;
        }

        prev = now;
    }

    if ( !Settings.TurboMode )
        FrameTimer--;
    return;

}

/*** Video / Display related functions ***/
bool8
S9xInitUpdate ()
{
	memset(GFX.Screen, 0, IMAGE_WIDTH * IMAGE_HEIGHT * 2);
	return (TRUE);
}

bool8
S9xDeinitUpdate (int Width, int Height, bool8 sixteen_bit)
{
  update_video (Width, Height);
  return (TRUE);
}

void
S9xLoadSDD1Data ()
{

  Memory.FreeSDD1Data ();

  Settings.SDD1Pack = FALSE;

  if (strncmp (Memory.ROMName, "Star Ocean", 10) == 0)
    Settings.SDD1Pack = TRUE;

  if (strncmp (Memory.ROMName, "STREET FIGHTER ALPHA2", 21) == 0)
    Settings.SDD1Pack = TRUE;

  return;

}

/****************************************************************************
 * Note that these are DUMMY functions, and only allow Snes9x to
 * compile. Where possible, they will return an error signal.
 ***************************************************************************/

/*** splitpath function ***/
void
_splitpath (char const *buffer, char *drive, char *dir, char *fname,
	    char *ext)
{
  return;	/*** Do nothing - NGC code should NEVER call a function which relies on it ***/
}

void
_makepath (char *filename, const char *drive, const char *dir,
	   const char *fname, const char *ext)
{
  return;	/*** Do nothing - NGC code should NEVER call a function which relies on it ***/
}

char *
S9xBasename (char *name)
{
  return name;
}
