/****************************************************************************
 * Gamecube Input
 *
 * Use JOY1 and JOY2
 *
 * softdev - November 2005
 ****************************************************************************/

#include <gccore.h>
#include <snes9x.h>
#include <soundux.h>
#include "iplfont.h"
#include "display.h"
#include "memmap.h"
#include "gcgxvideo.h"

PADStatus gcjoypads[4];
extern int ConfigRequested;

unsigned short gcpadmap[] = { PAD_BUTTON_X, PAD_BUTTON_A, PAD_BUTTON_B, PAD_BUTTON_Y,
    PAD_BUTTON_UP, PAD_BUTTON_DOWN, PAD_BUTTON_LEFT, PAD_BUTTON_RIGHT,
    PAD_BUTTON_START, PAD_TRIGGER_Z, PAD_TRIGGER_R, PAD_TRIGGER_L };

unsigned int snespadmap[] = { SNES_A_MASK, SNES_B_MASK, SNES_X_MASK, SNES_Y_MASK,
    SNES_UP_MASK, SNES_DOWN_MASK, SNES_LEFT_MASK, SNES_RIGHT_MASK,
    SNES_START_MASK, SNES_SELECT_MASK, SNES_TR_MASK, SNES_TL_MASK };

extern int SaveSRAM(int mode, int slot, int type);
extern int slot;
extern int device;
extern int autoSaveLoad;

/****************************************************************************
 * Convert GC Joystick Readings to JOY
 *
 * SNES has the following buttons
 * Up/Down/Left/Right
 * A/B/X/Y
 * LEFT/RIGHT SHOULDERS
 ****************************************************************************/
unsigned int DecodeJoy( unsigned short p )
{
    unsigned int J = 0;
    int i;

    for ( i = 0; i < 12; i++ ) {
        if ( p & gcpadmap[i] )
            J |= snespadmap[i];
    }

    return J;
}

/****************************************************************************
 * V 1.0.1
 * 
 * Additional check for Analog X/Y
 ****************************************************************************/
int PADCAL = 40;

unsigned int GetAnalog( int Joy )
{

    signed char x,y;
    unsigned int i = 0;

    x = PAD_StickX(Joy);
    y = PAD_StickY(Joy);

    if (x * x + y * y > PADCAL * PADCAL) {

        if (x > 0 && y == 0) return SNES_RIGHT_MASK;
        if (x < 0 && y == 0) return SNES_LEFT_MASK;
        if (x == 0 && y > 0) return SNES_UP_MASK;
        if (x == 0 && y < 0) return SNES_DOWN_MASK;

        if ((float)y / x >= -2.41421356237 && (float)y / x < 2.41421356237) {
            if (x >= 0)
                i |= SNES_RIGHT_MASK;
            else
                i |= SNES_LEFT_MASK;
        }

        if ((float)x / y >= -2.41421356237 && (float)x / y < 2.41421356237) {
            if (y >= 0)
                i |= SNES_UP_MASK;
            else
                i |= SNES_DOWN_MASK;
        }
    }

    return i;
}

extern int gcScreenX;
extern int gcScreenY;
extern int SCOPEPADCAL;
int gcX = 0;
int gcY = 0;

char msg0[32];

unsigned int GetJoys(int which)
{
    unsigned int joypads;
    signed char px;

    /*** Check for menu, now CStick left/right (and if you use a HORI controller Z + R works too) ***/
    px = PAD_SubStickX (0);
    if ( (px < -PADCAL) || (PAD_ButtonsHeld(0) == ( PAD_TRIGGER_Z  | PAD_TRIGGER_R )) || 
		(PAD_ButtonsHeld(0) == ( PAD_TRIGGER_L  | PAD_TRIGGER_R | PAD_BUTTON_X | PAD_BUTTON_Y )))
    {
		if (autoSaveLoad && Memory.SRAMSize)
		{
			Settings.Paused =TRUE;
			S9xSetSoundMute(TRUE);
			
			SaveSRAM(1,slot,device);
			
			Settings.Paused =FALSE;
			S9xSetSoundMute(FALSE);
		}
		ConfigRequested = 1;
	}
	
	//unsigned short p = PAD_ButtonsHeld(0);
	//if (Settings.SuperScope){
	//}
	
	/*if (px > PADCAL){ 
		S9xNextController ();
		static char *controllers [] = {
			WINPROC_CONTROLERS0, 
			WINPROC_CONTROLERS1, 
			WINPROC_CONTROLERS2,
			WINPROC_CONTROLERS3, 
			WINPROC_CONTROLERS4, 
			WINPROC_CONTROLERS5, 
			WINPROC_CONTROLERS6
		};
		S9xSetInfoString (controllers [IPPU.Controller]);
		Settings.ControllerOption=IPPU.Controller;
	}*/

	//For debugging
	if (Settings.SuperScope == true || Settings.Mouse == true){
		signed char padX = PAD_StickX(0);
		signed char padY = PAD_StickY(0);
	
		if ((padX < -SCOPEPADCAL) || (padX > SCOPEPADCAL) || (padY > SCOPEPADCAL) || (padY < -SCOPEPADCAL)){
			gcX = 100 * gcScreenX / 256;
			gcY = 100 * gcScreenY / 224;
			sprintf(msg0, "X=%d Y=%d", gcX, gcY);
			//sprintf(msg0, "X=%d Y=%d", gcScreenX, gcScreenY);
			S9xSetInfoString (msg0);
		}
	}
    joypads = 0;

	if (Settings.SuperScope == false || Settings.Mouse == false){
		joypads = DecodeJoy( PAD_ButtonsHeld(which) );
		joypads |= GetAnalog(which);
	}
	
    return joypads;
}

