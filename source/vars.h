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

u8 stoneSurface[MAP_WIDTH_MAX] = {0};
u8 biomeSurface[MAP_WIDTH_MAX] = {0};
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
int craftingOffset = 0;
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
	"Using a copper pickaxe, you can break a Demonite tile after exactly 68 years",
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
	float angle;
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
	char name[32];
	int sizeX;
	int sizeY;
	bool isSolid;
	int weight;
	int health;
	int damage;
	int spriteSize;
	int type;
	int AItype;
	int dropCount;
	int drops[8];
	int dropChance[8];	 // 1/x chance (example: 100% is 1, 50% is 2, 25% is 4, etc...)
	int dropRange[8][2]; // [min, max] range for each drop
	int biomeCount;
	u8 biomes[8];
	bool isUnderground;
	bool isSurface;
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
	int atFrame;
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

typedef struct
{
	char name[32];
	bool isTileUnique; // If the tile doesn't change based un surrounding like mushrooms or air
	int tile;		   // if !isTileUnique, this value means the offset, else it's the real tile index
	int itemTile;
	int health;
	bool isDropUnique; // If !isDropUnique, the item will simple drop itself, else it'll drop the item specified in the drops array
	int drop;
	int toolCount; // how many types of tools can break this
	int tools[4];
	bool isSolid;
	bool isWall;
	bool isTool;
	int toolSpeed; // damage per frame to block
	int toolKnockBack;
	int toolType;
	int toolDamage;
	bool isItem; // To distinguish between non-placeable tiles and so
} TileProperties;

typedef struct
{
	char name[32];
	int surfaceTile;
	int surfaceWall;
	int undergroundTile;
	int undergroundWall;
} BiomeProperties;

// Define the player entity
Player player = {0, 0, 0, 0, 0, NULL, 16, 24, false, true, 1, 0, 0, true, false, 4, 100, 100, 0, 0, ANIM_NONE};

Entity entity[ENTITY_COUNT];

