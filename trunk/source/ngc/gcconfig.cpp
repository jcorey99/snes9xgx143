/****************************************************************************
 * GC Config and Information Screens 
 *
 * Most of this code is reused from FCEU-GC
 ****************************************************************************/

#include <gccore.h>
#include <string.h>
#include <memmap.h>
#include <snes9x.h>
#include <soundux.h>
#include <ogc/ipc.h>
#include "gcstate.h"
#include "iplfont.h"
#include "intl.h"

extern unsigned int copynow;
extern GXRModeObj *vmode;
extern unsigned int *xfb[2];
extern int whichfb;
extern int font_size[256];
extern int font_height;
extern unsigned int blit_lookup[4];
extern unsigned int blit_lookup_inv[4];
extern void GetISODirectory();
extern void AnimFrame();
extern int SaveTheSRAM( int mode, int slot, int type);
extern int OpenDVD();
extern int OpenSD();
#ifdef HW_RVL
extern int OpenFrontSD();
#endif
extern void dvd_motor_off();
extern void uselessinquiry ();
extern int timerstyle;
extern int PADCAL;

extern int showspinner;
extern int showcontroller;

extern int UseSDCARD;
extern int UseFrontSDCARD;
extern int allowupdown;

extern unsigned char isWii;

void Credits();
int MainMenu();
void SetScreen();
void ClearScreen();

void Reboot() {
#ifdef HW_RVL
                    // Thanks to hell_hibou
                    int fd = IOS_Open("/dev/stm/immediate", 0);
                    IOS_Ioctl(fd, 0x2001, NULL, 0, NULL, 0);
                    IOS_Close(fd);
#else
#define SOFTRESET_ADR ((volatile u32*)0xCC003024)
                    *SOFTRESET_ADR = 0x00000000;
#endif
}
/****************************************************************************
 * Font drawing support
 ****************************************************************************/
int GetTextWidth( char *text ) {
    unsigned int i, w = 0;

    for ( i = 0; i < strlen(text); i++ )
        w += font_size[(int)text[i]];

    return w;
}

int CentreTextPosition( char *text )
{
    return ( ( 640 - GetTextWidth(text) ) >> 1 );
}

void WriteCentre( int y, char *text )
{
    write_font( CentreTextPosition(text), y, text);
}

/**
 * Wait for user to press A or B. Returns 0 = B; 1 = A
 */
int WaitButtonAB ()
{
    int btns;

    while ( (PAD_ButtonsDown (0) & (PAD_BUTTON_A | PAD_BUTTON_B)) );

    while ( TRUE )
    {
        btns = PAD_ButtonsDown (0);
        if ( btns & PAD_BUTTON_A )
            return 1;
        else if ( btns & PAD_BUTTON_B )
            return 0;
    }
}

/**
 * Show a prompt with choice of two options. Returns 1 if A button was pressed
 and 0 if B button was pressed.
 */
int WaitPromptChoice (char *msg, char *bmsg, char *amsg)
{
    char choiceOption[80];  
    sprintf (choiceOption, "B = %s   :   A = %s", bmsg, amsg);

    ClearScreen ();  
    WriteCentre(220, msg);
    WriteCentre(220 + font_height, choiceOption);  
    SetScreen ();

    return WaitButtonAB ();
}

void WaitPrompt( char *msg )
{
    int quit = 0;

    while ( PAD_ButtonsDown(0) & PAD_BUTTON_A ) {} ;

    while( !(PAD_ButtonsDown(0) & PAD_BUTTON_A ) && (quit == 0 ))
    {
        ClearScreen();
        WriteCentre( 220, msg);
        WriteCentre( 220 + font_height, (char*)"Press A to Continue");

        if ( PAD_ButtonsDown(0) & PAD_BUTTON_A )
            quit = 1;

        SetScreen();
    }
}

void ShowAction( char *msg )
{
    ClearScreen();
    WriteCentre( 220 + ( font_height >> 1), msg);
    SetScreen();
}

/****************************************************************************
 * SetScreen
 ****************************************************************************/
void SetScreen()
{
    VIDEO_SetNextFramebuffer( xfb[whichfb] );
    VIDEO_Flush();
    VIDEO_WaitVSync();
}

/****************************************************************************
 * ClearScreen
 ****************************************************************************/
