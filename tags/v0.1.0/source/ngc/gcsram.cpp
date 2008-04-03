/****************************************************************************
 * SRAM Save
 *
 * All functions to save out the SRAM are in this module
 ****************************************************************************/
#include <gccore.h>
#include <sdcard.h>
#include <zlib.h>
#include <snes9x.h>
#include <memmap.h>
#include "saveicon.h"

#define SNESSAVEDIR "snes9x\\saves" 

extern void WaitPrompt( char *msg );
extern void ShowAction( char *msg );
static u8 SysArea[CARD_WORKAREA] ATTRIBUTE_ALIGN(32);
unsigned char savebuffer[0x22000] ATTRIBUTE_ALIGN (32);
card_dir CardDir;
card_file CardFile;
card_stat CardStatus;

int CARDSLOT = CARD_SLOTA;
int ISACTIVE[2] = { 0, 0 };
char saveinfo[4];
extern char mpads[4];
extern char PADMap( int padvalue, int padnum );
extern char padmenu[8][20];
extern void S9xSoftReset();

extern void uselessinquiry ();

#define SAVEBUFFERSIZE 65536

#if 0
typedef struct _card_block {
        u8 cmd[9];
        u32 cmd_len;
        u32 cmd_mode;
        u32 cmd_blck_cnt;
        u32 cmd_sector_addr;
        u32 cmd_retries;
        u32 attached;
        s32 result;
        u32 cid;
        u16 card_size;
        u32 mount_step;
        u32 format_step;
        u32 sector_size;
        u16 blocks;
        u32 latency;
        u32 cipher;
        u32 key[3];
        u32 transfer_cnt;
        u16 curr_fileblock;
        card_file *curr_file;
        struct card_dat *curr_dir;
        struct card_bat *curr_fat;
        void *workarea;
        void *cmd_usr_buf;
        lwpq_t wait_sync_queue;
        sysalarm timeout_svc;
        dsptask_t dsp_task;

        cardcallback card_ext_cb;
        cardcallback card_tx_cb;
        cardcallback card_exi_cb;
        cardcallback card_api_cb;
        cardcallback card_xfer_cb;
        cardcallback card_erase_cb;
        cardcallback card_unlock_cb;
} card_block;

card_block backup;
#endif

/****************************************************************************
 * Clear the savebuffer
 ****************************************************************************/
void ClearSaveBuffer ()
{
    memset (savebuffer, 0, SAVEBUFFERSIZE);
}

int MountTheCard(int slot)
{
	int tries = 0;
	int CardError;
	while ( tries < 10 )
	{
		*(unsigned long*)(0xcc006800) |= 1<<13;	/*** Disable Encryption ***/
		uselessinquiry();	
		VIDEO_WaitVSync();
		CardError = CARD_Mount(slot, SysArea, NULL);	/*** Don't need or want a callback ***/	
		if ( CardError == 0 )
			return 0;
		else {
			//if ( ISACTIVE[CARDSLOT] )
			if ( ISACTIVE[slot] )
			{
				EXI_ProbeReset();
				ISACTIVE[slot] = 0;
			}
		}
		tries++;
	}

	return 1;
}

int CardFileExists( char *filename, int slot )
{
	int CardError;

	CardError = CARD_FindFirst(slot, &CardDir, TRUE);
        while ( CardError != CARD_ERROR_NOFILE )
        {
              CardError = CARD_FindNext(&CardDir);

              if (strcmp((char *)CardDir.filename, filename) == 0 ) 
			    return 1;
        }

	return 0;
}

/****************************************************************************
 * 0.0.3
 *
 * Pack the contents of mpads and user options for saving
 ****************************************************************************/
void PackInfo()
{
	
	/*** Copy the contents of mpads ***/
	memcpy(&saveinfo, &mpads, 4);

	/*** And add the config values ***/
	saveinfo[0] |= ( Settings.DisableSoundEcho != 0 ? 0x10 : 0 );
	saveinfo[1] |= ( Settings.ReverseStereo != 0 ? 0x10 : 0 );
	saveinfo[2] |= ( Settings.Transparency != 0 ? 0x10 : 0 );
	saveinfo[3] |= ( Settings.DisplayFrameRate != 0 ? 0x10 : 0 );

	memcpy(&savebuffer[sizeof(saveicon) + 64], &saveinfo, 4);
}


void UnpackInfo()
{
	int i;

	memcpy(&saveinfo, &savebuffer[sizeof(saveicon)+64], 4);
	
	Settings.DisableSoundEcho = ( saveinfo[0] >> 4 ) ? 1:0;
	Settings.ReverseStereo = (saveinfo[1] >> 4 ) ? 1:0;
	Settings.Transparency = (saveinfo[2] >> 4) ? 1:0;
	Settings.DisplayFrameRate = (saveinfo[3] >> 4) ? 1:0;

	for ( i = 0; i < 4; i++ )
	{
		saveinfo[i] &= 0xf;

		if ( saveinfo[i] > 3 ) saveinfo[i] = 0;
		
		//padmenu[i+1][16] = PADMap( saveinfo[i], i);
	}		
}

