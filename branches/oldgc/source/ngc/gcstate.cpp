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

#define SNESSAVEDIR "snes9x\\saves" 
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
		
		memset (buf, 0, SAVEBUFFERSIZE);
		
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
	
	if (CardError == 0)
	{
		/*** Get Sector Size ***/
		CARD_GetSectorSize (slot, &SectorSize);
		
		if (datasize)
		{			
			/*** Calculate number of blocks required ***/
			blocks = (datasize / SectorSize) * SectorSize;
			if (datasize % SectorSize)
				blocks += SectorSize;
						
			/*** Does this file exist ? ***/
			if (CardFileExists (filename, slot))
			{
				/*** Try to open the file ***/
				CardError = CARD_Open (slot, filename, &CardFile);
				if (CardError)
				{
                    CARD_Unmount (slot);
					WaitPrompt ("Unable to open card file!");
					return 0;
				}
				
				if ( (s32)blocks > CardFile.len )  /*** new data is longer ***/
				{
                    CARD_Close (&CardFile);
                   
                    /*** Try to create temp file to check available space ***/
                    CardError = CARD_Create (slot, "TEMPFILESNES9XGX201", blocks, &CardFile);
                    if (CardError)
                    {
                        CARD_Unmount (slot);
                        WaitPrompt ("Not enough space to update file!");
                        return 0;
                    }
                    
                    /*** Delete the temporary file ***/
                    CARD_Close (&CardFile);
                    CardError = CARD_Delete(slot, "TEMPFILESNES9XGX201");
                    if (CardError)
                    {
                        CARD_Unmount (slot);
                        WaitPrompt ("Unable to delete temporary file!");
                        return 0;
                    }
                    
                    /*** Delete the existing shorter file ***/
                    CardError = CARD_Delete(slot, filename);
                    if (CardError)
                    {
                        CARD_Unmount (slot);
                        WaitPrompt ("Unable to delete existing file!");
                        return 0;
                    }
                    
                    /*** Create new, longer file ***/
                    CardError = CARD_Create (slot, filename, blocks, &CardFile);
                    if (CardError)
                    {
                        CARD_Unmount (slot);
                        WaitPrompt ("Unable to create updated card file!");
                        return 0;
                    }
				}
			}
			else  /*** no file existed, create new one ***/
			{
                /*** Create new file ***/
                CardError = CARD_Create (slot, filename, blocks, &CardFile);
                if (CardError)
                {
                    CARD_Unmount (slot);
                    if ( CardError = CARD_ERROR_INSSPACE )
                        WaitPrompt ("Not enough space to create file!");
                    else
                        WaitPrompt ("Unable to create card file!");
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
			WaitPrompt ("This game does not appear to use SRAM");
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
int NGCFreezeGame (int device, int slot)
{
    char filename[1024];
    sd_file *handle;
    int len = 0;
    int wrote = 0;
    int offset = 0;
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
	
	memset(savebuffer, 0, SAVEBUFFERSIZE);

	/*** Copy in save icon ***/
    offset = sizeof (saveicon);
    memcpy(savebuffer, saveicon, offset);

    /*** And the freezecomment ***/
    sprintf (freezecomment[1], "%s", Memory.ROMName);        
	memcpy(savebuffer + offset, freezecomment, 64);
    offset += 64;
	
	/*** Zip and copy in the freeze ***/
	outBytes = (uLongf) 0x22000;
	err = compress2((Bytef*)(savebuffer+offset+8), (uLongf*)&outBytes, (const Bytef *)membuffer, (uLongf)bufoffset, Z_BEST_COMPRESSION);

	if(err != Z_OK)
	{
		sprintf (msg, "zip error %s ",zError(err));
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
    
    if (device == 1)  /*** SDCard slot ***/
    {
		sprintf (filename, "dev%d:\\%s\\%08x.snz", slot, SNESSAVEDIR, Memory.ROMCRC32);
		
        handle = SDCARD_OpenFile (filename, "wb");
        
        if (handle > 0)
        {
            ShowAction ("Saving state game in SDCARD ...");
			
            len = SDCARD_WriteFile (handle, savebuffer, offset);
            SDCARD_CloseFile (handle);

            if (len != offset)
                WaitPrompt ("Error writing state file");
            else
            {
                sprintf (filename, "Saved %d bytes successfully", offset);
                WaitPrompt (filename);
            }   
        }
        else
        {
            sprintf(msg, "Couldn't save to dev%d:\\%s\\", slot, SNESSAVEDIR);
            WaitPrompt (msg);
        }
    }
    else  /*** MC in slot ***/
    {
        ShowAction ("Saving STATE game in MCARD ..."); 
		
        sprintf (filename, "%s.snz", Memory.ROMName);
		
        ret = SaveBufferToMC ( savebuffer, slot, filename, offset );

        if ( ret )
        {
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
	int ret = 0;
    
    bufoffset = 0;
    
    if (device == 1)  /*** Load state from SDCARD ***/
    {
		sprintf (filename, "dev%d:\\%s\\%08x.snz", slot, SNESSAVEDIR, Memory.ROMCRC32);
		
        handle = SDCARD_OpenFile (filename, "rb");
        
        if (handle > 0)
        {
            ShowAction ("Loading STATE file...");
			
            offset = 0;
            while ((read = SDCARD_ReadFile (handle, savebuffer+offset, 2048)) > 0){
                offset += read;
            }
            SDCARD_CloseFile (handle);
			if (offset == 0) return 0;
        }
        else{
            WaitPrompt ("No STATE file found");
            return 0;
        }
    }
    else       /*** Load state from MCARD ***/
    {
	    sprintf (filename, "%s.snz", Memory.ROMName);
		
        ShowAction ("Loading STATE file from MCARD...");
        
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

    if (err != Z_OK)
    {
        sprintf (msg, "Unzip error %s ",zError(err));
        WaitPrompt (msg);
        return 0;
    }

    if (outBytes != decompressedsize)
    {
        WaitPrompt ("Unzipped size doesn't match expected size!");
        return 0;
    }
	
    if (S9xUnfreezeGame ("AGAME") != SUCCESS)
    {
        WaitPrompt ("Error thawing");
        return 0;
    }
	else{
		S9xSetSoundMute (TRUE); //To avoid sound glitch
		
		sprintf (msg, "Loaded %d bytes successfully", zipsize);
		WaitPrompt (msg);
	}
    
	//free(savebuffer);
    return 1;
}
