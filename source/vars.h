// Game variables

#include "defs.h"

int frame = 0;
int scrollX = 0;
int scrollY = 0;
int scale = 128; // 128 is x2, 256 is x1, 512 is x0.5
int chunk = 0;

int gametime = 0;
int darkness = 0;

char worldFileName[64];

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
bool interacting;
int touchFrame;

const char *titleSplashes[] = {
	"Does this work?",
	"Definetly not inspired by a small game called Terraria?!",
	"Always patch the ROM kids, you don't want to meet him...",
	"bazinga",
	"Yeah no wonder they didn't make a version for the DS...",
	"Ain't that fantastic?",
	"By AzizBgBoss, for the DS community!",
	"67",
	"No braincells harmed while making this",
	"If this crashes, trust me, it’s a feature",
	"Now with 2 percent fewer bugs!",
	"Runs at a positive amount of FPS",
	"Built with duct tape and hope",
	"Tested on real hardware (once per leap year)",
	"Yeah we're good it works on my DS",
	"Blame libfat not me",
	"This took way too long",
	"67, again...",
	"What? You've never seen trees float before?",
	"Dig straight down for a (painful) surprise!",
	"DS homebrew never dies",
	"Powered by late nights and caffeine",
	"If you blow real hard under the shoulder buttons, they should work again (make sure any saliva doesn't get there).",
	"4 megabytes of RAM? How generous!",
	"Check the other games I made on my github!",
	"TerrariaDS would be honored to run on your 3DS",
	"May the long splash screen at the start of the game fix your fish-ass attention span",
	"17K downloads?! Thanks, I LOVE YOU!",
	"Based on code and prayers",
	"\"An idiot admires complexity, a genius admires simplicity\" - Probably Terry A. Davis",
	"First we Terraria, then we DS... Let's TerrariaDS",
	"No, no tutorial for you",
	"If terrain generation feels slow, look away, it will feel faster",
	"Impressively under 4MB for RAM",
	"The DS has a 67MHz CPU, lol 67 (I'm sorry I have to grow up)",
	"Procedural generation means I don’t know either",
	"TerrariaDS: legally distinct block mining building crafting fighting fishing simulator",
	"CPU fan for your DS? Yeah I'm not a fan (get it? lol)",
	"Implemented corruption! But not the Terraria one",
	"I'll fix it later (there's never later)",
	"Please don't sue me :(",
	"The comments are not only for you to understand, but for me to not make the same mistake again...",
};

u16 *nullSprite;

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
	int health;
	int maxHealth;
	int fall;
	int invincibilityFrames;
	u8 animation;
} Player;

typedef struct
{
	int type;
	bool exists;
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
	int health;
	int fall;
	u8 animation;
} Entity;

typedef struct
{
	int sizeX;
	int sizeY;
	bool isSolid;
	int weight;
	int health;
} EntityProperties;

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
Player player = {MAP_WIDTH * 8 / 2, 0, 0, 0, 0, NULL, 16, 24, false, true, 1, 0, true, false, 4, 100, 100, 0, 0, ANIM_NONE};

Entity entity[ENTITY_COUNT];

EntityProperties entities[3] = {
	{16, 12, true, 1, 20},
	{16, 12, true, 1, 30},
	{16, 12, true, 1, 100},
};

// Define MAX_ITEMS_TOTAL slots for item entities
Item item[MAX_ITEMS_TOTAL];

// Define crafting recipes
CraftingRecipe craftingRecipes[] = {
	{TILE_WOOD_WALL, 4, 1, {TILE_PLANKS}, {1}},
	{TILE_STONE_WALL, 4, 1, {TILE_STONE}, {1}},
	{TILE_DIRT_WALL, 4, 1, {TILE_DIRT}, {1}},
	{TILE_PLANKS, 1, 1, {TILE_WOOD_WALL}, {4}},
	{TILE_STONE, 1, 1, {TILE_STONE_WALL}, {4}},
	{TILE_DIRT, 1, 1, {TILE_DIRT_WALL}, {4}},
	{TILE_WOODEN_DOOR_CLOSED_3, 1, 1, {TILE_PLANKS}, {6}},
	{ITEM_TIN_PICKAXE, 1, 2, {TILE_PLANKS, TILE_TIN_ORE}, {1, 3}},
	{ITEM_TIN_AXE, 1, 2, {TILE_PLANKS, TILE_TIN_ORE}, {1, 3}},
	{ITEM_TIN_LONGSWORD, 1, 2, {TILE_PLANKS, TILE_TIN_ORE}, {1, 3}},
	{ITEM_TIN_HAMMER, 1, 2, {TILE_PLANKS, TILE_TIN_ORE}, {1, 1}},
	{ITEM_COPPER_PICKAXE, 1, 2, {TILE_PLANKS, TILE_COPPER_ORE}, {1, 3}},
	{ITEM_COPPER_AXE, 1, 2, {TILE_PLANKS, TILE_COPPER_ORE}, {1, 3}},
	{ITEM_COPPER_LONGSWORD, 1, 2, {TILE_PLANKS, TILE_COPPER_ORE}, {1, 3}},
	{ITEM_COPPER_HAMMER, 1, 2, {TILE_PLANKS, TILE_COPPER_ORE}, {1, 1}},
};

u16 *bg2Map;

FILE *audioFile;