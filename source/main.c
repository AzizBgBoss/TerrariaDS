/*---------------------------------------------------------------------------------

Terraria like game for the Nintendo DS
AzizBgBoss - https://github.com/AzizBgBoss

---------------------------------------------------------------------------------*/

/*
TODO: We're making the game more like terraria desktop.

We're gonna start with Terraria 0.1:
V    Dirt
V    Stone
V    Trees
	Basic Tools and Wooden Weapons
	Clouds
	Tooltips
	Mushrooms
	Damage Numbers
	Splitting Stacks
	FPS Meter (Unnecessary)
*/

#include <nds.h>
#include <stdio.h>
#include <math.h>
#include <fat.h>
#include <maxmod9.h>
#include <stdlib.h>
#include <time.h>

#include "tilemap.h"
#include "bg.h"
#include "sprites.h"
#include "inv.h"
#include "font.h"
#include "ui.h"
#include "von.h"
#include "soundbank.h"
#include "soundbank_bin.h"

#define MAP_WIDTH 512
#define MAP_HEIGHT 32

#define MAX_ITEMS 16 // Maximum items to be rendered

// Define game elements
#define TILE_AIR 0
#define TILE_GRASS 1
#define TILE_DIRT 2
#define TILE_STONE 3
#define TILE_WOODLOG 4
#define TILE_LEAVES 5
#define TILE_MUSHROOM 6

#define ITEM_PICKAXE 101
#define ITEM_AXE 102
#define ITEM_SWORD 103
#define ITEM_HAMMER 104

// Define animation
#define ANIM_NONE 0
#define ANIM_WALK 1
#define ANIM_JUMP 2

int frame = 0;
int scrollX = 0;
int scrollY = 0;
int scale = 256; // 128 is x2, 256 is x1, 512 is x0.5
int chunk = 0;

u8 gameTerrain[MAP_WIDTH * MAP_HEIGHT] = {0};
int gameTerrainHealth[MAP_WIDTH * MAP_HEIGHT] = {0};

u8 inventory[8 * 4] = {0};
u8 inventoryQuantity[8 * 4] = {0};
u8 inventorySelection = 0;

u16 *inventorySelectionSprite;
u16 *itemHandSprite;

typedef struct
{
	int x, y; // Player position
	int renderX, renderY;
	int anim_frame;		 // Animation frame
	u16 *sprite_gfx_mem; // Pointer to sprite graphics memory
	int sizeX, sizeY;	 // Size of the sprite, may be used for collision detection
	bool isJumping;		 // Is the player jumping
	bool isSolid;		 // Is the player solid (can collide with other objects)
	int weight;			 // Zero if not affected by gravity
	int velocity;		 // Velocity for jumping or falling
	bool isOnGround;	 // Is the player on the ground
	bool isLookingLeft;
	int tileRange;
	u8 animation;
} Player;

typedef struct
{
	int x, y;
	int renderX, renderY;
	u16 *sprite_gfx_mem;
	int sizeX, sizeY;
	bool exists;
	int tile;
	u8 quantity;
	int velocity;
} Item;

typedef struct
{
	int x, y;
	int renderX, renderY;
	u16 *sprite_gfx_mem;
} Sprite;

// Define the player entity
Player player = {0, 0, 0, 0, 0, NULL, 32, 48, false, true, 1, 0, true, false, 4, ANIM_NONE};

// Define 64 slots for item entities
Item item[64] = {{0, 0, 0, 0, NULL, 8, 8, false, 60, 0, 0}};

Sprite destructionSprite = {0, 0, 0, 0, NULL};

u16 *bg2Map;

void Bg1SetTile(int x, int y, int tile)
{
	bg2Map[x + y * 64] = tile;
}

void Bg1UpSetTile(int x, int y, int tile)
{
	BG_MAP_RAM(2)
	[x + y * 32] = tile;
}

void Bg3UpSetTile(int x, int y, int tile)
{
	BG_MAP_RAM(3)
	[x + y * 32] = tile;
}

void print(int x, int y, const char *text)
{
	while (*text)
	{
		if (*text == '\n') // If we reach a newline character, move to the next line
		{
			x = 0;							// Reset x to the beginning of the line
			y++;							// Move to the next line
			if (y >= SCREEN_HEIGHT / 8 - 1) // If we reach the bottom of the screen, stop printing
				return;
			text++;
			continue; // Skip the newline character
		}
		Bg3UpSetTile(x, y, (char)(*text - 8 * 4));
		x++;
		if (x >= SCREEN_WIDTH / 8 - 1)
		{
			x = 0;
			y++;
		}
		text++;
	}
}

static int clamp(int val, int min, int max)
{
	if (val < min)
		return min;
	if (val > max)
		return max;
	return val;
}

