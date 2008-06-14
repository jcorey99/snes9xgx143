/*****************************************************************************
 * Snes9x 1.4.3 - GameCube Video Layer
 *
 * Minimal GX based video layer
 *****************************************************************************/
#include <gccore.h>
#include <ogcsys.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <malloc.h>

#include "snes9x.h"
#include "memmap.h"
#include "debug.h"
#include "ppu.h"
#include "snapshot.h"
#include "gfx.h"
#include "display.h"
#include "apu.h"
#include "soundux.h"

#include "iplfont.h"

/*** Using Two framebuffers ***/
unsigned int *xfb[2] = { NULL, NULL };   /*** Framebuffer - used throughout ***/

GXRModeObj *vmode;			       /*** Generic GXRModeObj ***/
int whichfb = 0;			       /*** Remember which buffer is in use ***/

/*** GX Support ***/
#define TEX_WIDTH 256
#define TEX_HEIGHT 256
#define WIDTH 640
#define DEFAULT_FIFO_SIZE 256 * 1024
#define FSNES_HEIGHT 224
#define FSNES_WIDTH 256

/* #define FORCE_PAL50 1 */

unsigned int copynow = GX_FALSE;
int xscaler = 640;
static unsigned char gp_fifo[DEFAULT_FIFO_SIZE] __attribute__((__aligned__(32)));
/*** 0.0.2 Update - extend texture mem to support 512 x 240 ***/
static unsigned char texturemem[ TEX_WIDTH * (TEX_HEIGHT + 8) * 4 ] __attribute__((__aligned__(32)));
/*** 0.0.2 Update ends ***/
GXTexObj texobj;
GXColor background = {0, 0, 0, 0xff};
static Mtx projectionMatrix,modelViewMatrix;
extern void InitZip();

u8 vmode_60hz = 0;

/****************************************************************************
 * VideoThreading
 *
 * This thread attempts to 'buy-back' the time spent inside the video
 * retrace.
 *
 * The emulator occasionally gets close to 17ms when DSP/Transparency etc
 * are all brought into play.
 *
 * The focus of this port is gameplay, so a few dropped frames occasionally
 * isn't the end of the world, but losing upto 50% is!
 ****************************************************************************/
#define TSTACK 16384
lwpq_t videoblankqueue;
lwp_t vbthread;
static uint8 vbstack[TSTACK];

static void *vbgetback( void *arg )
{
    while(1)
    {
        VIDEO_WaitVSync();
        LWP_SuspendThread(vbthread);
    }
}

void InitVideoThread()
{
    /*** Initialise a new queue ***/
    LWP_InitQueue(&videoblankqueue);

    /*** Create the thread on this queue ***/
    LWP_CreateThread(&vbthread, vbgetback, NULL, vbstack, TSTACK, 80);	
}

/****************************************************************************
 * S9xSetPalette
 *
 * No palette required - using RGB565 colour
 ****************************************************************************/

void S9xSetPalette()
{
    return;
}

/****************************************************************************
 * Initialise the GX 
 ****************************************************************************/

void StartGX()
{
    /*** Clear out FIFO area ***/
    memset(&gp_fifo, 0, DEFAULT_FIFO_SIZE);

    /*** Initialise GX ***/
    GX_Init(&gp_fifo, DEFAULT_FIFO_SIZE);	
    GX_SetCopyClear(background, 0x00ffffff);

    /*** Additions from libogc ***/
    GX_SetViewport(0,0,vmode->fbWidth,vmode->efbHeight,0,1);
    GX_SetDispCopyYScale((f32)vmode->xfbHeight/(f32)vmode->efbHeight);
    GX_SetDispCopySrc(0,0,vmode->fbWidth,vmode->efbHeight);
    GX_SetDispCopyDst(vmode->fbWidth,vmode->xfbHeight);

    GX_SetPixelFmt(GX_PF_RGB8_Z24, GX_ZC_LINEAR);

    GX_SetCullMode(GX_CULL_NONE);
    GX_SetZMode(GX_FALSE,GX_ALWAYS,GX_TRUE);
    GX_SetColorUpdate(GX_TRUE);
    GX_CopyDisp(xfb[whichfb],GX_TRUE);

    /*** Additions from ogc spaceship ***/
    GX_SetDispCopyGamma(GX_GM_1_0);

    GX_ClearVtxDesc();
    GX_SetVtxAttrFmt(GX_VTXFMT0,GX_VA_POS,GX_POS_XYZ,GX_F32,0);
    GX_SetVtxAttrFmt(GX_VTXFMT0,GX_VA_TEX0,GX_TEX_ST,GX_F32,0);
    GX_SetVtxDesc(GX_VA_POS,GX_DIRECT);
    GX_SetVtxDesc(GX_VA_TEX0,GX_DIRECT);
    GX_Flush(); // Thanks to eke-eke

    S9xSetRenderPixelFormat(RGB565); 
}

