/****************************************************************************
 * Snes9x 1.51 Nintendo Wii/Gamecube Port
 *
 * softdev July 2006
 * crunchy2 May-June 2007
 * Michniewski 2008
 * Tantric September 2008
 *
 * input.cpp
 *
 * Wii/Gamecube controller management
 ***************************************************************************/

#include <gccore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <ogcsys.h>
#include <unistd.h>
#include <wiiuse/wpad.h>

#include "snes9x.h"
#include "memmap.h"

#include "snes9xGX.h"
#include "button_mapping.h"
#include "s9xconfig.h"
#include "menu.h"
#include "video.h"
#include "input.h"

/****************************************************************************
 * Controller Functions
 *
 * The following map the NGC Pads to the *NEW* controller system.
 ***************************************************************************/
 
 unsigned int snespadmap[] = { 
	SNES_A_MASK, SNES_B_MASK, 
	SNES_X_MASK, SNES_Y_MASK,
	SNES_TL_MASK, SNES_TR_MASK,
	SNES_SELECT_MASK, SNES_START_MASK,
	SNES_UP_MASK, SNES_DOWN_MASK, 
	SNES_LEFT_MASK, SNES_RIGHT_MASK
};

/*** Gamecube controller Padmap ***/
unsigned int gcpadmap[] = {
  PAD_BUTTON_A, PAD_BUTTON_B,
  PAD_BUTTON_X, PAD_BUTTON_Y,
  PAD_TRIGGER_L, PAD_TRIGGER_R,
  PAD_TRIGGER_Z, PAD_BUTTON_START,
  PAD_BUTTON_UP, PAD_BUTTON_DOWN,
  PAD_BUTTON_LEFT, PAD_BUTTON_RIGHT
};
/*** Wiimote Padmap ***/
unsigned int wmpadmap[] = {
  WPAD_BUTTON_B, WPAD_BUTTON_2,
  WPAD_BUTTON_1, WPAD_BUTTON_A,
  0x0000, 0x0000,
  WPAD_BUTTON_MINUS, WPAD_BUTTON_PLUS,
  WPAD_BUTTON_RIGHT, WPAD_BUTTON_LEFT,
  WPAD_BUTTON_UP, WPAD_BUTTON_DOWN
};
/*** Classic Controller Padmap ***/
unsigned int ccpadmap[] = {
  WPAD_CLASSIC_BUTTON_A, WPAD_CLASSIC_BUTTON_B,
  WPAD_CLASSIC_BUTTON_X, WPAD_CLASSIC_BUTTON_Y,
  WPAD_CLASSIC_BUTTON_FULL_L, WPAD_CLASSIC_BUTTON_FULL_R,
  WPAD_CLASSIC_BUTTON_MINUS, WPAD_CLASSIC_BUTTON_PLUS,
  WPAD_CLASSIC_BUTTON_UP, WPAD_CLASSIC_BUTTON_DOWN,
  WPAD_CLASSIC_BUTTON_LEFT, WPAD_CLASSIC_BUTTON_RIGHT
};
/*** Nunchuk + wiimote Padmap ***/
unsigned int ncpadmap[] = {
  WPAD_BUTTON_A, WPAD_BUTTON_B,
  WPAD_NUNCHUK_BUTTON_C, WPAD_NUNCHUK_BUTTON_Z,
  WPAD_BUTTON_2, WPAD_BUTTON_1,
  WPAD_BUTTON_MINUS, WPAD_BUTTON_PLUS,
  WPAD_BUTTON_UP, WPAD_BUTTON_DOWN,
  WPAD_BUTTON_LEFT, WPAD_BUTTON_RIGHT
};
/*** Superscope : GC controller button mapping ***/
unsigned int gcscopemap[] = { PAD_TRIGGER_Z, PAD_BUTTON_B,
  PAD_BUTTON_A, PAD_BUTTON_Y, PAD_BUTTON_X, PAD_BUTTON_START
};
/*** Superscope : wiimote button mapping ***/
unsigned int wmscopemap[] = { WPAD_BUTTON_MINUS, WPAD_BUTTON_B,
  WPAD_BUTTON_A, WPAD_BUTTON_UP, WPAD_BUTTON_DOWN, WPAD_BUTTON_PLUS
};