int getElementTile(int tile)
{
	switch (tile)
	{
	case TILE_GRASS:
		return 0;
	case TILE_DIRT:
		return 4;
	case TILE_STONE:
		return 8;
	case TILE_WOODLOG:
		return 12;
	case TILE_MUSHROOM:
		return 80;
	case TILE_LEAVES:
		return 64;
	case ITEM_PICKAXE:
		return 16;
	case ITEM_SWORD:
		return 68;
	case ITEM_AXE:
		return 72;
	case ITEM_HAMMER:
		return 76;
	default:
		return 60;
	}
}

char *getElementName(int element)
{
	switch (element)
	{
	case TILE_GRASS:
		return "Grass";
	case TILE_DIRT:
		return "Dirt";
	case TILE_STONE:
		return "Stone";
	case TILE_WOODLOG:
		return "Wood Log";
	case TILE_MUSHROOM:
		return "Mushroom";
	case TILE_LEAVES:
		return "Leaves";
	case ITEM_PICKAXE:
		return "Pickaxe";
	case ITEM_SWORD:
		return "Sword";
	case ITEM_AXE:
		return "Axe";
	case ITEM_HAMMER:
		return "Hammer";
	default:
		return "";
	}
}

int getElementHealth(int element)
{
	switch (element)
	{
	case TILE_GRASS:
		return 100;
	case TILE_DIRT:
		return 100;
	case TILE_STONE:
		return 1000;
	case TILE_WOODLOG:
		return 500;
	case TILE_LEAVES:
		return 50;
	case TILE_MUSHROOM:
		return 10;
	default:
		return 0;
	}
}

bool isTileSolid(int tile)
{
	switch (tile)
	{
	case TILE_GRASS:
	case TILE_DIRT:
	case TILE_STONE:
		return true;
	default:
		return false;
	}
}

void setGameTerrain(int x, int y, int tile)
{
	if (x < 0 || x >= MAP_WIDTH || y < 0 || y >= MAP_HEIGHT)
		return;

	gameTerrain[x + y * MAP_WIDTH] = tile;

	if (x < chunk * 4 || x > chunk * 4 + 31)
		return;

	x = x % 32;
	Bg1SetTile(x * 2, y * 2, getElementTile(tile) + 0);
	Bg1SetTile(x * 2 + 1, y * 2, getElementTile(tile) + 1);
	Bg1SetTile(x * 2, y * 2 + 1, getElementTile(tile) + 2);
	Bg1SetTile(x * 2 + 1, y * 2 + 1, getElementTile(tile) + 3);
}

void setInventory(int slot, int item, int quantity)
{
	if (slot < 0 || slot >= 8 * 4)
		return;
	int x = (slot % 8) * 4 + 1;
	int y = (slot / 8) * -4 + 21;
	if (item == 0 || quantity == 0)
	{
		inventory[slot] = 0;
		inventoryQuantity[slot] = 0;
		Bg1UpSetTile(x, y, 63);
		Bg1UpSetTile(x + 1, y, 63);
		Bg1UpSetTile(x, y + 1, 63);
		Bg1UpSetTile(x + 1, y + 1, 63);
		print(x, y + 2, "   ");
		return;
	}
	inventory[slot] = item;
	inventoryQuantity[slot] = quantity;
	Bg1UpSetTile(x, y, getElementTile(item) + 0);
	Bg1UpSetTile(x + 1, y, getElementTile(item) + 1);
	Bg1UpSetTile(x, y + 1, getElementTile(item) + 2);
	Bg1UpSetTile(x + 1, y + 1, getElementTile(item) + 3);
	print(x, y + 2, "   ");
	if (quantity == 1)
		return;
	char buffer[3];
	itoa(quantity, buffer, 10);
	print(x, y + 2, buffer);
}

bool giveInventory(int item, int quantity)
{
	for (int i = 0; i < 8 * 4; i++)
	{
		if (inventory[i] == item && inventoryQuantity[i] < 99)
		{
			setInventory(i, item, inventoryQuantity[i] + quantity);
			return true;
		}
	}
	for (int i = 0; i < 8 * 4; i++)
	{
		if (inventory[i] == 0)
		{
			setInventory(i, item, quantity);
			return true;
		}
	}
	return false;
}

void setInventorySelection(u8 slot)
{
	inventorySelection = slot;
	int x = (slot % 8) * 4 * 8;
	int y = ((slot / 8) * -4 + 20) * 8;
	print(1, 7, "                ");
	print(1, 7, getElementName(inventory[slot]));
	oamSet(&oamMain, 0, x, y, 0, 0, SpriteSize_32x32, SpriteColorFormat_256Color, inventorySelectionSprite, -1, false, false, false, false, false);
	oamUpdate(&oamMain);
}

void dropItem(int x, int y, int tile, int quantity)
{
	int index = 0;
	while (item[index].exists)
		index++;
	// we can do item[index] = {...} directly but it'll get confusing
	item[index].x = x * 16 + 4; // Drop in the middle of the tile
	item[index].y = y * 16 + 4;
	item[index].exists = true;
	item[index].tile = tile;
	item[index].quantity = quantity;

	dmaCopy(tilemapTiles + 4 * 4 * getElementTile(tile), item[index].sprite_gfx_mem, 8 * 8);
}

