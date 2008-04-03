/****************************************************************************
 * GameCube SNES9x
 *
 * Based on MSDOS VESA Renderer
 *
 * softdev - November 2005
 ****************************************************************************/

#include <gccore.h>
#include <sdcard.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <ogcsys.h>
#include "snes9x.h"
#include "memmap.h"
#include "debug.h"
#include "ppu.h"
#include "snapshot.h"
#include "gfx.h"
#include "display.h"
#include "apu.h"
#include "soundux.h"
#include "port.h"
#include "gcconfig.h"
#include "gctime.h"

uint32 screen_width = 0;
uint32 screen_height = 0;

/*** Video defaults ***/
unsigned char *offscreen;
unsigned char *subscreen;
unsigned char *Delta;
unsigned int timerres;
unsigned char isWii = false;
int ConfigRequested = 0;

extern void InitGCVideo();
extern void DrawFrame( int x, int y);
extern unsigned int GetJoys( int which );
extern void S9xSetPalette16();
extern bool SoundSetup();
extern void GenerateSound();
extern void SetupSound ();
extern void AudioGo();
extern int snesromsize;
extern int ConfigMenu();
extern void unpackanim();
static unsigned char *inquiry=(unsigned char *)0x80000004;
extern void dvd_inquiry();

void (*PSOReload)() = (void(*)())0x80001800;
static void reset_cb() {
    PSOReload();
}
tb_t prev;
tb_t now;

/****************************************************************************
 * Dummy Message Handler
 ****************************************************************************/
void S9xMessage (int type, int message_no, const char *str)
{
	char buffer[2048];
	strcpy(buffer, str);
	buffer[3*35] = 0;
	S9xSetInfoString(str);
}

/****************************************************************************
 * GetFilenameInc - PlaceHolder
 ****************************************************************************/
const char *S9xGetFilenameInc (const char *e)
{
	return (char *)"NONE";
}

extern unsigned int FrameTimer;		/*** FrameTimer
					     Incremented by libogc VBL ***/

int timerstyle = 0;
unsigned int timediffallowed;

void S9xSyncSpeed()
{

	if ( timerstyle == 0 ) {
		while ( FrameTimer == 0 ) { usleep(100); };	/*** Yay! - we're ahead ***/ 

		if ( FrameTimer > Settings.SkipFrames )
			FrameTimer = Settings.SkipFrames;

		if ( ( FrameTimer > 1 ) && ( IPPU.SkippedFrames < Settings.SkipFrames ))
		{
			IPPU.SkippedFrames++;
			IPPU.RenderThisFrame = FALSE;
		} 
		else
		{
			IPPU.RenderThisFrame = TRUE;
			IPPU.SkippedFrames = 0;
		}
	} else {
		mftb(&now);
		
		if ( tb_diff_usec(&now, &prev) > timediffallowed )
		{
			while ( tb_diff_usec(&now, &prev) < timediffallowed * 2) {
				mftb(&now);
			}

			if ( IPPU.SkippedFrames < Settings.SkipFrames ) {
				IPPU.SkippedFrames++;
				IPPU.RenderThisFrame = FALSE;
			} else {
				IPPU.SkippedFrames = 0;
				IPPU.RenderThisFrame = TRUE;
			}
		} else {
			/*** Ahead - so hold up ***/
			while ( tb_diff_usec(&now, &prev) < timediffallowed ) 
			{
				mftb(&now);
			}

			IPPU.RenderThisFrame = TRUE;
			IPPU.SkippedFrames = 0;
		}

		memcpy(&prev, &now, sizeof(tb_t));
	}

	FrameTimer--;
}

bool8  S9xInitUpdate (void)
{
	memset(GFX.Screen, 0, IMAGE_WIDTH * IMAGE_HEIGHT * 2);
	return TRUE;
}

/****************************************************************************
 * DeinitUpdate
 *
 * Calls DrawFrame in gcvideo
 ****************************************************************************/
bool8 S9xDeinitUpdate(int Width, int Height, bool8 sixteen_bit)
{
	DrawFrame(Width, Height);
	return TRUE;
}

/****************************************************************************
 * AutoSaveSRAM
 *
 * Does nothing for now
 ****************************************************************************/
void S9xAutoSaveSRAM ()
{
	/*if (Memory.SRAMSize > 0){
	}*/
	return;
}

/****************************************************************************
 * GetFilename - PlaceHolder
 ****************************************************************************/
const char *S9xGetFilename (const char *e)
{
	return "0";
}

/****************************************************************************
 * LoadSDD1Data - PlaceHolder
 ****************************************************************************/
