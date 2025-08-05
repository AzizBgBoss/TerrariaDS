
//{{BLOCK(mainscreenbg)

//======================================================================
//
//	mainscreenbg, 256x256@8, 
//	+ palette 256 entries, not compressed
//	+ 508 tiles (t|f reduced) not compressed
//	+ regular map (in SBBs), not compressed, 32x32 
//	Total size: 512 + 32512 + 2048 = 35072
//
//	Time-stamp: 2025-08-05, 20:25:06
//	Exported by Cearn's GBA Image Transmogrifier, v0.9.2
//	( http://www.coranac.com/projects/#grit )
//
//======================================================================

#ifndef GRIT_MAINSCREENBG_H
#define GRIT_MAINSCREENBG_H

#define mainscreenbgTilesLen 32512
extern const unsigned int mainscreenbgTiles[8128];

#define mainscreenbgMapLen 2048
extern const unsigned short mainscreenbgMap[1024];

#define mainscreenbgPalLen 512
extern const unsigned short mainscreenbgPal[256];

#endif // GRIT_MAINSCREENBG_H

//}}BLOCK(mainscreenbg)
