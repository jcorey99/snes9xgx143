#include <gccore.h>
#include <zlib.h>
#include "menuanim/snes9x.h"
#include "menuanim/snes.h"
#include "menuanim/controller.h"
#include "menuanim/mario.h"

static unsigned char animframe[32][mario_RAW];		//Unpack buffer
static unsigned char snespic[snes_RAW];
static unsigned char snes9xlogo[snes9x_RAW];
static unsigned char controller[controller_RAW];
extern unsigned int *xfb[2];
extern int whichfb;

static int whichframe = 0;
static int frameid = 0;
/*static int blobrow = 0;
static int blobdirection = 0;*/
int showspinner = 1;
int showcontroller = 0;

void unpackanim()
{
	int i;
	unsigned long inbytes, outbytes, res;
	unsigned char *zipframe = NULL;
	inbytes = 0;

	for ( i = 0; i < 32; i++ )
	{
        switch ( i )
        {
                case 0:
                        zipframe = (unsigned char *)&mario00;
                        inbytes = mario00_COMPRESSED;
                        break;
                case 1:
                        zipframe = (unsigned char *)&mario01;
                        inbytes = mario01_COMPRESSED;
                        break;
                case 2:
                        zipframe = (unsigned char *)&mario02;
                        inbytes = mario02_COMPRESSED;
                        break;
                case 3:
                        zipframe = (unsigned char *)&mario03;
                        inbytes = mario03_COMPRESSED;
                        break;
                case 4:
                        zipframe = (unsigned char *)&mario04;
                        inbytes = mario04_COMPRESSED;
                        break;
                case 5:
                        zipframe = (unsigned char *)&mario05;
                        inbytes = mario05_COMPRESSED;
                        break;
                case 6:
                        zipframe = (unsigned char *)&mario06;
                        inbytes = mario06_COMPRESSED;
                        break;
                case 7:
                        zipframe = (unsigned char *)&mario07;
                        inbytes = mario07_COMPRESSED;
                        break;
                case 8:
                        zipframe = (unsigned char *)&mario08;
                        inbytes = mario08_COMPRESSED;
                        break;
                case 9:
                        zipframe = (unsigned char *)&mario09;
                        inbytes = mario09_COMPRESSED;
                        break;
                case 10:
                        zipframe = (unsigned char *)&mario10;
                        inbytes = mario10_COMPRESSED;
                        break;
                case 11:
                        zipframe = (unsigned char *)&mario11;
                        inbytes = mario11_COMPRESSED;
                        break;
                case 12:
                        zipframe = (unsigned char *)&mario12;
                        inbytes = mario12_COMPRESSED;
                        break;
                case 13:
                        zipframe = (unsigned char *)&mario13;
                        inbytes = mario13_COMPRESSED;
                        break;
                case 14:
                        zipframe = (unsigned char *)&mario14;
                        inbytes = mario14_COMPRESSED;
                        break;
                case 15:
                        zipframe = (unsigned char *)&mario15;
                        inbytes = mario15_COMPRESSED;
                        break;
                case 16:
                        zipframe = (unsigned char *)&mario16;
                        inbytes = mario16_COMPRESSED;
                        break;
                case 17:
                        zipframe = (unsigned char *)&mario17;
                        inbytes = mario17_COMPRESSED;
                        break;
                case 18:
                        zipframe = (unsigned char *)&mario18;
                        inbytes = mario18_COMPRESSED;
                        break;
                case 19:
                        zipframe = (unsigned char *)&mario19;
                        inbytes = mario19_COMPRESSED;
                        break;
                case 20:
                        zipframe = (unsigned char *)&mario20;
                        inbytes = mario20_COMPRESSED;
                        break;
                case 21:
                        zipframe = (unsigned char *)&mario21;
                        inbytes = mario21_COMPRESSED;
                        break;
                case 22:
                        zipframe = (unsigned char *)&mario22;
                        inbytes = mario22_COMPRESSED;
                        break;
                case 23:
                        zipframe = (unsigned char *)&mario23;
                        inbytes = mario23_COMPRESSED;
                        break;
                case 24:
                        zipframe = (unsigned char *)&mario24;
                        inbytes = mario24_COMPRESSED;
                        break;
                case 25:
                        zipframe = (unsigned char *)&mario25;
                        inbytes = mario25_COMPRESSED;
                        break;
                case 26:
                        zipframe = (unsigned char *)&mario26;
                        inbytes = mario26_COMPRESSED;
                        break;
                case 27:
                        zipframe = (unsigned char *)&mario27;
                        inbytes = mario27_COMPRESSED;
                        break;
                case 28:
                        zipframe = (unsigned char *)&mario28;
                        inbytes = mario28_COMPRESSED;
                        break;
                case 29:
                        zipframe = (unsigned char *)&mario29;
                        inbytes = mario29_COMPRESSED;
                        break;
                case 30:
                        zipframe = (unsigned char *)&mario30;
                        inbytes = mario30_COMPRESSED;
                        break;
                case 31:
                        zipframe = (unsigned char *)&mario31;
                        inbytes = mario31_COMPRESSED;
                        break;

	}

	outbytes = mario_RAW;

	res = uncompress( (unsigned char *)&animframe[i], &outbytes, (unsigned char *)zipframe, inbytes );

	}	
   
	/*** And the snespic ***/
        inbytes = snes_COMPRESSED;
        outbytes = snes_RAW;
        res = uncompress( (unsigned char *)&snespic, &outbytes, (unsigned char *)&snes, inbytes);

        inbytes = snes9x_COMPRESSED;
        outbytes = snes9x_RAW;
        res = uncompress( (unsigned char *)&snes9xlogo, &outbytes, (unsigned char *)&snes9x, inbytes);

        inbytes = controller_COMPRESSED;
        outbytes = controller_RAW;
        res = uncompress( (unsigned char *)&controller, &outbytes, (unsigned char *)&cont, inbytes);
}