/****************************************************************************
 * Set for 256 x 256 texture
 ****************************************************************************/

void set256texture()
{
    GX_SetNumChans(0);              /* default, color = vertex color */
    GX_SetNumTexGens(1);
    GX_SetTexCoordGen(GX_TEXCOORD0,GX_TG_MTX2x4,GX_TG_TEX0,GX_IDENTITY);
    GX_SetTevOrder(GX_TEVSTAGE0,GX_TEXCOORD0,GX_TEXMAP0,GX_COLORNULL);
    GX_SetTevOp(GX_TEVSTAGE0,GX_REPLACE);

    GX_InitTexObj(&texobj,&texturemem,TEX_WIDTH,TEX_HEIGHT,
            GX_TF_RGB565,GX_REPEAT,GX_REPEAT,GX_FALSE);

    DCFlushRange(&texturemem, TEX_WIDTH * TEX_HEIGHT * 2);
    GX_LoadTexObj(&texobj,GX_TEXMAP0);
    GX_InvalidateTexAll();

    /* load projection matrix */
    /*** Setting Height to 648 get's it right ? ***/
    guOrtho(projectionMatrix,0,640,xscaler,0,-1,1);
    GX_LoadProjectionMtx(projectionMatrix,GX_ORTHOGRAPHIC);

    /* load model view matrix */
    c_guMtxScale(modelViewMatrix,640,xscaler,1);
    GX_LoadPosMtxImm(modelViewMatrix,GX_PNMTX0);
}

/****************************************************************************
 * Set for 512 x 256 texture
 ****************************************************************************/

void set512texture()
{
    GX_SetNumChans(0);              /* default, color = vertex color */
    GX_SetNumTexGens(1);
    GX_SetTexCoordGen(GX_TEXCOORD0,GX_TG_MTX2x4,GX_TG_TEX0,GX_IDENTITY);
    GX_SetTevOrder(GX_TEVSTAGE0,GX_TEXCOORD0,GX_TEXMAP0,GX_COLORNULL);
    GX_SetTevOp(GX_TEVSTAGE0,GX_REPLACE);

    GX_InitTexObj(&texobj,&texturemem,TEX_WIDTH * 2,TEX_HEIGHT,
            GX_TF_RGB565,GX_REPEAT,GX_REPEAT,GX_FALSE);

    DCFlushRange(&texturemem, TEX_WIDTH * TEX_HEIGHT * 4);
    GX_LoadTexObj(&texobj,GX_TEXMAP0);
    GX_InvalidateTexAll();

    /* load projection matrix */
    /*** Setting Height to 648 get's it right ? ***/
    guOrtho(projectionMatrix,0,640,xscaler,0,-1,1);
    GX_LoadProjectionMtx(projectionMatrix,GX_ORTHOGRAPHIC);

    /* load model view matrix */
    c_guMtxScale(modelViewMatrix,640,xscaler,1);
    GX_LoadPosMtxImm(modelViewMatrix,GX_PNMTX0);
}

/****************************************************************************
 * BuildTexture
 *
 * This function basically mirror images the Snes screen in 4x4 tiles
 ****************************************************************************/