void ClearScreen()
{

    copynow = GX_FALSE;
    whichfb ^= 1;
    VIDEO_ClearFrameBuffer(vmode, xfb[whichfb], COLOR_WHITE);
    AnimFrame();
}

/****************************************************************************
 * Welcome and ROM Information
 ****************************************************************************/
void RomInfo() {
    char work[1024];
    char titles[10][MENU_STRING_LENGTH] = {
        { MENU_INFO_ROM }, { MENU_INFO_ROMID }, { MENU_INFO_COMPANY },
        { MENU_INFO_SIZE }, { MENU_INFO_SRAM }, { MENU_INFO_TYPE },
        { MENU_INFO_CHECKSUM }, { MENU_INFO_TVTYPE }, { MENU_INFO_FRAMES },
        { MENU_INFO_CRC32 }
    };

    int i, p, quit = 0;

    showspinner = 0;
    while ( quit == 0 ) {
        p = 96;
        ClearScreen();

        /*** Title ***/
        write_font(CentreTextPosition((char*)MENU_CREDITS_TITLE) , ( 480 - ( 16 * font_height )) >> 1 , (char*)MENU_CREDITS_TITLE);

        /*** Print titles ***/
        for ( i = 0; i < 10; i++ )
            write_font( 48, p += font_height, titles[i]);

        /*** Show some basic ROM information ***/
        p = 96;
        write_font( 592 - GetTextWidth(Memory.ROMName), p += font_height, Memory.ROMName);
        write_font( 592 - GetTextWidth(Memory.ROMId), p += font_height, Memory.ROMId);
        write_font( 592 - GetTextWidth(Memory.CompanyId), p += font_height, Memory.CompanyId);
        sprintf(work, "%d", Memory.ROMSize);
        write_font( 592 - GetTextWidth(work), p += font_height, work);
        sprintf(work, "%d", Memory.SRAMSize);
        write_font( 592 - GetTextWidth(work), p += font_height, work);
        sprintf(work, "%d", Memory.ROMType);
        write_font( 592 - GetTextWidth(work), p += font_height, work);
        sprintf(work, "%04x / %04x", Memory.ROMChecksum, Memory.ROMComplementChecksum);
        write_font( 592 - GetTextWidth(work), p += font_height, work);
        sprintf(work, "%s", Settings.PAL ? "PAL" : "NTSC");
        write_font( 592 - GetTextWidth(work), p += font_height, work);
        sprintf(work, "%d", Memory.ROMFramesPerSecond);
        write_font( 592 - GetTextWidth(work), p += font_height, work);
        sprintf(work, "%08X", Memory.ROMCRC32);
        write_font( 592 - GetTextWidth(work), p += font_height, work);

        strcpy(work, MENU_INFO_ENJOY);
        write_font( CentreTextPosition(work), 400, work);

        SetScreen();
        if (PAD_ButtonsDown(0) & (PAD_BUTTON_A | PAD_BUTTON_B) ) quit = 1;
    }

    showspinner = 1;
}

/****************************************************************************
 * Credits section
 ****************************************************************************/
void Credits() {
    int quit = 0;

    showspinner = 0;

    while (quit == 0) {
        ClearScreen();

        /*** Title ***/
        write_font(CentreTextPosition((char*)MENU_CREDITS_TITLE) , ( 480 - ( 16 * font_height )) >> 1 , (char*)MENU_CREDITS_TITLE);

        int p = 80;

        WriteCentre( p += font_height, (char*)"Snes9x 1.43 - Snes9x Team");
        WriteCentre( p += font_height, (char*)MENU_CREDITS_GCPORT " - softdev");
        WriteCentre( p += font_height, (char*)MENU_CREDITS_GXINFO " - http://www.gc-linux.org");
        WriteCentre( p += font_height, (char*)MENU_CREDITS_FONT " - Qoob/or9");
        WriteCentre( p += font_height, (char*)"LibOGC - shagkur/wntrmute");
        WriteCentre( p += font_height, (char*)"DVD lib - Ninjamod Team");

        p += font_height;
        WriteCentre( p += font_height, (char*)MENU_CREDITS_TEST ": Mithos / luciddream / softdev");
        //WriteCentre( p += font_height, (char*)"Mithos / luciddream / softdev");

        p += font_height;
        WriteCentre( p += font_height, (char*)MENU_CREDITS_GREETS ": brakken, HonkeyKong");
        WriteCentre( p += font_height, (char*)"cedy_nl, raz, scognito");

        //p += font_height;
        WriteCentre( p += font_height, (char*)MENU_CREDITS_EXTRAS ": KruLLo, Askot, dsbomb");
        WriteCentre( p += font_height, (char*)MENU_CREDITS_SUPPORT " - http://www.tehskeen.com");

        SetScreen();
        if (PAD_ButtonsDown(0) & (PAD_BUTTON_A | PAD_BUTTON_B) ) quit = 1;
    }

    showspinner = 1;
}