const EntityProperties entities[ENTITIES] = {
	{
		"Green Slime",
		16,
		12,
		true,
		1,
		14,
		6,
		SpriteSize_32x32,
		ENTITY_TYPE_HOSTILE,
		ENTITY_AI_SLIME,
		7,
		{ITEM_COPPER_COIN, TILE_COPPER_ORE, TILE_TIN_ORE, ITEM_COPPER_COIN, ITEM_SILVER_COIN, ITEM_GOLD_COIN, ITEM_GEL},
		{1, 32, 32, 12, 12, 12, 1},
		{{3, 3}, {3, 13}, {3, 13}, {50, 99}, {50, 99}, {1, 3}, {1, 2}},
		1,
		{BIOME_FOREST},
		false,
		true,
	},
	{
		"Red Slime",
		16,
		12,
		true,
		1,
		35,
		12,
		SpriteSize_32x32,
		ENTITY_TYPE_HOSTILE,
		ENTITY_AI_SLIME,
		7,
		{ITEM_COPPER_COIN, TILE_COPPER_ORE, TILE_TIN_ORE, ITEM_COPPER_COIN, ITEM_SILVER_COIN, ITEM_GOLD_COIN, ITEM_GEL},
		{1, 32, 32, 12, 12, 12, 1},
		{{8, 8}, {3, 13}, {3, 13}, {50, 99}, {50, 99}, {1, 3}, {2, 5}},
		1,
		{BIOME_FOREST},
		true,
		false,
	},
	{
		"Blue Slime",
		16,
		12,
		true,
		1,
		25,
		7,
		SpriteSize_32x32,
		ENTITY_TYPE_HOSTILE,
		ENTITY_AI_SLIME,
		7,
		{ITEM_COPPER_COIN, TILE_COPPER_ORE, TILE_TIN_ORE, ITEM_COPPER_COIN, ITEM_SILVER_COIN, ITEM_GOLD_COIN, ITEM_GEL},
		{1, 32, 32, 12, 12, 12, 1},
		{{25, 25}, {3, 13}, {3, 13}, {50, 99}, {50, 99}, {1, 3}, {1, 2}},
		1,
		{BIOME_FOREST},
		true,
		true,
	},
	{
		"Bunny",
		16,
		13,
		true,
		1,
		5,
		0,
		SpriteSize_32x32,
		ENTITY_TYPE_PASSIVE,
		ENTITY_AI_BUNNY,
		0,
		{},
		{},
		{},
		2,
		{BIOME_FOREST, BIOME_SNOW},
		false,
		true,
	},
	{
		"Zombie",
		16,
		24,
		true,
		1,
		45,
		14,
		SpriteSize_32x64,
		ENTITY_TYPE_HOSTILE,
		ENTITY_AI_ZOMBIE,
		1,
		{ITEM_COPPER_COIN},
		{1},
		{{60, 60}},
		1,
		{BIOME_FOREST},
		false,
		true,
	},
	{
		"Demon Eye",
		16,
		16,
		true,
		0,
		60,
		18,
		SpriteSize_32x32,
		ENTITY_TYPE_HOSTILE,
		ENTITY_AI_EYE,
		1,
		{ITEM_COPPER_COIN},
		{1},
		{{75, 75}},
		1,
		{BIOME_FOREST},
		false,
		true,
	},
	{
		"Cataract Eye",
		16,
		16,
		true,
		0,
		65,
		18,
		SpriteSize_32x32,
		ENTITY_TYPE_HOSTILE,
		ENTITY_AI_EYE,
		1,
		{ITEM_COPPER_COIN},
		{1},
		{{75, 75}},
		1,
		{BIOME_FOREST},
		false,
		true,
	},
	{
		"Slimed Zombie",
		16,
		24,
		true,
		1,
		40,
		13,
		SpriteSize_32x64,
		ENTITY_TYPE_HOSTILE,
		ENTITY_AI_ZOMBIE,
		2,
		{ITEM_COPPER_COIN, ITEM_GEL},
		{1, 1},
		{{55, 55}, {1, 3}},
		1,
		{BIOME_FOREST},
		false,
		true,
	},
	{
		"Sand Slime",
		16,
		12,
		true,
		1,
		50,
		15,
		SpriteSize_32x32,
		ENTITY_TYPE_HOSTILE,
		ENTITY_AI_SLIME,
		2,
		{ITEM_COPPER_COIN, ITEM_GEL},
		{1, 1},
		{{75, 75}, {1, 2}},
		1,
		{BIOME_DESERT},
		false,
		true,
	},
	{
		"Ice Slime",
		16,
		12,
		true,
		1,
		30,
		8,
		SpriteSize_32x32,
		ENTITY_TYPE_HOSTILE,
		ENTITY_AI_SLIME,
		2,
		{ITEM_COPPER_COIN, ITEM_GEL},
		{1, 1},
		{{50, 50}, {1, 2}},
		1,
		{BIOME_SNOW},
		false,
		true,
	},
	{
		"Frozen Zombie",
		16,
		24,
		true,
		1,
		50,
		16,
		SpriteSize_32x64,
		ENTITY_TYPE_HOSTILE,
		ENTITY_AI_ZOMBIE,
		1,
		{ITEM_COPPER_COIN},
		{1},
		{{80, 80}},
		1,
		{BIOME_SNOW},
		false,
		true,
	},
};