void BuildTexture( uint8 *src, uint8 *dst, int worker1, int worker2, int worker3)
{

    asm("subi 4,4,2");
    asm("subi 3,3,2");
    asm("lis 6,0");
    asm("ori 6,6,64");		/*** 64 tiles per block ***/
    asm("lis 7,0");
    asm("ori 7,7,60");		/*** 60 tiles vertical todo ***/	

    asm("domoretileshi:");
    asm("mtctr 6");
    asm("tilerowhi:");

    /*** Build a block of 64 tiles ***/
    asm("lhz 5,512(3)");		/*** Get pixel one, row one ***/
    asm("sthu 5,2(4)");
    asm("lhz 5,510(3)");		/*** Get pixel two, row one ***/
    asm("sthu 5,2(4)");		/*** Store this double ***/
    asm("lhz 5,508(3)");
    asm("sthu 5,2(4)");
    asm("lhz 5,506(3)");
    asm("sthu 5,2(4)");

    asm("lhz 5,1536(3)");		/*** Get pixel one, row two ***/
    asm("sthu 5,2(4)");
    asm("lhz 5,1534(3)");		/*** Get pixel two, row two ***/
    asm("sthu 5,2(4)");		/*** Store again ***/
    asm("lhz 5,1532(3)");
    asm("sthu 5,2(4)");
    asm("lhz 5,1530(3)");
    asm("sthu 5,2(4)");

    asm("lhz 5,2560(3)");		/*** Get pixel one, row three ***/
    asm("sthu 5,2(4)");
    asm("lhz 5,2558(3)");		/*** Get pixel two, row three ***/
    asm("sthu 5,2(4)");		/*** Store again ***/
    asm("lhz 5,2556(3)");
    asm("sthu 5,2(4)");
    asm("lhz 5,2554(3)");
    asm("sthu 5,2(4)");

    asm("lhz 5,3584(3)");
    asm("sthu 5,2(4)");
    asm("lhz 5,3582(3)");		/*** Get pixel two, row four ***/
    asm("sthu 5,2(4)");		/*** Store again ***/
    asm("lhz 5,3580(3)");
    asm("sthu 5,2(4)");
    asm("lhz 5,3578(3)");
    asm("sthu 5,2(4)");

    asm("subi 3,3,8");		/*** Move back for next pixel pair ***/
    asm("bdnz tilerowhi");		/*** Continue for 64 tiles ***/

    asm("addi 3,3,4608");		/*** Prepare for next 64 tile block ***/
    asm("subi 7,7,1");		
    asm("cmpwi 7,0");
    asm("bne domoretileshi");

}

/****************************************************************************
 * BuiltTextureHI
 *
 * Same as previous renderer, but for 128 tiles per row
 ****************************************************************************/
void BuildTextureHi( uint8 *src, uint8 *dst, int worker1, int worker2, int worker3)
{

    asm("subi 4,4,2");
    asm("subi 3,3,2");
    asm("lis 6,0");
    asm("ori 6,6,128");             /*** 128 tiles per block ***/
    asm("lis 7,0");
    asm("ori 7,7,60");              /*** 60 tiles vertical todo ***/

    asm("domoretiles:");
    asm("mtctr 6");
    asm("tilerow:");

    /*** Build a block of 64 tiles ***/
    asm("lhz 5,1024(3)");            /*** Get pixel one, row one ***/
    asm("sthu 5,2(4)");
    asm("lhz 5,1022(3)");            /*** Get pixel two, row one ***/
    asm("sthu 5,2(4)");             /*** Store this double ***/
    asm("lhz 5,1020(3)");
    asm("sthu 5,2(4)");
    asm("lhz 5,1018(3)");
    asm("sthu 5,2(4)");

    asm("lhz 5,2048(3)");           /*** Get pixel one, row two ***/
    asm("sthu 5,2(4)");
    asm("lhz 5,2046(3)");           /*** Get pixel two, row two ***/
    asm("sthu 5,2(4)");             /*** Store again ***/
    asm("lhz 5,2044(3)");
    asm("sthu 5,2(4)");
    asm("lhz 5,2042(3)");
    asm("sthu 5,2(4)");

    asm("lhz 5,3072(3)");           /*** Get pixel one, row three ***/
    asm("sthu 5,2(4)");
    asm("lhz 5,3070(3)");           /*** Get pixel two, row three ***/
    asm("sthu 5,2(4)");             /*** Store again ***/
    asm("lhz 5,3068(3)");
    asm("sthu 5,2(4)");
    asm("lhz 5,3066(3)");
    asm("sthu 5,2(4)");

    asm("lhz 5,4096(3)");
    asm("sthu 5,2(4)");
    asm("lhz 5,4094(3)");           /*** Get pixel two, row four ***/
    asm("sthu 5,2(4)");             /*** Store again ***/
    asm("lhz 5,4092(3)");
    asm("sthu 5,2(4)");
    asm("lhz 5,4090(3)");
    asm("sthu 5,2(4)");

    asm("subi 3,3,8");              /*** Move back for next pixel pair ***/
    asm("bdnz tilerow");            /*** Continue for 64 tiles ***/

    asm("addi 3,3,5120");           /*** Prepare for next 64 tile block ***/
    asm("subi 7,7,1");
    asm("cmpwi 7,0");
    asm("bne domoretiles");

}

/****************************************************************************
 * DrawFrame
 ****************************************************************************/
static int lastwidth = 0;
static int lastheight = 0;
static int texoffset = 0;

