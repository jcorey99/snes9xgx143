/****************************************************************************
 * Drive Code Sending
 *
 * These codes are courtesy of Ninjamod 
 * and are used with permission.
 *
 * http://www.ninjamod.com
 ****************************************************************************/
 
#include <gccore.h>

extern volatile long *dvd;

unsigned int Drive04[] = {
0x40D000,
12,0xf4e1a538,0xcfdc0080,0xf4712aea,
12,0x08806ef4,0xe1135fcf,0xdc6e80a0,
12,0xf9b801f4,0xe1295fcf,0xf47447d0,
12,0x40f7204c,0x80f474d6,0x9c08f720,
12,0xd6fcf474,0x28ae08f7,0x20d2fc80,
12,0x04c4dafc,0xf4747ed4,0x08f000c8,
12,0xdafcf500,0x01e803fc,0xe200a0f4,
12,0x7400ec40,0xf51002f5,0x1003f510,
12,0x04f51005,0xf51006f5,0x1007f510,
12,0x08f51009,0xf5100af5,0x100bf510,
12,0x0cf5100d,0xc8dafcf5,0x0002e803,
12,0xfc3d01f4,0x74f9ec40,0x8002f020,
12,0xc88480c0,0x9c81dcb4,0x80f53000,
12,0xf444f9d1,0x40f8aa00,0x10f4d0f4,
12,0xd140f001,0xdcb480f5,0x3000f748,
12,0xaa00e907,0xf4c4f9d1,0x4010fed8,
12,0x32e81df7,0x48a800e8,0x26f748ab,
12,0x00e820f7,0x48e100e8,0x1af748ee,
12,0x00e83bd8,0x55e82ffe,0x7104fd20,
12,0x00f45100,0xd240a0f5,0x1000fef2,
12,0xf9f4d200,0xd2407104,0xfd0a00f2,
12,0x49fd0500,0x5104f236,0xfef721bc,
12,0xfff731bc,0xfffeccb5,0x80fd5300,
12,0xea0cccb5,0x80c4b081,0xccb680c4,
12,0x9481dcb4,0x80f8e000,0x10a0f510,
12,0x01f51002,0xf51003fe,0xccdafcf7,
12,0x00feffc4,0xdafccc44,0xfcf700fe,
12,0xffc444fc,0xf27cd004,0xcc5b80d8,
12,0x01e9027c,0x04f47585,0xd1405120,
12,0x7134f47d,0xc18508e9,0x1b8000cd,
12,0xdafcd800,0xe906f701,0xf7ffea03,
12,0xf50908c5,0xdafcf475,0xf8d14014,
12,0xfe8001ea,0xe2f710ff,0xf721f749,
12,0x0806e905,0x8502f511,0x0121f479,
12,0x00f000e9,0x0e8000f4,0xc9f8d140,
12,0xd900e803,0xf5100921,0xd906e90f,
12,0x6106f4c8,0xf8d140d8,0x00e802d5,
12,0x064106f4,0xe06fdccf,0xccdafcf7,
12,0x00fdffc4,0xdafccc44,0xfcf700fe,
12,0xffc444fc,0xf27cd004,0xcc5b80d8,
12,0x01e9027c,0x04f475ed,0xd1405120,
12,0xfef4e05c,0xcbcf0000,0x740a0800,
12,0x01000000,0x00000000,0x00000080,
12,0x00000000,0x00000000,0x00000000,
00,0x00804c,
03,0x00D04000,
99 };