const TileProperties tileProperties[TILES] = {
	{"", true, 0, 56, 0, true, 0, 0, {}, false, false, false, 0, 0, -1, 0}, // TILE_AIR
	{"Planks", false, 4, 28, 100, false, 0, 1, {TOOL_TYPE_PICKAXE}, true, false, false, 0, 0, -1, 0, false},
	{"Dirt", false, 1, 16, 100, false, 0, 1, {TOOL_TYPE_PICKAXE}, true, false, false, 0, 0, -1, 0, false},
	{"Stone", false, 2, 20, 150, false, 0, 1, {TOOL_TYPE_PICKAXE}, true, false, false, 0, 0, -1, 0, false},
	{"Wood Log", true, 2, 56, 200, true, TILE_PLANKS, 1, {TOOL_TYPE_AXE}, false, false, false, 0, 0, -1, 0, false},
	{"Leaves", false, 3, 32, 50, false, 0, 1, {TOOL_TYPE_AXE}, false, false, false, 0, 0, -1, 0, false},
	{"Mushroom", true, 1, 24, 10, false, 0, 3, {TOOL_TYPE_PICKAXE, TOOL_TYPE_AXE, TOOL_TYPE_LONGSWORD}, false, false, false, 0, 0, -1, 0, false},
	{"Dirt Wall", false, 5, 36, 100, false, 0, 1, {TOOL_TYPE_HAMMER}, false, true, false, 0, 0, -1, 0, false},
	{"Stone Wall", false, 6, 40, 100, false, 0, 1, {TOOL_TYPE_HAMMER}, false, true, false, 0, 0, -1, 0, false},
	{"Demonite Brick", false, 7, 44, INT_MAX, false, 0, 1, {TOOL_TYPE_PICKAXE}, true, false, false, 0, 0, -1, 0, false},
	{"Wood Wall", false, 8, 48, 100, false, 0, 1, {TOOL_TYPE_HAMMER}, false, true, false, 0, 0, -1, 0, false},
	{"Copper Ore", false, 9, 52, 200, false, 0, 1, {TOOL_TYPE_PICKAXE}, true, false, false, 0, 0, -1, 0, false},

	{"", true, 92, 56, 50, true, TILE_WOODEN_DOOR_CLOSED_3, 1, {TOOL_TYPE_PICKAXE}, true, false, false, 0, 0, -1, 0, false},
	{"", true, 95, 56, 50, true, TILE_WOODEN_DOOR_CLOSED_3, 1, {TOOL_TYPE_PICKAXE}, true, false, false, 0, 0, -1, 0, false},
	{"Wooden Door", true, 98, 64, 50, true, TILE_WOODEN_DOOR_CLOSED_3, 1, {TOOL_TYPE_PICKAXE}, true, false, false, 0, 0, -1, 0, false},

	{"", true, 90, 56, 50, true, TILE_WOODEN_DOOR_CLOSED_3, 1, {TOOL_TYPE_PICKAXE}, false, false, false, 0, 0, -1, 0, false},
	{"", true, 91, 56, 50, true, TILE_WOODEN_DOOR_CLOSED_3, 1, {TOOL_TYPE_PICKAXE}, false, false, false, 0, 0, -1, 0, false},
	{"", true, 93, 56, 50, true, TILE_WOODEN_DOOR_CLOSED_3, 1, {TOOL_TYPE_PICKAXE}, false, false, false, 0, 0, -1, 0, false},
	{"", true, 94, 56, 50, true, TILE_WOODEN_DOOR_CLOSED_3, 1, {TOOL_TYPE_PICKAXE}, false, false, false, 0, 0, -1, 0, false},
	{"", true, 96, 56, 50, true, TILE_WOODEN_DOOR_CLOSED_3, 1, {TOOL_TYPE_PICKAXE}, false, false, false, 0, 0, -1, 0, false},
	{"", true, 97, 56, 50, true, TILE_WOODEN_DOOR_CLOSED_3, 1, {TOOL_TYPE_PICKAXE}, false, false, false, 0, 0, -1, 0, false},
	{"", true, 99, 56, 50, true, TILE_WOODEN_DOOR_CLOSED_3, 1, {TOOL_TYPE_PICKAXE}, false, false, false, 0, 0, -1, 0, false},
	{"", true, 100, 56, 50, true, TILE_WOODEN_DOOR_CLOSED_3, 1, {TOOL_TYPE_PICKAXE}, false, false, false, 0, 0, -1, 0, false},
	{"", true, 102, 56, 50, true, TILE_WOODEN_DOOR_CLOSED_3, 1, {TOOL_TYPE_PICKAXE}, false, false, false, 0, 0, -1, 0, false},
	{"", true, 103, 56, 50, true, TILE_WOODEN_DOOR_CLOSED_3, 1, {TOOL_TYPE_PICKAXE}, false, false, false, 0, 0, -1, 0, false},
	{"", true, 105, 56, 50, true, TILE_WOODEN_DOOR_CLOSED_3, 1, {TOOL_TYPE_PICKAXE}, false, false, false, 0, 0, -1, 0, false},
	{"", true, 106, 56, 50, true, TILE_WOODEN_DOOR_CLOSED_3, 1, {TOOL_TYPE_PICKAXE}, false, false, false, 0, 0, -1, 0, false},

	{"Tin Ore", false, 12, 68, 200, false, 0, 1, {TOOL_TYPE_PICKAXE}, true, false, false, 0, 0, -1, 0, false},

	{"Copper Pickaxe", true, 0, 4, 0, true, 0, 0, {}, false, false, true, 1, 2, TOOL_TYPE_PICKAXE, 4, true},
	{"Copper Axe", true, 0, 8, 0, true, 0, 0, {}, false, false, true, 1, 2, TOOL_TYPE_AXE, 4, true},
	{"Copper Long Sword", true, 0, 0, 0, true, 0, 0, {}, false, false, true, 1, 5, TOOL_TYPE_LONGSWORD, 8, true},
	{"Copper Hammer", true, 0, 12, 0, true, 0, 0, {}, false, false, true, 1, 1, TOOL_TYPE_HAMMER, 1, true},
	{"Tin Pickaxe", true, 0, 84, 0, true, 0, 0, {}, false, false, true, 2, 2, TOOL_TYPE_PICKAXE, 5, true},
	{"Tin Axe", true, 0, 88, 0, true, 0, 0, {}, false, false, true, 2, 2, TOOL_TYPE_AXE, 5, true},
	{"Tin Long Sword", true, 0, 80, 0, true, 0, 0, {}, false, false, true, 2, 5, TOOL_TYPE_LONGSWORD, 10, true},
	{"Tin Hammer", true, 0, 92, 0, true, 0, 0, {}, false, false, true, 2, 1, TOOL_TYPE_HAMMER, 1, true},
	{"Copper Coin", true, 0, 96, 0, true, 0, 0, {}, false, false, false, 0, 0, -1, 0, true},
	{"Silver Coin", true, 0, 100, 0, true, 0, 0, {}, false, false, false, 0, 0, -1, 0, true},
	{"Gold Coin", true, 0, 104, 0, true, 0, 0, {}, false, false, false, 0, 0, -1, 0, true},
	{"Platinum Coin", true, 0, 108, 0, true, 0, 0, {}, false, false, false, 0, 0, -1, 0, true},
	{"Gel", true, 0, 112, 0, true, 0, 0, {}, false, false, false, 0, 0, -1, 0, true},

	{"Sand", false, 13, 116, 100, false, 0, 1, {TOOL_TYPE_PICKAXE}, true, false, false, 0, 0, -1, 0, false},
	{"Sandstone Wall", false, 14, 120, 100, false, 0, 1, {TOOL_TYPE_HAMMER}, false, true, false, 0, 0, -1, 0, false},
	{"Hardened Sand", false, 15, 124, 150, false, 0, 1, {TOOL_TYPE_PICKAXE}, true, false, false, 0, 0, -1, 0, false},

	{"Snow", false, 16, 128, 100, false, 0, 1, {TOOL_TYPE_PICKAXE}, true, false, false, 0, 0, -1, 0, false},
	{"Ice", false, 17, 132, 150, false, 0, 1, {TOOL_TYPE_PICKAXE}, true, false, false, 0, 0, -1, 0, false},
};

const BiomeProperties biomes[BIOMES] = {
	{"Forest", TILE_DIRT, TILE_DIRT_WALL, TILE_STONE, TILE_STONE_WALL},
	{"Desert", TILE_SAND, TILE_SANDSTONE_WALL, TILE_HARDENED_SAND, TILE_SANDSTONE_WALL},
	{"Snow Biome", TILE_SNOW, TILE_DIRT_WALL, TILE_ICE, TILE_STONE_WALL},
};
// Define MAX_ITEMS_TOTAL slots for item entities
Item item[MAX_ITEMS_TOTAL];

// Define crafting recipes
const CraftingRecipe craftingRecipes[] = {
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
	{TILE_HARDENED_SAND, 1, 1, {TILE_SAND}, {2}},
	{TILE_SANDSTONE_WALL, 4, 1, {TILE_HARDENED_SAND}, {1}},
	{TILE_HARDENED_SAND, 1, 1, {TILE_SANDSTONE_WALL}, {4}},
	{TILE_ICE, 1, 1, {TILE_SNOW}, {2}},
	{TILE_SNOW, 2, 1, {TILE_ICE}, {1}},
};

u16 *bg2Map;
