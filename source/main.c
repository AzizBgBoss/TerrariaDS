/*---------------------------------------------------------------------------------

Terraria like game for the Nintendo DS
AzizBgBoss - https://github.com/AzizBgBoss

---------------------------------------------------------------------------------*/

#include <nds.h>
#include <stdio.h>
#include <math.h>
#include <fat.h>
#include <maxmod9.h>
#include <stdlib.h>
#include <time.h>
#include <filesystem.h>

#include "tilemap.h"
#include "bg.h"
#include "sprites.h"
#include "inv.h"
#include "font.h"
#include "ui.h"
#include "von.h"
#include "items.h"
#include "intro.h"
#include "mainscreenbg.h"
#include "mainscreenui.h"

#include "soundbank.h"

#define MAP_WIDTH 1024
#define MAP_HEIGHT 64

#define MAX_ITEMS 16 // Maximum items to be rendered

// Define game elements
#define TILE_AIR 0
#define TILE_PLANKS 1
#define TILE_DIRT 2
#define TILE_STONE 3
#define TILE_WOODLOG 4
#define TILE_LEAVES 5
#define TILE_MUSHROOM 6

// Make sure to update isTileWall
#define TILE_DIRT_WALL 7
#define TILE_STONE_WALL 8

#define TILE_DEMONITE_BRICK 9 // Indestructible brick

#define TILE_WOOD_WALL 10

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
int scale = 128; // 128 is x2, 256 is x1, 512 is x0.5
int chunk = 0;

u8 gameTerrain[MAP_WIDTH * MAP_HEIGHT] = {0};
int gameTerrainHealth[MAP_WIDTH * MAP_HEIGHT] = {0};

u8 inventory[8 * 4] = {0};
u8 inventoryQuantity[8 * 4] = {0};
u8 inventorySelection = 0;
u8 craftingSelection = 0;

u16 *inventorySelectionSprite;
u16 *itemHandSprite;

bool debug = false;
bool inventoryOpen = false;
bool craftingOpen = false;

FILE *f;

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

typedef struct
{
	int item;
	int quantity;
	int ingredientCount;
	int itemsNeeded[4];
	int itemsNeededQuantity[4];
} CraftingRecipe;

// Define the player entity
Player player = {MAP_WIDTH * 8 / 2, 0, 0, 0, 0, NULL, 16, 24, false, true, 1, 0, true, false, 4, ANIM_NONE};

// Define 64 slots for item entities
Item item[64] = {{0, 0, 0, 0, NULL, 8, 8, false, 60, 0, 0}};

// Define crafting recipes
CraftingRecipe craftingRecipes[] = {
	{
		ITEM_HAMMER,
		1,
		2,
		{TILE_PLANKS, TILE_STONE},
		{1, 1},
	},
	{
		TILE_WOOD_WALL,
		4,
		1,
		{
			TILE_PLANKS,
		},
		{
			1,
		},
	},
	{
		TILE_STONE_WALL,
		4,
		1,
		{
			TILE_STONE,
		},
		{
			1,
		},
	},
	{
		TILE_DIRT_WALL,
		4,
		1,
		{
			TILE_DIRT,
		},
		{
			1,
		},
	},
	{TILE_PLANKS,
	 1,
	 1,
	 {
		 TILE_WOOD_WALL,
	 },
	 {
		 4,
	 }},
	{TILE_STONE,
	 1,
	 1,
	 {
		 TILE_STONE_WALL,
	 },
	 {
		 4,
	 }},
	{TILE_DIRT,
	 1,
	 1,
	 {
		 TILE_DIRT_WALL,
	 },
	 {
		 4,
	 }}};

u16 *bg2Map;

void Bg1SetTile(int x, int y, int tile)
{
	if (x < 0 || x >= 64 || y < 0 || y >= 64)
		return; // Prevent out of bounds access
	bg2Map[x + y * 64] = tile;
}

void Bg0UpFill(int tile)
{
	for (int i = 0; i < 32 * 32; i++)
		BG_MAP_RAM(0)
	[i] = tile;
}

void Bg0UpSetTile(int x, int y, int tile)
{
	BG_MAP_RAM(0)
	[x + y * 32] = tile;
}

