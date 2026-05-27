// Game engine utilities

#include "defs.h"

int getElementTile(int tile, int x, int y) // Tile will change based on surrounding tiles
{
    int tileabove = (y > 0) ? gameTerrain[x + (y - 1) * MAP_WIDTH_MAX] : TILE_AIR;
    int tilebelow = (y < mapHeight - 1) ? gameTerrain[x + (y + 1) * MAP_WIDTH_MAX] : TILE_AIR;
    int tileleft = (x > 0) ? gameTerrain[x - 1 + y * MAP_WIDTH_MAX] : TILE_AIR;
    int tileright = (x < mapWidth - 1) ? gameTerrain[x + 1 + y * MAP_WIDTH_MAX] : TILE_AIR;

    int offset = 0;
    if (tileProperties[tile].isTileUnique)
    {
        return tileProperties[tile].tile;
    } else {
        offset = tileProperties[tile].tile;
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
    return tileProperties[item].itemTile;
}

const char *getElementName(int element)
{
    return tileProperties[element].name;
}

int getElementHealth(int element)
{
    return tileProperties[element].health;
}

int getElementDrop(int element)
{
    if (tileProperties[element].isDropUnique) {
        return tileProperties[element].drop;
    } else {
        return element;
    }
}

bool isToolCompatible(int tool, int tile)
{
    bool ok = false;
    for (int i = 0; i < tileProperties[tile].toolCount; i++)
    {
        if (tileProperties[tile].tools[i] == tileProperties[tool].toolType)
        {
            ok = true;
            break;
        }
    }
    return ok;
}

bool isTileSolid(int tile)
{
    return tileProperties[tile].isSolid;
}

bool isElementWall(int tile)
{
    return tileProperties[tile].isWall;
}

int getItemSpeed(int item)
{
    return tileProperties[item].toolSpeed;
}

int getItemDamage(int item)
{
    return tileProperties[item].toolDamage;
}

int getItemKnockback(int item)
{
    return tileProperties[item].toolKnockBack;
}

void changeTextBackground()
{
    BGCTRL[0] = BG_TILE_BASE(1) | BG_MAP_BASE(0) | BG_COLOR_256 | BG_32x32 | BG_PRIORITY(3);
    dmaCopy(invTiles, (void *)CHAR_BASE_BLOCK(1), invTilesLen);
    dmaFillHalfWords(0, (void *)SCREEN_BASE_BLOCK(0), 2048);
    Bg0UpFill(0);
};

int getPlayerItemQuantity(int item)
{
    int count = 0;
    for (int i = 0; i < 8 * 4; i++)
    {
        if (inventory[i] == item)
        {
            count += inventoryQuantity[i];
        }
    }
    return count;
}

void setInventorySelectionNoSound(u8 slot)
{
    slot = slot % (inventoryOpen ? 32 : 8);
    inventorySelection = inventoryOpen ? clamp(slot, 0, 31) : clamp(slot, 0, 7);
    int x = (inventorySelection % 8) * 4 * 8;
    int y = ((inventorySelection / 8) * -4 + 20) * 8;
    if (!craftingOpen)
    {
        if (inventoryOpen)
        {
            print(1, 7, "                              ");
            print(1, 7, getElementName(inventory[inventorySelection]));
        }
        else
        {
            print(1, 19, "                             ");
            print(1, 19, getElementName(inventory[inventorySelection]));
        }
        oamSet(&oamMain, 0, x, y, 0, 0, SpriteSize_32x32, SpriteColorFormat_256Color, inventorySelectionSprite, -1, false, false, false, false, false);
        oamUpdate(&oamMain);
    }
}

void setInventorySelection(u8 slot)
{
    setInventorySelectionNoSound(slot);
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
        print(16, 7, getElementName(craftingRecipes[slot + craftingOffset * 16].item));
        for (int i = 0; i < craftingRecipes[slot + craftingOffset * 16].ingredientCount; i++)
        {
            Bg1UpSetTile(16, 8 + i * 2, getItemTile(craftingRecipes[slot + craftingOffset * 16].itemsNeeded[i]) + 0);
            Bg1UpSetTile(16 + 1, 8 + i * 2, getItemTile(craftingRecipes[slot + craftingOffset * 16].itemsNeeded[i]) + 1);
            Bg1UpSetTile(16, 8 + 1 + i * 2, getItemTile(craftingRecipes[slot + craftingOffset * 16].itemsNeeded[i]) + 2);
            Bg1UpSetTile(16 + 1, 8 + 1 + i * 2, getItemTile(craftingRecipes[slot + craftingOffset * 16].itemsNeeded[i]) + 3);
            print(16 + 2, 8 + i * 2, "              ");
            print(16 + 2, 8 + i * 2, getElementName(craftingRecipes[slot + craftingOffset * 16].itemsNeeded[i]));
            print(16 + 2, 9 + i * 2, "              ");
            printVal(16 + 2, 9 + i * 2, getPlayerItemQuantity(craftingRecipes[slot + craftingOffset * 16].itemsNeeded[i]));
            printDirect("/");
            printValDirect(craftingRecipes[slot + craftingOffset * 16].itemsNeededQuantity[i]);
        }
        oamSet(&oamMain, 0, x, y, 0, 0, SpriteSize_32x32, SpriteColorFormat_256Color, inventorySelectionSprite, -1, false, false, false, false, false);
        oamUpdate(&oamMain);
    }
    mmEffect(SFX_ENU_TICK);
}

void renderInventoryNoSound()
{
    if (!craftingOpen)
    {
        Bg1UpFill(63);
        for (int i = 0; i < (inventoryOpen ? 8 * 4 : 8); i++) // Only show from the lowest 8 slots if using hotbar mode
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
                print((i % 8) * 4 + 1, (i / 8) * -4 + 23, "   ");
                if (inventoryQuantity[i] > 1)
                {
                    char buffer[3];
                    itoa(inventoryQuantity[i], buffer, 10);
                    print((i % 8) * 4 + 1, (i / 8) * -4 + 23, buffer);
                }
            }
        }
    }
}

void renderInventory()
{
    if (!craftingOpen)
    {
        renderInventoryNoSound();
        setInventorySelection(inventorySelection);
    }
};