/****************************************************************************
 * DrawMenu
 *
 * Display the requested menu
 ****************************************************************************/
void DrawMenu(char *title, char items[][MENU_STRING_LENGTH], int maxitems, int select)
{
    int i,w,p,h;

    ClearScreen();

    /*** Draw Title Centred ***/
    h = (480 - (( maxitems + 3 ) * font_height)) / 2;
    write_font( CentreTextPosition(title), h, title);

    p = h + ( font_height << 1 );

    for( i = 0; i < maxitems; i++ )
    {
        w = CentreTextPosition(items[i]);
        h = GetTextWidth(items[i]);

        if ( i == select )
            writex( w, p, h, font_height, (char *)items[i], blit_lookup_inv );
        else
            writex( w, p, h, font_height, (char *)items[i], blit_lookup );

        p += font_height;
    }

    SetScreen();
}

/****************************************************************************
 * PADMap
 *
 * Remap a pad to the correct key
 ****************************************************************************/
extern unsigned short gcpadmap[12];
char PADMap( int padvalue, int padnum ) {
    char padkey;

    switch( padvalue ) {
        default:
        case 0: gcpadmap[padnum] = PAD_BUTTON_A; padkey = 'A'; break;
        case 1: gcpadmap[padnum] = PAD_BUTTON_B; padkey = 'B'; break;
        case 2: gcpadmap[padnum] = PAD_BUTTON_X; padkey = 'X'; break;
        case 3: gcpadmap[padnum] = PAD_BUTTON_Y; padkey = 'Y'; break;
    }

    return padkey;
}

/****************************************************************************
 * PAD Configuration
 *
 * This screen simply let's the user swap A/B/X/Y around.
 ****************************************************************************/