int scopeTurbo = 0; // tracks whether superscope turbo is on or off

/*** Mouse : GC controller button mapping ***/
unsigned int gcmousemap[] = { PAD_BUTTON_A, PAD_BUTTON_B };
/*** Mouse : wiimote button mapping ***/
unsigned int wmmousemap[] = { WPAD_BUTTON_A, WPAD_BUTTON_B };
/*** Justifier : GC controller button mapping ***/
unsigned int gcjustmap[] = { PAD_BUTTON_A, PAD_BUTTON_B, PAD_BUTTON_START };
/*** Justifier : wiimote button mapping ***/
unsigned int wmjustmap[] = { WPAD_BUTTON_A, WPAD_BUTTON_B, WPAD_BUTTON_PLUS };

/****************************************************************************
 * WPAD_Stick
 *
 * Get X/Y value from Wii Joystick (classic, nunchuk) input
 ***************************************************************************/

s8 WPAD_Stick(u8 chan, u8 right, int axis)
{
	float mag = 0.0;
	float ang = 0.0;
	WPADData *data = WPAD_Data(chan);

	switch (data->exp.type)
	{
		case WPAD_EXP_NUNCHUK:
		case WPAD_EXP_GUITARHERO3:
			if (right == 0)
			{
				mag = data->exp.nunchuk.js.mag;
				ang = data->exp.nunchuk.js.ang;
			}
			break;

		case WPAD_EXP_CLASSIC:
			if (right == 0)
			{
				mag = data->exp.classic.ljs.mag;
				ang = data->exp.classic.ljs.ang;
			}
			else
			{
				mag = data->exp.classic.rjs.mag;
				ang = data->exp.classic.rjs.ang;
			}
			break;

		default:
			break;
	}

	/* calculate x/y value (angle need to be converted into radian) */
	if (mag > 1.0) mag = 1.0;
	else if (mag < -1.0) mag = -1.0;
	double val;

	if(axis == 0) // x-axis
		val = mag * sin((PI * ang)/180.0f);
	else // y-axis
		val = mag * cos((PI * ang)/180.0f);

	return (s8)(val * 128.0f);
}

// hold superscope/mouse/justifier cursor positions
//static int cursor_x[5] = {0,0,0,0,0};
//static int cursor_y[5] = {0,0,0,0,0};

/****************************************************************************
 * UpdateCursorPosition
 *
 * Updates X/Y coordinates for Superscope/mouse/justifier position
 ***************************************************************************/

void UpdateCursorPosition (int pad, int &pos_x, int &pos_y)
{
	#define SCOPEPADCAL 20

	// gc left joystick
	signed char pad_x = PAD_StickX (pad);
	signed char pad_y = PAD_StickY (pad);

	if (pad_x > SCOPEPADCAL){
		pos_x += (pad_x*1.0)/SCOPEPADCAL;
		if (pos_x > 256) pos_x = 256;
	}
	if (pad_x < -SCOPEPADCAL){
		pos_x -= (pad_x*-1.0)/SCOPEPADCAL;
		if (pos_x < 0) pos_x = 0;
	}

	if (pad_y < -SCOPEPADCAL){
		pos_y += (pad_y*-1.0)/SCOPEPADCAL;
		if (pos_y > 224) pos_y = 224;
	}
	if (pad_y > SCOPEPADCAL){
		pos_y -= (pad_y*1.0)/SCOPEPADCAL;
		if (pos_y < 0) pos_y = 0;
	}

#ifdef HW_RVL
	struct ir_t ir;		// wiimote ir
	WPAD_IR(pad, &ir);
	if (ir.valid)
	{
		pos_x = (ir.x * 256) / 640;
		pos_y = (ir.y * 224) / 480;
	}
	else
	{
		signed char wm_ax = WPAD_Stick (pad, 0, 0);
		signed char wm_ay = WPAD_Stick (pad, 0, 1);

		if (wm_ax > SCOPEPADCAL){
			pos_x += (wm_ax*1.0)/SCOPEPADCAL;
			if (pos_x > 256) pos_x = 256;
		}
		if (wm_ax < -SCOPEPADCAL){
			pos_x -= (wm_ax*-1.0)/SCOPEPADCAL;
			if (pos_x < 0) pos_x = 0;
		}

		if (wm_ay < -SCOPEPADCAL){
			pos_y += (wm_ay*-1.0)/SCOPEPADCAL;
			if (pos_y > 224) pos_y = 224;
		}
		if (wm_ay > SCOPEPADCAL){
			pos_y -= (wm_ay*1.0)/SCOPEPADCAL;
			if (pos_y < 0) pos_y = 0;
		}
	}
#endif

}

