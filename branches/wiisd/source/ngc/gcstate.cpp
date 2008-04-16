/****************************************************************************
 * Snes9x 1.43 - GX Edition -Taken from snesgx2.0
 *
 * NGC Snapshots Memory File System
 *
 * This is a single global memory file controller. Don't even think of opening two
 * at the same time !
 *
 * There's just enough here to do SnapShots - you should add anything else you
 * need.
 *
 * softdev July 2006
 * crunchy2 May 2007-July 2007
 * Askot February 2008
 ****************************************************************************/
#include <gccore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sdcard.h>
#include <zlib.h>

#include "gcstate.h"
#include "snes9x.h"
#include "memmap.h"
#include "soundux.h"
#include "snapshot.h"
#include "srtc.h"
#ifdef HW_RVL
#include "tff.h"
#include "integer.h"
#endif

//#include "Snes9xGx.h"
//#include "filesel.h"
//#include "ftfont.h"
//#include "smbload.h"
//#include "mcsave.h"
//#include "saveicon.h"

/*extern "C"
{
#include "smb.h"
}*/

#include "gcconfig.h"

#define SNESDIR "snes9x"
#define SAVEDIR "saves" 
#define SAVEBUFFERSIZE 0x30000
#define MEMBUFFER (512 * 1024)

extern unsigned short saveicon[1024];

extern void S9xSRTCPreSaveState ();
extern void NGCFreezeStruct ();
extern bool8 S9xUnfreezeGame (const char *filename);

static u8 SysArea[CARD_WORKAREA] ATTRIBUTE_ALIGN(32);
extern unsigned char savebuffer[SAVEBUFFERSIZE] ATTRIBUTE_ALIGN (32);

extern int MountTheCard(int slot);
extern int CardFileExists(char *filename, int slot);
extern card_dir CardDir;
extern card_file CardFile;
extern card_stat CardStatus;

static int bufoffset;
static char membuffer[MEMBUFFER];

char freezecomment[2][32] = { {"Snes9x 1.43GX011 Freeze"}, {"Freeze"} };

/****************************************************************************
 * Load savebuffer from Memory Card file
 ****************************************************************************/
int LoadBufferFromMC (unsigned char *buf, int slot, char *filename)
{
	int CardError;
	unsigned int blocks;
	unsigned int SectorSize;
	char msg[80];
    int bytesleft = 0;
    int bytesread = 0;

	/*** Initialize Card System ***/
	memset (SysArea, 0, CARD_WORKAREA);
	CARD_Init ("SNES", "00");
	
	/*** Try to mount the card ***/
	CardError = MountTheCard(slot);
	
	if (CardError == 0)
	{
		/*** Get Sector Size ***/
		CARD_GetSectorSize (slot, &SectorSize);
		
		if (!CardFileExists (filename, slot))
		{
			sprintf(msg,"Unable to open file");
			WaitPrompt (msg);
			return 0;
		}
		
		memset (&CardFile, 0, sizeof (CardFile));
		CardError = CARD_Open (slot, filename, &CardFile);
		
		blocks = CardFile.len;
		
		if (blocks < SectorSize)
			blocks = SectorSize;
		
		if (blocks % SectorSize)
			blocks += SectorSize;
		
		memset (buf, 0, 0x22000);
		
		bytesleft = blocks;
		bytesread = 0;
		while (bytesleft > 0)
		{
			CARD_Read (&CardFile, buf + bytesread, SectorSize, bytesread);
			bytesleft -= SectorSize;
			bytesread += SectorSize;
		}
		CARD_Close (&CardFile);
		CARD_Unmount (slot);
	}
	else{
		sprintf(msg, "Unable to Mount MCARD Slot %s!", (slot == CARD_SLOTA) ? "A" : "B");
		WaitPrompt (msg);
	}
	return bytesread;
}


/****************************************************************************
 * Write savebuffer to Memory Card file
 ****************************************************************************/