u8 mpads[4];
void ConfigPADMenu() {
    int PadMenuCount = 8;
    char PadMenu[8][MENU_STRING_LENGTH] = { 
        { MENU_CONFIG_SETUP }, 
        { MENU_CONFIG_A }, { MENU_CONFIG_B }, { MENU_CONFIG_X }, { MENU_CONFIG_Y },
        { MENU_CONFIG_ANALOG }, { MENU_CONFIG_UPDOWN },
        { MENU_EXIT }
    };
    enum CONFIG_MENU {
        CONFIG_SETUP,
        CONFIG_A, CONFIG_B, CONFIG_X, CONFIG_Y,
        CONFIG_CLIP, CONFIG_UPDOWN,
        CONFIG_EXIT
    };

    static int PadSetup = 0;
    int ControllerMap[5][4] = { 
        { 0,1,2,3 }, { 2,0,1,3 },
        { 2,0,3,1 }, { 2,3,1,0 },
        { 2,1,3,0 } 
    };
    int ControllerMapcount = 5;

    int ChosenMenu = 0;
    short j;
    int redraw = 1;
    int quit = 0;
    int i;

    showcontroller = 1;
    for ( i = 0; i < 4; i++ )
        mpads[i] = ControllerMap[PadSetup][i];

    while ( quit == 0 ) {
        if ( redraw ) {
            sprintf(PadMenu[CONFIG_A], MENU_CONFIG_A " %c", PADMap(mpads[0], 0));
            sprintf(PadMenu[CONFIG_B], MENU_CONFIG_B " %c", PADMap(mpads[1], 1));
            sprintf(PadMenu[CONFIG_X], MENU_CONFIG_X " %c", PADMap(mpads[2], 2));
            sprintf(PadMenu[CONFIG_Y], MENU_CONFIG_Y " %c", PADMap(mpads[3], 3));
            sprintf(PadMenu[CONFIG_SETUP],MENU_CONFIG_SETUP " %c", PadSetup + 65);
            sprintf(PadMenu[CONFIG_UPDOWN], MENU_CONFIG_UPDOWN " %s", allowupdown ? MENU_ON : MENU_OFF);
            sprintf(PadMenu[CONFIG_CLIP],MENU_CONFIG_ANALOG " %d", PADCAL);
            DrawMenu((char*)MENU_CONFIG_TITLE, PadMenu, PadMenuCount, ChosenMenu);
        }

        redraw = 1;

        j = PAD_ButtonsDown(0);

        if (j & PAD_BUTTON_B) {
            quit = 1;
        }

        if ( j & PAD_BUTTON_DOWN ) {
            ChosenMenu++;
            redraw = 1;
        }

        if ( j & PAD_BUTTON_UP ) {
            ChosenMenu--;
            redraw = 1;
        }

        if ( j & PAD_BUTTON_A ) {
            redraw = 1;
            switch( ChosenMenu ) {

                case CONFIG_SETUP: PadSetup++;
                        if ( PadSetup == ControllerMapcount )
                            PadSetup = 0;
                        for ( i = 0; i < 4; i++ )
                            mpads[i] = ControllerMap[PadSetup][i];
                        i = -1;
                        break;

                case CONFIG_A: i = 0; break;

                case CONFIG_B: i = 1; break;

                case CONFIG_X: i = 2; break;

                case CONFIG_Y: i = 3; break;

                case CONFIG_CLIP: i = -1;
                        PADCAL += 5;
                        if ( PADCAL > 70 )
                            PADCAL = 30;
                        redraw = 1;
                        break;

                case CONFIG_UPDOWN: i = -1;
                        allowupdown ^= 1;
                        break;

                case CONFIG_EXIT: quit = 1; break;
                default: break;
            }

            if ( ( quit == 0 ) && ( i >= 0)) {
                mpads[i]++;
                if ( mpads[i] == 4 )
                    mpads[i] = 0;

            }
        }

        if ( j & PAD_BUTTON_B ) quit = 1;

        if ( j & PAD_BUTTON_LEFT ) {
            i = -1;
            switch(ChosenMenu) {
                case CONFIG_SETUP: PadSetup--;
                        if ( PadSetup < 0 )
                            PadSetup = 0;
                        for ( i = 0; i < 4; i++ )
                            mpads[i] = ControllerMap[PadSetup][i];
                        i = -1;
                        redraw = 1;
                        break;
                case CONFIG_A: i = 0;
                        break;
                case CONFIG_B: i = 1;
                        break;
                case CONFIG_X: i = 2;
                        break;
                case CONFIG_Y: i = 3;
                        break;
                case CONFIG_CLIP: PADCAL -= 5;
                        if (PADCAL < 30) PADCAL = 30;
                        break;
                case CONFIG_UPDOWN: allowupdown = 0;
                        break;
                default: break;
            }
            if ( ( i >= 0 ) && ( mpads[i] > 0 ) )
                    mpads[i]--;
        }
        if ( j & PAD_BUTTON_RIGHT ) {
            i = -1;
            switch(ChosenMenu) {
                case CONFIG_SETUP: PadSetup++;
                        if ( PadSetup > 4 )
                            PadSetup = 4;
                        for ( i = 0; i < 4; i++ )
                            mpads[i] = ControllerMap[PadSetup][i];
                        i = -1;
                        break;
                case CONFIG_A: i = 0;
                        break;
                case CONFIG_B: i = 1;
                        break;
                case CONFIG_X: i = 2;
                        break;
                case CONFIG_Y: i = 3;
                        break;
                case CONFIG_CLIP: PADCAL += 5;
                        if (PADCAL > 70) PADCAL = 70;
                        break;
                case CONFIG_UPDOWN: allowupdown = 1;
                        break;
                default: break;
            }
            if ( i >= 0 ) {
                mpads[i]++;
                if ( mpads[i] > 3 )
                    mpads[i] = 3;

            }
        }

        if ( ChosenMenu < 0 )
            ChosenMenu = PadMenuCount - 1;

        if ( ChosenMenu == PadMenuCount )
            ChosenMenu = 0;
    }

    showcontroller = 0;
    return;
}