/****************************************************************************
 * decodepad
 *
 * Reads the changes (buttons pressed, etc) from a controller and reports
 * these changes to Snes9x
 ***************************************************************************/

unsigned int decodepad (int pad)
{
	int i;
	float t;
	unsigned int J = 0;

	signed char pad_x = PAD_StickX (pad);
	signed char pad_y = PAD_StickY (pad);
	u32 jp = PAD_ButtonsHeld (pad);

#ifdef HW_RVL
	signed char wm_ax = 0;
	signed char wm_ay = 0;
	u32 wp = 0;
	wm_ax = WPAD_Stick ((u8)pad, 0, 0);
	wm_ay = WPAD_Stick ((u8)pad, 0, 1);
	wp = WPAD_ButtonsHeld (pad);

	u32 exp_type;
	if ( WPAD_Probe(pad, &exp_type) != 0 )
		exp_type = WPAD_EXP_NONE;
#endif

	/***
	Gamecube Joystick input
	***/
	// Is XY inside the "zone"?
	if (pad_x * pad_x + pad_y * pad_y > PADCAL * PADCAL)
	{
		/*** we don't want division by zero ***/
		if (pad_x > 0 && pad_y == 0)
			J |= SNES_RIGHT_MASK;
		if (pad_x < 0 && pad_y == 0)
			J |= SNES_LEFT_MASK;
		if (pad_x == 0 && pad_y > 0)
			J |= SNES_UP_MASK;
		if (pad_x == 0 && pad_y < 0)
			J |= SNES_DOWN_MASK;

		if (pad_x != 0 && pad_y != 0)
		{
			/*** Recalc left / right ***/
			t = (float) pad_y / pad_x;
			if (t >= -2.41421356237 && t < 2.41421356237)
			{
				if (pad_x >= 0)
					J |= SNES_RIGHT_MASK;
				else
					J |= SNES_LEFT_MASK;
			}

			/*** Recalc up / down ***/
			t = (float) pad_x / pad_y;
			if (t >= -2.41421356237 && t < 2.41421356237)
			{
				if (pad_y >= 0)
					J |= SNES_UP_MASK;
				else
					J |= SNES_DOWN_MASK;
			}
		}
	}
#ifdef HW_RVL
	/***
	Wii Joystick (classic, nunchuk) input
	***/
	// Is XY inside the "zone"?
	if (wm_ax * wm_ax + wm_ay * wm_ay > PADCAL * PADCAL)
	{

	    if (wm_ax > 0 && wm_ay == 0)
			J |= SNES_RIGHT_MASK;
	    if (wm_ax < 0 && wm_ay == 0)
			J |= SNES_LEFT_MASK;
	    if (wm_ax == 0 && wm_ay > 0)
			J |= SNES_UP_MASK;
	    if (wm_ax == 0 && wm_ay < 0)
			J |= SNES_DOWN_MASK;

	    if (wm_ax != 0 && wm_ay != 0)
		{
			/*** Recalc left / right ***/
			t = (float) wm_ay / wm_ax;
			if (t >= -2.41421356237 && t < 2.41421356237)
			{
				if (wm_ax >= 0)
				{
					J |= SNES_RIGHT_MASK;
				}
				else
				{
					J |= SNES_LEFT_MASK;
				}
			}

			/*** Recalc up / down ***/
			t = (float) wm_ax / wm_ay;
			if (t >= -2.41421356237 && t < 2.41421356237)
			{
				if (wm_ay >= 0)
				{
					J |= SNES_UP_MASK;
				}
				else
				{
					J |= SNES_DOWN_MASK;
				}
			}
		}
	}
#endif

	// Report pressed buttons (gamepads)
	for (i = 0; i < MAXJP; i++)
	{
		if ( (jp & gcpadmap[i])											// gamecube controller
		#ifdef HW_RVL
		|| ( (exp_type == WPAD_EXP_NONE) && (wp & wmpadmap[i]) )	// wiimote
		|| ( (exp_type == WPAD_EXP_CLASSIC) && (wp & ccpadmap[i]) )	// classic controller
		|| ( (exp_type == WPAD_EXP_NUNCHUK) && (wp & ncpadmap[i]) )	// nunchuk + wiimote
		#endif
		)
		{
			J |= snespadmap[i];
		}
	}

	return J;
}