void S9xLoadSDD1Data ()
{

    Memory.FreeSDD1Data ();

    Settings.SDD1Pack=FALSE;

    if (strncmp (Memory.ROMName, "Star Ocean", 10) == 0)
	Settings.SDD1Pack = TRUE;

    if ( strncmp(Memory.ROMName, "STREET FIGHTER ALPHA2", 21)==0)
	Settings.SDD1Pack = TRUE;

    return;

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

/****************************************************************************
 * OpenSoundDevice
 *
 * Calls SetupSound in gcaudio
 ****************************************************************************/
unsigned char S9xOpenSoundDevice (int mode, unsigned char pStereo, int BufferSize)
{
	SetupSound ();
	return TRUE;
}

/****************************************************************************
 * Exit - WILL NEVER GET HERE!
 ****************************************************************************/
void S9xExit( void)
{
	return;
}

/****************************************************************************
 * Configure defaults
 ****************************************************************************/

void configDefaults()
{
	/*** These are the defaults chosen for GC ***/

	/*** Sound ***/
	Settings.SoundPlaybackRate = 5;
	Settings.Stereo = TRUE;
	Settings.SoundBufferSize = 0;
	Settings.SixteenBitSound = TRUE;
	Settings.SoundSync = FALSE;
	Settings.DisableSoundEcho = 0;
	Settings.AltSampleDecode = 0;
	Settings.SoundEnvelopeHeightReading = FALSE;
	Settings.FixFrequency = 0;
	Settings.CyclesPercentage = 100;
	Settings.InterpolatedSound = TRUE;
	Settings.APUEnabled = Settings.NextAPUEnabled = TRUE;
	Settings.SoundMixInterval = 0;
	Settings.H_Max = SNES_CYCLES_PER_SCANLINE;
	Settings.SkipFrames = 10;
	Settings.ShutdownMaster = TRUE;
	Settings.FrameTimePAL = 20000;
	Settings.FrameTimeNTSC = 16667;
	Settings.DisableSampleCaching = FALSE;
	Settings.DisableMasterVolume = FALSE;
	Settings.Mouse = FALSE;
	Settings.SuperScope = FALSE;
	Settings.MultiPlayer5 = FALSE;
	Settings.TurboMode = FALSE;
	Settings.TurboSkipFrames = 40;
	Settings.ControllerOption = SNES_MULTIPLAYER5;
	Settings.Transparency = TRUE;
	Settings.SixteenBit = TRUE;
	Settings.SupportHiRes = TRUE;
	Settings.NetPlay = FALSE;
	Settings.ServerName [0] = 0;
	Settings.ThreadSound = FALSE;
	Settings.AutoSaveDelay = 30;
	Settings.HBlankStart = (256 * Settings.H_Max) / SNES_HCOUNTER_MAX;
	Settings.DisplayFrameRate = FALSE;
	Settings.ReverseStereo = FALSE;

	Settings.SwapJoypads = FALSE;

}

/****************************************************************************
 * Emulation loop
 ****************************************************************************/
void Emulate()
{

	AudioGo();
	FrameTimer = 0;
	
	/*** Sync ***/
	VIDEO_WaitVSync();

	mftb(&prev);
	if ( Settings.PAL )
		timediffallowed = Settings.FrameTimePAL;
	else
		timediffallowed = Settings.FrameTimeNTSC;

	while (1)
	{
		if ( Settings.Paused ) {
			S9xSetSoundMute(TRUE);
		} else {
			/*** This is the real emulator loop ***/
			S9xMainLoop();
			if ( ConfigRequested ) {
				ConfigMenu();
				ConfigRequested = 0;
			}
			S9xSetSoundMute(FALSE);
		}

	}
}

/****************************************************************************
 * Program Entry - Main
 ****************************************************************************/
int choosenSDSlot = 0;

int main()
{

	unsigned int save_flags;

	InitGCVideo();
        SYS_SetResetCallback(reset_cb);
	unpackanim();
	SDCARD_Init();
	
	/*** Get Drive Type ***/
	dvd_inquiry();
	int driveid = (int)inquiry[2];

	if ( ( driveid != 4 ) && ( driveid != 6 ) && ( driveid != 8 ) ) {
		isWii = true;
	}

	/*** Configure defaults, a la msdos ***/
	configDefaults();

	/*** Allocate SNES Memory ***/
	if ( !Memory.Init() )
		while(1) {};

	if ( !S9xInitAPU() )
		while (1) {};

  	S9xSetRenderPixelFormat(RGB565);

	S9xInitSound( 5, TRUE, 1024 );

		offscreen = (unsigned char *)malloc( ( IMAGE_WIDTH  * IMAGE_HEIGHT * 2));
		GFX.Screen = offscreen;
		GFX.Pitch = (IMAGE_WIDTH * 2);
		subscreen = (unsigned char *)malloc ( IMAGE_WIDTH * IMAGE_HEIGHT * 2 );
		GFX.SubScreen = subscreen;
		GFX.ZBuffer = (unsigned char *)malloc((IMAGE_WIDTH  * IMAGE_HEIGHT * 2));
		GFX.SubZBuffer = (unsigned char *)malloc(IMAGE_WIDTH  * IMAGE_HEIGHT * 2);
		GFX.Delta = ( GFX.SubScreen - GFX.Screen ) >> 1;
	
	/*** Initialise Inputs ***/
	if ( !S9xGraphicsInit() )
		while(1) {};

        save_flags = CPU.Flags;
        if ( !Memory.LoadROM("BLANKROM.SMC"))
                while(1) {};
        CPU.Flags = save_flags;

	Memory.LoadSRAM( "DVD" );
	
	choosenSDSlot = !WaitPromptChoice("Choose a SLOT to load Roms from SDCARD", "SLOT B", "SLOT A");
	ConfigMenu();

	Emulate();

	return 0;
}

