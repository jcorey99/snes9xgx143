/****************************************************************************
 * Snes9x 1.51 Nintendo Wii/Gamecube Port
 *
 * softdev July 2006
 * crunchy2 May 2007
 * Michniewski 2008
 * Tantric September 2008
 *
 * s9xconfig.cpp
 *
 * Configuration parameters are here for easy maintenance.
 * Refer to Snes9x.h for all combinations.
 * The defaults used here are taken directly from porting.html
 ***************************************************************************/

#include <gccore.h>
#include "snes9x.h"
#include "snes9xGX.h"
#include "smbop.h"

struct SGCSettings GCSettings;

void
DefaultSettings ()
{
	/************** GameCube/Wii Settings *********************/
	GCSettings.LoadMethod = METHOD_AUTO; // Auto, SD, DVD, USB, Network (SMB)
	GCSettings.SaveMethod = METHOD_AUTO; // Auto, SD, Memory Card Slot A, Memory Card Slot B, USB, Network (SMB)
	sprintf (GCSettings.LoadFolder,"snes9x143/roms"); // Path to game files
	sprintf (GCSettings.SaveFolder,"snes9x143/saves"); // Path to save files
	sprintf (GCSettings.CheatFolder,"snes9x143/cheats"); // Path to cheat files
	GCSettings.AutoLoad = 1;
	GCSettings.AutoSave = 1;

	GCSettings.VerifySaves = 0;

	// custom SMB settings
	strncpy (GCSettings.smbip, "", 15); // IP Address of share server
	strncpy (GCSettings.smbuser, "", 19); // Your share user
	strncpy (GCSettings.smbpwd, "", 19); // Your share user password
	strncpy (GCSettings.smbshare, "", 19); // Share name on server

	GCSettings.smbip[15] = 0;
	GCSettings.smbuser[19] = 0;
	GCSettings.smbpwd[19] = 0;
	GCSettings.smbshare[19] = 0;

	GCSettings.gcip[0] = 0;
	GCSettings.gwip[0] = 0;
	GCSettings.mask[0] = 0;
	GCSettings.smbsvid[0] = 0;
	GCSettings.smbgcid[0] = 0;

	GCSettings.Superscope = 0;
    GCSettings.Mouse = 0;
	GCSettings.Justifier = 0;

	GCSettings.Zoom = 0;	// zooming default off
	GCSettings.ZoomLevel = 1.0; // zoom level
	GCSettings.render = 2; // Unfiltered
	GCSettings.widescreen = 0; // no aspect ratio correction

	GCSettings.xshift = 0;	// video shift
	GCSettings.yshift = 0;

	/****************** SNES9x Settings ***********************/

	// Default ALL to false
	memset (&Settings, 0, sizeof (Settings));

	// General

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
    Settings.ReverseStereo = TRUE;

    Settings.SwapJoypads = FALSE;
}