int SaveBufferToMC (unsigned char *buf, int slot, char *filename, int datasize)
{
	int CardError;
	unsigned int blocks;
	unsigned int SectorSize;
	char msg[80];
	
	/*** Initialize Card System ***/
	memset (SysArea, 0, CARD_WORKAREA);
	CARD_Init ("SNES", "00");
	
	/*** Try to mount the card ***/
	CardError = MountTheCard(slot);
	
	if (CardError == 0) {
		/*** Get Sector Size ***/
		CARD_GetSectorSize (slot, &SectorSize);
		
		if (datasize) {			
			/*** Calculate number of blocks required ***/
			blocks = (datasize / SectorSize) * SectorSize;
			if (datasize % SectorSize)
				blocks += SectorSize;
						
			/*** Does this file exist ? ***/
			if (CardFileExists (filename, slot)) {
				/*** Try to open the file ***/
				CardError = CARD_Open (slot, filename, &CardFile);
				if (CardError)
				{
                    CARD_Unmount (slot);
					WaitPrompt((char*)"Unable to open card file!");
					return 0;
				}
				
				if ( (s32)blocks > CardFile.len )  /*** new data is longer ***/
				{
                    CARD_Close (&CardFile);
                   
                    /*** Try to create temp file to check available space ***/
                    CardError = CARD_Create (slot, "TEMPFILESNES9XGX011", blocks, &CardFile);
                    if (CardError) {
                        CARD_Unmount (slot);
                        WaitPrompt((char*)"Not enough space to update file!");
                        return 0;
                    }
                    
                    /*** Delete the temporary file ***/
                    CARD_Close (&CardFile);
                    CardError = CARD_Delete(slot, "TEMPFILESNES9XGX011");
                    if (CardError) {
                        CARD_Unmount (slot);
                        WaitPrompt((char*)"Unable to delete temporary file!");
                        return 0;
                    }
                    
                    /*** Delete the existing shorter file ***/
                    CardError = CARD_Delete(slot, filename);
                    if (CardError) {
                        CARD_Unmount (slot);
                        WaitPrompt((char*)"Unable to delete existing file!");
                        return 0;
                    }
                    
                    /*** Create new, longer file ***/
                    CardError = CARD_Create (slot, filename, blocks, &CardFile);
                    if (CardError) {
                        CARD_Unmount (slot);
                        WaitPrompt((char*)"Unable to create updated card file!");
                        return 0;
                    }
				}
			} else { /*** no file existed, create new one ***/
                /*** Create new file ***/
                CardError = CARD_Create (slot, filename, blocks, &CardFile);
                if (CardError)
                {
                    CARD_Unmount (slot);
                    if ( CardError == CARD_ERROR_INSSPACE )
                        WaitPrompt((char*)"Not enough space to create file!");
                    else
                        WaitPrompt((char*)"Unable to create card file!");
                    return 0;
                }
            }
			
			/*** Now, have an open file handle, ready to send out the data ***/
			CARD_GetStatus (slot, CardFile.filenum, &CardStatus);
			CardStatus.icon_addr = 0x0;
			CardStatus.icon_fmt = 2;
			CardStatus.icon_speed = 1;
			CardStatus.comment_addr = 2048;
			CARD_SetStatus (slot, CardFile.filenum, &CardStatus);
			
			int byteswritten = 0;
			int bytesleft = blocks;
			while (bytesleft > 0)
			{
				CardError =	CARD_Write (&CardFile, buf + byteswritten, SectorSize, byteswritten);
				bytesleft -= SectorSize;
				byteswritten += SectorSize;
			}
			
			CARD_Close (&CardFile);
			CARD_Unmount (slot);

            return byteswritten;
		}
		else
			WaitPrompt((char*)"This game does not appear to use SRAM");
	}
	else{
		sprintf(msg, "Unable to Mount MCARD Slot %s!", (slot == CARD_SLOTA) ? "A" : "B");
		WaitPrompt (msg);
	}
	return 0;
}