void destroyItem(int id)
{
	item[id].exists = false;
	dmaCopy(tilemapTiles + 4 * 4 * getElementTile(0), item[id].sprite_gfx_mem, 8 * 8);
}

void breakTile(int x, int y, int speed)
{
	gameTerrainHealth[x + y * MAP_WIDTH] += speed;
	for (int i = 0; i < 10; i++)
	{
		if (gameTerrainHealth[x + y * MAP_WIDTH] - 1 == getElementHealth(gameTerrain[x + y * MAP_WIDTH]) / 10 * i)
		{
			dmaCopy(tilemapTiles + 8 * 8 * (i + 5), destructionSprite.sprite_gfx_mem, 16 * 16);
		}
	}
	destructionSprite.x = x * 16;
	destructionSprite.y = y * 16;
	if (gameTerrainHealth[x + y * MAP_WIDTH] >= getElementHealth(gameTerrain[x + y * MAP_WIDTH]))
	{
		dmaCopy(tilemapTiles + 8 * 8 * 15, destructionSprite.sprite_gfx_mem, 16 * 16);
		dropItem(x, y, gameTerrain[x + y * MAP_WIDTH], 1);
		setGameTerrain(x, y, 0);
		gameTerrainHealth[x + y * MAP_WIDTH] = 0;
	}
}

int rando(int min, int max)
{
	return rand() % (max - min + 1) + min;
}

bool saveMapToFile(const char *filename)
{
	FILE *file = fopen(filename, "wb"); // "wb" = write binary
	if (!file)
	{
		print(0, 0, "Failed to open file for writing: map.dat");
		return false;
	}

	print(0, 0, "Saving map...");
	size_t bytesWritten = fwrite(gameTerrain, 1, sizeof(gameTerrain), file);
	print(0, 0, "Map saved to map.dat");
	fclose(file);
	print(0, 0, "File closed");

	if (bytesWritten != sizeof(gameTerrain))
	{
		print(0, 0, "Map save error");
		return false;
	}

	print(0, 0, "Map saved to map.dat");
	return true;
}

bool loadMapFromFile(const char *filename)
{
	chunk = -6;
	FILE *file = fopen(filename, "rb"); // "rb" = read binary
	if (!file)
	{
		print(0, 0, "Failed to open file for reading: map.dat");
		return false;
	}

	size_t bytesRead = fread(gameTerrain, 1, sizeof(gameTerrain), file);
	fclose(file);

	if (bytesRead != sizeof(gameTerrain))
	{
		print(0, 0, "Map load error");
		return false;
	}

	print(0, 0, "Map loaded from map.dat");
	return true;
}

// Hash function
int hash(int x)
{
	x = (x << 13) ^ x;
	return (x * (x * x * 15731 + 789221) + 1376312589) & 0x7fffffff;
}

// Linear interpolation
float lerp(float a, float b, float t)
{
	return a + t * (b - a);
}

// Fade curve (Perlin-style smoothing function)
float fade(float t)
{
	return t * t * t * (t * (t * 6 - 15) + 10);
}

// Gradient function
float grad(int hash, float x)
{
	// Use last bit to determine gradient direction
	return (hash & 1) == 0 ? x : -x;
}

// 1D Perlin noise
float perlin1d(float x)
{
	int xi = (int)x;
	float xf = x - xi;

	int h0 = hash(xi);
	int h1 = hash(xi + 1);

	float g0 = grad(h0, xf);
	float g1 = grad(h1, xf - 1.0f);

	float u = fade(xf);

	return lerp(g0, g1, u); // Returns value in range ~[-1, 1]
}

float perlin2d(float x, float y)
{
	int xi = (int)x;
	int yi = (int)y;
	float xf = x - xi;
	float yf = y - yi;

	int h00 = hash(xi + hash(yi));
	int h01 = hash(xi + hash(yi + 1));
	int h10 = hash(xi + 1 + hash(yi));
	int h11 = hash(xi + 1 + hash(yi + 1));

	float g00 = grad(h00, xf) + grad(h00, yf);
	float g01 = grad(h01, xf) + grad(h01, yf - 1.0f);
	float g10 = grad(h10, xf - 1.0f) + grad(h10, yf);
	float g11 = grad(h11, xf - 1.0f) + grad(h11, yf - 1.0f);

	float u = fade(xf);
	float v = fade(yf);

	return lerp(lerp(g00, g10, u), lerp(g01, g11, u), v); // Returns value in range ~[-2, 2]
}

float fractalPerlin1D(float x, int octaves, float persistence, float scale)
{
	float total = 0;
	float frequency = scale;
	float amplitude = 1;
	float maxAmplitude = 0;

	for (int i = 0; i < octaves; i++)
	{
		total += perlin1d(x * frequency) * amplitude;
		maxAmplitude += amplitude;

		amplitude *= persistence;
		frequency *= 2.0f;
	}

	return total / maxAmplitude; // normalize to -1..1
}

