// Game variables

#include "defs.h"

int frame = 0;
int scrollX = 0;
int scrollY = 0;
int scale = 128; // 128 is x2, 256 is x1, 512 is x0.5
int lastCamTileX = -67;
int lastCamTileY = -67;

int mapWidth = 1024, mapHeight = 64;

int gametime = 0;
int darkness = 0;

char worldFileName[64];

u8 gameTerrain[MAP_WIDTH_MAX * MAP_HEIGHT_MAX] = {0};
u16 gameTerrainHealth[MAP_WIDTH_MAX * MAP_HEIGHT_MAX] = {0};

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
	"If this crashes, trust me, it's a feature",
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
	"Procedural generation means I don't know either",
	"TerrariaDS: legally distinct block mining building crafting fighting fishing simulator",
	"CPU fan for your DS? Yeah I'm not a fan (get it? lol)",
	"Implemented corruption! But not the Terraria one",
	"I'll fix it later (there's never later)",
	"Please don't sue me :(",
	"The comments are not only for you to understand, but for me to not make the same mistake again...",
};

u16 *nullSprite;

FILE *f;

NitroRom *audioRom = NULL;
u16 audioFileId = 0;
u32 audioLength = 0;
u32 audioPosition = 0;
bool audioReady = false;

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
	int velocityX;		 // Velocity for horizontal movement
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
	int x, y; // Entity position
	int renderX, renderY;
	int anim_frame;		 // Animation frame
	u16 *sprite_gfx_mem; // Pointer to sprite graphics memory
	int sizeX, sizeY;	 // Size of the sprite, may be used for collision detection
	bool isJumping;		 // Is the entity jumping
	bool isSolid;		 // Is the entity solid (can collide with other objects)
	int weight;			 // Zero if not affected by gravity
	int velocity;		 // Velocity for jumping or falling
	int velocityX;		 // Velocity for horizontal movement
	bool isOnGround;	 // Is the entity on the ground
	bool isLookingLeft;
	int health;
	int fall;
	u8 animation;
	int nextTick;
} Entity;

typedef struct
{
	int sizeX;
	int sizeY;
	bool isSolid;
	int weight;
	int health;
	int damage;
	int spriteSize;
	int type;
	int dropCount;
	int drops[8];
	int dropChance[8];	 // 1/x chance (example: 100% is 1, 50% is 2, 25% is 4, etc...)
	int dropRange[8][2]; // [min, max] range for each drop
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
Player player = {0, 0, 0, 0, 0, NULL, 16, 24, false, true, 1, 0, 0, true, false, 4, 100, 100, 0, 0, ANIM_NONE};

Entity entity[ENTITY_COUNT];

EntityProperties entities[ENTITIES] = {
	{
		16,
		12,
		true,
		1,
		14,
		6,
		SpriteSize_32x32,
		ENTITY_TYPE_HOSTILE,
		6,
		{ITEM_COPPER_COIN, TILE_COPPER_ORE, TILE_TIN_ORE, ITEM_COPPER_COIN, ITEM_SILVER_COIN, ITEM_GOLD_COIN},
		{1, 32, 32, 12, 12, 12},
		{{3, 3}, {3, 13}, {3, 13}, {50, 99}, {50, 99}, {1, 3}},
	}, // Green slime
	{
		16,
		12,
		true,
		1,
		35,
		12,
		SpriteSize_32x32,
		ENTITY_TYPE_HOSTILE,
		6,
		{ITEM_COPPER_COIN, TILE_COPPER_ORE, TILE_TIN_ORE, ITEM_COPPER_COIN, ITEM_SILVER_COIN, ITEM_GOLD_COIN},
		{1, 32, 32, 12, 12, 12},
		{{8, 8}, {3, 13}, {3, 13}, {50, 99}, {50, 99}, {1, 3}},
	}, // Red slime
	{
		16,
		12,
		true,
		1,
		100,
		67,
		SpriteSize_32x32,
		ENTITY_TYPE_SPECIAL,
		1,
		{TILE_MUSHROOM},
		{1},
		{{1, 1}},
	}, // Blue slime
	{
		16,
		13,
		true,
		1,
		5,
		0,
		SpriteSize_32x32,
		ENTITY_TYPE_PASSIVE,
		0,
		{},
		{},
		{},
	}, // Bunny
	{
		16,
		24,
		true,
		1,
		45,
		14,
		SpriteSize_32x64,
		ENTITY_TYPE_HOSTILE,
		1,
		{ITEM_COPPER_COIN},
		{1},
		{{60, 60}},
	}, // Zombie
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