/*
 * Return x bytes from memory buffer
 */
int GetMem (char *buffer, int len)
{
  memcpy (buffer, membuffer + bufoffset, len);
  bufoffset += len;

  return len;
}

/*
 * Put some values in memory buffer
 */
static void PutMem (char *buffer, int len)
{
  memcpy (membuffer + bufoffset, buffer, len);
  bufoffset += len;
}

void NGCFreezeBlock (char *name, uint8 * block, int size)
{
  char buffer[512];

  sprintf (buffer, "%s:%06d:", name, size);
  PutMem (buffer, strlen (buffer));
  PutMem ((char *) block, size);
}

/*
 * NGCFreezeMembuffer
 */
static int NGCFreezeMemBuffer ()
{
    int i;
    char buffer[1024];
    
    bufoffset = 0;
    
    S9xUpdateRTC ();
    S9xSRTCPreSaveState ();
    
    for (i = 0; i < 8; i++)
    {
        SoundData.channels[i].previous16[0] = (int16) SoundData.channels[i].previous[0];
        SoundData.channels[i].previous16[1] = (int16) SoundData.channels[i].previous[1];
    }
    
    sprintf (buffer, "%s:%04d\n", SNAPSHOT_MAGIC, SNAPSHOT_VERSION);
    PutMem (buffer, strlen (buffer));
    sprintf (buffer, "NAM:%06d:%s%c", (int) strlen (Memory.ROMFilename) + 1,Memory.ROMFilename, 0);
    
    PutMem (buffer, strlen (buffer) + 1);
    
    NGCFreezeStruct ();
    
    return 0;
}

/*
 * NGCUnFreezeBlock
 */
int NGCUnFreezeBlock (char *name, uint8 * block, int size){
  char buffer[20], *e;
  int len = 0;
  int rem = 0;

  GetMem (buffer, 11);

  if (strncmp (buffer, name, 3) != 0 || buffer[3] != ':' || buffer[10] != ':' || (len = strtol (&buffer[4], &e, 10)) == 0 || e != buffer + 10)
    {
      return WRONG_FORMAT;
    }

  if (len > size)
    {
      rem = len - size;
      len = size;
    }

  ZeroMemory (block, size);

  GetMem ((char *) block, len);

  if (rem){
    bufoffset += rem;
  }

  return SUCCESS;
}

/*
 * Save state in Nintendo Gamecube
 */