void AnimFrame()
{
	
	int x,y,offset;
	unsigned int background;
	unsigned int *frame;

	frameid++;
	if ( frameid > 3 )
	{
		whichframe++;
		if ( whichframe > 31 )
			whichframe = 0;
	
		/*if ( blobdirection ) {
			blobrow++;
			if ( blobrow > 330 ) {
				blobdirection ^= 1;
			}	
		} else {
			blobrow--;
			if ( blobrow < 4 ) {
				blobdirection ^= 1;
			}
		}*/

		frameid = 0;
	}

	if ( showspinner ) {
		offset = 102640; //10480
		frame = (unsigned int *)&animframe[whichframe];
		background = frame[0];

		for ( y = 0; y < mario_HEIGHT; y++)
		{
			for ( x = 0; x < mario_WIDTH >> 1; x++ )
			{
				xfb[whichfb][ x + offset ] = frame[x + ( y * (mario_WIDTH >> 1))];
			}
			offset += 320;
		}

		offset = 10256;
		frame = (unsigned int *)&snes9xlogo[0];

		for ( y = 0; y < snes9x_HEIGHT; y++ )
		{
			for ( x = 0; x < (snes9x_WIDTH >> 1); x++ )
				xfb[whichfb][ x + offset ] = frame[x + ( y * (snes9x_WIDTH >> 1) )];
			offset += 320;		
		}
	}

	if ( showcontroller ) {
		offset = 57519;
		frame = (unsigned int *)&controller[0];

		for ( y = 0; y < controller_HEIGHT; y++)
		{
			for ( x = 0; x < controller_WIDTH >> 1; x++ )
			{
				xfb[whichfb][ x + offset ] = frame[x + ( y * (controller_WIDTH >> 1))];
			}
			offset += 320;
		}
	}

	offset = 109136;
	frame = (unsigned int *)&snespic[0];
	for ( y = 0; y < snes_HEIGHT; y++ )
	{
		for ( x = 0; x < (snes_WIDTH >> 1); x++ )
			xfb[whichfb][ x + offset ] = frame[x + ( y * (snes_WIDTH >> 1) )];
		offset += 320;		
	}

}