int SdSlotCount = 3;
char SdSlots[3][10] = {
    { "Slot A" }, { "Slot B" }, { "Wii SD"}
};
enum SLOTS {
    SLOT_A, SLOT_B, SLOT_WIISD
};
int ChosenSlot = 0;
int ChosenDevice = 1;
void SaveMenu(int SaveType) { // 0=SRAM, 1=STATE
    int ChosenMenu = 0;
    int SaveMenuCount = 5;
    char SaveMenu[5][MENU_STRING_LENGTH] = { 
        { MENU_SAVE_SAVE }, { MENU_SAVE_LOAD },
        { MENU_SAVE_DEVICE }, { "SDCard" },
        { MENU_EXIT }
    };
    enum SAVE_MENU {
        SAVE_SAVE, SAVE_LOAD,
        SAVE_DEVICE, SAVE_SLOT,
        SAVE_EXIT
    };

    char saveTitle[MENU_STRING_LENGTH];
    int redraw = 1;
    int quit = 0;
    short j;

    if (!isWii)
        SdSlotCount = 2;
#ifdef HW_RVL
    ChosenSlot = 2;
    ChosenDevice = 1;
#endif

    while ( quit == 0 ) {
        if ( (ChosenDevice == 0) && (ChosenSlot > 1) ) // MemCard, limit slot
            ChosenSlot = 1;
        if ( redraw ){
            sprintf(saveTitle, "%s " MENU_SAVE_TITLE, SaveType ? "State" : "SRAM");
            sprintf(SaveMenu[SAVE_SAVE], MENU_SAVE_SAVE " %s", SaveType ? "State" : "SRAM");
            sprintf(SaveMenu[SAVE_LOAD], MENU_SAVE_LOAD " %s", SaveType ? "State" : "SRAM");

            sprintf(SaveMenu[SAVE_DEVICE], MENU_SAVE_DEVICE ": %s", ChosenDevice ? "SDCard" : "MemCard");
            sprintf(SaveMenu[SAVE_SLOT], "%s: %s", ChosenDevice ? "SDCard" : "MemCard",
                SdSlots[ChosenSlot]);
            DrawMenu(saveTitle, SaveMenu, SaveMenuCount, ChosenMenu);
            redraw = 0;
        } 

        j = PAD_ButtonsDown(0);

        if ( j & PAD_BUTTON_DOWN ) {
            ChosenMenu++;
            redraw = 1;
        }

        if ( j & PAD_BUTTON_UP ) {
            ChosenMenu--;
            redraw = 1;
        }

        if ( j & PAD_BUTTON_A ) {
            redraw = 1;

            while( PAD_ButtonsDown(0) & PAD_BUTTON_A ) {};
            switch( ChosenMenu ) {
                case SAVE_SAVE: 
                    if (SaveType) NGCFreezeGame(ChosenDevice, ChosenSlot); // Save State
                    else SaveTheSRAM(1, ChosenSlot, ChosenDevice); //Save SRAM
                    quit = 1;
                    break;
                case SAVE_LOAD:
                    if (SaveType) NGCUnfreezeGame(ChosenDevice, ChosenSlot); // Load State
                    else SaveTheSRAM(0, ChosenSlot, ChosenDevice);  // Load SRAM
                    quit = 1;
                    break;
                case SAVE_DEVICE:
                    ChosenDevice ^= 1;
                    redraw = 1;
                    break;
                case SAVE_SLOT:
                    ChosenSlot++;
                    if (ChosenSlot >= SdSlotCount)
                        ChosenSlot = 0;
                    redraw = 1;
                    break;
                case SAVE_EXIT:
                    quit = 1; 
                    break;
            }
        } 

        if (j & PAD_BUTTON_RIGHT) {
            if (ChosenMenu == SAVE_SLOT) {
                ChosenSlot++;
                if (ChosenSlot >= SdSlotCount)
                    ChosenSlot = SdSlotCount - 1;
                redraw = 1;
            } else if (ChosenMenu == SAVE_DEVICE) {
                ChosenDevice ^= 1;
                redraw = 1;
            }
        }

        if (j & PAD_BUTTON_LEFT) {
            if (ChosenMenu == SAVE_SLOT) {
                ChosenSlot--;
                if (ChosenSlot < 0)
                    ChosenSlot = 0;
                redraw = 1;
            } else if (ChosenMenu == SAVE_DEVICE) {
                ChosenDevice ^= 1;
                redraw = 1;
            }
        }

        if (j & PAD_BUTTON_B) quit = 1;

        if ( ChosenMenu < 0 ) ChosenMenu = SaveMenuCount - 1;

        if ( ChosenMenu == SaveMenuCount ) ChosenMenu = 0;
    }
}