int NGCFreezeGame (int device, int slot) {
    char filename[1024];
    sd_file *handle;
    unsigned int len = 0;
    u32 offset = 0;
    char msg[100];
    unsigned long outBytes = 0;
    unsigned long zippedsize = 0;
    unsigned long decompressedsize = 0;
    int err = 0;
    int ret = 0;

    //S9xSetSoundMute (TRUE);
    //S9xPrepareSoundForSnapshotSave (FALSE);

    NGCFreezeMemBuffer ();

    //S9xPrepareSoundForSnapshotSave (TRUE);
    //S9xSetSoundMute (FALSE);

    memset(savebuffer, 0, 0x22000);

    /*** Copy in save icon ***/
    offset = sizeof (saveicon);
    memcpy(savebuffer, saveicon, offset);

    /*** And the freezecomment ***/
    sprintf (freezecomment[1], "%s", Memory.ROMName);        
    memcpy(savebuffer + offset, freezecomment, 64);
    offset += 64;

    /*** Zip and copy in the freeze ***/
    outBytes = (uLongf) SAVEBUFFERSIZE;
    err = compress2((Bytef*)(savebuffer+offset+8), (uLongf*)&outBytes, (const Bytef *)membuffer, (uLongf)bufoffset, Z_BEST_COMPRESSION);

    if(err != Z_OK) {
        sprintf (msg, "Zip error %s ",zError(err));
        WaitPrompt (msg);
        return 0;
    }

    zippedsize = (int)outBytes;
    memcpy (savebuffer+offset, &zippedsize, 4);
    offset += 4;

    decompressedsize = (int)bufoffset;
    memcpy (savebuffer+offset, &decompressedsize, 4);
    offset += 4;

    offset += zippedsize;

    if (device == 1) { /*** SDCard slot ***/
        if (slot < 2) {
            sprintf (filename, "dev%d:\\%s\\%s\\%08X.snz", slot, SNESDIR, SAVEDIR, Memory.ROMCRC32);

            handle = SDCARD_OpenFile (filename, "wb");
            if (handle > 0) {
                ShowAction((char*)"Saving state game in SDCARD ...");

                len = SDCARD_WriteFile (handle, savebuffer, offset);
                SDCARD_CloseFile (handle);

                if (len != offset)
                    WaitPrompt((char*)"Error writing state file");
                else {
                    sprintf (filename, "Saved %d bytes successfully", offset);
                    ShowAction(filename);
                }
            } else {
                sprintf(msg, "Couldn't save to %s", filename);
                WaitPrompt (msg);
            }
        } else { // WiiSD
#ifdef HW_RVL
            ShowAction((char*)"Saving STATE to Wii SD...");
            sprintf(filename, "/%s/%s/%08X.snz", SNESDIR, SAVEDIR, Memory.ROMCRC32);
            FIL fp;
            WORD written = 0;
            u32 total_written = 0, datasize = offset;
            int res;

            if ((res = f_open(&fp, filename, FA_CREATE_ALWAYS | FA_WRITE)) != FR_OK) {
                sprintf(msg, "f_open failed, error %d", res);
                WaitPrompt(msg);
                return 1;
            }
            sprintf(msg, "Writing %d bytes..", datasize);
            WaitPrompt(msg);
            // Can only write 64k at a time
            while (offset > 65000) {
                if ((res = f_write(&fp, &savebuffer[total_written], 65000, &written)) != FR_OK) {
                    sprintf(msg, "f_write failed, error %d", res);
                    WaitPrompt(msg);
                    f_close(&fp);
                    return 1;
                }
                offset -= written;
                total_written += written;
            }
            // Write last 64k
            if ((res = f_write(&fp, savebuffer+total_written, offset, &written)) != FR_OK) {
                sprintf(msg, "f_write failed, error %d", res);
                WaitPrompt(msg);
                f_close(&fp);
                return 1;
            }
            offset -= written;
            total_written += written;
            sprintf(msg, "Wrote %d of %d bytes", total_written, datasize);
            ShowAction(msg);
            if (total_written == datasize) {
                sprintf(msg, "Saved %d bytes.", written);
                ShowAction(msg);
                f_close(&fp);
                return 0;
            }

            sprintf(msg, "Write size mismatch, %d of %d bytes", written, datasize);
            WaitPrompt(msg);
            sprintf(msg, "Unable to save %s", filename);
            WaitPrompt(msg);
            f_close(&fp);
            return 1;
#endif
        }
    } else { /*** MC in slot ***/
        ShowAction((char*)"Saving STATE game in MCARD ..."); 

        sprintf (filename, "%s.snz", Memory.ROMName);
        ret = SaveBufferToMC ( savebuffer, slot, filename, offset );

        if ( ret ) {
            sprintf (filename, "Saved %d bytes successfully", ret);
            WaitPrompt (filename);
        }
    }

    //free(savebuffer);
    return 0;
}

/*
 * NGCUnfreezeGame
 */