void renderCrafting()
{
    if (craftingOpen)
    {
        Bg1UpFill(63);
        setCraftingSelection(craftingSelection);
        int tilesToRender = ((sizeof(craftingRecipes) / sizeof(craftingRecipes[0])) - craftingOffset * 16 <= 16)
                                ? ((sizeof(craftingRecipes) / sizeof(craftingRecipes[0])) - craftingOffset * 16)
                                : 16;
        for (int i = 0; i < tilesToRender; i++)
        {
            Bg1UpSetTile((i % 4) * 4 + 1, (i / 4) * -4 + 21, getItemTile(craftingRecipes[i + craftingOffset * 16].item) + 0);
            Bg1UpSetTile((i % 4) * 4 + 2, (i / 4) * -4 + 21, getItemTile(craftingRecipes[i + craftingOffset * 16].item) + 1);
            Bg1UpSetTile((i % 4) * 4 + 1, (i / 4) * -4 + 22, getItemTile(craftingRecipes[i + craftingOffset * 16].item) + 2);
            Bg1UpSetTile((i % 4) * 4 + 2, (i / 4) * -4 + 22, getItemTile(craftingRecipes[i + craftingOffset * 16].item) + 3);
            print((i % 4) * 4 + 1, (i / 4) * -4 + 23, "   ");
            if (craftingRecipes[i + craftingOffset * 16].quantity > 1)
            {
                printVal((i % 4) * 4 + 1, (i / 4) * -4 + 23, craftingRecipes[i + craftingOffset * 16].quantity);
            }
        }
        printVal(16, 19, craftingOffset + 1);
        printDirect("/");
        printValDirect(((sizeof(craftingRecipes) / sizeof(craftingRecipes[0])) / 16) + 1);
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
    lcdMainOnTop();
    inventoryOpen = false;
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
    print(0, 3, "TerrariaDS v" VERSION "\n\
By AzizBgBoss\n\
https://github.com/AzizBgBoss/TerrariaDS");
    renderInventory();
    mmEffect(SFX_ENU_CLOSE);
}

void inventorySetFull()
{
    lcdMainOnBottom();
    inventoryOpen = true;
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
    renderInventory();
    mmEffect(SFX_ENU_OPEN);
}

void inventorySetCrafting()
{
    lcdMainOnBottom();
    inventoryOpen = true;
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
        Bg0UpSetTile(16 + j % 4, 20 + j / 4, 96 + j);
    for (int j = 0; j < 16; j++)
        Bg0UpSetTile(20 + j % 4, 20 + j / 4, 112 + j);

    for (int j = 0; j < 16; j++)
        Bg0UpSetTile(24 + j % 4, 20 + j / 4, 64 + j);
    for (int j = 0; j < 16; j++)
        Bg0UpSetTile(28 + j % 4, 20 + j / 4, 80 + j);
    clearPrint();
    renderCrafting();
    mmEffect(SFX_ENU_OPEN);
}

void setGameTerrain(int x, int y, int tile)
{
    if (x < 0 || x >= mapWidth || y < 0 || y >= mapHeight)
        return;

    gameTerrain[x + y * MAP_WIDTH_MAX] = tile;

    int camTileX = (scrollX / 8) - 32 + 1;
    int camTileY = (scrollY / 8) - 32 + 1;

    // Check if tile is within the currently rendered 64x64 region
    int rx = x - camTileX;
    int ry = y - camTileY;
    if (rx < 0 || rx >= 64 || ry < 0 || ry >= 64)
        return;

    // Re-render tile and neighbors
    Bg1SetTile((rx + camTileX % 64) % 64, (ry + camTileY % 64) % 64, getElementTile(tile, x, y));
    if (rx > 0)
        Bg1SetTile((rx - 1 + camTileX % 64) % 64, (ry + camTileY % 64) % 64, getElementTile(gameTerrain[(x - 1) + y * MAP_WIDTH_MAX], x - 1, y));
    if (rx < 63)
        Bg1SetTile((rx + 1 + camTileX % 64) % 64, (ry + camTileY % 64) % 64, getElementTile(gameTerrain[(x + 1) + y * MAP_WIDTH_MAX], x + 1, y));
    if (ry > 0)
        Bg1SetTile((rx + camTileX % 64) % 64, (ry - 1 + camTileY % 64) % 64, getElementTile(gameTerrain[x + (y - 1) * MAP_WIDTH_MAX], x, y - 1));
    if (ry < 63)
        Bg1SetTile((rx + camTileX % 64) % 64, (ry + 1 + camTileY % 64) % 64, getElementTile(gameTerrain[x + (y + 1) * MAP_WIDTH_MAX], x, y + 1));
}

void playerPutGameTerrain(int x, int y, int tile)
{
    if (x < 0 || x >= mapWidth || y < 0 || y >= mapHeight)
        return;

    // The difference is that we check the surrounding tiles to prevent placing tiles out of thin air
    bool canPlace = false;
    if (tile == TILE_WOODEN_DOOR_CLOSED_3)
    {
        if (isTileSolid(gameTerrain[x + (y + 1) * MAP_WIDTH_MAX]) &&
            isTileSolid(gameTerrain[x + (y - 3) * MAP_WIDTH_MAX]))
        {
            if (!isTileSolid(gameTerrain[x + y * MAP_WIDTH_MAX]) &&
                !isTileSolid(gameTerrain[x + (y - 1) * MAP_WIDTH_MAX]) &&
                !isTileSolid(gameTerrain[x + (y - 2) * MAP_WIDTH_MAX]))
            {
                canPlace = true;
            }
        }
    }
    else
    {
        for (int dx = -1; dx <= 1; dx++)
        {
            for (int dy = -1; dy <= 1; dy++)
            {
                if (dx == 0 && dy == 0)
                    continue; // Skip the tile itself
                int nx = x + dx;
                int ny = y + dy;
                if (nx < 0 || nx >= mapWidth || ny < 0 || ny >= mapHeight)
                    continue; // Out of bounds
                if (gameTerrain[nx + ny * MAP_WIDTH_MAX] != TILE_AIR)
                {
                    canPlace = true;
                    break;
                }
            }
            if (canPlace)
                break;
        }
    }
    if (!canPlace)
        return; // Cannot place tile if no surrounding tiles are solid

    Bg1SetTile(x, y, getElementTile(tile, x, y));
    inventoryQuantity[inventorySelection]--;
    setInventory(inventorySelection, inventory[inventorySelection], inventoryQuantity[inventorySelection]);
    setGameTerrain(x, y, tile);
    if (tile == TILE_WOODEN_DOOR_CLOSED_3)
    {
        setGameTerrain(x, y - 1, TILE_WOODEN_DOOR_CLOSED_2);
        setGameTerrain(x, y - 2, TILE_WOODEN_DOOR_CLOSED_1);
    }
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

bool takeInventory(int item, int quantity)
{
    // Count total first to fail early
    int total = 0;
    for (int i = 0; i < 8 * 4; i++)
        if (inventory[i] == item)
            total += inventoryQuantity[i];
    if (total < quantity)
        return false; // Can't remove more than we have

    while (quantity > 0)
    {
        for (int i = 0; i < 8 * 4; i++)
        {
            int j = (item >= ITEM_COPPER_COIN && item <= ITEM_PLATINUM_COIN) ? (8 * 4 - 1 - i) : i;
            if (inventory[j] == item && inventoryQuantity[j] > 0)
            {
                int remove = min(inventoryQuantity[j], quantity);
                setInventory(j, item, inventoryQuantity[j] - remove);
                if (inventoryQuantity[j] == 0)
                    setInventory(j, 0, 0); // Clear slot
                quantity -= remove;
                if (quantity <= 0)
                    return true;
            }
        }
    }
    return true;
}

bool giveInventory(int item, int quantity)
{
    while (quantity > 0)
    {
        bool changed = false;

        // Fill existing stacks first
        for (int i = 0; i < 8 * 4; i++)
        {
            int j = (item >= ITEM_COPPER_COIN && item <= ITEM_PLATINUM_COIN) ? (8 * 4 - 1 - i) : i; // If it's a coin, start from the end for convenience
            if (inventory[j] == item && inventoryQuantity[j] < 100)
            {
                int add = min(100 - inventoryQuantity[j], quantity);

                setInventory(j, item, inventoryQuantity[j] + add);

                quantity -= add;
                changed = true;

                if (quantity <= 0)
                    return true;
            }
        }

        // Create new stacks
        for (int i = 0; i < 8 * 4; i++)
        {
            int j = (item >= ITEM_COPPER_COIN && item <= ITEM_PLATINUM_COIN) ? (8 * 4 - 1 - i) : i;
            if (inventory[j] == 0)
            {
                int add = min(100, quantity);

                setInventory(j, item, add);

                quantity -= add;
                changed = true;

                if (quantity <= 0)
                    return true;
            }
        }

        // No space left
        if (!changed)
            return false;
    }

    return true;
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
    if (quantity <= 0)
        return;
    if (x < 0 || x >= mapWidth || y < 0 || y >= mapHeight)
        return;
    int index = 0;
    while (item[index].exists)
        index++;
    // we can do item[index] = {...} directly but it'll get confusing
    item[index].x = x * 8 + 4; // Drop in the middle of the tile
    item[index].y = y * 8 + 4;
    item[index].exists = true;
    item[index].tile = tile;
    item[index].quantity = quantity;

    dmaCopy(((const u8 *)itemsTiles) + 8 * 8 * getItemTile(tile), item[index].sprite_gfx_mem, 16 * 16);
}

void destroyItem(int id)
{
    item[id].exists = false;
}

void breakTile(int x, int y, int speed)
{
    gameTerrainHealth[x + y * MAP_WIDTH_MAX] += speed;
    if (gameTerrainHealth[x + y * MAP_WIDTH_MAX] >= getElementHealth(gameTerrain[x + y * MAP_WIDTH_MAX]))
    {
        // Special blocks handling
        if (gameTerrain[x + y * MAP_WIDTH_MAX] == TILE_WOODLOG)
        {
            // Trees have special treatment: break all the logs and leaves above it
            for (int i = y - 1; i >= 0; i--)
            {
                if (gameTerrain[x + i * MAP_WIDTH_MAX] == TILE_WOODLOG)
                {
                    dropItem(x, i, getElementDrop(TILE_WOODLOG), 1);
                    setGameTerrain(x, i, 0);
                    gameTerrainHealth[x + i * MAP_WIDTH_MAX] = 0;
                }
                else if (gameTerrain[x + i * MAP_WIDTH_MAX] == TILE_LEAVES)
                { // Leaves don't drop anything if broken from tree
                    setGameTerrain(x, i, 0);
                    gameTerrainHealth[x + i * MAP_WIDTH_MAX] = 0;
                    if (gameTerrain[x - 1 + i * MAP_WIDTH_MAX] == TILE_LEAVES)
                    {
                        setGameTerrain(x - 1, i, 0);
                        gameTerrainHealth[x - 1 + i * MAP_WIDTH_MAX] = 0;
                    }
                    if (gameTerrain[x + 1 + i * MAP_WIDTH_MAX] == TILE_LEAVES)
                    {
                        setGameTerrain(x + 1, i, 0);
                        gameTerrainHealth[x + 1 + i * MAP_WIDTH_MAX] = 0;
                    }
                }
                else
                {
                    break; // Stop breaking when we hit a non-log or non-leaf tile
                }
            }
        }
        else if (gameTerrain[x + y * MAP_WIDTH_MAX] == TILE_WOODEN_DOOR_CLOSED_1)
        {
            setGameTerrain(x, y + 1, TILE_AIR);
            setGameTerrain(x, y + 2, TILE_AIR);
        }
        else if (gameTerrain[x + y * MAP_WIDTH_MAX] == TILE_WOODEN_DOOR_CLOSED_2)
        {
            setGameTerrain(x, y - 1, TILE_AIR);
            setGameTerrain(x, y + 1, TILE_AIR);
        }
        else if (gameTerrain[x + y * MAP_WIDTH_MAX] == TILE_WOODEN_DOOR_CLOSED_3)
        {
            setGameTerrain(x, y - 1, TILE_AIR);
            setGameTerrain(x, y - 2, TILE_AIR);
        }
        else if (gameTerrain[x + y * MAP_WIDTH_MAX] == TILE_WOODEN_DOOR_OPEN_1)
        {
            setGameTerrain(x + 1, y, TILE_AIR);
            setGameTerrain(x, y + 1, TILE_AIR);
            setGameTerrain(x + 1, y + 1, TILE_AIR);
            setGameTerrain(x, y + 2, TILE_AIR);
            setGameTerrain(x + 1, y + 2, TILE_AIR);
        }
        else if (gameTerrain[x + y * MAP_WIDTH_MAX] == TILE_WOODEN_DOOR_OPEN_2)
        {
            setGameTerrain(x - 1, y, TILE_AIR);
            setGameTerrain(x, y + 1, TILE_AIR);
            setGameTerrain(x - 1, y + 1, TILE_AIR);
            setGameTerrain(x, y + 2, TILE_AIR);
            setGameTerrain(x - 1, y + 2, TILE_AIR);
        }
        else if (gameTerrain[x + y * MAP_WIDTH_MAX] == TILE_WOODEN_DOOR_OPEN_3)
        {
            setGameTerrain(x, y - 1, TILE_AIR);
            setGameTerrain(x + 1, y - 1, TILE_AIR);
            setGameTerrain(x + 1, y, TILE_AIR);
            setGameTerrain(x, y + 1, TILE_AIR);
            setGameTerrain(x + 1, y + 1, TILE_AIR);
        }
        else if (gameTerrain[x + y * MAP_WIDTH_MAX] == TILE_WOODEN_DOOR_OPEN_4)
        {
            setGameTerrain(x - 1, y, TILE_AIR);
            setGameTerrain(x, y - 1, TILE_AIR);
            setGameTerrain(x - 1, y - 1, TILE_AIR);
            setGameTerrain(x, y + 1, TILE_AIR);
            setGameTerrain(x - 1, y + 1, TILE_AIR);
        }
        else if (gameTerrain[x + y * MAP_WIDTH_MAX] == TILE_WOODEN_DOOR_OPEN_5)
        {
            setGameTerrain(x, y - 2, TILE_AIR);
            setGameTerrain(x + 1, y - 2, TILE_AIR);
            setGameTerrain(x + 1, y - 1, TILE_AIR);
            setGameTerrain(x, y, TILE_AIR);
            setGameTerrain(x + 1, y, TILE_AIR);
        }
        else if (gameTerrain[x + y * MAP_WIDTH_MAX] == TILE_WOODEN_DOOR_OPEN_6)
        {
            setGameTerrain(x - 1, y - 2, TILE_AIR);
            setGameTerrain(x, y - 2, TILE_AIR);
            setGameTerrain(x - 1, y - 1, TILE_AIR);
            setGameTerrain(x, y, TILE_AIR);
            setGameTerrain(x - 1, y, TILE_AIR);
        }
        else if (gameTerrain[x + y * MAP_WIDTH_MAX] == TILE_WOODEN_DOOR_OPEN_RIGHT_1)
        {
            setGameTerrain(x + 1, y, TILE_AIR);
            setGameTerrain(x, y + 1, TILE_AIR);
            setGameTerrain(x + 1, y + 1, TILE_AIR);
            setGameTerrain(x, y + 2, TILE_AIR);
            setGameTerrain(x + 1, y + 2, TILE_AIR);
        }
        else if (gameTerrain[x + y * MAP_WIDTH_MAX] == TILE_WOODEN_DOOR_OPEN_RIGHT_2)
        {
            setGameTerrain(x - 1, y, TILE_AIR);
            setGameTerrain(x, y + 1, TILE_AIR);
            setGameTerrain(x - 1, y + 1, TILE_AIR);
            setGameTerrain(x, y + 2, TILE_AIR);
            setGameTerrain(x - 1, y + 2, TILE_AIR);
        }
        else if (gameTerrain[x + y * MAP_WIDTH_MAX] == TILE_WOODEN_DOOR_OPEN_RIGHT_3)
        {
            setGameTerrain(x, y - 1, TILE_AIR);
            setGameTerrain(x + 1, y - 1, TILE_AIR);
            setGameTerrain(x + 1, y, TILE_AIR);
            setGameTerrain(x, y + 1, TILE_AIR);
            setGameTerrain(x + 1, y + 1, TILE_AIR);
        }
        else if (gameTerrain[x + y * MAP_WIDTH_MAX] == TILE_WOODEN_DOOR_OPEN_RIGHT_4)
        {
            setGameTerrain(x - 1, y, TILE_AIR);
            setGameTerrain(x, y - 1, TILE_AIR);
            setGameTerrain(x - 1, y - 1, TILE_AIR);
            setGameTerrain(x, y + 1, TILE_AIR);
            setGameTerrain(x - 1, y + 1, TILE_AIR);
        }
        else if (gameTerrain[x + y * MAP_WIDTH_MAX] == TILE_WOODEN_DOOR_OPEN_RIGHT_5)
        {
            setGameTerrain(x, y - 2, TILE_AIR);
            setGameTerrain(x + 1, y - 2, TILE_AIR);
            setGameTerrain(x + 1, y - 1, TILE_AIR);
            setGameTerrain(x, y, TILE_AIR);
            setGameTerrain(x + 1, y, TILE_AIR);
        }
        else if (gameTerrain[x + y * MAP_WIDTH_MAX] == TILE_WOODEN_DOOR_OPEN_RIGHT_6)
        {
            setGameTerrain(x - 1, y - 2, TILE_AIR);
            setGameTerrain(x, y - 2, TILE_AIR);
            setGameTerrain(x - 1, y - 1, TILE_AIR);
            setGameTerrain(x, y, TILE_AIR);
            setGameTerrain(x - 1, y, TILE_AIR);
        }
        dropItem(x, y, getElementDrop(gameTerrain[x + y * MAP_WIDTH_MAX]), 1);
        setGameTerrain(x, y, TILE_AIR);
        gameTerrainHealth[x + y * MAP_WIDTH_MAX] = 0;
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

// FIXME: this feels inefficient, need to fix it
void interact(int x, int y)
{
    if (gameTerrain[x + y * MAP_WIDTH_MAX] == TILE_WOODEN_DOOR_CLOSED_1)
    {
        mmEffect(SFX_DOOR_OPEN);
        // Open to the left
        if (gameTerrain[x - 1 + y * MAP_WIDTH_MAX] == TILE_AIR &&
            gameTerrain[x - 1 + (y + 1) * MAP_WIDTH_MAX] == TILE_AIR &&
            gameTerrain[x - 1 + (y + 2) * MAP_WIDTH_MAX] == TILE_AIR)
        {
            setGameTerrain(x - 1, y, TILE_WOODEN_DOOR_OPEN_1);
            setGameTerrain(x, y, TILE_WOODEN_DOOR_OPEN_2);
            setGameTerrain(x - 1, y + 1, TILE_WOODEN_DOOR_OPEN_3);
            setGameTerrain(x, y + 1, TILE_WOODEN_DOOR_OPEN_4);
            setGameTerrain(x - 1, y + 2, TILE_WOODEN_DOOR_OPEN_5);
            setGameTerrain(x, y + 2, TILE_WOODEN_DOOR_OPEN_6);
        }
        // Open to the right
        else if (gameTerrain[x + 1 + y * MAP_WIDTH_MAX] == TILE_AIR &&
                 gameTerrain[x + 1 + (y + 1) * MAP_WIDTH_MAX] == TILE_AIR &&
                 gameTerrain[x + 1 + (y + 2) * MAP_WIDTH_MAX] == TILE_AIR)
        {
            setGameTerrain(x, y, TILE_WOODEN_DOOR_OPEN_RIGHT_1);
            setGameTerrain(x + 1, y, TILE_WOODEN_DOOR_OPEN_RIGHT_2);
            setGameTerrain(x, y + 1, TILE_WOODEN_DOOR_OPEN_RIGHT_3);
            setGameTerrain(x + 1, y + 1, TILE_WOODEN_DOOR_OPEN_RIGHT_4);
            setGameTerrain(x, y + 2, TILE_WOODEN_DOOR_OPEN_RIGHT_5);
            setGameTerrain(x + 1, y + 2, TILE_WOODEN_DOOR_OPEN_RIGHT_6);
        }
    }
    else if (gameTerrain[x + y * MAP_WIDTH_MAX] == TILE_WOODEN_DOOR_CLOSED_2)
    {
        mmEffect(SFX_DOOR_OPEN);
        if (gameTerrain[x - 1 + (y - 1) * MAP_WIDTH_MAX] == TILE_AIR &&
            gameTerrain[x - 1 + y * MAP_WIDTH_MAX] == TILE_AIR &&
            gameTerrain[x - 1 + (y + 1) * MAP_WIDTH_MAX] == TILE_AIR)
        {
            setGameTerrain(x - 1, y - 1, TILE_WOODEN_DOOR_OPEN_1);
            setGameTerrain(x, y - 1, TILE_WOODEN_DOOR_OPEN_2);
            setGameTerrain(x - 1, y, TILE_WOODEN_DOOR_OPEN_3);
            setGameTerrain(x, y, TILE_WOODEN_DOOR_OPEN_4);
            setGameTerrain(x - 1, y + 1, TILE_WOODEN_DOOR_OPEN_5);
            setGameTerrain(x, y + 1, TILE_WOODEN_DOOR_OPEN_6);
        }
        else if (gameTerrain[x + 1 + (y - 1) * MAP_WIDTH_MAX] == TILE_AIR &&
                 gameTerrain[x + 1 + y * MAP_WIDTH_MAX] == TILE_AIR &&
                 gameTerrain[x + 1 + (y + 1) * MAP_WIDTH_MAX] == TILE_AIR)
        {
            setGameTerrain(x, y - 1, TILE_WOODEN_DOOR_OPEN_RIGHT_1);
            setGameTerrain(x + 1, y - 1, TILE_WOODEN_DOOR_OPEN_RIGHT_2);
            setGameTerrain(x, y, TILE_WOODEN_DOOR_OPEN_RIGHT_3);
            setGameTerrain(x + 1, y, TILE_WOODEN_DOOR_OPEN_RIGHT_4);
            setGameTerrain(x, y + 1, TILE_WOODEN_DOOR_OPEN_RIGHT_5);
            setGameTerrain(x + 1, y + 1, TILE_WOODEN_DOOR_OPEN_RIGHT_6);
        }
    }
    else if (gameTerrain[x + y * MAP_WIDTH_MAX] == TILE_WOODEN_DOOR_CLOSED_3)
    {
        mmEffect(SFX_DOOR_OPEN);
        if (gameTerrain[x - 1 + (y - 2) * MAP_WIDTH_MAX] == TILE_AIR &&
            gameTerrain[x - 1 + (y - 1) * MAP_WIDTH_MAX] == TILE_AIR &&
            gameTerrain[x - 1 + y * MAP_WIDTH_MAX] == TILE_AIR)
        {
            setGameTerrain(x - 1, y - 2, TILE_WOODEN_DOOR_OPEN_1);
            setGameTerrain(x, y - 2, TILE_WOODEN_DOOR_OPEN_2);
            setGameTerrain(x - 1, y - 1, TILE_WOODEN_DOOR_OPEN_3);
            setGameTerrain(x, y - 1, TILE_WOODEN_DOOR_OPEN_4);
            setGameTerrain(x - 1, y, TILE_WOODEN_DOOR_OPEN_5);
            setGameTerrain(x, y, TILE_WOODEN_DOOR_OPEN_6);
        }
        else if (gameTerrain[x + 1 + (y - 2) * MAP_WIDTH_MAX] == TILE_AIR &&
                 gameTerrain[x + 1 + (y - 1) * MAP_WIDTH_MAX] == TILE_AIR &&
                 gameTerrain[x + 1 + y * MAP_WIDTH_MAX] == TILE_AIR)
        {
            setGameTerrain(x, y - 2, TILE_WOODEN_DOOR_OPEN_RIGHT_1);
            setGameTerrain(x + 1, y - 2, TILE_WOODEN_DOOR_OPEN_RIGHT_2);
            setGameTerrain(x, y - 1, TILE_WOODEN_DOOR_OPEN_RIGHT_3);
            setGameTerrain(x + 1, y - 1, TILE_WOODEN_DOOR_OPEN_RIGHT_4);
            setGameTerrain(x, y, TILE_WOODEN_DOOR_OPEN_RIGHT_5);
            setGameTerrain(x + 1, y, TILE_WOODEN_DOOR_OPEN_RIGHT_6);
        }
    }
    // Closing left-opening door
    else if (gameTerrain[x + y * MAP_WIDTH_MAX] == TILE_WOODEN_DOOR_OPEN_1)
    {
        mmEffect(SFX_DOOR_CLOSE);
        setGameTerrain(x, y, TILE_AIR);
        setGameTerrain(x, y + 1, TILE_AIR);
        setGameTerrain(x, y + 2, TILE_AIR);
        setGameTerrain(x + 1, y, TILE_WOODEN_DOOR_CLOSED_1);
        setGameTerrain(x + 1, y + 1, TILE_WOODEN_DOOR_CLOSED_2);
        setGameTerrain(x + 1, y + 2, TILE_WOODEN_DOOR_CLOSED_3);
    }
    else if (gameTerrain[x + y * MAP_WIDTH_MAX] == TILE_WOODEN_DOOR_OPEN_2)
    {
        mmEffect(SFX_DOOR_CLOSE);
        setGameTerrain(x - 1, y, TILE_AIR);
        setGameTerrain(x - 1, y + 1, TILE_AIR);
        setGameTerrain(x - 1, y + 2, TILE_AIR);
        setGameTerrain(x, y, TILE_WOODEN_DOOR_CLOSED_1);
        setGameTerrain(x, y + 1, TILE_WOODEN_DOOR_CLOSED_2);
        setGameTerrain(x, y + 2, TILE_WOODEN_DOOR_CLOSED_3);
    }
    else if (gameTerrain[x + y * MAP_WIDTH_MAX] == TILE_WOODEN_DOOR_OPEN_3)
    {
        mmEffect(SFX_DOOR_CLOSE);
        setGameTerrain(x, y - 1, TILE_AIR);
        setGameTerrain(x, y, TILE_AIR);
        setGameTerrain(x, y + 1, TILE_AIR);
        setGameTerrain(x + 1, y - 1, TILE_WOODEN_DOOR_CLOSED_1);
        setGameTerrain(x + 1, y, TILE_WOODEN_DOOR_CLOSED_2);
        setGameTerrain(x + 1, y + 1, TILE_WOODEN_DOOR_CLOSED_3);
    }
    else if (gameTerrain[x + y * MAP_WIDTH_MAX] == TILE_WOODEN_DOOR_OPEN_4)
    {
        mmEffect(SFX_DOOR_CLOSE);
        setGameTerrain(x - 1, y - 1, TILE_AIR);
        setGameTerrain(x - 1, y, TILE_AIR);
        setGameTerrain(x - 1, y + 1, TILE_AIR);
        setGameTerrain(x, y - 1, TILE_WOODEN_DOOR_CLOSED_1);
        setGameTerrain(x, y, TILE_WOODEN_DOOR_CLOSED_2);
        setGameTerrain(x, y + 1, TILE_WOODEN_DOOR_CLOSED_3);
    }
    else if (gameTerrain[x + y * MAP_WIDTH_MAX] == TILE_WOODEN_DOOR_OPEN_5)
    {
        mmEffect(SFX_DOOR_CLOSE);
        setGameTerrain(x, y - 2, TILE_AIR);
        setGameTerrain(x + 1, y - 2, TILE_AIR);
        setGameTerrain(x + 1, y - 1, TILE_AIR);
        setGameTerrain(x, y, TILE_AIR);
        setGameTerrain(x + 1, y, TILE_AIR);
    }
    else if (gameTerrain[x + y * MAP_WIDTH_MAX] == TILE_WOODEN_DOOR_OPEN_6)
    {
        mmEffect(SFX_DOOR_CLOSE);
        setGameTerrain(x - 1, y - 2, TILE_AIR);
        setGameTerrain(x, y - 2, TILE_AIR);
        setGameTerrain(x - 1, y - 1, TILE_AIR);
        setGameTerrain(x, y, TILE_AIR);
        setGameTerrain(x - 1, y, TILE_AIR);
    }
    // Closing right-opening door
    else if (gameTerrain[x + y * MAP_WIDTH_MAX] == TILE_WOODEN_DOOR_OPEN_RIGHT_1)
    {
        mmEffect(SFX_DOOR_CLOSE);
        setGameTerrain(x, y, TILE_WOODEN_DOOR_CLOSED_1);
        setGameTerrain(x, y + 1, TILE_WOODEN_DOOR_CLOSED_2);
        setGameTerrain(x, y + 2, TILE_WOODEN_DOOR_CLOSED_3);
        setGameTerrain(x + 1, y, TILE_AIR);
        setGameTerrain(x + 1, y + 1, TILE_AIR);
        setGameTerrain(x + 1, y + 2, TILE_AIR);
    }
    else if (gameTerrain[x + y * MAP_WIDTH_MAX] == TILE_WOODEN_DOOR_OPEN_RIGHT_2)
    {
        mmEffect(SFX_DOOR_CLOSE);
        setGameTerrain(x - 1, y, TILE_WOODEN_DOOR_CLOSED_1);
        setGameTerrain(x - 1, y + 1, TILE_WOODEN_DOOR_CLOSED_2);
        setGameTerrain(x - 1, y + 2, TILE_WOODEN_DOOR_CLOSED_3);
        setGameTerrain(x, y, TILE_AIR);
        setGameTerrain(x, y + 1, TILE_AIR);
        setGameTerrain(x, y + 2, TILE_AIR);
    }
    else if (gameTerrain[x + y * MAP_WIDTH_MAX] == TILE_WOODEN_DOOR_OPEN_RIGHT_3)
    {
        mmEffect(SFX_DOOR_CLOSE);
        setGameTerrain(x, y - 1, TILE_WOODEN_DOOR_CLOSED_1);
        setGameTerrain(x, y, TILE_WOODEN_DOOR_CLOSED_2);
        setGameTerrain(x, y + 1, TILE_WOODEN_DOOR_CLOSED_3);
        setGameTerrain(x + 1, y - 1, TILE_AIR);
        setGameTerrain(x + 1, y, TILE_AIR);
        setGameTerrain(x + 1, y + 1, TILE_AIR);
    }
    else if (gameTerrain[x + y * MAP_WIDTH_MAX] == TILE_WOODEN_DOOR_OPEN_RIGHT_4)
    {
        mmEffect(SFX_DOOR_CLOSE);
        setGameTerrain(x - 1, y - 1, TILE_WOODEN_DOOR_CLOSED_1);
        setGameTerrain(x - 1, y, TILE_WOODEN_DOOR_CLOSED_2);
        setGameTerrain(x - 1, y + 1, TILE_WOODEN_DOOR_CLOSED_3);
        setGameTerrain(x, y - 1, TILE_AIR);
        setGameTerrain(x, y, TILE_AIR);
        setGameTerrain(x, y + 1, TILE_AIR);
    }
    else if (gameTerrain[x + y * MAP_WIDTH_MAX] == TILE_WOODEN_DOOR_OPEN_RIGHT_5)
    {
        mmEffect(SFX_DOOR_CLOSE);
        setGameTerrain(x, y - 2, TILE_WOODEN_DOOR_CLOSED_1);
        setGameTerrain(x, y - 1, TILE_WOODEN_DOOR_CLOSED_2);
        setGameTerrain(x, y, TILE_WOODEN_DOOR_CLOSED_3);
        setGameTerrain(x + 1, y - 2, TILE_AIR);
        setGameTerrain(x + 1, y - 1, TILE_AIR);
        setGameTerrain(x + 1, y, TILE_AIR);
    }
    else if (gameTerrain[x + y * MAP_WIDTH_MAX] == TILE_WOODEN_DOOR_OPEN_RIGHT_6)
    {
        mmEffect(SFX_DOOR_CLOSE);
        setGameTerrain(x - 1, y - 2, TILE_WOODEN_DOOR_CLOSED_1);
        setGameTerrain(x, y - 2, TILE_WOODEN_DOOR_CLOSED_2);
        setGameTerrain(x - 1, y - 1, TILE_WOODEN_DOOR_CLOSED_3);
        setGameTerrain(x, y, TILE_AIR);
        setGameTerrain(x - 1, y, TILE_AIR);
    }
}

int spawnEntity(int type, int x, int y)
{
    int i = 0;
    while (i < ENTITY_COUNT && entity[i].exists) // Check count first other wise it will overflow
        i++;
    if (i >= ENTITY_COUNT)
        return -1; // No available entity slot
    if (x < 0 || x >= mapWidth * 8 || y < 0 || y >= mapHeight * 8)
        return -1;

    entity[i].type = type;
    entity[i].exists = true;
    entity[i].angle = 0;
    entity[i].x = x;
    entity[i].y = y;
    entity[i].renderX = 0;
    entity[i].renderY = 0;
    entity[i].anim_frame = 0;
    entity[i].sizeX = entities[type].sizeX;
    entity[i].sizeY = entities[type].sizeY;
    entity[i].isJumping = false;
    entity[i].isSolid = entities[type].isSolid;
    entity[i].weight = entities[type].weight;
    entity[i].velocity = 0;
    entity[i].velocityX = 0;
    entity[i].isOnGround = true;
    entity[i].isLookingLeft = false;
    entity[i].health = entities[type].health;
    entity[i].fall = 0;
    entity[i].animation = ANIM_NONE;
    entity[i].nextTick = 0;

    int offset = 0;
    for (int i = 0; i < type; i++)
    {
        if (entities[i].spriteSize == SpriteSize_32x64)
            offset++; // Since 32x64 sprites use two columns
    }

    if (entities[type].spriteSize == SpriteSize_32x32)
    {
        entity[i].sprite_gfx_mem = oamAllocateGfx(&oamSub, SpriteSize_32x32, SpriteColorFormat_256Color);
        dmaCopy(((const u8 *)entitiesTiles) + entity[i].type * 32 * 32 + 32 * 32 * offset, entity[i].sprite_gfx_mem, 32 * 32);
    }
    else if (entities[type].spriteSize == SpriteSize_32x64)
    {
        entity[i].sprite_gfx_mem = oamAllocateGfx(&oamSub, SpriteSize_32x64, SpriteColorFormat_256Color);
        dmaCopy(((const u8 *)entitiesTiles) + entity[i].type * 32 * 32 + 32 * 32 * offset, entity[i].sprite_gfx_mem, 32 * 64);
    }

    return i;
}

void removeEntity(int id)
{
    entity[id].exists = false;
}

void killEntity(int id)
{
    removeEntity(id);
    const EntityProperties *e = &entities[entity[id].type]; // Pointers are so sexy
    if (e->dropCount > 0)
    {
        for (int i = 0; i < e->dropCount; i++)
        {
            if (rando(1, e->dropChance[i]) == 1)
                dropItem(entity[id].x / 8 + rando(-2, 2), entity[id].y / 8 + rando(-2, 2), e->drops[i],
                         rando(e->dropRange[i][0], e->dropRange[i][1]));
        }
    }
}

bool checkPlayerCollision(int x, int y, int sizeX, int sizeY)
{
    // AABB collision detection
    if (player.x < x + sizeX &&
        player.x + player.sizeX > x &&
        player.y < y + sizeY &&
        player.y + player.sizeY > y)
    {
        return true;
    }
    return false;
}

int detectEntity(int x, int y)
{
    for (int i = 0; i < ENTITY_COUNT; i++)
    {
        if (entity[i].exists)
        {
            // AABB collision detection
            if (x < entity[i].x + entity[i].sizeX &&
                x >= entity[i].x &&
                y < entity[i].y + entity[i].sizeY &&
                y >= entity[i].y)
            {
                return i;
            }
        }
    }
    return -1;
}

void damageEntity(int id, int damage)
{
    if (entity[id].exists)
    {
        mmEffect(SFX_HIT);
        entity[id].health -= damage;
    }
}

int getHighestTileYFrom(int x, int yy)
{

    if (x < 0 || x >= mapWidth)
        return mapHeight;
    for (int y = yy; y < mapHeight; y++)
    {
        if (isTileSolid(gameTerrain[x + y * MAP_WIDTH_MAX]))
        {
            return y;
        }
    }
    // else
    for (int y = 0; y < yy; y++)
    {
        if (isTileSolid(gameTerrain[x + y * MAP_WIDTH_MAX]))
        {
            return y;
        }
    }
    return mapHeight; // No solid tile found
}

int getHighestTileY(int x)
{
    return getHighestTileYFrom(x, 0);
}

void setPlayerAnimFrame(int frame)
{
    if (frame == player.anim_frame)
        return;
    if (frame < 0)
        frame = 0;
    player.anim_frame = frame;
    dmaCopy(((const u8 *)spritesTiles) + 32 * 64 * player.anim_frame, player.sprite_gfx_mem, 32 * 64);
}

void setEntityAnimFrame(int id, int frame)
{
    if (frame == entity[id].anim_frame)
        return;
    if (frame < 0)
        frame = 0;
    entity[id].anim_frame = frame;

    int offset = 0;
    for (int i = 0; i < entity[id].type; i++)
    {
        if (entities[i].spriteSize == SpriteSize_32x64)
            offset++; // Since 32x64 sprites use two columns
    }

    if (entities[entity[id].type].spriteSize == SpriteSize_32x32)
    {
        dmaCopy(((const u8 *)entitiesTiles) + offset * 32 * 32 + entity[id].type * 32 * 32 + entity[id].anim_frame * 32 * 32 * ENTITY_SPRITESHEET_WIDTH, entity[id].sprite_gfx_mem, 32 * 32);
    }
    else if (entities[entity[id].type].spriteSize == SpriteSize_32x64)
    {
        dmaCopy(((const u8 *)entitiesTiles) + offset * 32 * 32 + entity[id].type * 32 * 32 + entity[id].anim_frame * 32 * 32 * ENTITY_SPRITESHEET_WIDTH, entity[id].sprite_gfx_mem, 32 * 64);
    }
}

void delay(int seconds)
{
    int targetFrame = frame + seconds * 60; // Assuming 60 FPS
    while (frame < targetFrame)
    {
        frame++;
        swiWaitForVBlank();
        mmStreamUpdate();
    }
}

// I honestly wanted to make a struct of SaveData but i had some problems with its stack or whatever it is

bool saveMapToFile(const char *filen)
{
    mmEffect(SFX_ENU_CLOSE);

    if (mkdir("terrarias/", 0777) == -1 && errno != EEXIST)
    {
        print(0, 0, "Failed to create directory 'terrarias/'");
        return false;
    }

    char filename[128];
    snprintf(filename, sizeof(filename), "terrarias/%s", filen);

    FILE *file = fopen(filename, "wb"); // "wb" = write binary
    if (!file)
    {
        print(0, 0, "Failed to open file for writing: ");
        printDirect(filename);
        return false;
    }

    print(0, 0, "Saving map...");
    uint32_t magic = 0x550B12A2; // This magic number is for v0.2+ worlds
    size_t bytesWritten = fwrite(&magic, 1, 4, file);
    bytesWritten += fwrite(&mapWidth, 1, 4, file);
    bytesWritten += fwrite(&mapHeight, 1, 4, file);
    bytesWritten += fwrite(stoneSurface, 1, sizeof(stoneSurface), file);
    bytesWritten += fwrite(biomeSurface, 1, sizeof(biomeSurface), file);
    bytesWritten += fwrite(gameTerrain, 1, sizeof(gameTerrain), file);
    bytesWritten += fwrite(inventory, 1, sizeof(inventory), file);
    bytesWritten += fwrite(inventoryQuantity, 1, sizeof(inventoryQuantity), file);
    fclose(file);

    if (bytesWritten != 4 + 4 + 4 + sizeof(stoneSurface) + sizeof(biomeSurface) + sizeof(gameTerrain) + sizeof(inventory) + sizeof(inventoryQuantity))
    {
        print(0, 0, "Map save error");
        return false;
    }

    print(0, 0, "Map saved to ");
    printDirect(filename);
    return true;
}

bool loadMapFromFile(const char *filen)
{
    mmEffect(SFX_ENU_OPEN);

    char filename[128];
    snprintf(filename, sizeof(filename), "terrarias/%s", filen);

    FILE *file = fopen(filename, "rb"); // "rb" = read binary
    if (!file)
    {
        print(0, 0, "Failed to open file for reading: ");
        printDirect(filename);
        return false;
    }

    print(0, 0, "Loading map...");
    uint32_t magic;
    size_t bytesRead = fread(&magic, 1, 4, file);
    if (magic == 0xA212B055) // Try to load old world
    {
        clearPrint();
        printSmart(0, 0, "Old world format detected, attempting to load..."); // Old worlds sizes are 1024x64
        delay(3);
        bytesRead += fread(gameTerrain, 1, 1024 * 64 * sizeof(u8), file);
        bytesRead += fread(inventory, 1, sizeof(inventory), file);
        bytesRead += fread(inventoryQuantity, 1, sizeof(inventoryQuantity), file);
        fclose(file);
        mapWidth = 1024;
        mapHeight = 64;
        memset(stoneSurface,(int) 64 * MAX_STONE_HEIGHT, sizeof(stoneSurface));
        memset(biomeSurface, BIOME_FOREST, sizeof(biomeSurface));
        for (int i = 0; i < 8 * 4; i++)
        {
            if (inventory[i] >= 100)
                inventory[i] -= 73; // New version removes the 100 offset of items
        }

        if (bytesRead != 4 + 1024 * 64 * sizeof(u8) + sizeof(inventory) + sizeof(inventoryQuantity))
        {
            print(0, 0, "Map load error");
            return false;
        }

        clearPrint();
        printSmart(0, 0, "Old world loaded successfully! Consider saving it in the new format to avoid issues in the future.\n\nEven if backwards compatibility was implemented, this world can still experience some issues.");
        delay(5);
    }
    else
    {
        if (magic != 0x550B12A2)
        {
            print(0, 0, "Invalid map file: ");
            printDirect(filename);
            fclose(file);
            return false;
        }
        bytesRead += fread(&mapWidth, 1, 4, file);
        bytesRead += fread(&mapHeight, 1, 4, file);
        bytesRead += fread(stoneSurface, 1, sizeof(stoneSurface), file);
        bytesRead += fread(biomeSurface, 1, sizeof(biomeSurface), file);
        bytesRead += fread(gameTerrain, 1, sizeof(gameTerrain), file);
        bytesRead += fread(inventory, 1, sizeof(inventory), file);
        bytesRead += fread(inventoryQuantity, 1, sizeof(inventoryQuantity), file);
        fclose(file);

        if (bytesRead != 4 + 4 + 4 + sizeof(stoneSurface) + sizeof(biomeSurface) + sizeof(gameTerrain) + sizeof(inventory) + sizeof(inventoryQuantity))
        {
            print(0, 0, "Map load error");
            return false;
        }
    }

    player.x = mapWidth * 8 / 2;
    player.y = 0;
    player.invincibilityFrames = 60 * 3;
    lastCamTileX = -67;
    lastCamTileY = -67;
    renderInventory();
    renderCrafting();
    print(0, 0, "Map loaded from ");
    printDirect(filename);

    strcpy(worldFileName, filen);

    return true;
}

void playerDamage(int damage)
{
    if (player.invincibilityFrames > 0)
        return; // Player is invincible
    damage = clamp(damage, 0, 400);
    player.health -= damage;
    if (player.health < 0)
        player.health = 0; // Virtually should never happen but just in case
    switch (rando(0, 2))
    {
    case 0:
        mmEffect(SFX_LAYER_HIT_0);
        break;
    case 1:
        mmEffect(SFX_LAYER_HIT_1);
        break;
    case 2:
        mmEffect(SFX_LAYER_HIT_2);
        break;
    }
    if (player.health == 0)
    { // Player is ded
        mmEffect(SFX_LAYER_KILLED);
        inventorySetHotbar();
        clearPrint();
        print(0, 0, "You died lol");
        oamSet(&oamSub, 0, 0, 0, 0, 0, SpriteSize_8x8, SpriteColorFormat_256Color, nullSprite, -1, false, false, false, false, false);
        oamUpdate(&oamSub);
        for (int i = 0; i < 600; i++)
        {
            printVal(0, 1, 10 - i / 60);
            printDirect(" ");
            swiWaitForVBlank();
            mmStreamUpdate();
        }
        clearPrint();
        player.x = mapWidth * 8 / 2;
        player.y = 0;
        player.velocityX = 0;
        player.velocity = 0;
        player.health = 100;
        player.invincibilityFrames = 300; // 5 seconds of invincibility after respawn
        inventorySetHotbar();
    }
    else
    {
        player.invincibilityFrames = 120; // 2 seconds of invincibility
    }
}

void playerHeal(int health)
{
    health = clamp(health, 0, 400);
    player.health += health;
    if (player.health > player.maxHealth)
        player.health = player.maxHealth; // Cap at max health
}

void generateWorldName(char *nameBuffer, size_t bufferSize)
{
    const char *adjectives[] = {
        "Lush", "Dark", "Misty", "Silent", "Ancient",
        "Frozen", "Golden", "Hidden", "Enchanted", "Whispering",
        "Vast", "Forgotten", "Crimson", "Radiant", "Broken",
        "Twisted", "Sacred", "Forsaken", "Distant", "Wild",
        "Echoing", "Burning", "Lost", "Shimmering", "Fading"};

    const char *nouns[] = {
        "Valley", "Forest", "Mountain", "River", "Cavern",
        "Island", "Plains", "Hills", "Swamp", "Desert",
        "House", "Home", "Temple", "Ruins", "Depths",
        "Labyrinth", "Peak", "Marsh", "Glacier", "Wasteland",
        "Underworld", "Skylands", "Borderlands", "Outpost", "Sanctum"};

    const char *ofPhrases[] = {
        "Doom", "Mystery", "Shadows", "Light", "Wonders",
        "Echoes", "Dreams", "Legends", "Secrets", "Magic",
        "Happiness", "Joy", "Mischief", "Adventure", "Misery",
        "Despair", "Hope", "Chaos", "Order", "Silence",
        "Storms", "Time", "Flames", "Dust", "Stars", "Triple T",
        "Nothing", "Everything", "The Void", "Forgotten Kings", "Endings"};

    int adjIndex = rando(0, sizeof(adjectives) / sizeof(adjectives[0]) - 1);
    int nounIndex = rando(0, sizeof(nouns) / sizeof(nouns[0]) - 1);
    int ofIndex = rando(0, sizeof(ofPhrases) / sizeof(ofPhrases[0]) - 1);

    snprintf(nameBuffer, bufferSize, "%s %s of %s",
             adjectives[adjIndex],
             nouns[nounIndex],
             ofPhrases[ofIndex]);
}

void generateMap()
{
    u8 grassSurface[mapWidth];
    int seed = rando(0, 99999999);

    generateWorldName(worldFileName, sizeof(worldFileName));

    printDirect("Seed: ");
    printValDirect(seed);
    printDirect("\nGenerating ");
    printDirect(worldFileName);
    printDirect("...\n");
    strcat(worldFileName, ".ter");

    int idx = 0;
    while (idx < mapWidth)
    {
        int biome = rando(0, BIOMES - 1);
        int len = rando(32, mapWidth - idx);
        for (int i = 0; i < len && idx < mapWidth; i++)
        {
            biomeSurface[idx++] = biome;
        }
    }

    for (int x = 0; x < mapWidth; x++)
    {
        float wave = fractalPerlin1D(x, 4, 0.4f, 0.01f, seed) * 20.0f;
        grassSurface[x] = clamp((int)(wave + (MIN_GRASS_HEIGHT * mapHeight + MAX_GRASS_HEIGHT * mapHeight) / 2), MIN_GRASS_HEIGHT * mapHeight, MAX_GRASS_HEIGHT * mapHeight);
        if (x % (mapWidth / 32) == 0)
        {
            printDirect(".");
        }
    }

    // remove 1 block spikes, i fucking hate them
    printDirect("Removing spikes because they are annoying...\n");
    printDirect("."); // It needs one more lmao lol xd ong
    for (int x = 1; x < mapWidth - 1; x++)
    {
        if (grassSurface[x - 1] != grassSurface[x] && grassSurface[x + 1] != grassSurface[x])
        {
            grassSurface[x] += 1;
        }
        if (x % (mapWidth / 32) == 0)
        {
            printDirect(".");
        }
    }

    // Generate stone height surface
    printDirect("Generating stone surface...\n");
    for (int x = 0; x < mapWidth; x++)
    {
        float wave = fractalPerlin1D(x, 4, 0.4f, 0.01f, seed + 1) * 20.0f;
        stoneSurface[x] = clamp((int)(wave + (MIN_STONE_HEIGHT * mapHeight + MAX_STONE_HEIGHT * mapHeight) / 2), MIN_STONE_HEIGHT * mapHeight, MAX_STONE_HEIGHT * mapHeight);
        if (x % (mapWidth / 32) == 0)
        {
            printDirect(".");
        }
    }

    // Place terrain
    printDirect("Placing terrain...\n");
    for (int x = 0; x < mapWidth; x++)
    {
        for (int y = 0; y < mapHeight; y++)
        {
            if (y >= grassSurface[x] && y < stoneSurface[x])
            {
                setGameTerrain(x, y, biomes[biomeSurface[x]].surfaceTile);
            }
            else if (y >= stoneSurface[x])
            {
                setGameTerrain(x, y, biomes[biomeSurface[x]].undergroundTile);
            }
        }
        if (x % (mapWidth / 32) == 0)
        {
            printDirect(".");
        }
    }

    // Generate ores
    printDirect("Generating ores...\n");
    for (int x = 0; x < mapWidth; x++)
    {
        for (int y = 0; y < mapHeight; y++)
        {
            if (y >= stoneSurface[x])
            {
                float caveNoise = fractalPerlin2D(x, y, 6, 0.4f, 0.06f, seed + 69);
                if (caveNoise < -0.2f) // Adjust this threshold to control ores density
                {
                    setGameTerrain(x, y, TILE_COPPER_ORE);
                }
                else if (caveNoise > 0.2f)
                {
                    setGameTerrain(x, y, TILE_TIN_ORE);
                }
            }
        }
        if (x % (mapWidth / 32) == 0)
        {
            printDirect(".");
        }
    }

    // Generate caves
    printDirect("Generating caves...\n");
    for (int x = 0; x < mapWidth; x++)
    {
        for (int y = 0; y < mapHeight; y++)
        {
            if (y >= grassSurface[x])
            {
                float caveNoise = fractalPerlin2D(x, y, 6, 0.4f, 0.02f, seed);
                if (caveNoise < -0.15f) // Adjust this threshold to control cave density
                {
                    setGameTerrain(x, y, TILE_AIR); // Create a cave
                }
            }
        }
        if (x % (mapWidth / 32) == 0)
        {
            printDirect(".");
        }
    }

    // Adding walls
    printDirect("Adding walls...\n");
    for (int x = 0; x < mapWidth; x++)
    {
        for (int y = 0; y < mapHeight; y++)
        {
            if (gameTerrain[x + y * MAP_WIDTH_MAX] == TILE_AIR)
            {
                if (y >= grassSurface[x] && y < stoneSurface[x])
                {
                    setGameTerrain(x, y, biomes[biomeSurface[x]].surfaceWall);
                }
                else if (y >= stoneSurface[x])
                {
                    setGameTerrain(x, y, biomes[biomeSurface[x]].undergroundWall);
                }
            }
        }
        if (x % (mapWidth / 32) == 0)
        {
            printDirect(".");
        }
    }

    // Place trees
    printDirect("Placing trees...\n");
    for (int x = 1; x < mapWidth - 1; x++)
    {
        if (rando(1, TREE_CHANCE) == 1 && gameTerrain[x + (grassSurface[x] + 1) * MAP_WIDTH_MAX] == TILE_DIRT && biomeSurface[x] == BIOME_FOREST)
        {
            int tree_height = rando(MIN_TREE_HEIGHT, MAX_TREE_HEIGHT);
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
        if (x % (mapWidth / 32) == 0)
        {
            printDirect(".");
        }
    }

    // Place mushrooms
    printDirect("Placing mushrooms...\n");
    for (int x = 1; x < mapWidth - 1; x++)
    {
        if (rando(1, MUSHROOM_CHANCE) == 1 && gameTerrain[x + grassSurface[x] * MAP_WIDTH_MAX] == TILE_DIRT && gameTerrain[x + (grassSurface[x] - 1) * MAP_WIDTH_MAX] == TILE_AIR && biomeSurface[x] == BIOME_FOREST)
        {
            setGameTerrain(x, grassSurface[x] - 1, TILE_MUSHROOM);
        }
        if (x % (mapWidth / 32) == 0)
        {
            printDirect(".");
        }
    }

    // Place demonite bricks to limit the world
    printDirect("Placing demonite bricks at the bottom so you don't escape >:) ...\n");
    for (int x = 0; x < mapWidth - 1; x++)
    {
        setGameTerrain(x, mapHeight - 1, TILE_DEMONITE_BRICK);
        if (x % (mapWidth / 32) == 0)
        {
            printDirect(".");
        }
    }

    // Clean Inventory

    for (int i = 0; i < 8 * 4; i++)
    {
        inventory[i] = TILE_AIR;
        inventoryQuantity[i] = 0;
    }

    player.x = mapWidth * 8 / 2;
    player.y = 0;
    player.health = 100;
    player.maxHealth = 100;
    player.invincibilityFrames = 300;
    gametime = 0;

    for (int i = 0; i < ENTITY_COUNT; i++)
    {
        removeEntity(i);
    }

    for (int i = 0; i < MAX_ITEMS; i++)
    {
        destroyItem(i);
    }

    printDirect("Giving you some tools to start with...");
    giveInventory(ITEM_COPPER_LONGSWORD, 1);
    giveInventory(ITEM_COPPER_AXE, 1);
    giveInventory(ITEM_COPPER_PICKAXE, 1);
}

bool setStreamAudio(const char *path)
{
    if (audioRom == NULL)
        audioRom = nitroromGetSelf();

    audioReady = false;
    audioPosition = 0;

    if (audioRom == NULL)
        return false;

    int fileId = nitroromResolvePath(audioRom, NITROROM_ROOT_DIR, path);
    if (fileId < 0)
        return false;

    audioFileId = fileId;
    audioLength = nitroromGetFileSize(audioRom, audioFileId);
    audioReady = audioLength > 0;
    return audioReady;
}

mm_word on_stream_request(mm_word length, mm_addr dest, mm_stream_formats format)
{
    uint8_t *target = (uint8_t *)dest;

    if (!audioReady)
    {
        memset(target, 128, length);
        return length;
    }

    size_t bytesLeft = audioLength - audioPosition;
    size_t bytesToCopy = length < bytesLeft ? length : bytesLeft;

    if (!nitroromReadFile(audioRom, audioFileId, audioPosition, target, bytesToCopy))
    {
        memset(target, 128, length);
        audioPosition = 0;
        return length;
    }

    audioPosition += bytesToCopy;

    if (bytesToCopy < length)
    {
        memset(target + bytesToCopy, 128, length - bytesToCopy);
        audioPosition = 0;
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

void knockBackPlayer(int x, int y)
{
    if (player.invincibilityFrames)
        return; // Don't knock back if player is [TITLE CARD]
    player.velocityX += x;
    player.velocity += y;
}

void knockBackEntity(int id, int x, int y)
{
    if (entities[entity[id].type].AItype == ENTITY_AI_EYE)
        entity[id].angle = -entity[id].angle; // Reverse direction
    entity[id].velocityX += x;
    entity[id].velocity += y;
}

void changeEntityAngle(int id, float a)
{
    float diff = a - entity[id].angle;
    while (diff > M_PI)
        diff -= 2 * M_PI;
    while (diff < -M_PI)
        diff += 2 * M_PI;

    if (fabsf(diff) < DEG2RAD(5))
        entity[id].angle = a;
    else
        entity[id].angle += (diff > 0) ? DEG2RAD(5) : DEG2RAD(-5);
}

void changeEntityVelocityX(int id, int x)
{
    if (x > 0)
    {
        if (entity[id].velocityX < x)
            entity[id].velocityX++;
    }
    else
    {
        if (entity[id].velocityX > x)
            entity[id].velocityX--;
    }
}

void changeEntityVelocityY(int id, int y)
{
    if (y > 0)
    {
        if (entity[id].velocity < y)
            entity[id].velocity++;
    }
    else
    {
        if (entity[id].velocity > y)
            entity[id].velocity--;
    }
}

bool isInPlayerRadius(int x, int y, float range)
{
    return (player.x - x) * (player.x - x) + (player.y - y) * (player.y - y) < range * range;
}

bool isInPlayerRange(int x, int y)
{
    return isInPlayerRadius(x, y, SCREEN_WIDTH / 2);
}