void DrawFrame( int Width, int Height )
{

    float gs = 1.0;
    float gt = 1.0;
    int worker1, worker2, worker3;

    while ( LWP_ThreadIsSuspended( vbthread ) == 0 ) {
        usleep(50);
    }

    memset(&texturemem, 0, TEX_WIDTH * TEX_HEIGHT * 4);

    if ( lastwidth != Width )
    {
        if( Width != 512 )
            set256texture();
        else
            set512texture();

        lastwidth = Width;
    }

    if ( lastheight != Height ) {

        if ( Height == 224 )
            texoffset = 16;
        else
            texoffset = 8;

        lastheight = Height;
    }

    worker1 = worker2 = worker3 = 0;

    if ( lastwidth != 512 ) {
        BuildTexture( (unsigned char *)GFX.Screen, (unsigned char *)&texturemem[ texoffset * TEX_WIDTH * 2 ],
                worker1, worker2, worker3 );
        DCFlushRange(&texturemem, TEX_WIDTH * TEX_HEIGHT * 2);
    } else {
        BuildTextureHi( (unsigned char *)GFX.Screen, (unsigned char *)&texturemem[ texoffset * TEX_WIDTH * 4 ],
                worker1, worker2, worker3 );
        DCFlushRange(&texturemem, TEX_WIDTH * TEX_HEIGHT * 4);
    }

    whichfb ^= 1;

    /* setup GX */
    GX_InvalidateTexAll(); 

    // ok render the triangle now 
    GX_Begin(GX_QUADS,GX_VTXFMT0,4);
    {
        GX_Position3f32(0,0,0);
        GX_TexCoord2f32(0,0);

        GX_Position3f32(0,1,0);
        GX_TexCoord2f32(0,gt);

        GX_Position3f32(1,1,0);
        GX_TexCoord2f32(gs,gt);

        GX_Position3f32(1,0,0);
        GX_TexCoord2f32(gs,0);
    }
    GX_End();
    GX_DrawDone();
    copynow = GX_TRUE;
    VIDEO_SetNextFramebuffer(xfb[whichfb]);
    VIDEO_Flush();

    LWP_ResumeThread(vbthread);

}

/****************************************************************************
 * GX Chip Copy to XFB
 ****************************************************************************/
static void copy_to_xfb()
{
    if ( copynow == GX_TRUE ) {
        GX_CopyDisp(xfb[whichfb],GX_TRUE);
        GX_Flush();
        copynow = GX_FALSE;
    }
}

/****************************************************************************
 * SNES9x Dynamic Rate Control
 ****************************************************************************/
unsigned int FrameTimer = 0;
void SnesTimer( unsigned int dummy )
{
    FrameTimer++;
    copy_to_xfb();
}

/****************************************************************************
 * InitGCVideo
 ****************************************************************************/

void InitGCVideo() {
    /*** Start VIDEO - ALWAYS CALL FIRST IN LIBOGC ***/
    VIDEO_Init();
    PAD_Init();

    vmode = VIDEO_GetPreferredMode(NULL);

    switch(vmode->viTVMode)
    {
    	case VI_TVMODE_PAL_DS:
	case VI_TVMODE_PAL_INT:
	    vmode_60hz = 0;
	    break;

	case VI_TVMODE_EURGB60_PROG:
	case VI_TVMODE_EURGB60_DS:
    	case VI_TVMODE_NTSC_DS:
	case VI_TVMODE_NTSC_INT:
	case VI_TVMODE_NTSC_PROG:
	case VI_TVMODE_MPAL_INT:
    	default:
	    vmode_60hz = 1;
	    break;
    }

    VIDEO_Configure(vmode);

    xfb[0] = (unsigned int *)MEM_K0_TO_K1(SYS_AllocateFramebuffer(vmode));
    xfb[1] = (unsigned int *)MEM_K0_TO_K1(SYS_AllocateFramebuffer(vmode));

//    console_init(xfb[0],20,64,vmode->fbWidth,vmode->xfbHeight,vmode->fbWidth*2);
    VIDEO_ClearFrameBuffer( vmode, xfb[ 0 ], COLOR_BLACK );
    VIDEO_ClearFrameBuffer( vmode, xfb[ 1 ], COLOR_BLACK );
    VIDEO_SetNextFramebuffer(xfb[0]);
    //VIDEO_SetPostRetraceCallback(PAD_ScanPads);
    VIDEO_SetPostRetraceCallback((VIRetraceCallback)PAD_ScanPads);
    VIDEO_SetPreRetraceCallback(SnesTimer);
    VIDEO_SetBlack(FALSE);
    VIDEO_Flush();
    VIDEO_WaitVSync();
    if(vmode->viTVMode&VI_NON_INTERLACE) VIDEO_WaitVSync();

    copynow = GX_FALSE;
    StartGX();
    InitVideoThread();	

    /*** Initialise the font for menus ***/
    init_font();

    /*** Initialise the DVD subsystem ***/
    DVD_Init();
}