unsigned int Drive06[] = {
0x40D000,
12,0xf4e1a538,0xc7dc0080,0xf471c8e9,
12,0x08806ef4,0xe11a5fc7,0xdc6e80a0,
12,0xf9ac01f4,0xe1305fc7,0xf47447d0,
12,0x40f7204c,0x80f47442,0x9d08f720,
12,0xd6fcf474,0x45b108f7,0x20d2fc80,
12,0x04c4dafc,0xf4741ed4,0x08f000c8,
12,0xdafcf500,0x01e803fc,0xe200a0f4,
12,0x7400ec40,0xf51002f5,0x1003f510,
12,0x04f51005,0xf51006f5,0x1007f510,
12,0x08f51009,0xf5100af5,0x100bf510,
12,0x0cf5100d,0xc8dafcf5,0x0002e803,
12,0xfc3d01f4,0x7402ed40,0x8002f020,
12,0xc87880c0,0x9081dca8,0x80f53000,
12,0xf444f9d1,0x40f8aa00,0x10f4d0f4,
12,0xd140f001,0xdca880f5,0x3000f748,
12,0xaa00e907,0xf4c4f9d1,0x4010fed8,
12,0x32e81df7,0x48a800e8,0x26f748ab,
12,0x00e820f7,0x48e100e8,0x1af748ee,
12,0x00e83bd8,0x55e82ffe,0x7104fd20,
12,0x00f45100,0xd240a0f5,0x1000fef2,
12,0xf9f4d200,0xd2407104,0xfd0a00f2,
12,0x49fd0500,0x5104f236,0xfef721bc,
12,0xfff731bc,0xfffecca9,0x80fd5300,
12,0xea0ccca9,0x80c4a481,0xccaa80c4,
12,0x8881dca8,0x80f8e000,0x10a0f510,
12,0x01f51002,0xf51003fe,0xccdafcf7,
12,0x00feffc4,0xdafccc44,0xfcf700fe,
12,0xffc444fc,0xf27cd004,0xcc5b80d8,
12,0x01e9027c,0x04f47585,0xd1405120,
12,0x7134f47d,0xb98508e9,0x1b8000cd,
12,0xdafcd800,0xe906f701,0xf7ffea03,
12,0xf50908c5,0xdafcf475,0xf8d14014,
12,0xfe8001ea,0xe2f710ff,0xf721f749,
12,0x0806e905,0x8502f511,0x0121f479,
12,0x00f000e9,0x0e8000f4,0xc9f8d140,
12,0xd900e803,0xf5100921,0xd906e90f,
12,0x6106f4c8,0xf8d140d8,0x00e802d5,
12,0x064106f4,0xe08cdfc7,0xccdafcf7,
12,0x00fdffc4,0xdafccc44,0xfcf700fe,
12,0xffc444fc,0xf27cd004,0xcc5b80d8,
12,0x01e9027c,0x04f475ed,0xd1405120,
12,0xfef4e0de,0xcbc70000,0x740a0800,
12,0x01000000,0x00000000,0x00000080,
12,0x00000000,0x00000000,0x00000000,
00,0x00804c,
03,0x00D04000,
99 };