float fractalPerlin2D(float x, float y, int octaves, float persistence, float scale)
{
	float total = 0;
	float frequency = scale;
	float amplitude = 1;
	float maxAmplitude = 0;

	for (int i = 0; i < octaves; i++)
	{
		total += perlin2d(x * frequency, y * frequency) * amplitude;
		maxAmplitude += amplitude;

		amplitude *= persistence;
		frequency *= 2.0f;
	}

	return total / maxAmplitude; // normalize to -1..1
}

#define TREE_CHANCE 20
#define MUSHROOM_CHANCE 10
#define MIN_GRASS_HEIGHT 12
#define MAX_GRASS_HEIGHT 20
#define MIN_STONE_HEIGHT 17
#define MAX_STONE_HEIGHT 25

void generateMap()
{
	u8 grassSurface[MAP_WIDTH];
	u8 stoneSurface[MAP_WIDTH];

	// Generate grass height surface
	for (int x = 0; x < MAP_WIDTH; x++)
	{
		float wave = fractalPerlin1D(x, 4, 0.4f, 0.01f) * 20.0f;

		grassSurface[x] = clamp((int)(wave + MAP_HEIGHT / 2), MIN_GRASS_HEIGHT, MAX_GRASS_HEIGHT);
	}

	// remove 1 block spikes, i fucking hate them
	for (int x = 1; x < MAP_WIDTH - 1; x++)
	{
		if (grassSurface[x - 1] != grassSurface[x] && grassSurface[x + 1] != grassSurface[x])
		{
			grassSurface[x] = 0;
		}
	}

	// Generate stone height surface
	for (int x = 0; x < MAP_WIDTH; x++)
	{
		float wave = sinf((x + 100) / 7.0f) * 3; // different offset + scale
		stoneSurface[x] = clamp((int)(wave + MAP_HEIGHT / 2 + 5), MIN_STONE_HEIGHT, MAX_STONE_HEIGHT);
	}

	// Place terrain
	for (int x = 0; x < MAP_WIDTH; x++)
	{
		for (int y = 0; y < MAP_HEIGHT; y++)
		{
			if (y == grassSurface[x])
			{
				setGameTerrain(x, y, TILE_GRASS);
			}
			else if (y > grassSurface[x] && y <= stoneSurface[x])
			{
				setGameTerrain(x, y, TILE_DIRT);
			}
			else if (y > stoneSurface[x])
			{
				setGameTerrain(x, y, TILE_STONE);
			}
		}
	}

	// Place trees
	for (int x = 1; x < MAP_WIDTH - 1; x++)
	{
		if (rando(0, TREE_CHANCE) == 0)
		{
			int tree_height = rando(3, 5);
			// Tree trunk
			for (int i = 0; i < tree_height; i++)
				setGameTerrain(x, grassSurface[x] - 1 - i, TILE_WOODLOG);

			// Tree leaves (3x3 above trunk)
			for (int i = -1; i <= 1; i++)
			{
				for (int j = 0; j < 3; j++)
				{
					int leafX = x + i;
					int leafY = grassSurface[x] - tree_height - 1 - j;
					setGameTerrain(leafX, leafY, TILE_LEAVES); // No need for bounds check, the setGameTerrain function handles it fine twin <3
				}
			}
		}
	}

	// Place mushrooms
	for (int x = 1; x < MAP_WIDTH - 1; x++)
	{
		if (rando(0, MUSHROOM_CHANCE) == 0 && gameTerrain[x + (grassSurface[x] - 1) * MAP_WIDTH] == 0)
		{
			setGameTerrain(x, grassSurface[x] - 1, TILE_MUSHROOM);
		}
	}
}

