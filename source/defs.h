// Definitions

#pragma once // This is a cool thing I learned that allows me to not worry about including this file multiple times

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

// Textures

#include "tilemap.h"
#include "bg.h"
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

#define MAP_WIDTH 1024
#define MAP_HEIGHT 64

#define DAY_LENGTH 600 // Day length in seconds

#define MAX_ITEMS 16    // Maximum items to be rendered
#define MAX_ITEMS_TOTAL 64 // Maximum items in the world at once
#define ENTITY_COUNT 16 // Maximum number of entities in the game at once

#define TREE_CHANCE 10
#define MUSHROOM_CHANCE 10
#define MIN_GRASS_HEIGHT 12
#define MAX_GRASS_HEIGHT 20
#define MIN_STONE_HEIGHT 17
#define MAX_STONE_HEIGHT 25

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

    ITEM_COPPER_PICKAXE = 101,
    ITEM_COPPER_AXE,
    ITEM_COPPER_LONGSWORD,
    ITEM_COPPER_HAMMER,

    ITEM_TIN_PICKAXE,
    ITEM_TIN_AXE,
    ITEM_TIN_LONGSWORD,
    ITEM_TIN_HAMMER
};

enum // Entities
{
    ENTITY_GREEN_SLIME,
    ENTITY_RED_SLIME,
    ENTITY_BLUE_SLIME,
};

enum // Animations
{
    ANIM_NONE,
    ANIM_WALK,
    ANIM_JUMP,
};