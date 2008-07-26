/****************************************************************************
 * GameCube SNES9x
 *
 * Based on MSDOS VESA Renderer
 *
 * softdev - November 2005
 * Askot - May 2008
 ****************************************************************************/

#include <gccore.h>
#include <fat.h>
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

extern u8 vmode_60hz;

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
long long prev;
long long now;

int gcScreenX = 128;
int gcScreenY = 112;

int superscope_fire = 0;
int superscope_action = 0;
int superscope_pause = 0;
int superscope_turbo = 0;

int SCOPEPADCAL = 30;

char msg[32];

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

extern unsigned int FrameTimer;		/*** FrameTimer Incremented by libogc VBL ***/

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
    } 
    else {
        now = gettime();
        if ( diff_usec(prev, now) > timediffallowed )
        {
            while ( diff_usec(prev, now) < timediffallowed * 2) {
                now = gettime();
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
            while ( diff_usec(prev, now) < timediffallowed ) 
            {
	        now = gettime();
            }

            IPPU.RenderThisFrame = TRUE;
            IPPU.SkippedFrames = 0;
        }

        prev = now;
    }

    FrameTimer--;
}

bool8 S9xInitUpdate (void)
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
 * ReadSuperScope
 *
 * Emulate SuperScope through Joypads / ¿WiiMote?
 ****************************************************************************/
bool8 S9xReadSuperScopePosition (int &x, int &y, uint32 &buttons)
{
    if (Settings.SuperScope == true){
	
	    signed char padX = PAD_StickX(0);
        signed char padY = PAD_StickY(0);
       
        if (padX > SCOPEPADCAL){
            gcScreenX +=4;
            if (gcScreenX > 256) gcScreenX = 256;
        }
        if (padX < -SCOPEPADCAL){
            gcScreenX -=4;
            if (gcScreenX < 0) gcScreenX = 0;
        }
       
        if (padY < -SCOPEPADCAL){
			gcScreenY +=4;
            if (gcScreenY > 224) gcScreenY = 224;
        }
        if (padY > SCOPEPADCAL){
			gcScreenY -=4;
            if (gcScreenY < 0) gcScreenY = 0;            
        }
       
        x = gcScreenX;
        y = gcScreenY;

		unsigned short down = PAD_ButtonsDown(0);
		if (down & PAD_BUTTON_B ){ 
			superscope_fire |= 1;
		}
		if (down & PAD_BUTTON_A){ 
			superscope_action |= 1;
		}
		if (down & PAD_TRIGGER_Z){ 
			superscope_turbo ^= 1;
		}
		if (down & PAD_BUTTON_START){ 
			superscope_pause ^= 1;
		}

		unsigned short up = PAD_ButtonsUp(0);
		if (up & PAD_BUTTON_B ){ 
			superscope_fire &= ~1;
		}
		if (up & PAD_BUTTON_A){ 
			superscope_action &= ~1;
		}

		buttons = superscope_fire<<0 | superscope_action<<1 | superscope_turbo<<2 | superscope_pause<<3;

        return (TRUE);
    }
    else{
        return (FALSE);
    }
}

/****************************************************************************
 * ReadMousePosition
 *
 * Emulate mouse through Joypad
 ****************************************************************************/
bool8 S9xReadMousePosition (int which1, int &x, int &y, uint32 &buttons)
{
    if (which1 ==0 && Settings.Mouse == true){
	
	    signed char padX = PAD_StickX(0);
        signed char padY = PAD_StickY(0);
       
        if (padX > SCOPEPADCAL){
            gcScreenX +=4;
            if (gcScreenX > 256) gcScreenX = 256;
        }
        if (padX < -SCOPEPADCAL){
            gcScreenX -=4;
            if (gcScreenX < 0) gcScreenX = 0;
        }
       
        if (padY < -SCOPEPADCAL){
			gcScreenY +=4;
            if (gcScreenY > 224) gcScreenY = 224;
        }
        if (padY > SCOPEPADCAL){
			gcScreenY -=4;
            if (gcScreenY < 0) gcScreenY = 0;            
        }
       
        x = gcScreenX;
        y = gcScreenY;

		unsigned short down = PAD_ButtonsDown(0);
		if (down & PAD_BUTTON_B ){ 
			superscope_fire |= 1;
		}
		if (down & PAD_BUTTON_A){ 
			superscope_action |= 1;
		}

		unsigned short up = PAD_ButtonsUp(0);
		if (up & PAD_BUTTON_B ){ 
			superscope_fire &= ~1;
		}
		if (up & PAD_BUTTON_A){ 
			superscope_action &= ~1;
		}

		buttons = superscope_fire<<0 | superscope_action<<1;

        return (TRUE);
    }
    else{
        return (FALSE);
    }
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

    prev = gettime();
    if ( Settings.PAL )
    {
        if(vmode_60hz == 1)
            timerstyle = 1;
        else
            timerstyle = 0;
        timediffallowed = Settings.FrameTimePAL;
    }
    else
    {
        if(vmode_60hz == 1)
            timerstyle = 0;
        else
            timerstyle = 1;
        timediffallowed = Settings.FrameTimeNTSC;
    }

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
int main()
{
    unsigned int save_flags;

    InitGCVideo();
    SYS_SetResetCallback(reset_cb);
    unpackanim();
    fatInitDefault();

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

    ConfigMenu();

    Emulate();

    return 0;
}

