// Definitions

#pragma once // This is a cool thing I learned that allows me to not worry about including this file multiple times

#define VERSION "0.2alpha"

#include <nds.h>
#include <stdio.h>
#include <math.h>
#include <fat.h>
#include <maxmod9.h>
#include <stdlib.h>
#include <time.h>
#include <filesystem.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>

// Textures

#include "tilemap.h"
#include "bg.h"
#include "nbg.h"
#include "entities.h"
#include "sprites.h"
#include "inv.h"
#include "font.h"
#include "ui.h"
#include "von.h"
#include "items.h"
#include "intro.h"
#include "intro2.h"
#include "intro3.h"
#include "intro4.h"
#include "mainscreenbg.h"
#include "mainscreenbg2.h"
#include "mainscreenui.h"

// Soundbank

#include "soundbank.h"
#include "soundbank_bin.h"

#define MAP_WIDTH_MAX 1024
#define MAP_HEIGHT_MAX 256

#define DAY_LENGTH 600 // Day length in seconds

#define MAX_ITEMS 16       // Maximum items to be rendered
#define MAX_ITEMS_TOTAL 64 // Maximum items in the world at once
#define ENTITY_COUNT 16    // Maximum number of entities in the game at once

#define CHEST_COUNT 32     // Maximum number of chests in the game at once

#define MAX_ITEM_AGE 60 * 60 * 5 // 5 minutes

#define PLAYER_STATION_RANGE 8 // Radius to look for stations

#define TREE_CHANCE 3
#define MUSHROOM_CHANCE 4
#define MIN_TREE_HEIGHT 3
#define MAX_TREE_HEIGHT 9
#define MIN_GRASS_HEIGHT 0.2
#define MAX_GRASS_HEIGHT 0.3
#define MIN_STONE_HEIGHT 0.25
#define MAX_STONE_HEIGHT 0.4

#define MAX_SCALE 256
#define MIN_SCALE 128

#define INT_MAX 0x7fffffff

// Define game elements

enum // Make sure the indexing follows the one in the textures
{
    TILE_AIR,
    TILE_PLANKS,
    TILE_DIRT,
    TILE_STONE,
    TILE_WOODLOG,
    TILE_LEAVES,
    TILE_MUSHROOM,
    // Make sure to update isTileWall
    TILE_DIRT_WALL,
    TILE_STONE_WALL,
    TILE_DEMONITE_BRICK, // Indestructible brick
    TILE_WOOD_WALL,
    TILE_COPPER_ORE,
    // This sux and has to be improved
    TILE_WOODEN_DOOR_CLOSED_1,
    TILE_WOODEN_DOOR_CLOSED_2,
    TILE_WOODEN_DOOR_CLOSED_3,

    TILE_WOODEN_DOOR_OPEN_1,
    TILE_WOODEN_DOOR_OPEN_2,
    TILE_WOODEN_DOOR_OPEN_3,
    TILE_WOODEN_DOOR_OPEN_4,
    TILE_WOODEN_DOOR_OPEN_5,
    TILE_WOODEN_DOOR_OPEN_6,
    TILE_WOODEN_DOOR_OPEN_RIGHT_1,
    TILE_WOODEN_DOOR_OPEN_RIGHT_2,
    TILE_WOODEN_DOOR_OPEN_RIGHT_3,
    TILE_WOODEN_DOOR_OPEN_RIGHT_4,
    TILE_WOODEN_DOOR_OPEN_RIGHT_5,
    TILE_WOODEN_DOOR_OPEN_RIGHT_6,

    TILE_TIN_ORE,

    ITEM_COPPER_PICKAXE,
    ITEM_COPPER_AXE,
    ITEM_COPPER_LONGSWORD,
    ITEM_COPPER_HAMMER,

    ITEM_TIN_PICKAXE,
    ITEM_TIN_AXE,
    ITEM_TIN_LONGSWORD,
    ITEM_TIN_HAMMER,

    ITEM_COPPER_COIN,
    ITEM_SILVER_COIN,
    ITEM_GOLD_COIN,
    ITEM_PLATINUM_COIN,

    ITEM_GEL,

    TILE_SAND,
    TILE_SANDSTONE_WALL,
    TILE_HARDENED_SAND,

    TILE_SNOW,
    TILE_ICE,

    TILE_WORKBENCH_1,
    TILE_WORKBENCH_2,

    TILE_IRON_ORE,
    TILE_GOLD_ORE,

    TILE_FURNACE_1,
    TILE_FURNACE_2,
    TILE_FURNACE_3,
    TILE_FURNACE_4,
    TILE_FURNACE_5,
    TILE_FURNACE_6,

    ITEM_COPPER_BAR,
    ITEM_TIN_BAR,
    ITEM_IRON_BAR,
    ITEM_GOLD_BAR,

    TILE_ANVIL_1,
    TILE_ANVIL_2,

    ITEM_IRON_PICKAXE,
    ITEM_IRON_AXE,
    ITEM_IRON_LONGSWORD,
    ITEM_IRON_HAMMER,

    ITEM_GOLD_PICKAXE,
    ITEM_GOLD_AXE,
    ITEM_GOLD_LONGSWORD,
    ITEM_GOLD_HAMMER,

    ITEM_WOODEN_HAMMER,

    TILE_CHEST_1,
    TILE_CHEST_2,
    TILE_CHEST_3,
    TILE_CHEST_4,

    TILES,
};

#define ENTITY_SPRITESHEET_WIDTH 16

enum // Entities
{
    ENTITY_GREEN_SLIME,
    ENTITY_RED_SLIME,
    ENTITY_BLUE_SLIME,
    ENTITY_BUNNY,
    ENTITY_ZOMBIE,
    ENTITY_DEMON_EYE,
    ENTITY_CATARACT_EYE,
    ENTITY_SLIMED_ZOMBIE,
    ENTITY_SAND_SLIME,
    ENTITY_ICE_SLIME,
    ENTITY_FROZEN_ZOMBIE,

    ENTITIES, // This is a cursed technique I learned in a Dojo in Tokyo where it automatically defines the size of the enum
};

enum // Entity type
{
    ENTITY_TYPE_PASSIVE,
    ENTITY_TYPE_HOSTILE,
    ENTITY_TYPE_SPECIAL,
};

enum // Entity AI type
{
    ENTITY_AI_NONE,
    ENTITY_AI_EYE,
    ENTITY_AI_SLIME,
    ENTITY_AI_BUNNY,
    ENTITY_AI_ZOMBIE,
};

enum // Animations
{
    ANIM_NONE,
    ANIM_WALK,
    ANIM_JUMP,
};

enum // Biomes
{
    BIOME_FOREST,
    BIOME_DESERT,
    BIOME_SNOW,

    BIOMES, // Long live the C-monks
};

enum // Tool type
{
    TOOL_TYPE_PICKAXE,
    TOOL_TYPE_AXE,
    TOOL_TYPE_LONGSWORD,
    TOOL_TYPE_HAMMER,
};

enum // Special parameter for tiles
{
    SPECIAL_NONE,
    SPECIAL_DOOR,      // specialParams[] = {int isOpen (0-1), int isRight (0-1), int part (1-6)}
    SPECIAL_TREE,      // specialParams[] = {int leafTile}
    SPECIAL_WORKBENCH, // specialParams[] = {int part (1-2)}
    SPECIAL_FURNACE,   // specialParams[] = {int part (1-6)}
    SPECIAL_ANVIL,     // specialParams[] = {int part (1-2)}
    SPECIAL_CHEST,     // specialParams[] = {int part (1-4)}

    SPECIALS,
};