/****************************************************************************
 * NGCReportButtons
 *
 * Called on each rendered frame
 * Our way of putting controller input into Snes9x
 ***************************************************************************/
unsigned int GetJoys(int which)
{
	s8 gc_px = PAD_SubStickX (0);
	s8 gc_py = PAD_SubStickY (0);

	u16 gc_pb = PAD_ButtonsHeld (0);

#ifdef HW_RVL
	s8 wm_sx = WPAD_Stick (0,1,0);
	s8 wm_sy = WPAD_Stick (0,1,1);
	u32 wm_pb = WPAD_ButtonsHeld (0);	// wiimote / expansion button info
#endif


	/*** Check for video zoom ***/
	if (GCSettings.Zoom)
	{
		if (gc_py < -36 || gc_py > 36)
			zoom ((float) gc_py / -36);
#ifdef HW_RVL
		if (wm_sy < -36 || wm_sy > 36)
			zoom ((float) wm_sy / -36);
#endif
	}

    Settings.TurboMode = ( (gc_px > 70)
#ifdef HW_RVL
							|| (wm_sx > 70)
#endif
							);	// RIGHT on c-stick and on classic ctrlr right joystick

	/*** Check for menu:
	       CStick left
	       OR "L+R+X+Y" (eg. Hombrew/Adapted SNES controllers)
	       OR "Home" on the wiimote or classic controller
	       OR LEFT on classic right analog stick***/

    if ((gc_px < -70) ||
        ((gc_pb & PAD_TRIGGER_L) &&
         (gc_pb & PAD_TRIGGER_R ) &&
         (gc_pb & PAD_BUTTON_X) &&
         (gc_pb & PAD_BUTTON_Y ))
#ifdef HW_RVL
		 || (wm_pb & WPAD_BUTTON_HOME)
		 || (wm_pb & WPAD_CLASSIC_BUTTON_HOME)
		 || (wm_sx < -70)
#endif
       )
    {
        ConfigRequested = 1;	// go to the menu
		return 0;
    }
    else
    {
        return decodepad (which);
    }
}
/*
void SetControllers ()
{
	if (Settings.MultiPlayer5Master == true)
	{
		S9xSetController (0, CTL_JOYPAD, 0, 0, 0, 0);
		S9xSetController (1, CTL_MP5, 1, 2, 3, -1);
	}
	else if (Settings.SuperScopeMaster == true)
	{
		S9xSetController (0, CTL_JOYPAD, 0, 0, 0, 0);
		S9xSetController (1, CTL_SUPERSCOPE, 1, 0, 0, 0);
	}
	else if (Settings.MouseMaster == true)
	{
		S9xSetController (0, CTL_MOUSE, 0, 0, 0, 0);
		if (GCSettings.Mouse == 2)
			S9xSetController (1, CTL_MOUSE, 1, 0, 0, 0);
		else
			S9xSetController (1, CTL_JOYPAD, 1, 0, 0, 0);
	}
	else if (Settings.JustifierMaster == true)
	{
		S9xSetController (0, CTL_JOYPAD, 0, 0, 0, 0);
		if(GCSettings.Justifier == 2)
			S9xSetController(1, CTL_JUSTIFIER, 1, 0, 0, 0);
		else
			S9xSetController(1, CTL_JUSTIFIER, 0, 0, 0, 0);
	}
	else
	{
		// Plugin 2 Joypads by default
		S9xSetController (0, CTL_JOYPAD, 0, 0, 0, 0);
		S9xSetController (1, CTL_JOYPAD, 1, 0, 0, 0);
	}
}
*/
