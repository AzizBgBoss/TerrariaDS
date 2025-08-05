
//{{BLOCK(intro)

//======================================================================
//
//	intro, 256x256@8, 
//	+ palette 256 entries, not compressed
//	+ 335 tiles (t|f reduced) not compressed
//	+ regular map (in SBBs), not compressed, 32x32 
//	Total size: 512 + 21440 + 2048 = 24000
//
//	Time-stamp: 2025-08-05, 20:25:05
//	Exported by Cearn's GBA Image Transmogrifier, v0.9.2
//	( http://www.coranac.com/projects/#grit )
//
//======================================================================

#ifndef GRIT_INTRO_H
#define GRIT_INTRO_H

#define introTilesLen 21440
extern const unsigned int introTiles[5360];

#define introMapLen 2048
extern const unsigned short introMap[1024];

#define introPalLen 512
extern const unsigned short introPal[256];

#endif // GRIT_INTRO_H

//}}BLOCK(intro)