void Bg1UpFill(int tile)
{
	for (int i = 0; i < 32 * 32; i++)
		BG_MAP_RAM(2)
	[i] = tile;
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

int cx, cy; // Current cursor position

void clearPrint()
{
	cx = 0;
	cy = 0;
	for (int y = 0; y < SCREEN_HEIGHT / 8; y++)
	{
		for (int x = 0; x < SCREEN_WIDTH / 8; x++)
		{
			Bg3UpSetTile(x, y, 0); // Clear the screen
		}
	}
}

void print(int x, int y, const char *text)
{
	while (*text)
	{
		if (*text == '\n') // If we reach a newline character, move to the next line
		{
			x = 0;						   // Reset x to the beginning of the line
			y++;						   // Move to the next line
			if (y > SCREEN_HEIGHT / 8 - 1) // If we reach the bottom of the screen, stop printing
				return;
			text++;
			continue; // Skip the newline character
		}
		Bg3UpSetTile(x, y, (char)(*text - 8 * 4));
		x++;
		if (x > SCREEN_WIDTH / 8 - 1)
		{
			x = 0;
			y++;
		}
		text++;
	}
	cx = x; // Update current cursor position
	cy = y;
}

void printDirect(const char *text)
{
	print(cx, cy, text);
}

void printVal(int x, int y, int value)
{
	char buffer[32];
	itoa(value, buffer, 10);
	print(x, y, buffer);
}

void printValDirect(int value)
{
	char buffer[32];
	itoa(value, buffer, 10);
	printDirect(buffer);
}

static int clamp(int val, int min, int max)
{
	if (val < min)
		return min;
	if (val > max)
		return max;
	return val;
}

int getElementTile(int tile, int x, int y) // Tile will change based on surrounding tiles
{
	int tileabove = (y > 0) ? gameTerrain[x + (y - 1) * MAP_WIDTH] : TILE_AIR;
	int tilebelow = (y < MAP_HEIGHT - 1) ? gameTerrain[x + (y + 1) * MAP_WIDTH] : TILE_AIR;
	int tileleft = (x > 0) ? gameTerrain[x - 1 + y * MAP_WIDTH] : TILE_AIR;
	int tileright = (x < MAP_WIDTH - 1) ? gameTerrain[x + 1 + y * MAP_WIDTH] : TILE_AIR;

	int offset = 0;
	switch (tile)
	{
	case TILE_AIR:
		return 0;
	case TILE_DIRT:
		offset = 1;
		break;
	case TILE_STONE:
		offset = 2;
		break;
	case TILE_LEAVES:
		offset = 3;
		break;
	case TILE_PLANKS:
		offset = 4;
		break;
	case TILE_DIRT_WALL:
		offset = 5;
		break;
	case TILE_STONE_WALL:
		offset = 6;
		break;
	case TILE_DEMONITE_BRICK:
		offset = 7;
		break;
	case TILE_WOOD_WALL:
		offset = 8;
		break;
	case TILE_WOODLOG:
		return 2;
	case TILE_MUSHROOM:
		return 1;
	default:
		return 8;
	}

	if (tileabove == TILE_AIR)
	{
		if (tilebelow == TILE_AIR)
		{
			if (tileleft == TILE_AIR)
			{
				if (tileright == TILE_AIR)
				{
					return offset * 9 + 0; // Middle
				}
				else
				{
					return offset * 9 + 0; // Top right corner
				}
			}
			else
			{
				if (tileright == TILE_AIR)
				{
					return offset * 9 + 2; // Top left corner
				}
				else
				{
					return offset * 9 + 1; // Top edge
				}
			}
		}
		else
		{
			if (tileleft == TILE_AIR)
			{
				if (tileright == TILE_AIR)
				{
					return offset * 9 + 1; // Top edge
				}
				else
				{
					return offset * 9 + 0; // Top left corner
				}
			}
			else
			{
				if (tileright == TILE_AIR)
				{
					return offset * 9 + 2; // Top right corner
				}
				else
				{
					return offset * 9 + 1; // Top edge
				}
			}
		}
	}
	else
	{
		if (tilebelow == TILE_AIR)
		{
			if (tileleft == TILE_AIR)
			{
				if (tileright == TILE_AIR)
				{
					return offset * 9 + 7; // Bottom middle
				}
				else
				{
					return offset * 9 + 6; // Bottom left corner
				}
			}
			else
			{
				if (tileright == TILE_AIR)
				{
					return offset * 9 + 8; // Bottom right corner
				}
				else
				{
					return offset * 9 + 7; // Bottom edge
				}
			}
		}
		else
		{
			if (tileleft == TILE_AIR)
			{
				if (tileright == TILE_AIR)
				{
					return offset * 9 + 4; // Middle
				}
				else
				{
					return offset * 9 + 3; // Left edge
				}
			}
			else
			{
				if (tileright == TILE_AIR)
				{
					return offset * 9 + 5; // Left edge
				}
				else
				{
					return offset * 9 + 4; // Middle
				}
			}
		}
	}
}

int getItemTile(int item)
{
	switch (item)
	{
	case ITEM_SWORD:
		return 0;
	case ITEM_PICKAXE:
		return 4;
	case ITEM_AXE:
		return 8;
	case ITEM_HAMMER:
		return 12;
	case TILE_DIRT:
		return 16;
	case TILE_STONE:
		return 20;
	case TILE_MUSHROOM:
		return 24;
	case TILE_PLANKS:
		return 28;
	case TILE_LEAVES:
		return 32;
	case TILE_DIRT_WALL:
		return 36;
	case TILE_STONE_WALL:
		return 40;
	case TILE_DEMONITE_BRICK:
		return 44;
	case TILE_WOOD_WALL:
		return 48;
	default:
		return 56;
	}
}

char *getElementName(int element)
{
	switch (element)
	{
	case TILE_DIRT:
		return "Dirt";
	case TILE_STONE:
		return "Stone";
	case TILE_WOODLOG:
		return "Wood Log";
	case TILE_PLANKS:
		return "Planks";
	case TILE_MUSHROOM:
		return "Mushroom";
	case TILE_LEAVES:
		return "Leaves";
	case TILE_DIRT_WALL:
		return "Dirt Wall";
	case TILE_STONE_WALL:
		return "Stone Wall";
	case TILE_DEMONITE_BRICK:
		return "Demonite Brick";
	case TILE_WOOD_WALL:
		return "Wood Wall";
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
	case TILE_DIRT:
		return 100;
	case TILE_STONE:
		return 300;
	case TILE_WOODLOG:
		return 200;
	case TILE_PLANKS:
		return 100;
	case TILE_LEAVES:
		return 50;
	case TILE_MUSHROOM:
		return 10;
	case TILE_DIRT_WALL:
		return 100;
	case TILE_STONE_WALL:
		return 150;
	case TILE_DEMONITE_BRICK:
		return INFINITY;
	case TILE_WOOD_WALL:
		return 100;
	default:
		return 0;
	}
}

int getElementDrop(int element)
{
	switch (element)
	{
	case TILE_WOODLOG:
		return TILE_PLANKS; // Drops planks when broken
	default:
		return element; // Drops itself
	}
}

bool isToolCompatible(int tool, int tile)
{
	switch (tool)
	{
	case ITEM_PICKAXE:
		return tile == TILE_STONE || tile == TILE_DIRT || tile == TILE_PLANKS || tile == TILE_MUSHROOM || tile == TILE_DEMONITE_BRICK;
	case ITEM_AXE:
		return tile == TILE_WOODLOG || tile == TILE_LEAVES || tile == TILE_MUSHROOM;
	case ITEM_SWORD:
		return tile == TILE_MUSHROOM; // Can break mushrooms
	case ITEM_HAMMER:
		return tile == TILE_DIRT_WALL || tile == TILE_STONE_WALL || tile == TILE_WOOD_WALL; // Can break walls
	default:
		return false;
	}
}

bool isTileSolid(int tile)
{
	switch (tile)
	{
	case TILE_DIRT:
	case TILE_STONE:
	case TILE_PLANKS:
	case TILE_DEMONITE_BRICK:
		return true;
	default:
		return false;
	}
}

bool isElementWall(int tile)
{
	switch (tile)
	{
	case TILE_DIRT_WALL:
	case TILE_STONE_WALL:
	case TILE_WOOD_WALL:
		return true;
	default:
		return false;
	}
}

int rando(int min, int max)
{
	return rand() % (max - min + 1) + min;
}

void setInventorySelection(u8 slot)
{
	inventorySelection = slot;
	int x = (slot % 8) * 4 * 8;
	int y = ((slot / 8) * -4 + 20) * 8;
	if (!craftingOpen)
	{
		print(1, 19, "                ");
		print(1, 19, getElementName(inventory[slot]));
		oamSet(&oamMain, 0, x, y, 0, 0, SpriteSize_32x32, SpriteColorFormat_256Color, inventorySelectionSprite, -1, false, false, false, false, false);
		oamUpdate(&oamMain);
	}
	mmEffect(SFX_ENU_TICK);
}

void setCraftingSelection(u8 slot)
{
	clearPrint();
	craftingSelection = slot;
	int x = (slot % 4) * 4 * 8;
	int y = ((slot / 4) * -4 + 20) * 8;
	if (craftingOpen)
	{
		print(16, 7, "              ");
		print(16, 7, getElementName(craftingRecipes[slot].item));
		for (int i = 0; i < craftingRecipes[slot].ingredientCount; i++)
		{
			Bg1UpSetTile(16, 8 + i * 2, getItemTile(craftingRecipes[slot].itemsNeeded[i]) + 0);
			Bg1UpSetTile(16 + 1, 8 + i * 2, getItemTile(craftingRecipes[slot].itemsNeeded[i]) + 1);
			Bg1UpSetTile(16, 8 + 1 + i * 2, getItemTile(craftingRecipes[slot].itemsNeeded[i]) + 2);
			Bg1UpSetTile(16 + 1, 8 + 1 + i * 2, getItemTile(craftingRecipes[slot].itemsNeeded[i]) + 3);
			print(16 + 2, 8 + i * 2, "              ");
			print(16 + 2, 8 + i * 2, getElementName(craftingRecipes[slot].itemsNeeded[i]));
			print(16 + 2, 9 + i * 2, "              ");
			printVal(16 + 2, 9 + i * 2, craftingRecipes[slot].itemsNeededQuantity[i]);
		}
		oamSet(&oamMain, 0, x, y, 0, 0, SpriteSize_32x32, SpriteColorFormat_256Color, inventorySelectionSprite, -1, false, false, false, false, false);
		oamUpdate(&oamMain);
	}
	mmEffect(SFX_ENU_TICK);
}

void renderInventory()
{
	if (!craftingOpen)
	{
		Bg1UpFill(63);
		for (int i = 0; i < 8 * 4; i++)
		{
			if (inventory[i] == 0 || inventoryQuantity[i] == 0)
			{
				Bg1UpSetTile((i % 8) * 4 + 1, (i / 8) * -4 + 21, 63);
				Bg1UpSetTile((i % 8) * 4 + 2, (i / 8) * -4 + 21, 63);
				Bg1UpSetTile((i % 8) * 4 + 1, (i / 8) * -4 + 22, 63);
				Bg1UpSetTile((i % 8) * 4 + 2, (i / 8) * -4 + 22, 63);
				print((i % 8) * 4 + 1, (i / 8) * -4 + 23, "   ");
			}
			else
			{
				Bg1UpSetTile((i % 8) * 4 + 1, (i / 8) * -4 + 21, getItemTile(inventory[i]) + 0);
				Bg1UpSetTile((i % 8) * 4 + 2, (i / 8) * -4 + 21, getItemTile(inventory[i]) + 1);
				Bg1UpSetTile((i % 8) * 4 + 1, (i / 8) * -4 + 22, getItemTile(inventory[i]) + 2);
				Bg1UpSetTile((i % 8) * 4 + 2, (i / 8) * -4 + 22, getItemTile(inventory[i]) + 3);
				print((i % 8) * 4 + 2, (i / 8) * -4 + 23, "   ");
				if (inventoryQuantity[i] > 1)
				{
					char buffer[3];
					itoa(inventoryQuantity[i], buffer, 10);
					print((i % 8) * 4 + 1, (i / 8) * -4 + 23, buffer);
				}
			}
		}
		setInventorySelection(inventorySelection);
	}
}

void renderCrafting()
{
	if (craftingOpen)
	{
		Bg1UpFill(63);
		setCraftingSelection(craftingSelection);
		int tilesToRender = (sizeof(craftingRecipes) / sizeof(craftingRecipes[0]) <= 16) ? sizeof(craftingRecipes) / sizeof(craftingRecipes[0]) : 16;
		for (int i = 0; i < tilesToRender; i++)
		{
			Bg1UpSetTile((i % 4) * 4 + 1, (i / 4) * -4 + 21, getItemTile(craftingRecipes[i].item) + 0);
			Bg1UpSetTile((i % 4) * 4 + 2, (i / 4) * -4 + 21, getItemTile(craftingRecipes[i].item) + 1);
			Bg1UpSetTile((i % 4) * 4 + 1, (i / 4) * -4 + 22, getItemTile(craftingRecipes[i].item) + 2);
			Bg1UpSetTile((i % 4) * 4 + 2, (i / 4) * -4 + 22, getItemTile(craftingRecipes[i].item) + 3);
			print((i % 4) * 4 + 1, (i / 4) * -4 + 23, "   ");
			if (craftingRecipes[i].quantity > 1)
			{
				printVal((i % 4) * 4 + 1, (i / 4) * -4 + 23, craftingRecipes[i].quantity);
			}
		}
	}
}

void setInventory(int slot, int item, int quantity)
{
	if (slot < 0 || slot >= 8 * 4)
		return;
	if (item == 0 || quantity == 0)
	{
		inventory[slot] = 0;
		inventoryQuantity[slot] = 0;
		renderInventory();
		return;
	}
	inventory[slot] = item;
	inventoryQuantity[slot] = quantity;
	renderInventory();
}

void inventorySetHotbar()
{
	craftingOpen = false;
	Bg0UpFill(0);
	Bg1UpFill(63);
	for (int i = 0; i < 32; i += 4)
	{
		for (int j = 0; j < 16; j++)
		{
			Bg0UpSetTile(i + (j % 4), 20 + j / 4, 16 + j);
		}
	}
	clearPrint();
	print(0, 0, "TerrariaDS v0.0alpha\n\
By AzizBgBoss\n\
https://github.com/AzizBgBoss/TerrariaDS\n\
A: Jump\n\
Left/Right: Move\n\
Up/Down: Zoom in/out\n\
X/Y: Switch items\n\
R: Open inventory\n\
Start: Save map\n\
Select: Load map\n");
	renderInventory();
	mmEffect(SFX_ENU_CLOSE);
}

void inventorySetFull()
{
	craftingOpen = false;
	Bg0UpFill(0);
	Bg1UpFill(63);
	for (int i = 0; i < 8 * 4; i++)
	{
		for (int j = 0; j < 16; j++)
		{
			Bg0UpSetTile((i % 8) * 4 + j % 4, 8 + (i / 8) * 4 + j / 4, 16 + j);
		}
	}
	for (int j = 0; j < 16; j++)
		Bg0UpSetTile(27 + j % 4, j / 4, 32 + j);
	clearPrint();
	print(0, 0, "Tap on a slot to select it.\n\
Hold and move an item to change it's slot.\n\
Press R to close inventory\n\
You can still move the player with the other buttons.\n\
Tap on the crafting icon on the top right to craft.	");
	renderInventory();
	mmEffect(SFX_ENU_OPEN);
}

void inventorySetCrafting()
{
	craftingOpen = true;
	Bg0UpFill(0);
	Bg1UpFill(63);
	for (int i = 0; i < 4 * 4; i++)
	{
		for (int j = 0; j < 16; j++)
		{
			Bg0UpSetTile((i % 4) * 4 + j % 4, 8 + (i / 4) * 4 + j / 4, 16 + j);
		}
	}

	for (int j = 0; j < 16; j++)
		Bg0UpSetTile(27 + j % 4, j / 4, 48 + j);
	for (int j = 0; j < 16; j++)
		Bg0UpSetTile(23 + j % 4, 20 + j / 4, 64 + j);
	for (int j = 0; j < 16; j++)
		Bg0UpSetTile(27 + j % 4, 20 + j / 4, 80 + j);
	clearPrint();
	renderCrafting();
	mmEffect(SFX_ENU_OPEN);
}

void setGameTerrain(int x, int y, int tile)
{
	if (x < 0 || x >= MAP_WIDTH || y < 0 || y >= MAP_HEIGHT)
		return;

	gameTerrain[x + y * MAP_WIDTH] = tile;

	if (x < chunk * 4 || x > chunk * 4 + 63)
		return;

	int rx = x % 64;
	// Re-render neighboring tiles
	Bg1SetTile(rx - 1, y, getElementTile(gameTerrain[x - 1 + y * MAP_WIDTH], x - 1, y));
	Bg1SetTile(rx + 1, y, getElementTile(gameTerrain[x + 1 + y * MAP_WIDTH], x + 1, y));
	Bg1SetTile(rx, y - 1, getElementTile(gameTerrain[x + (y - 1) * MAP_WIDTH], x, y - 1));
	Bg1SetTile(rx, y + 1, getElementTile(gameTerrain[x + (y + 1) * MAP_WIDTH], x, y + 1));
	Bg1SetTile(rx, y, getElementTile(tile, x, y));
}

void playerPutGameTerrain(int x, int y, int tile)
{
	if (x < 0 || x >= MAP_WIDTH || y < 0 || y >= MAP_HEIGHT)
		return;

	// The difference is that we check the surrounding tiles to prevent placing tiles out of thin air
	bool canPlace = false;
	for (int dx = -1; dx <= 1; dx++)
	{
		for (int dy = -1; dy <= 1; dy++)
		{
			if (dx == 0 && dy == 0)
				continue; // Skip the tile itself
			int nx = x + dx;
			int ny = y + dy;
			if (nx < 0 || nx >= MAP_WIDTH || ny < 0 || ny >= MAP_HEIGHT)
				continue; // Out of bounds
			if (gameTerrain[nx + ny * MAP_WIDTH] != TILE_AIR)
			{
				canPlace = true;
				break;
			}
		}
		if (canPlace)
			break;
	}
	if (!canPlace)
		return; // Cannot place tile if no surrounding tiles are solid

	Bg1SetTile(x, y, getElementTile(tile, x, y));
	inventoryQuantity[inventorySelection]--;
	setInventory(inventorySelection, inventory[inventorySelection], inventoryQuantity[inventorySelection]);
	setGameTerrain(x, y, tile);
	switch (rando(0, 2))
	{
	case 0:
		mmEffect(SFX_IG_0);
		break;
	case 1:
		mmEffect(SFX_IG_1);
		break;
	case 2:
		mmEffect(SFX_IG_2);
		break;
	}
}

bool giveInventory(int item, int quantity)
{
	for (int i = 0; i < 8 * 4; i++)
	{
		if (inventory[i] == item && inventoryQuantity[i] < 99 - quantity)
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

bool playerHasItem(int item, int quantity)
{
	for (int i = 0; i < 8 * 4; i++)
	{
		if (inventory[i] == item && inventoryQuantity[i] >= quantity)
			return true;
	}
	return false;
}

void dropItem(int x, int y, int tile, int quantity)
{
	int index = 0;
	while (item[index].exists)
		index++;
	// we can do item[index] = {...} directly but it'll get confusing
	item[index].x = x * 8 + 4; // Drop in the middle of the tile
	item[index].y = y * 8 + 4;
	item[index].exists = true;
	item[index].tile = tile;
	item[index].quantity = quantity;

	f = fopen("nitro:/items.img.bin", "rb");
	fseek(f, 8 * 8 * getItemTile(tile), SEEK_SET);
	fread(item[index].sprite_gfx_mem, 1, 16 * 16, f);
	fclose(f);
}

void destroyItem(int id)
{
	item[id].exists = false;
}

void breakTile(int x, int y, int speed)
{
	gameTerrainHealth[x + y * MAP_WIDTH] += speed;
	if (gameTerrainHealth[x + y * MAP_WIDTH] >= getElementHealth(gameTerrain[x + y * MAP_WIDTH]))
	{
		// Special blocks handling
		if (gameTerrain[x + y * MAP_WIDTH] == TILE_WOODLOG)
		{
			// Trees have special treatment: break all the logs and leaves above it
			for (int i = y - 1; i >= 0; i--)
			{
				if (gameTerrain[x + i * MAP_WIDTH] == TILE_WOODLOG)
				{
					dropItem(x, i, getElementDrop(TILE_WOODLOG), 1);
					setGameTerrain(x, i, 0);
					gameTerrainHealth[x + i * MAP_WIDTH] = 0;
				}
				else if (gameTerrain[x + i * MAP_WIDTH] == TILE_LEAVES)
				{ // Leaves don't drop anything if broken from tree
					setGameTerrain(x, i, 0);
					gameTerrainHealth[x + i * MAP_WIDTH] = 0;
					if (gameTerrain[x - 1 + i * MAP_WIDTH] == TILE_LEAVES)
					{
						setGameTerrain(x - 1, i, 0);
						gameTerrainHealth[x - 1 + i * MAP_WIDTH] = 0;
					}
					if (gameTerrain[x + 1 + i * MAP_WIDTH] == TILE_LEAVES)
					{
						setGameTerrain(x + 1, i, 0);
						gameTerrainHealth[x + 1 + i * MAP_WIDTH] = 0;
					}
				}
				else
				{
					break; // Stop breaking when we hit a non-log or non-leaf tile
				}
			}
		}
		dropItem(x, y, getElementDrop(gameTerrain[x + y * MAP_WIDTH]), 1);
		setGameTerrain(x, y, 0);
		gameTerrainHealth[x + y * MAP_WIDTH] = 0;
	}
	// Random sound effect for breaking tiles
	if (frame % 15 == 0) // Play sound every 10 frames
	{
		switch (rando(0, 2))
		{
		case 0:
			mmEffect(SFX_IG_0);
			break;
		case 1:
			mmEffect(SFX_IG_1);
			break;
		case 2:
			mmEffect(SFX_IG_2);
			break;
		}
	}
}

// I honestly wanted to make a struct of SaveData but i had some problems with its stack memory or whatever it is

bool saveMapToFile(const char *filename)
{
	FILE *file = fopen(filename, "wb"); // "wb" = write binary
	mmEffect(SFX_ENU_CLOSE);
	if (!file)
	{
		print(0, 0, "Failed to open file for writing: map.dat");
		return false;
	}

	print(0, 0, "Saving map...");
	uint32_t magic = 0xA212B055;
	size_t bytesWritten = fwrite(&magic, 1, 4, file);
	bytesWritten += fwrite(gameTerrain, 1, sizeof(gameTerrain), file);
	bytesWritten += fwrite(inventory, 1, sizeof(inventory), file);
	bytesWritten += fwrite(inventoryQuantity, 1, sizeof(inventoryQuantity), file);
	fclose(file);
	print(0, 0, "File closed");

	if (bytesWritten != 4 + sizeof(gameTerrain) + sizeof(inventory) + sizeof(inventoryQuantity))
	{
		print(0, 0, "Map save error");
		return false;
	}

	print(0, 0, "Map saved to map.dat");
	return true;
}

bool loadMapFromFile(const char *filename)
{
	FILE *file = fopen(filename, "rb"); // "rb" = read binary
	mmEffect(SFX_ENU_OPEN);
	if (!file)
	{
		print(0, 0, "Failed to open file for reading: map.dat");
		return false;
	}

	print(0, 0, "Loading map...");
	uint32_t magic;
	size_t bytesRead = fread(&magic, 1, 4, file);
	if (magic != 0xA212B055)
	{
		print(0, 0, "Invalid map file: map.dat");
		return false;
	}
	bytesRead += fread(gameTerrain, 1, sizeof(gameTerrain), file);
	bytesRead += fread(inventory, 1, sizeof(inventory), file);
	bytesRead += fread(inventoryQuantity, 1, sizeof(inventoryQuantity), file);
	fclose(file);

	if (bytesRead != 4 + sizeof(gameTerrain) + sizeof(inventory) + sizeof(inventoryQuantity))
	{
		print(0, 0, "Map load error");
		return false;
	}

	player.x = MAP_WIDTH * 8 / 2;
	player.y = 0;
	chunk = -6;
	renderInventory();
	renderCrafting();
	print(0, 0, "Map loaded from map.dat");
	return true;
}

// Hash function
static inline uint32_t murmur_32_scramble(uint32_t k)
{
	k *= 0xcc9e2d51;
	k = (k << 15) | (k >> 17);
	k *= 0x1b873593;
	return k;
}
uint32_t murmur3_32(const uint8_t *key, size_t len, uint32_t seed)
{
	uint32_t h = seed;
	uint32_t k;
	/* Read in groups of 4. */
	for (size_t i = len >> 2; i; i--)
	{
		// Here is a source of differing results across endiannesses.
		// A swap here has no effects on hash properties though.
		memcpy(&k, key, sizeof(uint32_t));
		key += sizeof(uint32_t);
		h ^= murmur_32_scramble(k);
		h = (h << 13) | (h >> 19);
		h = h * 5 + 0xe6546b64;
	}
	/* Read the rest. */
	k = 0;
	for (size_t i = len & 3; i; i--)
	{
		k <<= 8;
		k |= key[i - 1];
	}
	// A swap is *not* necessary here because the preceding loop already
	// places the low bytes in the low places according to whatever endianness
	// we use. Swaps only apply when the memory is copied in a chunk.
	h ^= murmur_32_scramble(k);
	/* Finalize. */
	h ^= len;
	h ^= h >> 16;
	h *= 0x85ebca6b;
	h ^= h >> 13;
	h *= 0xc2b2ae35;
	h ^= h >> 16;
	return h;
}

// Hash function for Perlin noise
int hash(int x, int seed)
{
	// Use MurmurHash3 to generate a hash value based on the input x and seed
	uint32_t hashValue = murmur3_32((const uint8_t *)&x, sizeof(x), seed);
	// Return the hash value modulo 256 to fit in a byte
	return hashValue & 0x7fffffff; // Ensure it's positive
								   // Note: This will return a value in the range [0, 255] which is suitable for use in Perlin noise calculations.
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
float perlin1d(float x, int seed)
{
	int xi = (int)x;
	float xf = x - xi;

	int h0 = hash(xi, seed);
	int h1 = hash(xi + 1, seed);

	float g0 = grad(h0, xf);
	float g1 = grad(h1, xf - 1.0f);

	float u = fade(xf);

	return lerp(g0, g1, u); // Returns value in range ~[-1, 1]
}

float grad2D(int hash, float x, float y)
{
	int h = hash & 7; // 8 directions
	switch (h)
	{
	case 0:
		return x + y;
	case 1:
		return -x + y;
	case 2:
		return x - y;
	case 3:
		return -x - y;
	case 4:
		return x;
	case 5:
		return -x;
	case 6:
		return y;
	default:
		return -y;
	}
}

float perlin2d(float x, float y, int seed)
{
	int xi = (int)floorf(x);
	int yi = (int)floorf(y);
	float xf = x - xi;
	float yf = y - yi;

	int aa = hash(xi + hash(yi, seed), seed);
	int ab = hash(xi + hash(yi + 1, seed), seed);
	int ba = hash(xi + 1 + hash(yi, seed), seed);
	int bb = hash(xi + 1 + hash(yi + 1, seed), seed);

	float dotAA = grad2D(aa, xf, yf);
	float dotBA = grad2D(ba, xf - 1, yf);
	float dotAB = grad2D(ab, xf, yf - 1);
	float dotBB = grad2D(bb, xf - 1, yf - 1);

	float u = fade(xf);
	float v = fade(yf);

	float x1 = lerp(dotAA, dotBA, u);
	float x2 = lerp(dotAB, dotBB, u);

	return lerp(x1, x2, v); // value in ~[-1, 1]
}

float fractalPerlin1D(float x, int octaves, float persistence, float scale, int seed)
{
	float total = 0;
	float frequency = scale;
	float amplitude = 1;
	float maxAmplitude = 0;

	for (int i = 0; i < octaves; i++)
	{
		total += perlin1d(x * frequency, seed) * amplitude;
		maxAmplitude += amplitude;

		amplitude *= persistence;
		frequency *= 2.0f;
	}

	return total / maxAmplitude; // normalize to -1..1
}

float fractalPerlin2D(float x, float y, int octaves, float persistence, float scale, int seed)
{
	float total = 0;
	float frequency = scale;
	float amplitude = 1;
	float maxAmplitude = 0;

	for (int i = 0; i < octaves; i++)
	{
		total += perlin2d(x * frequency, y * frequency, seed) * amplitude;
		maxAmplitude += amplitude;

		amplitude *= persistence;
		frequency *= 2.0f;
	}

	return total / maxAmplitude; // normalize to -1..1
}

#define TREE_CHANCE 10
#define MUSHROOM_CHANCE 10
#define MIN_GRASS_HEIGHT 12
#define MAX_GRASS_HEIGHT 20
#define MIN_STONE_HEIGHT 17
#define MAX_STONE_HEIGHT 25

void generateMap()
{
	u8 grassSurface[MAP_WIDTH];
	u8 stoneSurface[MAP_WIDTH];
	int seed = rando(0, 99999999);
	printDirect("\nSeed: ");
	printValDirect(seed);
	printDirect("\nGenerating terrain...\n");
	for (int x = 0; x < MAP_WIDTH; x++)
	{
		float wave = fractalPerlin1D(x, 4, 0.4f, 0.01f, seed) * 20.0f;
		grassSurface[x] = clamp((int)(wave + (MIN_GRASS_HEIGHT + MAX_GRASS_HEIGHT) / 2), MIN_GRASS_HEIGHT, MAX_GRASS_HEIGHT);
		if (x % (MAP_WIDTH / 32) == 0)
		{
			printDirect(".");
		}
	}

	// remove 1 block spikes, i fucking hate them
	printDirect("\nRemoving spikes because they are annoying...\n");
	for (int x = 1; x < MAP_WIDTH - 1; x++)
	{
		if (grassSurface[x - 1] != grassSurface[x] && grassSurface[x + 1] != grassSurface[x])
		{
			grassSurface[x] += 1;
		}
		if (x % (MAP_WIDTH / 32) == 0)
		{
			printDirect(".");
		}
	}

	// Generate stone height surface
	printDirect("\nGenerating stone surface...\n");
	for (int x = 0; x < MAP_WIDTH; x++)
	{
		float wave = fractalPerlin1D(x, 4, 0.4f, 0.01f, seed + 1) * 20.0f;
		stoneSurface[x] = clamp((int)(wave + (MIN_STONE_HEIGHT + MAX_STONE_HEIGHT) / 2), MIN_STONE_HEIGHT, MAX_STONE_HEIGHT);
		if (x % (MAP_WIDTH / 32) == 0)
		{
			printDirect(".");
		}
	}

	// Place terrain
	printDirect("\nPlacing terrain...\n");
	for (int x = 0; x < MAP_WIDTH; x++)
	{
		for (int y = 0; y < MAP_HEIGHT; y++)
		{
			if (y >= grassSurface[x] && y < stoneSurface[x])
			{
				setGameTerrain(x, y, TILE_DIRT);
			}
			else if (y >= stoneSurface[x])
			{
				setGameTerrain(x, y, TILE_STONE);
			}
		}
		if (x % (MAP_WIDTH / 32) == 0)
		{
			printDirect(".");
		}
	}

	// Generate caves
	printDirect("\nGenerating caves...\n");
	for (int x = 0; x < MAP_WIDTH; x++)
	{
		for (int y = 0; y < MAP_HEIGHT; y++)
		{
			if (y >= grassSurface[x])
			{
				float caveNoise = fractalPerlin2D(x, y, 6, 0.4f, 0.01f, seed);
				if (caveNoise < -0.15f) // Adjust this threshold to control cave density
				{
					setGameTerrain(x, y, TILE_AIR); // Create a cave
				}
			}
		}
		if (x % (MAP_WIDTH / 32) == 0)
		{
			printDirect(".");
		}
	}

	// Adding walls
	printDirect("\nAdding walls...\n");
	for (int x = 0; x < MAP_WIDTH; x++)
	{
		for (int y = 0; y < MAP_HEIGHT; y++)
		{
			if (gameTerrain[x + y * MAP_WIDTH] == TILE_AIR)
			{
				if (y >= grassSurface[x] && y < stoneSurface[x])
					setGameTerrain(x, y, TILE_DIRT_WALL);
				else if (y >= stoneSurface[x])
					setGameTerrain(x, y, TILE_STONE_WALL);
			}
		}
		if (x % (MAP_WIDTH / 32) == 0)
		{
			printDirect(".");
		}
	}

	// Place trees
	printDirect("\nPlacing trees...\n");
	for (int x = 1; x < MAP_WIDTH - 1; x++)
	{
		if (rando(0, TREE_CHANCE) == 0 && gameTerrain[x + (grassSurface[x] + 1) * MAP_WIDTH] == TILE_DIRT)
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
		if (x % (MAP_WIDTH / 32) == 0)
		{
			printDirect(".");
		}
	}

	// Place mushrooms
	printDirect("\nPlacing mushrooms...\n");
	for (int x = 1; x < MAP_WIDTH - 1; x++)
	{
		if (rando(0, MUSHROOM_CHANCE) == 0 && gameTerrain[x + grassSurface[x] * MAP_WIDTH] == TILE_DIRT && gameTerrain[x + (grassSurface[x] - 1) * MAP_WIDTH] == TILE_AIR)
		{
			setGameTerrain(x, grassSurface[x] - 1, TILE_MUSHROOM);
		}
		if (x % (MAP_WIDTH / 32) == 0)
		{
			printDirect(".");
		}
	}

	// Place demonite bricks to limit the world
	printDirect("\nPlacing demonite bricks at the bottom so you don't escape >:) ...\n");
	for (int x = 0; x < MAP_WIDTH - 1; x++)
	{
		setGameTerrain(x, MAP_HEIGHT - 1, TILE_DEMONITE_BRICK);
		if (x % (MAP_WIDTH / 32) == 0)
		{
			printDirect(".");
		}
	}
}

FILE *audioFile;

mm_word on_stream_request(mm_word length, mm_addr dest, mm_stream_formats format)
{
	uint8_t *target = (uint8_t *)dest;

	size_t bytesRead = fread(target, 1, length, audioFile);

	if (bytesRead < length)
	{
		// Fill rest with silence
		for (int i = bytesRead; i < length; i++)
		{
			target[i] = 128;
		}
		fseek(audioFile, 0, SEEK_SET);
	}

	return length;
}

#define PALETTE_LEN 256

u16 originalPalette[PALETTE_LEN];

void storeOriginalPalette()
{
	// Copy the BG palette to a buffer
	for (int i = 0; i < PALETTE_LEN; i++)
	{
		originalPalette[i] = BG_PALETTE[i];
	}
}

void fadeInPalette(int steps, int delay)
{
	for (int s = 0; s <= steps; s++)
	{
		for (int i = 0; i < PALETTE_LEN; i++)
		{
			u16 color = originalPalette[i];

			int r = color & 0x1F;
			int g = (color >> 5) & 0x1F;
			int b = (color >> 10) & 0x1F;

			// Scale each component
			int r2 = (r * s) / steps;
			int g2 = (g * s) / steps;
			int b2 = (b * s) / steps;

			BG_PALETTE[i] = (b2 << 10) | (g2 << 5) | r2;
		}

		swiWaitForVBlank();
		mmStreamUpdate();
		for (int d = 0; d < delay; d++)
		{
			swiWaitForVBlank();
			mmStreamUpdate();
		}
	}
}

int main(void)
{
	scanKeys();

	nitroFSInit(NULL);

	mmInitDefault("nitro:/soundbank.bin");

	mmLoadEffect(SFX_IG_0);
	mmLoadEffect(SFX_IG_1);
	mmLoadEffect(SFX_IG_2);
	mmLoadEffect(SFX_RAB);
	mmLoadEffect(SFX_ENU_OPEN);
	mmLoadEffect(SFX_ENU_CLOSE);
	mmLoadEffect(SFX_ENU_TICK);

	if (!nitroFSInit(NULL))
	{
		consoleDemoInit();
		iprintf("nitroFSInit error.");
	}

	if (!fatInitDefault() || keysHeld() & KEY_START)
	{
		mmLoad(MOD_MODULE1);

		mmStart(MOD_MODULE1, MM_PLAY_LOOP);

		videoSetMode(MODE_0_2D | DISPLAY_BG0_ACTIVE);
		vramSetBankA(VRAM_A_MAIN_BG);
		BGCTRL[0] = BG_TILE_BASE(1) | BG_MAP_BASE(0) | BG_COLOR_256 | BG_32x32 | BG_PRIORITY(3);

		f = fopen("nitro:/von.img.bin", "rb");
		fread((void *)CHAR_BASE_BLOCK(1), 1, vonTilesLen, f);
		fclose(f);
		f = fopen("nitro:/von.map.bin", "rb");
		fread((void *)SCREEN_BASE_BLOCK(0), 1, vonMapLen, f);
		fclose(f);
		f = fopen("nitro:/von.pal.bin", "rb");
		fread((void *)BG_PALETTE, 1, vonPalLen, f);
		fclose(f);

		consoleDemoInit();
		iprintf("Greetings, twin. fatInitDefault hath failed, dear twin.\n\
Fear not, for this plight we shall overcome.\n\
If thou usest a DS cartridge, ensure that thou applyest the proper DLDI patch suited to thy cartridge's nature.\n\
If thou art upon a DSi, no need to patch anything; perchance thy SD card be corrupted, dear twin.\n\
Or, if thou playest on an emulator, be sure that SD emulation is enabled, for most such emulators do patch DLDI of their own accord.\n\
You shall press START to continue, with no saving abilities.");

		while (pmMainLoop())
		{
			swiWaitForVBlank();
			scanKeys();
			int pressed = keysDown();
			if (pressed & KEY_START)
				break;
		}
		mmStop();
	}

	srand(time(NULL));

	touchPosition touch;

	audioFile = fopen("nitro:/1.pcm", "rb");

	mm_stream mystream;
	mystream.sampling_rate = 11025;
	mystream.buffer_length = 1024;
	mystream.callback = on_stream_request;
	mystream.format = MM_STREAM_8BIT_MONO;
	mystream.timer = MM_TIMER0;
	mystream.manual = true;
	mmStreamOpen(&mystream);

	videoSetMode(MODE_0_2D | DISPLAY_BG0_ACTIVE | DISPLAY_BG3_ACTIVE);
	videoSetModeSub(MODE_3_2D | DISPLAY_BG0_ACTIVE | DISPLAY_BG1_ACTIVE);
	vramSetBankA(VRAM_A_MAIN_BG);
	vramSetBankC(VRAM_C_SUB_BG);
	BGCTRL[0] = BG_TILE_BASE(1) | BG_MAP_BASE(0) | BG_COLOR_256 | BG_32x32 | BG_PRIORITY(3);

	f = fopen("nitro:/intro.img.bin", "rb");
	fread((void *)CHAR_BASE_BLOCK(1), 1, introTilesLen, f);
	fclose(f);
	f = fopen("nitro:/intro.map.bin", "rb");
	fread((void *)SCREEN_BASE_BLOCK(0), 1, introMapLen, f);
	fclose(f);

	BGCTRL[3] = BG_TILE_BASE(3) | BG_MAP_BASE(3) | BG_COLOR_256 | BG_32x32 | BG_PRIORITY(0);
	f = fopen("nitro:/font.img.bin", "rb");
	fread((void *)CHAR_BASE_BLOCK(3), 1, fontTilesLen, f);
	fclose(f);
	dmaFillHalfWords(0, (void *)SCREEN_BASE_BLOCK(3), 2048);

	f = fopen("nitro:/intro.pal.bin", "rb");
	fread((void *)BG_PALETTE, 1, introPalLen, f);
	fclose(f);

	storeOriginalPalette();
	fadeInPalette(64, 8);

	f = fopen("nitro:/mainscreenbg.img.bin", "rb");
	fread((void *)CHAR_BASE_BLOCK(1), 1, mainscreenbgTilesLen, f);
	fclose(f);
	f = fopen("nitro:/mainscreenbg.map.bin", "rb");
	fread((void *)SCREEN_BASE_BLOCK(0), 1, mainscreenbgMapLen, f);
	fclose(f);
	f = fopen("nitro:/mainscreenbg.pal.bin", "rb");
	fread((void *)BG_PALETTE, 1, mainscreenbgPalLen, f);
	fclose(f);

	BGCTRL_SUB[0] = BG_TILE_BASE(1) | BG_MAP_BASE(0) | BG_COLOR_256 | BG_32x32 | BG_PRIORITY(3);
	f = fopen("nitro:/bg.img.bin", "rb");
	fread((void *)CHAR_BASE_BLOCK_SUB(1), 1, bgTilesLen, f);
	fclose(f);
	f = fopen("nitro:/bg.map.bin", "rb");
	fread((void *)SCREEN_BASE_BLOCK_SUB(0), 1, bgMapLen, f);
	fclose(f);

	BGCTRL_SUB[1] = BG_TILE_BASE(2) | BG_MAP_BASE(1) | BG_COLOR_256 | BG_32x32 | BG_PRIORITY(2);
	f = fopen("nitro:/mainscreenui.img.bin", "rb");
	fread((void *)CHAR_BASE_BLOCK_SUB(2), 1, mainscreenuiTilesLen, f);
	fclose(f);
	f = fopen("nitro:/mainscreenui.map.bin", "rb");
	fread((void *)SCREEN_BASE_BLOCK_SUB(1), 1, mainscreenuiMapLen, f);
	fclose(f);

	int bg2 = bgInitSub(3, BgType_ExRotation, BgSize_ER_512x512, 3, 3);
	bgWrapOn(bg2);
	bgSetPriority(bg2, 2);
	f = fopen("nitro:/tilemap.img.bin", "rb");
	fread(bgGetGfxPtr(bg2), 1, tilemapTilesLen, f);
	fclose(f);
	dmaFillHalfWords(TILE_AIR, bgGetMapPtr(bg2), 64 * 64 * 2);

	bg2Map = (u16 *)bgGetMapPtr(bg2); // Make sure this is defined before using generateMap or setGameTerrain so it doesn't tap into an undefined pointer and freeze

	f = fopen("nitro:/bg.pal.bin", "rb");
	fread((void *)BG_PALETTE_SUB, 1, bgPalLen, f);
	fclose(f);

	int x = 0;

	while (1)
	{
		swiWaitForVBlank();
		mmStreamUpdate();

		REG_BG0HOFS_SUB = x / 2;

		scanKeys();
		int pressed = keysDown();
		if (pressed & KEY_TOUCH)
		{
			touchRead(&touch);
			if (touch.px >= 56 && touch.px <= 199 && touch.py >= 56 && touch.py <= 87)
			{
				mmStreamClose();
				clearPrint();
				generateMap();
				break;
			}
			else if (touch.px >= 56 && touch.px <= 199 && touch.py >= 104 && touch.py <= 135)
			{
				mmStreamClose();
				if (fatInitDefault())
				{
					if (loadMapFromFile("map.dat"))
					{
						clearPrint();
						printDirect("Map loaded successfully!");
					}
					else
					{
						clearPrint();
						printDirect("Error loading map!\nFallback to generating map...");
						generateMap();
					}
				}
				else
				{
					clearPrint();
					printDirect("Error: FatInitDefault failed!\nFallback to generating map...");
					generateMap();
				}
				break;
			}
		}
		x++;
	}
	fclose(audioFile);

	videoSetMode(MODE_0_2D | DISPLAY_BG0_ACTIVE | DISPLAY_BG1_ACTIVE | DISPLAY_BG3_ACTIVE);
	videoSetModeSub(MODE_3_2D | DISPLAY_BG0_ACTIVE | DISPLAY_BG3_ACTIVE);

	vramSetBankA(VRAM_A_MAIN_BG);
	vramSetBankB(VRAM_B_MAIN_SPRITE);
	vramSetBankC(VRAM_C_SUB_BG);	 // for backgrounds on subscreen
	vramSetBankD(VRAM_D_SUB_SPRITE); // for sprites on subscreen

	BGCTRL[0] = BG_TILE_BASE(1) | BG_MAP_BASE(0) | BG_COLOR_256 | BG_32x32 | BG_PRIORITY(3);
	f = fopen("nitro:/inv.img.bin", "rb");
	fread((void *)CHAR_BASE_BLOCK(1), 1, invTilesLen, f);
	fclose(f);
	dmaFillHalfWords(0, (void *)SCREEN_BASE_BLOCK(0), 2048);
	BGCTRL[1] = BG_TILE_BASE(2) | BG_MAP_BASE(2) | BG_COLOR_256 | BG_32x32 | BG_PRIORITY(2);
	f = fopen("nitro:/items.img.bin", "rb");
	fread((void *)CHAR_BASE_BLOCK(2), 1, itemsTilesLen, f);
	fclose(f);
	dmaFillHalfWords(63, (void *)SCREEN_BASE_BLOCK(2), 2048);

	f = fopen("nitro:/inv.pal.bin", "rb");
	fread((void *)BG_PALETTE, 1, invPalLen, f);
	fclose(f);

	oamInit(&oamMain, SpriteMapping_1D_128, false);

	inventorySelectionSprite = oamAllocateGfx(&oamMain, SpriteSize_32x32, SpriteColorFormat_256Color);
	f = fopen("nitro:/ui.img.bin", "rb");
	fread(inventorySelectionSprite, 1, uiTilesLen, f);
	fclose(f);

	f = fopen("nitro:/ui.pal.bin", "rb");
	fread((void *)SPRITE_PALETTE, 1, uiPalLen, f);
	fclose(f);

	oamInit(&oamSub, SpriteMapping_1D_128, false);

	u16 *nullSprite = oamAllocateGfx(&oamSub, SpriteSize_16x16, SpriteColorFormat_256Color);
	dmaFillHalfWords(0, nullSprite, 16 * 16);

	player.sprite_gfx_mem = oamAllocateGfx(&oamSub, SpriteSize_32x64, SpriteColorFormat_256Color);
	f = fopen("nitro:/sprites.img.bin", "rb");
	fread(player.sprite_gfx_mem, 1, 32 * 64, f);
	fclose(f);

	itemHandSprite = oamAllocateGfx(&oamSub, SpriteSize_16x16, SpriteColorFormat_256Color);

	f = fopen("nitro:/tilemap.img.bin", "rb");
	fseek(f, 0, SEEK_SET);
	fread(itemHandSprite, 1, 16 * 16, f);
	fclose(f);

	for (int i = 0; i < MAX_ITEMS; i++)
	{
		item[i].sprite_gfx_mem = oamAllocateGfx(&oamSub, SpriteSize_16x16, SpriteColorFormat_256Color);
		// No need to copy anything, we'll do that later when we summon the item
	}

	f = fopen("nitro:/sprites.pal.bin", "rb");
	fread((void *)SPRITE_PALETTE_SUB, 1, spritesPalLen, f);
	fclose(f);

	audioFile = fopen("nitro:/2.pcm", "rb");
	mmStreamOpen(&mystream);

	// Setup inventory
	inventorySetHotbar();
	giveInventory(ITEM_SWORD, 1);
	giveInventory(ITEM_AXE, 1);
	giveInventory(ITEM_PICKAXE, 1);
	setInventorySelection(0);

	while (pmMainLoop())
	{
		swiWaitForVBlank();
		mmStreamUpdate();
		scanKeys();
		int pressed = keysDown();
		int held = keysHeld();

		if (pressed & KEY_R)
		{
			if (inventoryOpen)
			{
				inventoryOpen = false;
				lcdMainOnTop();
				inventorySetHotbar();
			}
			else
			{
				inventoryOpen = true;
				lcdMainOnBottom();
				inventorySetFull();
			}
		}

		if (held & KEY_A && held & KEY_B && held & KEY_X && held & KEY_Y)
		{
			debug = !debug;
		}

		if (pressed & KEY_START)
			saveMapToFile("map.dat");
		if (pressed & KEY_SELECT)
			loadMapFromFile("map.dat");

		if (pressed & KEY_X)
		{
			if (inventorySelection < 8 * 4 - 1)
				setInventorySelection(inventorySelection + 1);
			else
				setInventorySelection(0);
		}

		if (pressed & KEY_Y)
		{
			if (inventorySelection > 0)
				setInventorySelection(inventorySelection - 1);
			else
				setInventorySelection(8 * 4 - 1);
		}

		if (held & KEY_DOWN)
			scale *= 1.01;
		if (held & KEY_UP)
			scale *= 0.99;

		if (scale > 256)
			scale = 256;
		if (scale < 128)
			scale = 128;

		// Draw player sprite
		if (pressed & KEY_A)
		{
			// Jumping logic
			if (player.isOnGround)
			{
				player.isJumping = true;
				player.velocity = -8; // Set a negative velocity for jumping
				player.isOnGround = false;
				player.animation = ANIM_JUMP;
			}
		}

		if (held & KEY_RIGHT)
		{
			if ((player.x + player.sizeX) / 8 < MAP_WIDTH)
			{
				if (!isTileSolid(gameTerrain[(player.x + player.sizeX) / 8 + player.y / 8 * MAP_WIDTH]) &&
					!isTileSolid(gameTerrain[(player.x + player.sizeX) / 8 + (player.y + player.sizeY / 2) / 8 * MAP_WIDTH]) &&
					!isTileSolid(gameTerrain[(player.x + player.sizeX) / 8 + (player.y + player.sizeY - 1) / 8 * MAP_WIDTH]))
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
				if (!isTileSolid(gameTerrain[(player.x - 1) / 8 + player.y / 8 * MAP_WIDTH]) &&
					!isTileSolid(gameTerrain[(player.x - 1) / 8 + (player.y + player.sizeY / 2) / 8 * MAP_WIDTH]) &&
					!isTileSolid(gameTerrain[(player.x - 1) / 8 + (player.y + player.sizeY - 1) / 8 * MAP_WIDTH]))
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
			if (isTileSolid(gameTerrain[player.x / 8 + (player.y + player.velocity) / 8 * MAP_WIDTH]))
			{
				player.velocity = 0;
				player.y /= 8;
				player.y *= 8;
			}
			if (isTileSolid(gameTerrain[(player.x + player.sizeX - 1) / 8 + (player.y + player.velocity) / 8 * MAP_WIDTH]))
			{
				player.velocity = 0;
				player.y /= 8;
				player.y *= 8;
			}
		}

		if (!player.isOnGround)
		{
			player.velocity += player.weight; // Apply gravity
			if (player.velocity > 7)
				player.velocity = 7;
			player.y += player.velocity;
			player.animation = ANIM_JUMP; // Fall and jump look the same
			if (player.y + player.sizeY > MAP_HEIGHT * 8)
			{
				player.y = MAP_HEIGHT * 8 - player.sizeY; // Prevent falling out of the map
				player.isOnGround = true;
				player.isJumping = false;
				player.velocity = 0; // Reset velocity when on the ground
			}
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

		int TLCtileX = TLCx / 8;
		int TLCtileY = TLCy / 8;
		int TRCtileX = TRCx / 8;
		int TRCtileY = TRCy / 8;
		int BLCtileX = BLCx / 8;
		int BLCtileY = BLCy / 8;
		int BRCtileX = BRCx / 8;
		int BRCtileY = BRCy / 8;

		if (inventoryOpen)
		{ // Inventory interaction on sub-screen
			if (pressed & KEY_TOUCH)
			{
				if (craftingOpen)
				{
					touchRead(&touch);
					for (int i = 0; i < 4 * 4; i++)
					{
						int x = (i % 4) * 4 * 8;
						int y = ((i / 4) * -4 + 20) * 8;
						if (touch.px >= x && touch.px < x + 32 && touch.py >= y && touch.py < y + 32)
						{
							if (i < sizeof(craftingRecipes) / sizeof(craftingRecipes[0]))
							{
								setCraftingSelection(i);
								renderCrafting();
								break;
							}
						}
					}
					if (touch.px >= 27 * 8 && touch.px < 31 * 8 && touch.py >= 0 * 8 && touch.py < 4 * 8) // player touched the crafting button
					{
						inventorySetFull();
					}
					if (touch.px >= 23 * 8 && touch.px < 31 * 8 && touch.py >= 20 * 8 && touch.py < 24 * 8) // player touched the Craft button
					{
						if (craftingSelection >= 0 && craftingSelection < sizeof(craftingRecipes) / sizeof(craftingRecipes[0]))
						{
							bool craftable = true;
							for (int i = 0; i < craftingRecipes[craftingSelection].ingredientCount; i++)
							{
								if (playerHasItem(craftingRecipes[craftingSelection].itemsNeeded[i], craftingRecipes[craftingSelection].itemsNeededQuantity[i]) == false)
								{
									craftable = false;
								}
							}
							if (craftable)
							{
								for (int i = 0; i < craftingRecipes[craftingSelection].ingredientCount; i++)
								{
									giveInventory(craftingRecipes[craftingSelection].itemsNeeded[i], -craftingRecipes[craftingSelection].itemsNeededQuantity[i]);
								}
								giveInventory(craftingRecipes[craftingSelection].item, craftingRecipes[craftingSelection].quantity);
								print(0, 1, "                                  ");
								print(0, 1, "Crafted ");
								printDirect(getElementName(craftingRecipes[craftingSelection].item));
							}
							else
							{
								print(0, 1, "                                  ");
								print(0, 1, "Oops! You don't have enough items!");
							}
						}
					}
				}
				else
				{
					touchRead(&touch);
					// Player is selecting an item from the inventory
					for (int i = 0; i < 8 * 4; i++)
					{
						int x = (i % 8) * 4 * 8;
						int y = ((i / 8) * -4 + 20) * 8;
						if (touch.px >= x && touch.px < x + 32 && touch.py >= y && touch.py < y + 32)
						{
							setInventorySelection(i);
							break;
						}
					}

					if (touch.px >= 27 * 8 && touch.px < 31 * 8 && touch.py >= 0 * 8 && touch.py < 4 * 8) // player touched the crafting button
					{
						inventorySetCrafting();
					}
				}
			}
			else if (held & KEY_TOUCH) // Player is moving an item
			{
				touchRead(&touch);
				int itemToMove = -1, destination = -1;
				for (int i = 0; i < 8 * 4; i++)
				{
					int x = (i % 8) * 4 * 8;
					int y = ((i / 8) * -4 + 20) * 8;
					if (touch.px >= x && touch.px < x + 32 && touch.py >= y && touch.py < y + 32)
					{
						itemToMove = i;
						break;
					}
				}
				while (held & KEY_TOUCH)
				{
					swiWaitForVBlank();
					mmStreamUpdate();
					scanKeys();
					held = keysHeld();
					if (held & KEY_TOUCH)
						touchRead(&touch);
				}
				for (int i = 0; i < 8 * 4; i++)
				{
					int x = (i % 8) * 4 * 8;
					int y = ((i / 8) * -4 + 20) * 8;
					if (touch.px >= x && touch.px < x + 32 && touch.py >= y && touch.py < y + 32)
					{
						destination = i;
						break;
					}
				}
				if (itemToMove != -1 && destination != -1 && itemToMove != destination && inventory[itemToMove] != 0)
				{
					if (inventory[itemToMove] == inventory[destination]) // If both items are the same
					{
						int totalQuantity = inventoryQuantity[itemToMove] + inventoryQuantity[destination];
						if (totalQuantity > 99) // Limit quantity to 99
						{
							setInventory(destination, inventory[itemToMove], 99);				 // Set destination to 99
							setInventory(itemToMove, inventory[itemToMove], totalQuantity - 99); // Set itemToMove to the remaining quantity
						}
						else
						{
							setInventory(destination, inventory[itemToMove], totalQuantity); // Combine quantities
							setInventory(itemToMove, 0, 0);									 // Clear the itemToMove slot
						}
					}
					else // If items are different
					{
						if (inventory[destination] == 0) // Just move the itemToMove to the destination
						{
							setInventory(destination, inventory[itemToMove], inventoryQuantity[itemToMove]);
							setInventory(itemToMove, 0, 0); // Clear the itemToMove slot
						}
						else // Swap items
						{
							int tempItem = inventory[destination];
							int tempQuantity = inventoryQuantity[destination];
							setInventory(destination, inventory[itemToMove], inventoryQuantity[itemToMove]);
							setInventory(itemToMove, tempItem, tempQuantity); // Swap the items
						}
					}
				}
			}
		}
		else
		{ // Game world interaction on sub-screen
			if (held & KEY_TOUCH)
			{
				touchRead(&touch);
				int dx = touch.px - SCREEN_WIDTH / 2;
				int dy = touch.py - SCREEN_HEIGHT / 2;

				int worldX = player.x + player.sizeX / 2 + (dx * scale) / 256;
				int worldY = player.y + player.sizeY / 2 + (dy * scale) / 256;

				if (worldX >= 0 && worldX < MAP_WIDTH * 8 && worldY >= 0 && worldY < MAP_HEIGHT * 8)
				{

					int worldTouchX = worldX / 8;
					int worldTouchY = worldY / 8;

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
									playerPutGameTerrain(worldTouchX, worldTouchY, inventory[inventorySelection]);
								}
								else
								{
									if (!isTileSolid(inventory[inventorySelection]))
										playerPutGameTerrain(worldTouchX, worldTouchY, inventory[inventorySelection]);
								}
							}
							else if (isElementWall(gameTerrain[worldTouchX + worldTouchY * MAP_WIDTH]) && gameTerrain[worldTouchX + worldTouchY * MAP_WIDTH] != inventory[inventorySelection])
							{
								breakTile(worldTouchX, worldTouchY, 1);
							}
						}
						else if (inventory[inventorySelection] >= 100 && inventory[inventorySelection] < 200 // Object is an item, not a tile
								 && inventoryQuantity[inventorySelection])
						{
							if (isToolCompatible(inventory[inventorySelection], gameTerrain[worldTouchX + worldTouchY * MAP_WIDTH])) // Check if the tool can break the tile
							{
								if (gameTerrain[worldTouchX + worldTouchY * MAP_WIDTH])
									breakTile(worldTouchX, worldTouchY, 1);
							}
						}
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
			player.y = BLCtileY * 8 - player.sizeY;
		}

		if (isTileSolid(gameTerrain[BRCtileX + BRCtileY * MAP_WIDTH]))
		{
			player.isOnGround = true;
			player.isJumping = false;
			player.velocity = 0; // Reset velocity when on the ground
			player.y = BRCtileY * 8 - player.sizeY;
		}

		if (!isTileSolid(gameTerrain[BLCtileX + BLCtileY * MAP_WIDTH]) && !isTileSolid(gameTerrain[BRCtileX + BRCtileY * MAP_WIDTH]))
		{
			player.isOnGround = false;
			player.isJumping = true;
		}

		scrollX = player.x - SCREEN_WIDTH / 2 + player.sizeX / 2;
		scrollY = player.y - SCREEN_HEIGHT / 2 + player.sizeY / 2;

		for (int i = 0; i < 64; i++)
		{
			if (item[i].exists)
			{
				// Item falls until it hits a tile
				// TODO: if item inside a tile, try to get it to the closest side with a non solid tile
				if (!isTileSolid(gameTerrain[item[i].x / 8 + (item[i].y + 8 + item[i].velocity) / 8 * MAP_WIDTH]))
				{
					item[i].velocity = 1;
				}
				else
				{
					item[i].velocity = 0;
				}
				// If item touches the player, remove it and set it to his inventory
				if (item[i].x >= player.x - 8 * 2 && item[i].x < player.x + player.sizeX + 8 * 2 && item[i].y >= player.y + 8 * 2 && item[i].y < player.y + player.sizeY + 8 * 2)
				{
					if (giveInventory(item[i].tile, item[i].quantity))
					{
						destroyItem(i); // Only destroy item entity if the player has enough inventory space
						mmEffect(SFX_RAB);
					}
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

				if (item[i].y > MAP_HEIGHT * 8)
				{
					destroyItem(i); // Remove item if it falls out of the map
					continue;
				}

				item[i].renderX = item[i].x - scrollX;
				item[i].renderY = item[i].y - scrollY;
			}
		}

		if (debug)
		{
			char buffer[4] = "    ";
			itoa(player.x / 8, buffer, 10);
			print(0, 0, "X: ");
			print(3, 0, buffer);
			itoa(player.y / 8, buffer, 10);
			print(0, 1, "Y: ");
			print(3, 1, buffer);
			itoa(chunk, buffer, 10);
			print(0, 2, "Chunk: ");
			print(7, 2, buffer);
			itoa(scrollX, buffer, 10);
			print(0, 3, "ScrollX: ");
			print(9, 3, buffer);
			itoa(scrollY, buffer, 10);
			print(0, 4, "ScrollY: ");
			print(9, 4, buffer);
			itoa(scale, buffer, 10);
			print(0, 5, "Scale: ");
			print(7, 5, buffer);
		}

		// Compute screen-relative render coordinates
		player.renderX = player.x - scrollX;
		player.renderY = player.y - scrollY;

		// Apply scroll
		bgSetCenter(bg2, player.renderX + player.sizeX / 2, player.renderY + player.sizeY / 2);
		bgSetScroll(bg2, scrollX + player.renderX + player.sizeX / 2, scrollY + player.renderY + player.sizeY / 2);
		bgSetScale(bg2, scale, scale);
		oamRotateScale(&oamSub, 0, degreesToAngle(0), scale * 2 * (player.isLookingLeft ? -1 : 1), scale * 2);
		REG_BG0HOFS_SUB = scrollX / 8;
		REG_BG0VOFS_SUB = scrollY / 8;

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

		// When the player reaches the 3rd quarter of the background, swap the first 4 rows in the bg with the next 4 rows in the map
		// so when it wraps it gives the illusion of continuity
		if (player.x > 512 / 8 * 5 + chunk * 32)
		{
			if (chunk < MAP_WIDTH / 4 - 1)
			{
				int newTile = (chunk % 16) * 4;
				for (int x = 0; x < 4; x++)
				{
					for (int y = 0; y < MAP_HEIGHT; y++)
					{
						Bg1SetTile(newTile + x, y, getElementTile(gameTerrain[64 + chunk * 4 + x + y * MAP_WIDTH], 64 + chunk * 4 + x, y));
					}
				}
				chunk++;
			}
		}
		else if (player.x < 512 / 8 * 3 + chunk * 32)
		{
			if (chunk > 0)
			{
				chunk--;
				int newTile = (chunk % 16) * 4;
				for (int x = 0; x < 4; x++)
				{
					for (int y = 0; y < MAP_HEIGHT; y++)
					{
						Bg1SetTile(newTile + x, y, getElementTile(gameTerrain[chunk * 4 + x + y * MAP_WIDTH], chunk * 4 + x, y));
					}
				}
			}
		}

		f = fopen("nitro:/sprites.img.bin", "rb");
		fseek(f, 32 * 64 * player.anim_frame, SEEK_SET);
		fread(player.sprite_gfx_mem, 1, 32 * 64, f);
		fclose(f);

		oamSet(&oamSub, 0, player.renderX - player.sizeX / 2, player.renderY - player.sizeY / 2, 1, 0, SpriteSize_32x64, SpriteColorFormat_256Color, player.sprite_gfx_mem, 0, false, false, player.isLookingLeft, false, false);
		// oamSet(&oamSub, 1, player.renderX, player.renderY, 0, 0, SpriteSize_16x16, SpriteColorFormat_256Color, itemHandSprite, -1, false, false, player.isLookingLeft, false, false);
		// Render dropped items
		u8 renderedItems = 0;

		for (int i = 0; i < 64; i++)
		{
			if (renderedItems < MAX_ITEMS)
			{
				if (item[i].exists)
				{
					item[i].renderX = item[i].x - scrollX - (player.x - item[i].x) * 256 / scale + player.x - item[i].x; // Adjust for player position
					item[i].renderY = item[i].y - scrollY - (player.y - item[i].y) * 256 / scale + player.y - item[i].y; // Adjust for player position
					if (item[i].renderX >= 0 && item[i].renderX < SCREEN_WIDTH && item[i].renderY >= 0 && item[i].renderY < SCREEN_HEIGHT)
					{
						oamRotateScale(&oamSub, renderedItems + 2, degreesToAngle(0), scale * 2, scale * 2);
						oamSet(&oamSub, renderedItems + 2, item[i].renderX - item[i].sizeX * 256 / scale, item[i].renderY - item[i].sizeY * 256 / scale, 0, 0, SpriteSize_16x16, SpriteColorFormat_256Color, item[i].sprite_gfx_mem, renderedItems + 2, false, false, false, false, false);
						renderedItems++;
					}
				}
			}
		}
		// Clean up used sprites
		for (int i = renderedItems; i < MAX_ITEMS; i++)
		{
			oamSet(&oamSub, i + 2, 0, 0, 0, 0, SpriteSize_8x8, SpriteColorFormat_256Color, nullSprite, -1, false, false, false, false, false);
		}

		oamUpdate(&oamSub);
		bgUpdate();

		frame++;
	}

	return 0;
}