int SaveToCard( int mode, int outbytes, int slot )
{
	char savefilename[128];
	char action[80];
	int CardError;
	unsigned int SectorSize;
	unsigned int blocks;
	char comment[2][32] = { { "Snes9x 1.43 SRAM (GX0.1.0)" }, { "Savegame" } };
	int filesize;
	int sbo;

	/*** Copy in the icon ***/
	memcpy(&savebuffer, &saveicon, sizeof(saveicon));

	/*** Make the save file name ***/
	sprintf(savefilename,"%s.srm", Memory.ROMName);
	sprintf(comment[1], Memory.ROMName);

	/*** Copy comments to savebuffer ***/
	memcpy(&savebuffer[ sizeof(saveicon)], &comment[0], 64);
	outbytes += ( sizeof(saveicon) + 68 );

	/*** Initialise the card ***/
	memset(&SysArea, 0, CARD_WORKAREA);
	//CARD_Init("SNES", "00", FALSE);
	CARD_Init("SNES", "00");
	
	/*** Mount the card ***/
	CardError = MountTheCard(slot);

	/*** Suspected error in libogc. A return of 1 means the card is not mounted completely ***/	
	if ( CardError == 0 )
	{
		/*** Get sector size ***/
		CardError = CARD_GetSectorSize(slot, &SectorSize);		

		switch ( mode )
		{
			case 0:	/*** Load a saved SRAM ***/
				{
				if ( !CardFileExists(savefilename, slot) )
				{	WaitPrompt("No SRAM Save Found");
					return 0;
				}

				memset(&CardFile, 0, sizeof(CardFile));
				CardError = CARD_Open(slot, savefilename, &CardFile);

				blocks = filesize = CardFile.len;

				if ( blocks < SectorSize )
					blocks = SectorSize;

				if ( blocks % SectorSize )
					blocks++;

				/*** Just read the file back in ***/
				sbo = 0;
				int size = blocks;
				while ( blocks > 0 )
				{
					CARD_Read(&CardFile, &savebuffer[sbo], SectorSize, sbo);
					sbo += SectorSize;
					blocks -= SectorSize;
				}

				CARD_Close(&CardFile);

				CARD_Unmount(slot);

				sprintf(action, "Loaded %d bytes successfully", size);
				WaitPrompt(action);

				filesize = Memory.SRAMSize ? ( 1 << (Memory.SRAMSize + 3)) * 128 : 0;
				memcpy(&mpads[0], &savebuffer[sizeof(saveicon)+64], 4);
				if ( filesize )
					memcpy(&Memory.SRAM[0], &savebuffer[sizeof(saveicon)+68], filesize);

				UnpackInfo();

				ISACTIVE[slot] = 1;
				S9xSoftReset();	//Auto reset feature after loading file
				break;
			}
			case 1: /*** Save an SRAM Game ***/
			
				/*** Determine number of blocks on this card ***/
				blocks = ( outbytes / SectorSize ) * SectorSize;
				if ( outbytes % SectorSize )
					blocks += SectorSize;

				if ( CardFileExists(savefilename, slot) ) {
					CardError = CARD_Open(slot, savefilename, &CardFile);
					if ( CardError ) {
						sprintf(action,"Error Open : %d", CardError);
						WaitPrompt(action);
						return 0;
					}
				}
				else
				{			
					/*** Create the save game ***/
					CardError = CARD_Create(slot, savefilename, blocks, &CardFile);

					if ( CardError ) {
						sprintf(action,"Error create : %d %d", CardError, slot);
						WaitPrompt(action);
						return 0;
					}
				}

                CARD_GetStatus( slot, CardFile.filenum, &CardStatus);
                CardStatus.icon_addr = 0x0;
                CardStatus.icon_fmt = 2;
                CardStatus.icon_speed = 1;
                CardStatus.comment_addr = sizeof(saveicon);
                CARD_SetStatus( slot, CardFile.filenum, &CardStatus);

				/*** And write the blocks out ***/
				sbo = 0;
				while( outbytes > 0 )
				{
					CardError = CARD_Write(&CardFile, &savebuffer[sbo], SectorSize, sbo);
					outbytes -= SectorSize;
					sbo += SectorSize;
				}
			
				CARD_Close(&CardFile);
				CARD_Unmount(slot);

				sprintf(action, "Saved %d bytes successfully", blocks);
				WaitPrompt(action);
				ISACTIVE[slot]=1;
				return 1;
			break;
		}		
	} else {
		WaitPrompt("Unable to mount memory card!");
	}
	return 0;
}