//---------------------------------------------------------------------------------
int main(void)
{
	scanKeys();
	if (!fatInitDefault() || keysHeld() & KEY_START)
	{
		mmInitDefaultMem((mm_addr)soundbank_bin);
		mmLoad(MOD_MODULE1);

		mmStart(MOD_MODULE1, MM_PLAY_LOOP);

		videoSetMode(MODE_0_2D | DISPLAY_BG0_ACTIVE);
		vramSetBankA(VRAM_A_MAIN_BG);
		BGCTRL[0] = BG_TILE_BASE(1) | BG_MAP_BASE(0) | BG_COLOR_256 | BG_32x32 | BG_PRIORITY(3);
		dmaCopy(vonTiles, (void *)CHAR_BASE_BLOCK(1), vonTilesLen);
		dmaCopy(vonMap, (void *)SCREEN_BASE_BLOCK(0), vonMapLen);
		dmaCopy(vonPal, BG_PALETTE, vonPalLen);
		consoleDemoInit();
		iprintf("Greetings, twin. FatInitDefault hath failed, dear twin.\n\
Fear not, for this plight we shall overcome.\n\
If thou usest a DS cartridge, ensure that thou applyest the proper DLDI patch suited to thy cartridge's nature.\n\
If thou art upon a DSi, no need to patch anything; perchance thy SD card be corrupted, dear twin.\n\
Or, if thou playest on an emulator, be sure that SD emulation is enabled, for most such emulators do patch DLDI of their own accord.");

		while (pmMainLoop())
		{
			swiWaitForVBlank();
		}
		return 1;
	}

	srand(time(NULL));

	touchPosition touch;

	videoSetMode(MODE_0_2D | DISPLAY_BG0_ACTIVE | DISPLAY_BG1_ACTIVE | DISPLAY_BG3_ACTIVE);
	videoSetModeSub(MODE_5_2D | DISPLAY_BG0_ACTIVE | DISPLAY_BG2_ACTIVE);

	vramSetBankA(VRAM_A_MAIN_BG);
	vramSetBankB(VRAM_B_MAIN_SPRITE);
	vramSetBankC(VRAM_C_SUB_BG);	 // for backgrounds on subscreen
	vramSetBankD(VRAM_D_SUB_SPRITE); // for sprites on subscreen

	BGCTRL[0] = BG_TILE_BASE(1) | BG_MAP_BASE(0) | BG_COLOR_256 | BG_32x32 | BG_PRIORITY(3);
	dmaCopy(invTiles, (void *)CHAR_BASE_BLOCK(1), invTilesLen);
	dmaCopy(invMap, (void *)SCREEN_BASE_BLOCK(0), invMapLen);
	BGCTRL[1] = BG_TILE_BASE(2) | BG_MAP_BASE(2) | BG_COLOR_256 | BG_32x32 | BG_PRIORITY(2);
	dmaCopy(tilemapTiles, (void *)CHAR_BASE_BLOCK(2), tilemapTilesLen);
	dmaFillHalfWords(63, (void *)SCREEN_BASE_BLOCK(2), 2048);
	BGCTRL[3] = BG_TILE_BASE(3) | BG_MAP_BASE(3) | BG_COLOR_256 | BG_32x32 | BG_PRIORITY(0);
	dmaCopy(fontTiles, (void *)CHAR_BASE_BLOCK(3), fontTilesLen);
	dmaFillHalfWords(0, (void *)SCREEN_BASE_BLOCK(3), 2048);

	dmaCopy(invPal, BG_PALETTE, invPalLen);

	BGCTRL_SUB[0] = BG_TILE_BASE(1) | BG_MAP_BASE(0) | BG_COLOR_256 | BG_32x32 | BG_PRIORITY(3);
	dmaCopy(bgTiles, (void *)CHAR_BASE_BLOCK_SUB(1), bgTilesLen);
	dmaCopy(bgMap, (void *)SCREEN_BASE_BLOCK_SUB(0), bgMapLen);

	int bg2 = bgInitSub(2, BgType_ExRotation, BgSize_ER_512x512, 3, 3);
	bgWrapOn(bg2);
	bgSetPriority(bg2, 2);
	bgSetScale(bg2, 256, 256);
	dmaCopy(tilemapTiles, bgGetGfxPtr(bg2), tilemapTilesLen);
	// Fill with empty tiles
	dmaFillHalfWords(63, bgGetMapPtr(bg2), 64 * 64 * 2);

	bg2Map = (u16 *)bgGetMapPtr(bg2);

	dmaCopy(tilemapPal, BG_PALETTE_SUB, tilemapPalLen);

	oamInit(&oamMain, SpriteMapping_1D_128, false);

	inventorySelectionSprite = oamAllocateGfx(&oamMain, SpriteSize_32x32, SpriteColorFormat_256Color);
	dmaCopy(uiTiles, inventorySelectionSprite, uiTilesLen);

	dmaCopy(uiPal, SPRITE_PALETTE, uiPalLen);

	oamInit(&oamSub, SpriteMapping_1D_128, false);

	u16 *nullSprite = oamAllocateGfx(&oamSub, SpriteSize_16x16, SpriteColorFormat_256Color);
	dmaFillHalfWords(0, nullSprite, 16 * 16);

	player.sprite_gfx_mem = oamAllocateGfx(&oamSub, SpriteSize_32x64, SpriteColorFormat_256Color);
	dmaCopy(spritesTiles, player.sprite_gfx_mem, 32 * 64);

	itemHandSprite = oamAllocateGfx(&oamSub, SpriteSize_16x16, SpriteColorFormat_256Color);
	dmaCopy(tilemapTiles + 8 * 8 * 4, itemHandSprite, 16 * 16);

	for (int i = 0; i < MAX_ITEMS; i++)
	{
		item[i].sprite_gfx_mem = oamAllocateGfx(&oamSub, SpriteSize_8x8, SpriteColorFormat_256Color);
		// No need to copy anything, we'll do that later when we summon the item
	}

	destructionSprite.sprite_gfx_mem = oamAllocateGfx(&oamSub, SpriteSize_16x16, SpriteColorFormat_256Color);

	dmaCopy(spritesPal, SPRITE_PALETTE_SUB, spritesPalLen);

	generateMap();

	giveInventory(ITEM_SWORD, 1);
	giveInventory(ITEM_AXE, 1);
	giveInventory(ITEM_PICKAXE, 1);
	setInventorySelection(0);

	print(0, 0, "Terraria DS version 0.1alpha\nBy AzizBgBoss\nhttps://github.com/AzizBgBoss/TerrariaDS\n\nPress A to jump\nPress L and R to switch items");

	while (pmMainLoop())
	{
		swiWaitForVBlank();

		scanKeys();
		int pressed = keysDown();
		int held = keysHeld();
		if (pressed & KEY_START)
			saveMapToFile("map.dat");
		if (pressed & KEY_SELECT)
			loadMapFromFile("map.dat");

		if (pressed & KEY_R)
		{
			if (inventorySelection < 8 * 4 - 1)
				setInventorySelection(inventorySelection + 1);
			else
				setInventorySelection(0);
		}

		if (pressed & KEY_L)
		{
			if (inventorySelection > 0)
				setInventorySelection(inventorySelection - 1);
			else
				setInventorySelection(8 * 4 - 1);
		}

		if (held & KEY_X)
			scale++;
		if (held & KEY_Y)
			scale--;

		if (scale > 512)
			scale = 512; // Max scale (0.5x)
		if (scale < 192)
			scale = 192; // Min scale (1.5x)

		// Draw player sprite
		if (pressed & KEY_A)
		{
			// Jumping logic
			if (player.isOnGround)
			{
				player.isJumping = true;
				player.velocity = -10; // Set a negative velocity for jumping
				player.isOnGround = false;
				player.animation = ANIM_JUMP;
			}
		}

		if (held & KEY_RIGHT)
		{
			if ((player.x + player.sizeX) / 16 < MAP_WIDTH)
			{
				if (!isTileSolid(gameTerrain[(player.x + player.sizeX) / 16 + player.y / 16 * MAP_WIDTH]) &&
					!isTileSolid(gameTerrain[(player.x + player.sizeX) / 16 + (player.y + player.sizeY / 2) / 16 * MAP_WIDTH]) &&
					!isTileSolid(gameTerrain[(player.x + player.sizeX) / 16 + (player.y + player.sizeY - 1) / 16 * MAP_WIDTH]))
				{
					player.isLookingLeft = false;
					player.x++;
					player.animation = ANIM_WALK;
				}
			}
		}
		else if (held & KEY_LEFT)
		{
			if (player.x - 1 >= 0)
			{
				if (!isTileSolid(gameTerrain[(player.x - 1) / 16 + player.y / 16 * MAP_WIDTH]) &&
					!isTileSolid(gameTerrain[(player.x - 1) / 16 + (player.y + player.sizeY / 2) / 16 * MAP_WIDTH]) &&
					!isTileSolid(gameTerrain[(player.x - 1) / 16 + (player.y + player.sizeY - 1) / 16 * MAP_WIDTH]))
				{
					player.isLookingLeft = true;
					player.x--;
					player.animation = ANIM_WALK;
				}
			}
		}
		else
			player.animation = ANIM_NONE;

		if (!player.isOnGround)
		{
			if (isTileSolid(gameTerrain[player.x / 16 + (player.y + player.velocity) / 16 * MAP_WIDTH]))
			{
				player.velocity = 0;
				player.y /= 16;
				player.y *= 16;
			}
			if (isTileSolid(gameTerrain[(player.x + player.sizeX - 1) / 16 + (player.y + player.velocity) / 16 * MAP_WIDTH]))
			{
				player.velocity = 0;
				player.y /= 16;
				player.y *= 16;
			}
		}

		if (!player.isOnGround)
		{
			player.velocity += player.weight; // Apply gravity
			player.y += player.velocity;
			player.animation = ANIM_JUMP; // Fall and jump look the same
		}

		// Collision detection with ground
		int TLCx = player.x;
		int TLCy = player.y;
		int TRCx = player.x + player.sizeX - 1;
		int TRCy = player.y;
		int BLCx = player.x;
		int BLCy = player.y + player.sizeY;
		int BRCx = player.x + player.sizeX - 1;
		int BRCy = player.y + player.sizeY;

		int TLCtileX = TLCx / 16;
		int TLCtileY = TLCy / 16;
		int TRCtileX = TRCx / 16;
		int TRCtileY = TRCy / 16;
		int BLCtileX = BLCx / 16;
		int BLCtileY = BLCy / 16;
		int BRCtileX = BRCx / 16;
		int BRCtileY = BRCy / 16;

		if (held & KEY_TOUCH)
		{
			touchRead(&touch);
			char buffer[4];
			itoa(touch.px, buffer, 10);
			print(0, 0, buffer);
			itoa(touch.py, buffer, 10);
			print(0, 1, buffer);
			int worldX = scrollX + ((touch.px * 256) / scale);
			int worldY = scrollY + ((touch.py * 256) / scale);

			int worldTouchX = worldX / 16;
			int worldTouchY = worldY / 16;	

			itoa(worldTouchX, buffer, 10);
			print(0, 2, buffer);
			itoa(worldTouchY, buffer, 10);
			print(0, 3, buffer);
			// Check if touch within player range
			if (worldTouchX >= TLCtileX - player.tileRange && worldTouchX <= TRCtileX + player.tileRange &&
				worldTouchY >= TLCtileY - player.tileRange && worldTouchY <= BRCtileY + player.tileRange)
			{
				if (inventory[inventorySelection] >= 1 && inventory[inventorySelection] < 100 // Object is a tile, not an item
					&& inventoryQuantity[inventorySelection])
				{
					if (!gameTerrain[worldTouchX + worldTouchY * MAP_WIDTH])
					{
						if (!(worldTouchX >= TLCtileX && worldTouchX <= TRCtileX && worldTouchY >= TLCtileY && worldTouchY <= BLCtileY))
						{
							setGameTerrain(worldTouchX, worldTouchY, inventory[inventorySelection]);
							inventoryQuantity[inventorySelection]--;
							setInventory(inventorySelection, inventory[inventorySelection], inventoryQuantity[inventorySelection]);
						}
					}
				}
				else if (inventory[inventorySelection] >= 100 && inventory[inventorySelection] < 200 // Object is an item, not a tile
						 && inventoryQuantity[inventorySelection])
				{
					if (inventory[inventorySelection] == ITEM_PICKAXE)
					{
						if (gameTerrain[worldTouchX + worldTouchY * MAP_WIDTH])
							breakTile(worldTouchX, worldTouchY, 1);
					}
				}
			}
		}

		// Check if player is on the ground
		if (isTileSolid(gameTerrain[BLCtileX + BLCtileY * MAP_WIDTH]))
		{
			player.isOnGround = true;
			player.isJumping = false;
			player.velocity = 0; // Reset velocity when on the ground
			player.y = BLCtileY * 16 - player.sizeY;
		}

		if (isTileSolid(gameTerrain[BRCtileX + BRCtileY * MAP_WIDTH]))
		{
			player.isOnGround = true;
			player.isJumping = false;
			player.velocity = 0; // Reset velocity when on the ground
			player.y = BRCtileY * 16 - player.sizeY;
		}

		if (!isTileSolid(gameTerrain[BLCtileX + BLCtileY * MAP_WIDTH]) && !isTileSolid(gameTerrain[BRCtileX + BRCtileY * MAP_WIDTH]))
		{
			player.isOnGround = false;
			player.isJumping = true;
		}

		if (player.x >= SCREEN_WIDTH / 2 - player.sizeX / 2 && player.x < MAP_WIDTH * 16 - SCREEN_WIDTH / 2 - player.sizeX / 2)
			scrollX = player.x - SCREEN_WIDTH / 2 + player.sizeX / 2;
		else if (player.x >= MAP_WIDTH * 16 - SCREEN_WIDTH / 2 - player.sizeX / 2)
			scrollX = MAP_WIDTH * 16 - SCREEN_WIDTH;
		else
			scrollX = 0;

		if (player.y >= SCREEN_HEIGHT / 2 - player.sizeY / 2 && player.y < MAP_HEIGHT * 16 - SCREEN_HEIGHT / 2 - player.sizeY / 2)
			scrollY = player.y - SCREEN_HEIGHT / 2 + player.sizeY / 2;
		else if (player.y >= MAP_HEIGHT * 16 - SCREEN_HEIGHT / 2 - player.sizeY / 2)
			scrollY = MAP_HEIGHT * 16 - SCREEN_HEIGHT;
		else
			scrollY = 0;

		for (int i = 0; i < 64; i++)
		{
			if (item[i].exists)
			{
				// Item falls until it hits a tile
				if (!isTileSolid(gameTerrain[item[i].x / 16 + (item[i].y + 8 + item[i].velocity) / 16 * MAP_WIDTH]))
				{
					item[i].velocity++;
				}
				else
				{
					item[i].velocity = 0;
				}
				// If item touches the player, remove it and set it to his inventory
				if (item[i].x >= player.x && item[i].x < player.x + player.sizeX && item[i].y >= player.y && item[i].y < player.y + player.sizeY)
				{
					if (giveInventory(item[i].tile, item[i].quantity))
						destroyItem(i); // Only destroy item entity if the player has enough inventory space
				}

				// Check for close items to fuse together to not waste memory
				for (int j = 0; j < 64; j++)
				{
					if (i == j)
						continue;
					if (item[i].exists && item[j].exists && item[j].tile == item[i].tile && item[j].x >= item[i].x - 32 && item[j].x < item[i].x + 32 && item[j].y >= item[i].y - 32 && item[j].y < item[i].y + 32)
					{
						item[i].quantity += item[j].quantity;
						destroyItem(j);
					}
				}

				item[i].y += item[i].velocity;

				item[i].renderX = item[i].x - scrollX;
				item[i].renderY = item[i].y - scrollY;
			}
		}

		destructionSprite.renderX = destructionSprite.x - scrollX;
		destructionSprite.renderY = destructionSprite.y - scrollY;

		// Apply scroll
		bgSetScroll(bg2, scrollX, scrollY);
		bgSetScale(bg2, scale, scale);
		REG_BG0HOFS_SUB = scrollX / 8;
		REG_BG0VOFS_SUB = scrollY / 8;

		// Compute screen-relative render coordinates
		player.renderX = player.x - scrollX;
		player.renderY = player.y - scrollY;

		// Animate player
		switch (player.animation)
		{
		case ANIM_NONE:
			player.anim_frame = 0;
			break;
		case ANIM_WALK:
			player.anim_frame = (frame / 4) % 8 + 1;
			break;
		case ANIM_JUMP:
			player.anim_frame = 9;
			break;
		}

		char buffer[4];
		itoa(player.x / 16, buffer, 10);
		print(27, 0, buffer);
		itoa(player.y / 16, buffer, 10);
		print(27, 1, buffer);
		itoa(chunk, buffer, 10);
		print(27, 2, buffer);
		itoa(player.x, buffer, 10);
		print(27, 3, buffer);

		// When the player reaches the 3rd quarter of the background, swap the first 4 rows in the bg with the next 4 rows in the map
		// so when it wraps it gives the illusion of continuity
		if (player.x > 512 / 8 * 5 + chunk * 64)
		{
			if (chunk < MAP_WIDTH / 4 - 1)
			{
				int newTile = (chunk % 8) * 8;
				for (int x = 0; x < 4; x++)
				{
					for (int y = 0; y < 32; y++)
					{
						Bg1SetTile(newTile + x * 2, y * 2, getElementTile(gameTerrain[32 + chunk * 4 + x + y * MAP_WIDTH]) + 0);
						Bg1SetTile(newTile + x * 2 + 1, y * 2, getElementTile(gameTerrain[32 + chunk * 4 + x + y * MAP_WIDTH]) + 1);
						Bg1SetTile(newTile + x * 2, y * 2 + 1, getElementTile(gameTerrain[32 + chunk * 4 + x + y * MAP_WIDTH]) + 2);
						Bg1SetTile(newTile + x * 2 + 1, y * 2 + 1, getElementTile(gameTerrain[32 + chunk * 4 + x + y * MAP_WIDTH]) + 3);
					}
				}
				chunk++;
			}
		}
		else if (player.x < 512 / 8 * 3 + chunk * 64)
		{
			if (chunk > 0)
			{
				chunk--;
				int newTile = (chunk % 8) * 8;
				for (int x = 0; x < 4; x++)
				{
					for (int y = 0; y < 32; y++)
					{
						Bg1SetTile(newTile + x * 2, y * 2, getElementTile(gameTerrain[chunk * 4 + x + y * MAP_WIDTH]) + 0);
						Bg1SetTile(newTile + x * 2 + 1, y * 2, getElementTile(gameTerrain[chunk * 4 + x + y * MAP_WIDTH]) + 1);
						Bg1SetTile(newTile + x * 2, y * 2 + 1, getElementTile(gameTerrain[chunk * 4 + x + y * MAP_WIDTH]) + 2);
						Bg1SetTile(newTile + x * 2 + 1, y * 2 + 1, getElementTile(gameTerrain[chunk * 4 + x + y * MAP_WIDTH]) + 3);
					}
				}
			}
		}

		dmaCopy(spritesTiles + player.anim_frame * 16 * 32, player.sprite_gfx_mem, 32 * 64);

		oamSet(&oamSub, 0, player.renderX, player.renderY, 1, 0, SpriteSize_32x64, SpriteColorFormat_256Color, player.sprite_gfx_mem, -1, false, false, player.isLookingLeft, false, false);
		// oamSet(&oamSub, 1, player.renderX, player.renderY, 0, 0, SpriteSize_16x16, SpriteColorFormat_256Color, itemHandSprite, -1, false, false, player.isLookingLeft, false, false);
		oamSet(&oamSub, 2, destructionSprite.renderX, destructionSprite.renderY, 0, 0, SpriteSize_16x16, SpriteColorFormat_256Color, destructionSprite.sprite_gfx_mem, -1, false, false, false, false, false);
		// Render dropped items
		u8 renderedItems = 0;

		for (int i = 0; i < 64; i++)
		{
			if (renderedItems < MAX_ITEMS)
			{
				if (item[i].exists)
				{
					item[i].renderX = item[i].x - scrollX;
					item[i].renderY = item[i].y - scrollY;
					oamSet(&oamSub, renderedItems + 3, item[i].renderX, item[i].renderY, 0, 0, SpriteSize_8x8, SpriteColorFormat_256Color, item[i].sprite_gfx_mem, -1, false, false, false, false, false);
					renderedItems++;
				}
			}
		}
		// Clean up used sprites
		for (int i = renderedItems; i < MAX_ITEMS; i++)
		{
			oamSet(&oamSub, i + 3, 0, 0, 0, 0, SpriteSize_8x8, SpriteColorFormat_256Color, nullSprite, -1, false, false, false, false, false);
		}

		oamUpdate(&oamSub);
		bgUpdate();

		frame++;
	}

	return 0;
}
