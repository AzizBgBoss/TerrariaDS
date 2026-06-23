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
    }
    else
    {
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
    if (tileProperties[element].isDropUnique)
    {
        return tileProperties[element].drop;
    }
    else
    {
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
            print(1, 7, getElementName(currentInventory[inventorySelection]));
        }
        else
        {
            print(1, 19, "                             ");
            print(1, 19, getElementName(currentInventory[inventorySelection]));
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

void setCraftingSelectionNoSound(u8 slot)
{
    craftingOffset = clamp(craftingOffset, 0, craftableRecipesCount / 16);
    slot = clamp(slot, 0, clamp(craftableRecipesCount - craftingOffset - 1, 0, 15));
    craftingSelection = slot;
    int x = (slot % 4) * 4 * 8;
    int y = ((slot / 4) * -4 + 20) * 8;
    if (craftingOpen)
    {
        clearPrint();
        if (craftableRecipesCount > 0)
        {
            print(0, 7, "              ");
            print(0, 7, getElementName(craftingRecipes[craftableRecipes[slot + craftingOffset * 16]].item));
            for (int i = 0; i < craftingRecipes[craftableRecipes[slot + craftingOffset * 16]].ingredientCount; i++)
            {
                Bg1UpSetTile(16, 8 + i * 2, getItemTile(craftingRecipes[craftableRecipes[slot + craftingOffset * 16]].itemsNeeded[i]) + 0);
                Bg1UpSetTile(16 + 1, 8 + i * 2, getItemTile(craftingRecipes[craftableRecipes[slot + craftingOffset * 16]].itemsNeeded[i]) + 1);
                Bg1UpSetTile(16, 8 + 1 + i * 2, getItemTile(craftingRecipes[craftableRecipes[slot + craftingOffset * 16]].itemsNeeded[i]) + 2);
                Bg1UpSetTile(16 + 1, 8 + 1 + i * 2, getItemTile(craftingRecipes[craftableRecipes[slot + craftingOffset * 16]].itemsNeeded[i]) + 3);
                print(16 + 2, 8 + i * 2, "              ");
                print(16 + 2, 8 + i * 2, getElementName(craftingRecipes[craftableRecipes[slot + craftingOffset * 16]].itemsNeeded[i]));
                print(16 + 2, 9 + i * 2, "              ");
                printVal(16 + 2, 9 + i * 2, getPlayerItemQuantity(craftingRecipes[craftableRecipes[slot + craftingOffset * 16]].itemsNeeded[i]));
                printDirect("/");
                printValDirect(craftingRecipes[craftableRecipes[slot + craftingOffset * 16]].itemsNeededQuantity[i]);
            }
            oamSet(&oamMain, 0, x, y, 0, 0, SpriteSize_32x32, SpriteColorFormat_256Color, inventorySelectionSprite, -1, false, false, false, false, false);
        }
        else
        {
            oamSet(&oamMain, 0, -32, -32, 0, 0, SpriteSize_32x32, SpriteColorFormat_256Color, inventorySelectionSprite, -1, false, false, false, false, false);
        }
        oamUpdate(&oamMain);
    }
}

void setCraftingSelection(u8 slot)
{
    setCraftingSelectionNoSound(slot);
    mmEffect(SFX_ENU_TICK);
}

void renderInventoryNoSound()
{
    if (!craftingOpen)
    {
        Bg1UpFill(63);
        for (int i = 0; i < (inventoryOpen ? 8 * 4 : 8); i++) // Only show from the lowest 8 slots if using hotbar mode
        {
            if (currentInventory[i] == 0 || currentInventoryQuantity[i] == 0)
            {
                Bg1UpSetTile((i % 8) * 4 + 1, (i / 8) * -4 + 21, 63);
                Bg1UpSetTile((i % 8) * 4 + 2, (i / 8) * -4 + 21, 63);
                Bg1UpSetTile((i % 8) * 4 + 1, (i / 8) * -4 + 22, 63);
                Bg1UpSetTile((i % 8) * 4 + 2, (i / 8) * -4 + 22, 63);
                print((i % 8) * 4 + 1, (i / 8) * -4 + 23, "   ");
            }
            else
            {
                Bg1UpSetTile((i % 8) * 4 + 1, (i / 8) * -4 + 21, getItemTile(currentInventory[i]) + 0);
                Bg1UpSetTile((i % 8) * 4 + 2, (i / 8) * -4 + 21, getItemTile(currentInventory[i]) + 1);
                Bg1UpSetTile((i % 8) * 4 + 1, (i / 8) * -4 + 22, getItemTile(currentInventory[i]) + 2);
                Bg1UpSetTile((i % 8) * 4 + 2, (i / 8) * -4 + 22, getItemTile(currentInventory[i]) + 3);
                print((i % 8) * 4 + 1, (i / 8) * -4 + 23, "   ");
                if (currentInventoryQuantity[i] > 1)
                {
                    char buffer[3];
                    itoa(currentInventoryQuantity[i], buffer, 10);
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
        setCraftingSelectionNoSound(craftingSelection);
        int tilesToRender = (craftableRecipesCount - craftingOffset * 16 <= 16)
                                ? (craftableRecipesCount - craftingOffset * 16)
                                : 16;
        for (int i = 0; i < tilesToRender; i++)
        {
            Bg1UpSetTile((i % 4) * 4 + 1, (i / 4) * -4 + 21, getItemTile(craftingRecipes[craftableRecipes[i + craftingOffset * 16]].item) + 0);
            Bg1UpSetTile((i % 4) * 4 + 2, (i / 4) * -4 + 21, getItemTile(craftingRecipes[craftableRecipes[i + craftingOffset * 16]].item) + 1);
            Bg1UpSetTile((i % 4) * 4 + 1, (i / 4) * -4 + 22, getItemTile(craftingRecipes[craftableRecipes[i + craftingOffset * 16]].item) + 2);
            Bg1UpSetTile((i % 4) * 4 + 2, (i / 4) * -4 + 22, getItemTile(craftingRecipes[craftableRecipes[i + craftingOffset * 16]].item) + 3);
            print((i % 4) * 4 + 1, (i / 4) * -4 + 23, "   ");
            if (craftingRecipes[craftableRecipes[i + craftingOffset * 16]].quantity > 1)
            {
                printVal((i % 4) * 4 + 1, (i / 4) * -4 + 23, craftingRecipes[craftableRecipes[i + craftingOffset * 16]].quantity);
            }
        }
        printVal(16, 19, craftingOffset + 1);
        printDirect("/");
        printValDirect((craftableRecipesCount / 16) + 1);

        for (int j = 0; j < 16; j++)
            Bg0UpSetTile(24 + j % 4, 16 + j / 4, (craftingShowCraftableOnly ? 128 : 160) + j);
        for (int j = 0; j < 16; j++)
            Bg0UpSetTile(28 + j % 4, 16 + j / 4, (craftingShowCraftableOnly ? 144 : 176) + j);
    }
}

void setToInventory(u8 *targetI, u8 *targetQ, int slot, int item, int quantity)
{
    if (slot < 0 || slot >= 8 * 4)
        return;
    if (item == 0 || quantity == 0)
    {
        targetI[slot] = 0;
        targetQ[slot] = 0;
        renderInventory();
        return;
    }
    targetI[slot] = item;
    targetQ[slot] = quantity;
    renderInventory();
}

void setInventory(int slot, int item, int quantity)
{
    setToInventory(inventory, inventoryQuantity, slot, item, quantity);
}

void inventorySetHotbar()
{
    lcdMainOnTop();
    inventoryOpen = false;
    craftingOpen = false;
    isChestOpen = false;
    chestOpen = -1;
    currentInventory = inventory;
    currentInventoryQuantity = inventoryQuantity;
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
    if (chestOpen != -1)
    {
        if (isChestOpen)
        {
            for (int j = 0; j < 16; j++)
                Bg0UpSetTile(23 + j % 4, j / 4, 48 + j);
            for (int j = 0; j < 16; j++)
                Bg0UpSetTile(19 + j % 4, j / 4, 208 + j);
        }
        else
        {
            for (int j = 0; j < 16; j++)
                Bg0UpSetTile(23 + j % 4, j / 4, 192 + j);
            for (int j = 0; j < 16; j++)
                Bg0UpSetTile(19 + j % 4, j / 4, 224 + j);
        }
    }
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

    if (isChestOpen)
    {
        for (int j = 0; j < 16; j++)
            Bg0UpSetTile(27 + j % 4, j / 4, 192 + j);
    }
    else
    {
        for (int j = 0; j < 16; j++)
            Bg0UpSetTile(27 + j % 4, j / 4, 48 + j);
    }

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

bool setChestLink(int x, int y)
{
    for (int i = 0; i < CHEST_COUNT; i++)
    {
        if (chestLinks[i].active && x >= chestLinks[i].x && x < chestLinks[i].x + 2 && y >= chestLinks[i].y && y < chestLinks[i].y + 2)
        {
            return false; // Chest already exists at this location
        }
        if (!chestLinks[i].active)
        {
            chestLinks[i].active = true;
            chestLinks[i].x = x;
            chestLinks[i].y = y;

            for (int j = 0; j < 8 * 4; j++)
            {
                chestInventory[i][j] = 0;
                chestInventoryQuantity[i][j] = 0;
            }

            return true;
        }
    }
    return false;
}

int getChestLink(int x, int y)
{
    for (int i = 0; i < CHEST_COUNT; i++)
    {
        if (chestLinks[i].active && x >= chestLinks[i].x && x < chestLinks[i].x + 2 && y >= chestLinks[i].y && y < chestLinks[i].y + 2)
        {
            return i;
        }
    }
    return -1; // No chest found at this location
}

bool removeChestLink(int x, int y)
{
    int chestLink = getChestLink(x, y);
    if (chestLink != -1)
    {
        chestLinks[chestLink].active = false;
        return true;
    }
    return false; // No chest found at this location
}

void openChest(int chestLink)
{
    if (chestLink < 0 || chestLink >= CHEST_COUNT)
        return;
    chestOpen = chestLink;
    isChestOpen = true;
    currentInventory = chestInventory[chestOpen];
    currentInventoryQuantity = chestInventoryQuantity[chestOpen];
    inventorySetFull();
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
    if (tileProperties[tile].specialParam == SPECIAL_DOOR)
    {
        if (y - 3 < 0 || y + 1 >= mapHeight) // Whole door is out of bounds
        {
            canPlace = false;
        }
        else if (isTileSolid(gameTerrain[x + (y + 1) * MAP_WIDTH_MAX]) && // Check if the 2 tiles below and above the door are solid
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
    else if (tileProperties[tile].specialParam == SPECIAL_WORKBENCH || tileProperties[tile].specialParam == SPECIAL_ANVIL)
    {
        if (x + 1 >= mapWidth || y + 1 >= mapHeight) // Whole bench is out of bounds
        {
            canPlace = false;
        }
        else if (isTileSolid(gameTerrain[x + (y + 1) * MAP_WIDTH_MAX]) && // Check if the 2 tiles below and to the right of the bench are solid
                 isTileSolid(gameTerrain[x + 1 + (y + 1) * MAP_WIDTH_MAX]))
        {
            if (!isTileSolid(gameTerrain[x + y * MAP_WIDTH_MAX]) &&
                !isTileSolid(gameTerrain[x + 1 + y * MAP_WIDTH_MAX]))
            {
                canPlace = true;
            }
        }
    }
    else if (tileProperties[tile].specialParam == SPECIAL_FURNACE)
    {
        if (x - 1 < 0 || x + 1 >= mapWidth || y - 1 < 0 || y + 1 >= mapHeight) // Whole furnace is out of bounds
        {
            canPlace = false;
        }
        else
        {
            canPlace = true;
            for (int dx = -1; dx <= 1; dx++)
            {
                int nx = x + dx;
                for (int dy = -1; dy <= 0; dy++)
                {
                    int ny = y + dy;
                    if (gameTerrain[nx + ny * MAP_WIDTH_MAX] != TILE_AIR)
                    {
                        canPlace = false;
                        break;
                    }
                }
                if (!isTileSolid(gameTerrain[nx + (y + 1) * MAP_WIDTH_MAX]))
                {
                    canPlace = false;
                    break;
                }
            }
        }
    }
    else if (tileProperties[tile].specialParam == SPECIAL_CHEST)
    {
        if (x + 1 >= mapWidth || y - 1 < 0) // Whole chest is out of bounds
        {
            canPlace = false;
        }
        else if (isTileSolid(gameTerrain[x + (y + 1) * MAP_WIDTH_MAX]) && // Check if the 2 tiles below and to the right of the chest are solid
                 isTileSolid(gameTerrain[x + 1 + (y + 1) * MAP_WIDTH_MAX]))
        {
            canPlace = true;
            for (int dx = 0; dx <= 1; dx++)
            {
                int nx = x + dx;
                for (int dy = -1; dy <= 0; dy++)
                {
                    int ny = y + dy;
                    if (gameTerrain[nx + ny * MAP_WIDTH_MAX] != TILE_AIR)
                    {
                        canPlace = false;
                        break;
                    }
                }
                if (!canPlace)
                    break;
            }
            if (canPlace)
            {
                if (!setChestLink(x, y - 1))
                {
                    canPlace = false;
                }
            }
        }
    }
    else if (tileProperties[tile].specialParam == SPECIAL_ACORN)
    {
        if (y + 1 >= mapHeight || y - 2 < 0)
        {
            canPlace = false;
        }
        else if (isTileSolid(gameTerrain[x + (y + 1) * MAP_WIDTH_MAX]))
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

    if (tileProperties[tile].specialParam == SPECIAL_DOOR)
    {
        setGameTerrain(x, y - 1, tile - 1);
        setGameTerrain(x, y - 2, tile - 2);
    }
    else if (tileProperties[tile].specialParam == SPECIAL_WORKBENCH || tileProperties[tile].specialParam == SPECIAL_ANVIL)
    {
        setGameTerrain(x + 1, y, tile + 1);
    }
    else if (tileProperties[tile].specialParam == SPECIAL_FURNACE)
    {
        for (int dx = -1; dx <= 1; dx++)
        {
            int nx = x + dx;
            for (int dy = -1; dy <= 0; dy++)
            {
                int ny = y + dy;
                setGameTerrain(nx, ny, TILE_FURNACE_1 + (dx + 1) + (dy + 1) * 3);
            }
        }
    }
    else if (tileProperties[tile].specialParam == SPECIAL_CHEST)
    {
        for (int dx = 0; dx <= 1; dx++)
        {
            int nx = x + dx;
            for (int dy = -1; dy <= 0; dy++)
            {
                int ny = y + dy;
                setGameTerrain(nx, ny, TILE_CHEST_1 + dx + (dy + 1) * 2);
            }
        }
    }
    else if (tileProperties[tile].specialParam == SPECIAL_ACORN)
    {
        setGameTerrain(x, y - 1, TILE_ACORN_2);
        setGameTerrain(x, y - 2, TILE_ACORN_1);
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

bool giveToInventory(u8 *targetInv, u8 *targetQ, int item, int quantity)
{
    while (quantity > 0)
    {
        bool changed = false;

        // Fill existing stacks first
        for (int i = 0; i < 8 * 4; i++)
        {
            int j = (item >= ITEM_COPPER_COIN && item <= ITEM_PLATINUM_COIN) ? (8 * 4 - 1 - i) : i; // If it's a coin, start from the end for convenience
            if (targetInv[j] == item && targetQ[j] < 100)
            {
                int add = min(100 - targetQ[j], quantity);

                setToInventory(targetInv, targetQ, j, item, targetQ[j] + add);

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
            if (targetInv[j] == 0)
            {
                int add = min(100, quantity);

                setToInventory(targetInv, targetQ, j, item, add);

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

bool giveInventory(int item, int quantity)
{
    return giveToInventory(inventory, inventoryQuantity, item, quantity);
}

bool giveChest(int chestLink, int item, int quantity)
{
    return giveToInventory(chestInventory[chestLink], chestInventoryQuantity[chestLink], item, quantity);
}

bool isInventoryEmpty(u8 *targetInv, u8 *targetQ)
{
    for (int i = 0; i < 8 * 4; i++)
    {
        if (targetInv[i] != 0 && targetQ[i] > 0)
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
    {
        index++;
        if (index >= MAX_ITEMS_TOTAL)
            return;
    }
    // we can do item[index] = {...} directly but it'll get confusing
    // update: what the fucks does the comment above mean
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

int createParticle(int x, int y, int sprite)
{
    if (sprite == PARTICLE_NONE || sprite >= PARTICLES)
        return -1;
    if (x < 0 || x >= mapWidth * 8 || y < 0 || y >= mapHeight * 8)
        return -1;
    int index = 0;
    while (index < PARTICLE_COUNT && particles[index].exists)
        index++;

    if (index >= PARTICLE_COUNT)
        return -1;
    particles[index].x = x;
    particles[index].y = y;
    particles[index].sprite = sprite;
    particles[index].madeFrame = frame;
    particles[index].exists = true;
    if (sprite == PARTICLE_LEAF)
    {
        particles[index].weight = 0;
        particles[index].velocityX = rando(1, 2);
        particles[index].velocityY = rando(1, 2);
    }
    else if (sprite >= PARTICLE_COPPER_COIN && sprite <= PARTICLE_PLATINUM_COIN)
    {
        particles[index].weight = 0;
        particles[index].velocityX = 0;
        particles[index].velocityY = 0;
    }
    else
    {
        particles[index].weight = 1;
        particles[index].velocityX = rando(-2, 2);
        particles[index].velocityY = rando(-2, 2);
    }

    dmaCopy(((const u8 *)particlesTiles) + 8 * 8 * 3 * sprite + rando(0, 2) * 8 * 8, particles[index].sprite_gfx_mem, 8 * 8);
    return index;
}

void createParticles(int x, int y, int sprite, int count)
{
    for (int i = 0; i < count; i++)
        createParticle(x, y, sprite);
}

void destroyParticle(int id)
{
    particles[id].exists = false;
}

void breakTile(int x, int y, int speed)
{
    if (x < 0 || x >= mapWidth || y < 0 || y >= mapHeight)
        return;
    if (tileProperties[gameTerrain[x + y * MAP_WIDTH_MAX]].specialParam == SPECIAL_CHEST &&
        !isInventoryEmpty(chestInventory[getChestLink(x, y)], chestInventoryQuantity[getChestLink(x, y)]))
        return;
    if (y - 1 >= 0)
    {
        if (tileProperties[gameTerrain[x + (y - 1) * MAP_WIDTH_MAX]].specialParam != SPECIAL_NONE &&
            tileProperties[gameTerrain[x + y * MAP_WIDTH_MAX]].specialParam == SPECIAL_NONE)
            return;
    }
    gameTerrainHealth[x + y * MAP_WIDTH_MAX] += speed;
    if (gameTerrainHealth[x + y * MAP_WIDTH_MAX] >= getElementHealth(gameTerrain[x + y * MAP_WIDTH_MAX]))
    {
        // Special blocks handling
        if (tileProperties[gameTerrain[x + y * MAP_WIDTH_MAX]].specialParam == SPECIAL_TREE)
        {
            int log = gameTerrain[x + y * MAP_WIDTH_MAX];
            // Trees have special treatment: break all the logs and leaves above it
            for (int i = y; i >= 0; i--)
            {
                if (gameTerrain[x + i * MAP_WIDTH_MAX] == log)
                {
                    dropItem(x, i, getElementDrop(log), 1);
                    setGameTerrain(x, i, 0);
                    gameTerrainHealth[x + i * MAP_WIDTH_MAX] = 0;
                }
                else if (gameTerrain[x + i * MAP_WIDTH_MAX] == tileProperties[log].specialParams[0])
                { // Leaves don't drop anything if broken from tree
                    for (int j = -3; j <= 3; j++)
                    {
                        for (int k = -6; k <= 0; k++)
                        {
                            if (x + j >= 0 && x + j < mapWidth && i + k >= 0 && i + k < mapHeight)
                            {
                                if (gameTerrain[x + j + (i + k) * MAP_WIDTH_MAX] == tileProperties[log].specialParams[0])
                                {
                                    setGameTerrain(x + j, i + k, TILE_AIR);
                                    gameTerrainHealth[x + j + (i + k) * MAP_WIDTH_MAX] = 0;
                                    // TODO: drop the acorns dumbass
                                }
                            }
                        }
                    }
                }
                else
                {
                    break; // Stop breaking when we hit a non-log or non-leaf tile
                }
            }
        }
        else if (tileProperties[gameTerrain[x + y * MAP_WIDTH_MAX]].specialParam == SPECIAL_DOOR)
        {
            dropItem(x, y, getElementDrop(gameTerrain[x + y * MAP_WIDTH_MAX]), 1);
            if (tileProperties[gameTerrain[x + y * MAP_WIDTH_MAX]].specialParams[0] == 0) // Closed door
            {
                int offset = tileProperties[gameTerrain[x + y * MAP_WIDTH_MAX]].specialParams[2] - 1;
                for (int i = 0; i < 3; i++)
                {
                    setGameTerrain(x, y - offset + i, TILE_AIR);
                }
            }
            else if (tileProperties[gameTerrain[x + y * MAP_WIDTH_MAX]].specialParams[0] == 1) // Open door
            {
                int offsetX = (tileProperties[gameTerrain[x + y * MAP_WIDTH_MAX]].specialParams[2] - 1) % 2;
                int offsetY = (tileProperties[gameTerrain[x + y * MAP_WIDTH_MAX]].specialParams[2] - 1) / 2;
                for (int i = 0; i < 3; i++)
                {
                    for (int j = 0; j < 2; j++)
                    {
                        setGameTerrain(x - offsetX + j, y - offsetY + i, TILE_AIR);
                    }
                }
            }
        }
        else if (tileProperties[gameTerrain[x + y * MAP_WIDTH_MAX]].specialParam == SPECIAL_WORKBENCH ||
                 tileProperties[gameTerrain[x + y * MAP_WIDTH_MAX]].specialParam == SPECIAL_ANVIL)
        {
            dropItem(x, y, getElementDrop(gameTerrain[x + y * MAP_WIDTH_MAX]), 1);
            int offset = tileProperties[gameTerrain[x + y * MAP_WIDTH_MAX]].specialParams[0] - 1;
            for (int i = 0; i < 2; i++)
            {
                setGameTerrain(x - offset + i, y, TILE_AIR);
            }
        }
        else if (tileProperties[gameTerrain[x + y * MAP_WIDTH_MAX]].specialParam == SPECIAL_FURNACE)
        {
            dropItem(x, y, getElementDrop(gameTerrain[x + y * MAP_WIDTH_MAX]), 1);
            int offsetX = (tileProperties[gameTerrain[x + y * MAP_WIDTH_MAX]].specialParams[0] - 1) % 3;
            int offsetY = (tileProperties[gameTerrain[x + y * MAP_WIDTH_MAX]].specialParams[0] - 1) / 3;
            for (int i = 0; i < 2; i++)
            {
                for (int j = 0; j < 3; j++)
                {
                    setGameTerrain(x - offsetX + j, y - offsetY + i, TILE_AIR);
                }
            }
        }
        else if (tileProperties[gameTerrain[x + y * MAP_WIDTH_MAX]].specialParam == SPECIAL_CHEST ||
                 tileProperties[gameTerrain[x + y * MAP_WIDTH_MAX]].specialParam == SPECIAL_LIFE_CRYSTAL)
        {
            dropItem(x, y, getElementDrop(gameTerrain[x + y * MAP_WIDTH_MAX]), 1);
            int offsetX = (tileProperties[gameTerrain[x + y * MAP_WIDTH_MAX]].specialParams[0] - 1) % 2;
            int offsetY = (tileProperties[gameTerrain[x + y * MAP_WIDTH_MAX]].specialParams[0] - 1) / 2;
            for (int i = 0; i < 2; i++)
            {
                for (int j = 0; j < 2; j++)
                {
                    setGameTerrain(x - offsetX + j, y - offsetY + i, TILE_AIR);
                }
            }
        }
        else if (tileProperties[gameTerrain[x + y * MAP_WIDTH_MAX]].specialParam == SPECIAL_ACORN)
        {
            dropItem(x, y, getElementDrop(gameTerrain[x + y * MAP_WIDTH_MAX]), 1);
            int offset = tileProperties[gameTerrain[x + y * MAP_WIDTH_MAX]].specialParams[0] - 1;
            for (int i = 0; i < 3; i++)
            {
                setGameTerrain(x, y - offset + i, TILE_AIR);
            }
        }
        else
        {
            dropItem(x, y, getElementDrop(gameTerrain[x + y * MAP_WIDTH_MAX]), 1);
            setGameTerrain(x, y, TILE_AIR);
            gameTerrainHealth[x + y * MAP_WIDTH_MAX] = 0;
        }
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
        if (rando(0, 1) == 0)
        {
            createParticle(x * 8 + 4, y * 8 + 4, tileProperties[gameTerrain[x + y * MAP_WIDTH_MAX]].particle);
        }
    }
}

void interact(int x, int y)
{
    if (tileProperties[gameTerrain[x + y * MAP_WIDTH_MAX]].specialParam == SPECIAL_DOOR)
    {
        if (tileProperties[gameTerrain[x + y * MAP_WIDTH_MAX]].specialParams[0] == 0) // Closed door
        {
            int offset = tileProperties[gameTerrain[x + y * MAP_WIDTH_MAX]].specialParams[2] - 1;

            int buildTo = 0; // 0 = don't, -1 = left, 1 = right
            if (gameTerrain[x - 1 + (y - offset) * MAP_WIDTH_MAX] == TILE_AIR &&
                gameTerrain[x - 1 + (y - offset + 1) * MAP_WIDTH_MAX] == TILE_AIR &&
                gameTerrain[x - 1 + (y - offset + 2) * MAP_WIDTH_MAX] == TILE_AIR)
            {
                buildTo = -1;
            }
            else if (gameTerrain[x + 1 + (y - offset) * MAP_WIDTH_MAX] == TILE_AIR &&
                     gameTerrain[x + 1 + (y - offset + 1) * MAP_WIDTH_MAX] == TILE_AIR &&
                     gameTerrain[x + 1 + (y - offset + 2) * MAP_WIDTH_MAX] == TILE_AIR)
            {
                buildTo = 1;
            }

            if (buildTo != 0)
            {
                for (int i = 0; i < 6; i++)
                {
                    setGameTerrain(x + i % 2 + ((buildTo == 1) ? 0 : -1),
                                   y - offset + i / 2,
                                   tileProperties[gameTerrain[x + y * MAP_WIDTH_MAX]].drop + 1 + i + ((buildTo == 1) ? 6 : 0));
                }
            }

            mmEffect(SFX_DOOR_OPEN);
        }
        else if (tileProperties[gameTerrain[x + y * MAP_WIDTH_MAX]].specialParams[0] == 1) // Open door
        {
            int offsetX = (tileProperties[gameTerrain[x + y * MAP_WIDTH_MAX]].specialParams[2] - 1) % 2;
            int offsetY = (tileProperties[gameTerrain[x + y * MAP_WIDTH_MAX]].specialParams[2] - 1) / 2;
            int door = tileProperties[gameTerrain[x + y * MAP_WIDTH_MAX]].drop - 2;
            int isDoorRight = tileProperties[gameTerrain[x + y * MAP_WIDTH_MAX]].specialParams[1];
            for (int i = 0; i < 3; i++)
            {
                for (int j = 0; j < 2; j++)
                {
                    setGameTerrain(x - offsetX + j, y - offsetY + i, TILE_AIR);
                }
            }
            for (int i = 0; i < 3; i++)
            {
                setGameTerrain(x - offsetX + ((isDoorRight == 0) ? 1 : 0),
                               y - offsetY + i,
                               door + i);
            }

            mmEffect(SFX_DOOR_CLOSE);
        }
    }
    else if (tileProperties[gameTerrain[x + y * MAP_WIDTH_MAX]].specialParam == SPECIAL_CHEST)
    {
        int chestLink = getChestLink(x, y);
        if (chestLink != -1)
        {
            openChest(chestLink);
        }
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
        dmaCopy(((const u8 *)entitiesTiles) + entity[i].type * 32 * 32 + 32 * 32 * offset, entity[i].sprite_gfx_mem, 32 * 32);
    }
    else if (entities[type].spriteSize == SpriteSize_32x64)
    {
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
                dropItem(entity[id].x / 8 + rando(-1, 3), entity[id].y / 8 + rando(-1, 3), e->drops[i],
                         rando(e->dropRange[i][0], e->dropRange[i][1]));
        }
    }
    mmEffect(entityAISounds[entities[entity[id].type].AItype].killed);
    createParticles(entity[id].x, entity[id].y, entities[entity[id].type].deathParticle, 3);
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
        mmEffect(entityAISounds[entities[entity[id].type].AItype].hit);
        entity[id].health -= damage;
    }

    if (entity[id].health <= 0)
    {
        killEntity(id);
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

void setPlayerAnimFrame(int frame, int style)
{
    if (frame == player.anim_frame)
        return;
    if (frame < 0)
        frame = 0;
    player.anim_frame = frame;
    dmaCopy(((const u8 *)spritesTiles) + 32 * 64 * PLAYER_STYLES * player.anim_frame + 32 * 64 * style, player.sprite_gfx_mem, 32 * 64);
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

void waitForPress()
{
    printDirect("\n\nPress any key to continue...");
    scanKeys();
    while (!keysDown())
    {
        swiWaitForVBlank();
        mmStreamUpdate();
        scanKeys();
    }
}

bool confirm(const char *message)
{
    clearPrint();
    printSmart(0, 0, message);
    delay(2);
    printDirect("\n\nA: Yes\nB: No");
    scanKeys();
    while (1)
    {
        swiWaitForVBlank();
        mmStreamUpdate();
        scanKeys();
        if (keysDown() & KEY_A)
        {
            clearPrint();
            return true;
        }
        if (keysDown() & KEY_B)
        {
            clearPrint();
            return false;
        }
    }
}

// I honestly wanted to make a struct of SaveData but i had some problems with its stack or whatever it is

bool saveMapToFile(const char *filen)
{
    mmEffect(SFX_ENU_CLOSE);

    if (mkdir("terrarias/", 0777) == -1 && errno != EEXIST)
    {
        print(0, 0, "Failed to create directory 'terrarias/'");
        waitForPress();
        return false;
    }

    char filename[128];
    snprintf(filename, sizeof(filename), "terrarias/%s", filen);

    FILE *file = fopen(filename, "wb"); // "wb" = write binary
    if (!file)
    {
        print(0, 0, "Failed to open file for writing: ");
        printDirect(filename);
        waitForPress();
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
    bytesWritten += fwrite(chestInventory, 1, sizeof(chestInventory), file);
    bytesWritten += fwrite(chestInventoryQuantity, 1, sizeof(chestInventoryQuantity), file);
    bytesWritten += fwrite(chestLinks, 1, sizeof(chestLinks), file);
    fclose(file);

    if (bytesWritten != 4 + 4 + 4 + sizeof(stoneSurface) + sizeof(biomeSurface) + sizeof(gameTerrain) + sizeof(chestInventory) + sizeof(chestInventoryQuantity) + sizeof(chestLinks))
    {
        print(0, 0, "Map save error");
        waitForPress();
        return false;
    }

    print(0, 0, "Map saved to ");
    printDirect(filename);
    waitForPress();
    return true;
}

bool saveCharacterToFile(const char *filen)
{
    mmEffect(SFX_ENU_CLOSE);

    if (mkdir("terrarias/", 0777) == -1 && errno != EEXIST)
    {
        print(0, 0, "Failed to create directory 'terrarias/'");
        waitForPress();
        return false;
    }

    char filename[128];
    snprintf(filename, sizeof(filename), "terrarias/%s", filen);

    FILE *file = fopen(filename, "wb"); // "wb" = write binary
    if (!file)
    {
        print(0, 0, "Failed to open file for writing: ");
        printDirect(filename);
        waitForPress();
        return false;
    }

    print(0, 0, "Saving character...");
    uint32_t magic = 0xC0DEBEEF;
    size_t bytesWritten = fwrite(&magic, 1, 4, file);
    bytesWritten += fwrite(&player.maxHealth, 1, 4, file);
    bytesWritten += fwrite(&player.style, 1, 4, file);
    bytesWritten += fwrite(inventory, 1, sizeof(inventory), file);
    bytesWritten += fwrite(inventoryQuantity, 1, sizeof(inventoryQuantity), file);
    fclose(file);

    if (bytesWritten != 4 + 4 + 4 + sizeof(inventory) + sizeof(inventoryQuantity))
    {
        print(0, 0, "Character save error");
        waitForPress();
        return false;
    }

    print(0, 0, "Character saved to ");
    printDirect(filename);
    waitForPress();
    return true;
}

bool loadCharacterFromFile(const char *filen)
{
    mmEffect(SFX_ENU_OPEN);

    char filename[128];
    snprintf(filename, sizeof(filename), "terrarias/%s", filen);

    FILE *file = fopen(filename, "rb"); // "rb" = read binary
    if (!file)
    {
        print(0, 0, "Failed to open file for reading: ");
        printDirect(filename);
        waitForPress();
        return false;
    }

    print(0, 0, "Loading character...");
    uint32_t magic;
    size_t bytesRead = fread(&magic, 1, 4, file);
    if (magic == 0xC0DEBEEF)
    {
        bytesRead += fread(&player.maxHealth, 1, 4, file);
        bytesRead += fread(&player.style, 1, 4, file);
        bytesRead += fread(inventory, 1, sizeof(inventory), file);
        bytesRead += fread(inventoryQuantity, 1, sizeof(inventoryQuantity), file);
        fclose(file);
        if (bytesRead != 4 + 4 + 4 + sizeof(inventory) + sizeof(inventoryQuantity))
        {
            print(0, 0, "Character load error");
            waitForPress();
            return false;
        }
    }
    else
    {
        clearPrint();
        print(0, 0, "Invalid character file: ");
        printDirect(filename);
        fclose(file);
        waitForPress();
        return false;
    }

    print(0, 0, "Character loaded from ");
    printDirect(filename);
    printDirect(" (max health: ");
    printValDirect(player.maxHealth);
    printDirect(")\n");

    strcpy(characterName, filen);
    waitForPress();

    return true;
}

void delete (const char *filen)
{
    char filename[128];
    snprintf(filename, sizeof(filename), "terrarias/%s", filen);

    char notice[128 + 34 + 1];
    snprintf(notice, sizeof(notice), "Are you sure you want to delete %s?", filen);

    if (!confirm(notice))
    {
        return;
    }

    if (remove(filename) == 0)
    {
        print(0, 0, "Deleted: ");
        printDirect(filename);
        waitForPress();
    }
    else
    {
        print(0, 0, "Failed to delete: ");
        printDirect(filename);
        waitForPress();
    }
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
        waitForPress();
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
        memset(stoneSurface, (int)64 * MAX_STONE_HEIGHT, sizeof(stoneSurface));
        memset(biomeSurface, BIOME_FOREST, sizeof(biomeSurface));
        memset(chestInventory, 0, sizeof(chestInventory));
        memset(chestInventoryQuantity, 0, sizeof(chestInventoryQuantity));
        memset(chestLinks, 0, sizeof(chestLinks));

        strcpy(characterName, names[rando(0, NAMES_COUNT - 1)]);
        player.style = 0;
        player.maxHealth = 100;

        for (int i = 0; i < 8 * 4; i++)
        {
            if (inventory[i] >= 100)
                inventory[i] -= 73; // New version removes the 100 offset of items
        }

        if (bytesRead != 4 + 1024 * 64 * sizeof(u8) + sizeof(inventory) + sizeof(inventoryQuantity))
        {
            print(0, 0, "Map load error");
            waitForPress();
            return false;
        }

        clearPrint();
        printSmart(0, 0, "Old world loaded successfully and made a new character for it! Consider saving it in the new format to avoid issues in the future.\n\nEven if backwards compatibility was implemented, this world can still experience some issues. Try creating a new world for even more features!");
        waitForPress();
        giveInventory(ITEM_PLATINUM_COIN, 5);
        clearPrint();
        printSmart(0, 0, "Also for being an OG, I'm gifting you 5 platinum coins! Thanks for being a part of TerrariaDS! You can't do anything with them for now, but consider it an investment for future updates ;)");
        waitForPress();
    }
    else
    {
        if (magic != 0x550B12A2)
        {
            clearPrint();
            print(0, 0, "Invalid map file: ");
            printDirect(filename);
            fclose(file);
            waitForPress();
            return false;
        }
        bytesRead += fread(&mapWidth, 1, 4, file);
        bytesRead += fread(&mapHeight, 1, 4, file);
        bytesRead += fread(stoneSurface, 1, sizeof(stoneSurface), file);
        bytesRead += fread(biomeSurface, 1, sizeof(biomeSurface), file);
        bytesRead += fread(gameTerrain, 1, sizeof(gameTerrain), file);
        bytesRead += fread(chestInventory, 1, sizeof(chestInventory), file);
        bytesRead += fread(chestInventoryQuantity, 1, sizeof(chestInventoryQuantity), file);
        bytesRead += fread(chestLinks, 1, sizeof(chestLinks), file);
        fclose(file);

        if (bytesRead != 4 + 4 + 4 + sizeof(stoneSurface) + sizeof(biomeSurface) + sizeof(gameTerrain) + sizeof(chestInventory) + sizeof(chestInventoryQuantity) + sizeof(chestLinks))
        {
            print(0, 0, "Map load error");
            return false;
        }
    }

    player.x = mapWidth * 8 / 2;
    player.y = getHighestTileY(mapWidth / 2) * 8 - player.sizeY;
    lastCamTileX = -67;
    lastCamTileY = -67;
    player.health = 100;
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

    for (int i = 0; i < PARTICLE_COUNT; i++)
    {
        destroyParticle(i);
    }

    renderInventory();
    renderCrafting();
    print(0, 0, "Map loaded from ");
    printDirect(filename);

    strcpy(worldFileName, filen);
    waitForPress();

    return true;
}

void playerDamage(int damage, int hitType, const char *name)
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
        u8 coinsToDrop[4];
        for (int i = 0; i < 4; i++) // softcore drops half of wealth
        {
            coinsToDrop[i] = getPlayerItemQuantity(ITEM_COPPER_COIN + i) / 2;
            takeInventory(ITEM_COPPER_COIN + i, coinsToDrop[i]);
            dropItem(player.x / 8, player.y / 8, ITEM_COPPER_COIN + i, coinsToDrop[i]);
        }

        mmEffect(SFX_LAYER_KILLED);
        inventorySetHotbar();
        clearPrint();
        print(0, 0, "Player");
        if (hitType == HIT_NONE)
        {
            printSmartDirect(defaultDeathMessage[rando(0, DEFAULTDEATHMESSAGE_COUNT - 1)]);
            printDirect(".");
        }
        else if (hitType == HIT_FALL)
        {
            printSmartDirect(fallDeathMessage[rando(0, FALLDEATHMESSAGE_COUNT - 1)]);
        }
        else if (hitType == HIT_PVP)
        {
            printSmartDirect(defaultDeathMessage[rando(0, DEFAULTDEATHMESSAGE_COUNT - 1)]);
            printSmartDirect(" by ");
            printSmartDirect(name);
            printDirect(".");
        }
        printSmartDirect("\n\nYou dropped ");
        if (coinsToDrop[3] > 0)
        {
            printValDirect(coinsToDrop[3]);
            printSmartDirect(" platinum coin");
            if (coinsToDrop[3] > 1)
                printDirect("s");
            printDirect(", ");
        }
        if (coinsToDrop[2] > 0)
        {
            printValDirect(coinsToDrop[2]);
            printSmartDirect(" gold coin");
            if (coinsToDrop[2] > 1)
                printDirect("s");
            printDirect(", ");
        }
        if (coinsToDrop[1] > 0)
        {
            printValDirect(coinsToDrop[1]);
            printSmartDirect(" silver coin");
            if (coinsToDrop[1] > 1)
                printDirect("s");
            printDirect(", ");
        }
        if (coinsToDrop[0] > 0)
        {
            printValDirect(coinsToDrop[0]);
            printSmartDirect(" copper coin");
            if (coinsToDrop[0] > 1)
                printDirect("s");
            printDirect(". ");
        }
        if (coinsToDrop[0] + coinsToDrop[1] + coinsToDrop[2] + coinsToDrop[3] == 0)
        {
            printSmartDirect("nothing...");
        }
        oamSet(&oamSub, 0, 0, 0, 0, 0, SpriteSize_8x8, SpriteColorFormat_256Color, nullSprite, -1, false, false, false, false, false);
        oamUpdate(&oamSub);
        for (int i = 0; i < 600; i++)
        {
            print(0, 6, "Respawning in ");
            printValDirect(10 - i / 60);
            printDirect(" seconds...    ");
            swiWaitForVBlank();
            mmStreamUpdate();
        }
        clearPrint();
        player.x = mapWidth * 8 / 2;
        player.y = getHighestTileY(mapWidth / 2) * 8 - player.sizeY;
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

int plantTree(int x, int y)
{
    int tree_height = rando(MIN_TREE_HEIGHT, MAX_TREE_HEIGHT);
    // Tree trunk
    for (int i = 0; i < tree_height; i++)
        setGameTerrain(x, y - i, TILE_WOODLOG);

    // Tree leaves (7x7 above trunk)
    int leafRadius = rando(2, 4);
    for (int i = -3; i <= 3; i++)
    {
        for (int j = -3; j < 3; j++)
        {
            if (i * i + j * j >= leafRadius * leafRadius)
                continue;

            int leafX = x + i;
            int leafY = y - tree_height + 1 - leafRadius - j;

            setGameTerrain(leafX, leafY, TILE_LEAVES); // No need for bounds check, the setGameTerrain function handles it fine twin <3
        }
    }
    return leafRadius;
}

void generateMap()
{
    u8 grassSurface[mapWidth];
    int seed = rando(0, 0x7fffffff);

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
        int len = rando(32, mapWidth / 2);
        for (int i = 0; i < len && idx < mapWidth; i++)
        {
            biomeSurface[idx++] = biome;
        }
    }

    for (int x = 0; x < mapWidth; x++)
    {
        float wave = fractalPerlin1D(x, 4, 0.4f, 0.02f, seed) * 20.0f;
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

    for (int i = 0; i < mapWidth * mapHeight; i++)
    {
        setGameTerrain(i % mapWidth, i / mapWidth, 0);
        gameTerrainHealth[i] = 0;
    }

    // Place terrain
    printDirect("Placing terrain...\n");
    for (int x = 0; x < mapWidth; x++)
    {
        for (int y = 0; y < mapHeight; y++)
        {
            if (y >= grassSurface[x] && y < stoneSurface[x])
            {
                setGameTerrain(x, y, biomes[biomeSurface[clamp(x + fractalPerlin1D(y, 4, 0.4f, 0.1f, seed + 67) * 5, 0, mapWidth - 1)]].surfaceTile);
            }
            else if (y >= stoneSurface[x])
            {
                setGameTerrain(x, y, biomes[biomeSurface[clamp(x + fractalPerlin1D(y, 4, 0.4f, 0.1f, seed + 67) * 5, 0, mapWidth - 1)]].undergroundTile);
            }
        }
        if (x % (mapWidth / 32) == 0)
        {
            printDirect(".");
        }
    }

    // Generate ores
    printDirect("Generating ores...\n");
    int veins = rando(mapWidth * mapHeight / (64 * 2), mapWidth * mapHeight / (64 * 1)); // Number of ore veins to generate (proportional to map size)
    const int ores[] = {TILE_COPPER_ORE, TILE_TIN_ORE, TILE_IRON_ORE, TILE_GOLD_ORE};
    const int oresSize = sizeof(ores) / sizeof(ores[0]);
    int cur = 0;
    for (int i = 0; i < veins; i++)
    {
        int centerX = rando(0, mapWidth - 1);
        int centerY = rando(stoneSurface[centerX], mapHeight - 1);
        int ore = ores[rando(0, oresSize - 1)];

        int x = centerX, y = centerY;
        if (gameTerrain[x + y * MAP_WIDTH_MAX] == TILE_AIR)
            continue;
        setGameTerrain(x, y, ore);

        while (true)
        {
            x += rando(-1, 1);
            y += rando(-1, 1);
            if (x < 0 || x >= mapWidth || y < 0 || y >= mapHeight)
            {
                break;
            }
            if (gameTerrain[x + y * MAP_WIDTH_MAX] == TILE_AIR)
                continue;
            setGameTerrain(x, y, ore);

            int dist = abs(centerX - x) + abs(centerY - y);

            if (rando(0, 100) < dist * 2) // Chance to stop the vein (grows with distance)
                break;
        }

        if (((i + 1) * 32) / veins != cur)
            printDirect(".");
        cur = ((i + 1) * 32) / veins;
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
                if (caveNoise < -0.2f) // Adjust this threshold to control cave density
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
                    setGameTerrain(x, y, biomes[biomeSurface[clamp(x + fractalPerlin1D(y, 4, 0.4f, 0.1f, seed + 67) * 5, 0, mapWidth - 1)]].surfaceWall);
                }
                else if (y >= stoneSurface[x])
                {
                    setGameTerrain(x, y, biomes[biomeSurface[clamp(x + fractalPerlin1D(y, 4, 0.4f, 0.1f, seed + 67) * 5, 0, mapWidth - 1)]].undergroundWall);
                }
            }
        }
        if (x % (mapWidth / 32) == 0)
        {
            printDirect(".");
        }
    }

    // Place trees
    printDirect("Planting trees...\n");
    for (int x = 1; x < mapWidth - 1; x++)
    {
        if (rando(1, TREE_CHANCE) == 1 && gameTerrain[x + (grassSurface[x] + 1) * MAP_WIDTH_MAX] == TILE_DIRT && biomeSurface[x] == BIOME_FOREST)
        {
            x += plantTree(x, grassSurface[x] - 1) * 2;
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

    // Place life crystals
    printDirect("Placing life crystals...\n");
    int crystals = rando(1, mapWidth * mapHeight / 2048);
    cur = 0;
    for (int i = 0; i < crystals; i++)
    {
        int x = rando(0, mapWidth - 2);
        int y = rando(mapHeight / 2, mapHeight - 2); // don't forget demonite bricks below!

        y = getHighestTileYFrom(x, y);

        setGameTerrain(x, y - 1, TILE_LIFE_CRYSTAL_1);
        setGameTerrain(x + 1, y - 1, TILE_LIFE_CRYSTAL_2);
        setGameTerrain(x, y, TILE_LIFE_CRYSTAL_3);
        setGameTerrain(x + 1, y, TILE_LIFE_CRYSTAL_4);

        if (((i + 1) * 32) / crystals != cur)
            printDirect(".");
        cur = ((i + 1) * 32) / crystals;
    }

    // Place demonite bricks to limit the world
    printDirect("Placing demonite bricks at the bottom so you don't escape >:) ...\n");
    for (int x = 0; x < mapWidth; x++)
    {
        setGameTerrain(x, mapHeight - 1, TILE_DEMONITE_BRICK);
        if (x % (mapWidth / 32) == 0)
        {
            printDirect(".");
        }
    }

    player.x = mapWidth * 8 / 2;
    player.y = getHighestTileY(mapWidth / 2) * 8 - player.sizeY;
    player.health = 100;
    player.invincibilityFrames = 300;
    gametime = 0;
    lastCamTileX = -67;
    lastCamTileY = -67;

    for (int i = 0; i < ENTITY_COUNT; i++)
    {
        removeEntity(i);
    }

    for (int i = 0; i < MAX_ITEMS; i++)
    {
        destroyItem(i);
    }

    for (int i = 0; i < PARTICLE_COUNT; i++)
    {
        destroyParticle(i);
    }
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
    int8_t *target = (int8_t *)dest;

    if (!audioReady)
    {
        memset(target, 0, length);
        return length;
    }

    size_t bytesLeft = audioLength - audioPosition;
    size_t bytesToCopy = length < bytesLeft ? length : bytesLeft;

    if (!nitroromReadFile(audioRom, audioFileId, audioPosition, target, bytesToCopy))
    {
        memset(target, 0, length);
        audioPosition = 0;
        return length;
    }

    audioPosition += bytesToCopy;

    if (volume != 255)
    {
        for (size_t i = 0; i < bytesToCopy; i++)
        {
            int sample = target[i];

            // signed PCM scaling around 0
            sample = (sample * volume) / 255;

            if (sample < -128)
                sample = -128;
            if (sample > 127)
                sample = 127;

            target[i] = (int8_t)sample;
        }
    }

    if (bytesToCopy < length)
    {
        memset(target + bytesToCopy, 0, length - bytesToCopy);
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
    return (player.x + player.sizeX / 2 - x) * (player.x + player.sizeX / 2 - x) + (player.y + player.sizeY / 2 - y) * (player.y + player.sizeY / 2 - y) < range * range;
}

bool isInPlayerRange(int x, int y)
{
    return isInPlayerRadius(x, y, SCREEN_WIDTH / 2);
}

void consume(int selection)
{
    switch (inventory[inventorySelection])
    {
    case TILE_MUSHROOM:
        if (player.health < player.maxHealth)
        {
            playerHeal(10);
            mmEffect(SFX_MUSHROOM);
            setInventory(inventorySelection, inventory[inventorySelection], inventoryQuantity[inventorySelection] - 1);
        }
        break;
    case TILE_LIFE_CRYSTAL_3:
        if (player.maxHealth < 320)
        {
            player.maxHealth += 20;
            mmEffect(SFX_MAGIC);
            setInventory(inventorySelection, inventory[inventorySelection], inventoryQuantity[inventorySelection] - 1);
        }
        break;
    default:
        return;
    }
}

void swing()
{
    swingFrame = frame;
}