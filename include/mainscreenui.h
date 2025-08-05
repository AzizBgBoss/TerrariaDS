
//{{BLOCK(mainscreenui)

//======================================================================
//
//	mainscreenui, 256x256@8, 
//	+ palette 256 entries, not compressed
//	+ 65 tiles (t|f reduced) not compressed
//	+ regular map (in SBBs), not compressed, 32x32 
//	Total size: 512 + 4160 + 2048 = 6720
//
//	Time-stamp: 2025-08-05, 20:25:06
//	Exported by Cearn's GBA Image Transmogrifier, v0.9.2
//	( http://www.coranac.com/projects/#grit )
//
//======================================================================

#ifndef GRIT_MAINSCREENUI_H
#define GRIT_MAINSCREENUI_H

#define mainscreenuiTilesLen 4160
extern const unsigned int mainscreenuiTiles[1040];

#define mainscreenuiMapLen 2048
extern const unsigned short mainscreenuiMap[1024];

#define mainscreenuiPalLen 512
extern const unsigned short mainscreenuiPal[256];

#endif // GRIT_MAINSCREENUI_H

//}}BLOCK(mainscreenui)