/****************************************************************************
 * File Manager Menu
 ****************************************************************************/
int FileMenu () {
    int ChosenMenu = 0;
    int FileMenuCount = 3;
    char FileMenu[3][MENU_STRING_LENGTH] = {
        { "SRAM Manager"}, {"STATE Manager"},
        { MENU_EXIT }
    };
    enum FILE_MENU {
        FILE_SRAM, FILE_STATE,
        FILE_EXIT
    };

    int quit = 0;
    int redraw = 1;
    short j;

    while (quit == 0) {
        if ( redraw ) {
            strcpy(FileMenu[FILE_SRAM], "SRAM " MENU_FILE_MANAGER);
            strcpy(FileMenu[FILE_STATE], "STATE " MENU_FILE_MANAGER);
            DrawMenu((char*)MENU_FILE_TITLE, FileMenu, FileMenuCount, ChosenMenu);
        }

        j = PAD_ButtonsDown(0);

        if ( j & PAD_BUTTON_DOWN ) {
            ChosenMenu++;
            redraw = 1;
        }

        if ( j & PAD_BUTTON_UP ) {
            ChosenMenu--;
            redraw = 1;
        }

        if ( j & PAD_BUTTON_A ) {
            redraw = 1;
            switch (ChosenMenu) {
                case FILE_SRAM:
                    SaveMenu(0);
                    break;
                case FILE_STATE:
                    SaveMenu(1);
                    break;
                case FILE_EXIT:
                    quit = 1;
                    break;
            }
        }

        if ( j & PAD_BUTTON_B ) quit = 1;

        if ( ChosenMenu < 0 ) ChosenMenu = FileMenuCount - 1;

        if ( ChosenMenu == FileMenuCount ) ChosenMenu = 0;
    }

    return 0;
}

/****************************************************************************
 * Emulator Options
 ****************************************************************************/
void EmuOptionsMenu() {
    int EmuMenuCount = 8;
    char EmuMenu[8][MENU_STRING_LENGTH] = { 
        { MENU_EMU_SOUND }, { MENU_EMU_STEREO },
        { MENU_EMU_TRANSP }, { MENU_EMU_FPS },
        { MENU_EMU_INTERP }, { MENU_EMU_TIMER },
        { MENU_EMU_MULTITAP },
        { MENU_EXIT }
    };
    enum EMU_MENU {
        EMU_SOUND, EMU_STEREO, EMU_TRANSP,
        EMU_FPS, EMU_INTERP, EMU_TIMER,
        EMU_MULTITAP, EMU_EXIT
    };
    int ChosenMenu = 0;
    short j;
    int redraw = 1;
    int quit = 0;

    while ( quit == 0 ) {
        if ( redraw ) {
            sprintf(EmuMenu[EMU_SOUND], MENU_EMU_SOUND " %s", Settings.DisableSoundEcho ? "OFF" : "ON");
            sprintf(EmuMenu[EMU_STEREO], MENU_EMU_STEREO " %s", Settings.ReverseStereo ? "ON" : "OFF");
            sprintf(EmuMenu[EMU_TRANSP], MENU_EMU_TRANSP " %s", Settings.Transparency ? "ON" : "OFF" );
            sprintf(EmuMenu[EMU_FPS], MENU_EMU_FPS " %s", Settings.DisplayFrameRate ? "ON" : "OFF");
            sprintf(EmuMenu[EMU_INTERP], MENU_EMU_INTERP " %s", Settings.InterpolatedSound ? "ON" : "OFF");
            sprintf(EmuMenu[EMU_TIMER], MENU_EMU_TIMER " %s", timerstyle ? "Clock" : "VBlank");
            sprintf(EmuMenu[EMU_MULTITAP], MENU_EMU_MULTITAP " %s", Settings.MultiPlayer5 ? "ON" : "OFF");
            DrawMenu((char*)MENU_EMU_TITLE, EmuMenu, EmuMenuCount, ChosenMenu);
        }

        redraw = 1;

        j = PAD_ButtonsDown(0);

        if ( j & PAD_BUTTON_DOWN ) {
            ChosenMenu++;
            redraw = 1;
        }

        if ( j & PAD_BUTTON_UP ) {
            ChosenMenu--;
            redraw = 1;
        }

        if ( j & (PAD_BUTTON_A | PAD_BUTTON_LEFT | PAD_BUTTON_RIGHT) ) {
            redraw = 1;
            switch(ChosenMenu) {
                case EMU_SOUND:
                    Settings.DisableSoundEcho ^= 1;
                    break;
                case EMU_STEREO:
                    Settings.ReverseStereo ^= 1;
                    break;
                case EMU_TRANSP:
                    Settings.Transparency ^= 1;
                    break;
                case EMU_FPS:
                    Settings.DisplayFrameRate ^= 1;
                    break;
                case EMU_INTERP:	
                    Settings.InterpolatedSound ^= 1;
                    break;
                case EMU_TIMER:
                    timerstyle ^= 1;
                    break;
                case EMU_MULTITAP:
                    Settings.MultiPlayer5 ^= 1;
                    if (Settings.MultiPlayer5)
                        Settings.ControllerOption = SNES_MULTIPLAYER5;
                    else
                        Settings.ControllerOption = SNES_JOYPAD;
                    Settings.MultiPlayer5Master = Settings.MultiPlayer5;
                    break;	
                case EMU_EXIT: 	
                    // Only exit with A
                    if (j & PAD_BUTTON_A)
                        quit = 1;
                    break;

            }			
        }

        if (j & PAD_BUTTON_B) {
            quit = 1;
        }

        if (j & PAD_BUTTON_B) quit = 1;

        if ( ChosenMenu < 0 )
            ChosenMenu = EmuMenuCount - 1;

        if ( ChosenMenu == EmuMenuCount )
            ChosenMenu = 0;
    }
}