unsigned int Drive08[] = {
0x40D000,
12,0xf4e1a538,0xcfdc0080,0xf4717cea,
12,0x08806ef4,0xe1135fcf,0xdc6e80a0,
12,0xf9b601f4,0xe1295fcf,0xf47447d0,
12,0x40f7204c,0x80f47432,0x9d08f720,
12,0xd6fcf474,0x75ae08f7,0x20d2fc80,
12,0x04c4dafc,0xf474d9d4,0x08f000c8,
12,0xdafcf500,0x01e803fc,0xe200a0f4,
12,0x7400ec40,0xf51002f5,0x1003f510,
12,0x04f51005,0xf51006f5,0x1007f510,
12,0x08f51009,0xf5100af5,0x100bf510,
12,0x0cf5100d,0xc8dafcf5,0x0002e803,
12,0xfc3d01f4,0x74f5ec40,0x8002f020,
12,0xc88080c0,0x9881dcb0,0x80f53000,
12,0xf444f9d1,0x40f8aa00,0x10f4d0f4,
12,0xd140f001,0xdcb080f5,0x3000f748,
12,0xaa00e907,0xf4c4f9d1,0x4010fed8,
12,0x32e81df7,0x48a800e8,0x26f748ab,
12,0x00e820f7,0x48e100e8,0x1af748ee,
12,0x00e83bd8,0x55e82ffe,0x7104fd20,
12,0x00f45100,0xd240a0f5,0x1000fef2,
12,0xf9f4d200,0xd2407104,0xfd0a00f2,
12,0x49fd0500,0x5104f236,0xfef721bc,
12,0xfff731bc,0xfffeccb1,0x80fd5300,
12,0xea0cccb1,0x80c4ac81,0xccb280c4,
12,0x9081dcb0,0x80f8e000,0x10a0f510,
12,0x01f51002,0xf51003fe,0xccdafcf7,
12,0x00feffc4,0xdafccc44,0xfcf700fe,
12,0xffc444fc,0xf27cd004,0xcc5b80d8,
12,0x01e9027c,0x04f47585,0xd1405120,
12,0x7134f47d,0xc18508e9,0x1b8000cd,
12,0xdafcd800,0xe906f701,0xf7ffea03,
12,0xf50908c5,0xdafcf475,0xf8d14014,
12,0xfe8001ea,0xe2f710ff,0xf721f749,
12,0x0806e905,0x8502f511,0x0121f479,
12,0x00f000e9,0x0e8000f4,0xc9f8d140,
12,0xd900e803,0xf5100921,0xd906e90f,
12,0x6106f4c8,0xf8d140d8,0x00e802d5,
12,0x064106f4,0xe0bcdccf,0xccdafcf7,
12,0x00fdffc4,0xdafccc44,0xfcf700fe,
12,0xffc444fc,0xf27cd004,0xcc5b80d8,
12,0x01e9027c,0x04f475ed,0xd1405120,
12,0xfef4e0b5,0xcbcf0000,0x740a0800,
12,0x01000000,0x00000000,0x00000080,
12,0x00000000,0x00000000,0x00000000,
00,0x00804c,
03,0x00D04000,
99 };

/****************************************************************************
 * Drivecode
 *
 * Dumb C Implementation of Anaconda Send Code
 ****************************************************************************/

void SendDriveCode( int model )
{

			int i = 0;
			int length;
			unsigned int cmd = 0xfe010100;
			unsigned int loadaddress;
			unsigned int *DriveCode;
			
			switch( model )
			{	case 4: DriveCode = (unsigned int *)Drive04; break;
				  case 6: DriveCode = (unsigned int *)Drive06; break;
				  case 8: DriveCode = (unsigned int *)Drive08; break;
				  default: return;
			}
			
			loadaddress = DriveCode[i++];
			length = DriveCode[i++];
			
			while ( length == 12 )
			{

					/*** Address Select ***/
					dvd[0] |= 0x14;
					dvd[1] = 0;
					dvd[2] = cmd;
					dvd[3] = loadaddress;
					dvd[4] = (length << 16);
					dvd[5] = 0;
					dvd[6] = 0;
					dvd[7] = 3;
					while ( dvd[7] & 1 );
					
					dvd[0] |= 0x14;
					dvd[1] = 0;
					dvd[2] = DriveCode[i++];
					dvd[3] = DriveCode[i++];
					dvd[4] = DriveCode[i++];
					dvd[5] = 0;
					dvd[6] = 0;
					dvd[7] = 1;
					while ( dvd[7] & 1 );
								
					loadaddress += length;
					length = DriveCode[i++];
			}
			
			loadaddress = DriveCode[i++];
			length = DriveCode[i++];
									
			/*** Do SendDriveCommand End ***/
			dvd[0] |= 0x14;
			dvd[1] = 0;
			dvd[2] = cmd;
			dvd[3] = loadaddress;
			dvd[4] = (length << 16);
			dvd[5] = 0;
			dvd[6] = 0;
			dvd[7] = 3;
			while ( dvd[7] & 1 );
			
			dvd[0] |= 0x14;
			dvd[1] = 0;
			dvd[2] = DriveCode[i++];
			dvd[3] = 0;
			dvd[4] = 0;
			dvd[5] = 0;
			dvd[6] = 0;
			dvd[7] = 1;
			while ( dvd[7] & 1 );
			
}

