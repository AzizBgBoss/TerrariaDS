
//{{BLOCK(von)

//======================================================================
//
//	von, 256x256@8, 
//	+ palette 256 entries, not compressed
//	+ 418 tiles (t|f reduced) not compressed
//	+ regular map (in SBBs), not compressed, 32x32 
//	Total size: 512 + 26752 + 2048 = 29312
//
//	Time-stamp: 2025-08-05, 20:25:07
//	Exported by Cearn's GBA Image Transmogrifier, v0.9.2
//	( http://www.coranac.com/projects/#grit )
//
//======================================================================

#ifndef GRIT_VON_H
#define GRIT_VON_H

#define vonTilesLen 26752
extern const unsigned int vonTiles[6688];

#define vonMapLen 2048
extern const unsigned short vonMap[1024];

#define vonPalLen 512
extern const unsigned short vonPal[256];

#endif // GRIT_VON_H

//}}BLOCK(von)