int NGCUnfreezeGame (int device, int slot)
{
    char filename[1024];
    sd_file *handle;
    int read = 0;
    int offset = 0;
    char msg[80];

    unsigned long zipsize = 0;
    unsigned long decompressedsize = 0;
    unsigned long outBytes = 0;
    int err = 0;

    bufoffset = 0;

    //memset(savebuffer, 0, 0x22000);

    if (device == 1) { /*** Load state from SDCARD ***/
        if (slot < 2) {
            sprintf (filename, "dev%d:\\%s\\%s\\%08X.snz", slot, SNESDIR, SAVEDIR, Memory.ROMCRC32);

            handle = SDCARD_OpenFile (filename, "rb");
            if (handle > 0) {
                ShowAction((char*)"Loading STATE file...");

                offset = 0;
                while ((read = SDCARD_ReadFile (handle, savebuffer+offset, 2048)) > 0) {
                    offset += read;
                }
                SDCARD_CloseFile (handle);
                if (offset == 0) return 0;
            } else {
                WaitPrompt((char*)"No STATE file found");
                return 0;
            }
        } else { // WiiSD
#ifdef HW_RVL
            ShowAction((char*)"Loading STATE file from Wii SD...");    
            sprintf(filename, "/%s/%s/%08X.snz", SNESDIR, SAVEDIR, Memory.ROMCRC32);
            FIL fp;
            WORD bytes_read;
            u32 bytes_read_total;
            FILINFO finfo;

            int res;
            if ((res=f_stat(filename, &finfo)) != FR_OK) {
                if (res == FR_NO_FILE) {
                    sprintf(msg, "Unable to find %s.", filename);
                }
                else {
                    sprintf(msg, "f_stat failed, error %d", res);
                }
                WaitPrompt(msg);
                return 0;
            }
            if ((res=f_open(&fp, filename, FA_READ)) != FR_OK) {
                sprintf(msg, "Failed to open %s, error %d.", filename, res);
                WaitPrompt(msg);
                return 0;
            }

            bytes_read = bytes_read_total = 0;
            while(bytes_read_total < finfo.fsize) {
                if (f_read(&fp, &savebuffer[bytes_read_total], 0x200, &bytes_read) != FR_OK) {
                    WaitPrompt((char*)"f_read failed");
                    f_close(&fp);
                    return 0;
                }

                if (bytes_read == 0)
                    break;
                bytes_read_total += bytes_read;
            }

            if (bytes_read_total < finfo.fsize) {
                WaitPrompt((char*)"read failed");
                f_close(&fp);
                return 0;
            }
            sprintf(msg, "Read %d of %ld bytes.", bytes_read_total, finfo.fsize);
            ShowAction(msg);
            f_close(&fp);
            offset = bytes_read_total;
#endif
        }
    } else { /*** Load state from MCARD ***/
        sprintf (filename, "%s.snz", Memory.ROMName);
        ShowAction((char*)"Loading STATE file from MCARD...");

        offset = LoadBufferFromMC (savebuffer, slot, filename);
        if ( offset == 0) return 0;
    }

    // skip the saveicon and comment
    offset = (sizeof(saveicon) + 64);
    zipsize = 0;
    decompressedsize = 0;

    memcpy (&zipsize, savebuffer+offset, 4);
    offset += 4;

    memcpy (&decompressedsize, savebuffer+offset, 4);
    offset += 4;

    memset(membuffer, 0, MEMBUFFER);

    outBytes = MEMBUFFER;
    err = uncompress((Bytef *)membuffer, (uLongf*)&outBytes, (const Bytef *)(savebuffer+offset), zipsize);

    if (err != Z_OK) {
        sprintf (msg, "Unzip error %s ",zError(err));
        WaitPrompt (msg);
        return 0;
    }

    if (outBytes != decompressedsize) {
        WaitPrompt((char*)"Unzipped size doesn't match expected size!");
        return 0;
    }

    if (S9xUnfreezeGame ("AGAME") != SUCCESS) {
        WaitPrompt((char*)"Error thawing");
        return 0;
    } else {
        S9xSetSoundMute (TRUE); //To avoid sound glitch

        sprintf (msg, "Loaded %ld bytes successfully", zipsize);
        ShowAction(msg);
    }

    //free(savebuffer);
    return 1;
}
