/****************************************************************************
 * Snes9x 1.51 Nintendo Wii/Gamecube Port
 *
 * Tantric December 2008
 *
 * networkop.h
 *
 * Network and SMB support routines
 ****************************************************************************/

#ifndef _NETWORKOP_H_
#define _NETWORKOP_H_

void InitializeNetwork(bool silent);
bool ConnectShare (bool silent);
void CloseShare();

#endif