/****************************************************************************
 * Media Select Screen
 ****************************************************************************/
extern int haveSDdir;
int MediaSelect() {
    int MediaMenuCount = 5;
    char MediaMenu[5][MENU_STRING_LENGTH] = { 
        { MENU_MEDIA_SDCARD },
        { "SDCard: Wii SD" },
        { MENU_MEDIA_DVD },
        { MENU_MEDIA_STOPDVD },
        { MENU_EXIT }
    };
    enum MEDIA_MENU {
        MEDIA_SDCARD, MEDIA_SLOT,
        MEDIA_DVD, MEDIA_STOPDVD,
        MEDIA_EXIT
    };
    int ChosenMenu = 0;
    int quit = 0;
    short j;
    int redraw = 1;

#ifdef HW_RVL
    strcpy(MediaMenu[MEDIA_DVD], MediaMenu[MEDIA_EXIT]);
    MediaMenuCount = 3;
    ChosenSlot = 2; // default to WiiSD
#else
    SdSlotCount = 2;
#endif

    while ( quit == 0 ) {
        // TODO: straighten out haveSDdir vs haveWiiSDdir
        haveSDdir = 0;
        if ( redraw ) {
            sprintf(MediaMenu[MEDIA_SLOT], "SDCard: %s", SdSlots[ChosenSlot]);
            DrawMenu((char*)MENU_MEDIA_TITLE, MediaMenu, MediaMenuCount, ChosenMenu);
            redraw = 0;
        }

        j = PAD_ButtonsDown(0);
        if ( j & PAD_BUTTON_DOWN ) {
            ChosenMenu++;
            redraw = 1;
        }

        if ( j & PAD_BUTTON_UP ) {
            ChosenMenu--;
            redraw = 1;
        }

        if ( j & PAD_BUTTON_A ) {
            redraw = 1;
            switch ( ChosenMenu ) {
                case MEDIA_SDCARD:
#ifdef HW_RVL
                        if (ChosenSlot == 2) {
                            OpenFrontSD();
                        } else
#endif
                            OpenSD();
                        return 1;
                        break;
                case MEDIA_SLOT:
                        ChosenSlot++;
                        if (ChosenSlot >= SdSlotCount)
                            ChosenSlot = 0;
                        redraw = 1;
                        break;
                case MEDIA_DVD: 
#ifdef HW_RVL
                        // In Wii mode, this is just exit
                        quit = 1;
#else
                        UseSDCARD = 0;
                        UseFrontSDCARD = 0;
                        OpenDVD();
                        return 1;
#endif
                        break;
#ifndef HW_RVL
                case MEDIA_STOPDVD:
                        ShowAction((char*)MENU_MEDIA_STOPPING);
                        dvd_motor_off();
                        WaitPrompt((char*)MENU_MEDIA_STOPPED);
                case MEDIA_EXIT:
                        quit = 1;
                        break;
#endif
                default: break ;
            }
        }

        if ( (j & PAD_BUTTON_RIGHT) && (ChosenMenu == MEDIA_SLOT) ) {
            ChosenSlot++;
            if (ChosenSlot >= SdSlotCount)
                ChosenSlot = SdSlotCount - 1;
            redraw = 1;
        }

        if ( (j & PAD_BUTTON_LEFT) && (ChosenMenu == MEDIA_SLOT) ) {
            ChosenSlot--;
            if (ChosenSlot < 0)
                ChosenSlot = 0;
            redraw = 1;
        }

        if ( j & PAD_BUTTON_B )
            quit = 1;

        if ( ChosenMenu == MediaMenuCount  )
            ChosenMenu = 0;		

        if ( ChosenMenu < 0 )
            ChosenMenu = MediaMenuCount - 1;
    }

    return 0;
}