/****************************************************************************
 * Write savebuffer to SD card file
 ****************************************************************************/
int SaveBufferToSD (char *filepath, unsigned long datasize)
{
    sd_file *handle;
    
    //SDCARD_Init ();
    
    if (datasize)
    {
        handle = SDCARD_OpenFile (filepath, "wb");
        
        if (handle <= 0)
        {
            char msg[100];
            sprintf(msg, "Couldn't save %s", filepath);
            WaitPrompt (msg);
            return 0;
        }
        
        SDCARD_WriteFile (handle, savebuffer, datasize);
        SDCARD_CloseFile (handle);
    }
    
    return datasize;
}

/****************************************************************************
 * Save SRAM to SD Card
 ****************************************************************************/
int SaveSRAMToSD (int slot, unsigned long datasize)
{
    char filepath[1024];
    //int datasize;
    int offset;

    ShowAction ("Saving SRAM to SD...");    
    sprintf (filepath, "dev%d:\\%s\\%08x.srm", slot, SNESSAVEDIR, Memory.ROMCRC32);

	if (Memory.SRAMSize > 0){
		if ( datasize )
		{
			offset = SaveBufferToSD (filepath, datasize);
			
			if (offset > 0)
			{
				sprintf (filepath, "Saved %d bytes successfully", offset);
				WaitPrompt (filepath);
				return 1;
			}
		}else return 0;
	}
	else{
		sprintf (filepath, "This ROM file has no battery");
		WaitPrompt (filepath);
		return 0;
	}
	return 0;
}

/****************************************************************************
 * Load savebuffer from SD card file
 ****************************************************************************/
int LoadBufferFromSD (char *filepath)
{
    sd_file *handle;
    int offset = 0;
    int read = 0;
    
    //SDCARD_Init ();
    
    handle = SDCARD_OpenFile (filepath, "rb");
    
    if (handle <= 0)
    {        
        char msg[256];
        sprintf(msg, "Couldn't open %s", filepath);
        WaitPrompt (msg);       
        return 0;
    }
    
    //memset (savebuffer, 0, 0x22000);
    
    /*** This is really nice, just load the file and decode it ***/
    while ((read = SDCARD_ReadFile (handle, &savebuffer[offset], 1024)) > 0){ 
		offset += read;
	}    
    SDCARD_CloseFile (handle);
    
    return offset;
}

/****************************************************************************
 * Load SRAM From SD Card
 ****************************************************************************/
int LoadSRAMFromSD (int slot)
{
    char filepath[1024];
    int offset = 0;
    int size = 0;
	
    ShowAction ("Loading SRAM from SD...");    
    sprintf (filepath, "dev%d:\\%s\\%08x.srm", slot, SNESSAVEDIR, Memory.ROMCRC32);
    
    offset = LoadBufferFromSD (filepath);
    
    if (offset > 0)
    {
        sprintf (filepath, "Loaded %d bytes successfully", offset);
        WaitPrompt(filepath);
		
		size = Memory.SRAMSize ? ( 1 << (Memory.SRAMSize + 3)) * 128 : 0;
		
		if ( size > 0x20000 )
			size = 0x20000;
		
		if (offset == (size + 512)){
			memmove (savebuffer, savebuffer + 512, size);
		}
		//memcpy(&Memory.SRAM[0], &savebuffer[sizeof(saveicon)+68], size);
		memcpy (&Memory.SRAM[0], &savebuffer[0], size);
        S9xSoftReset();
		return 1;
    }
	else return 0;
}

int SaveSRAM( int mode, int slot, int type) //{ mode : {0=Save, 1=Load}, slot:{0=SlotA, 1=SlotB}, type:{0=MCard, 1=SDCard} }
{
	
	unsigned long size;
	unsigned long outbytes;
	int result = 0;
	outbytes = 0;
	memset(&savebuffer, 0, sizeof(savebuffer));

	if ( mode == 1 ) //1=Save, 0=Load
	{
		/*** Determine size of SRAM ***/
		size = Memory.SRAMSize ? ( 1 << (Memory.SRAMSize + 3)) * 128 : 0;

		if ( size > 0x20000 )
			size = 0x20000;

		outbytes = size;

		if (outbytes == 0)
			return 0;
		
		if (type == 0){ //Save in MCard else in SDCard
			memcpy(&savebuffer[sizeof(saveicon) + 68], (unsigned char *)Memory.SRAM, outbytes);
			PackInfo();
			result = SaveToCard(1, outbytes, slot);
		}
		else{
			memcpy (savebuffer, Memory.SRAM, outbytes);
			result = SaveSRAMToSD (slot, outbytes);
		}
		
		return result; 
	} else 
		/*** Now do the job of reading the data in ***/
		return (type == 0) ? SaveToCard(0, outbytes, slot) : LoadSRAMFromSD (slot); //Load in MCard:SDCard
}