/****************************************************************************
 * Configuration Screen
 *
 * This is the main screen the user sees when they press Z-RIGHT_SHOULDER
 ****************************************************************************/
int MainMenu() {
    int MainMenuCount = 10;
    char MainMenu[10][MENU_STRING_LENGTH] = {
        { MENU_MAIN_PLAY },
        { MENU_MAIN_RESET },
        { MENU_MAIN_LOAD },
        { MENU_MAIN_SAVE },
        { MENU_MAIN_INFO },
        { MENU_MAIN_JOYPADS },
        { MENU_MAIN_OPTIONS },
        { MENU_MAIN_RELOAD },
        { MENU_MAIN_REBOOT },
        { MENU_MAIN_CREDITS },
    };
    enum MAIN_MENU {
        MAIN_PLAY, MAIN_RESET, MAIN_LOAD,
        MAIN_SAVE, MAIN_INFO, MAIN_JOYPADS,
        MAIN_OPTIONS, MAIN_RELOAD, MAIN_REBOOT,
        MAIN_CREDITS
    };
    int ChosenMenu = 0;
    short j;
    int redraw, quit = 0;
    int isQuitting = 0;

#ifdef HW_RVL
    void (*PSOReload)() = (void(*)())0x90000020;
#else
    void (*PSOReload)() = (void(*)())0x80001800;
#endif

    copynow = GX_FALSE;
    Settings.Paused = TRUE;
    S9xSetSoundMute(TRUE);

    while ( quit == 0 ) {
        redraw = 1;
        if ( redraw )
            DrawMenu((char*)MENU_CREDITS_TITLE, MainMenu, MainMenuCount, ChosenMenu);

        j = PAD_ButtonsDown(0);

        if ( j & PAD_BUTTON_DOWN )
            ChosenMenu++;

        if ( j & PAD_BUTTON_UP )
            ChosenMenu--;

        if ( j & PAD_BUTTON_A ) {
            switch( ChosenMenu ) {
                case MAIN_PLAY:
                    quit = 1;
                    break;
                case MAIN_RESET:
                    S9xSoftReset();
                    quit = 1;
                    break;
                case MAIN_LOAD:
                    MediaSelect();
                    break;
                case MAIN_SAVE:
                    FileMenu();
                    break;
                case MAIN_INFO:
                    RomInfo();
                    break;
                case MAIN_JOYPADS:
                    ConfigPADMenu();
                    break;
                case MAIN_OPTIONS:
                    EmuOptionsMenu();
                    break;
                case MAIN_RELOAD:
                    PSOReload();
                    isQuitting = 1;
                    break;
                case MAIN_REBOOT:
                    Reboot();
                    break;
                case MAIN_CREDITS:
                    Credits();
                    break;
                default :
                    break;
            }
        }

        if ( j & PAD_BUTTON_B ) quit = 1;
        if ( ChosenMenu < 0 )
            ChosenMenu = MainMenuCount - 1;

        if ( ChosenMenu == MainMenuCount )
            ChosenMenu = 0;
    }

    /*** Remove any still held buttons ***/
    while (PAD_ButtonsHeld(0))
        VIDEO_WaitVSync();

    uselessinquiry ();		/*** Stop the DVD from causing clicks while playing ***/

    if (!isQuitting) {
        S9xSetSoundMute(FALSE);
        Settings.Paused = FALSE;
    }
    return 0;
}
