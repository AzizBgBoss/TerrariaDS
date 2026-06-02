/*---------------------------------------------------------------------------------

Terraria like game for the Nintendo DS
AzizBgBoss - https://github.com/AzizBgBoss

---------------------------------------------------------------------------------*/

#include "defs.h"

#include "vars.h"
#include "bgutils.h"
#include "mathutils.h"
#include "gameutils.h"

int main(void)
{
	scanKeys();

	mmInitDefaultMem((mm_addr)soundbank_bin);


	// Load sound effects
	for (int i = 0; i < MSL_NSAMPS; i++)
	{
		mmLoadEffect(i);
	}

	if (!fatInitDefault() || keysHeld() & KEY_START)
	{
		mmLoad(MOD_MODULE1);

		mmStart(MOD_MODULE1, MM_PLAY_LOOP);

		videoSetMode(MODE_0_2D | DISPLAY_BG0_ACTIVE);
		vramSetBankA(VRAM_A_MAIN_BG);
		BGCTRL[0] = BG_TILE_BASE(1) | BG_MAP_BASE(0) | BG_COLOR_256 | BG_32x32 | BG_PRIORITY(3);

		dmaCopy(vonTiles, (void *)CHAR_BASE_BLOCK(1), vonTilesLen);
		dmaCopy(vonMap, (void *)SCREEN_BASE_BLOCK(0), vonMapLen);
		dmaCopy(vonPal, BG_PALETTE, vonPalLen);

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

	int intro = rando(0, 9);

	if (intro == 0)
		setStreamAudio("special.pcm");
	else
		setStreamAudio("intro1.pcm");

	mm_stream mystream;
	mystream.sampling_rate = 11025;
	mystream.buffer_length = 1024;
	mystream.callback = on_stream_request;
	mystream.format = MM_STREAM_8BIT_MONO;
	mystream.timer = MM_TIMER0;
	mystream.manual = true;
	mmStreamOpen(&mystream);

	videoSetMode(MODE_0_2D | DISPLAY_BG0_ACTIVE);
	videoSetModeSub(MODE_3_2D | DISPLAY_BG0_ACTIVE | DISPLAY_BG1_ACTIVE);
	vramSetBankA(VRAM_A_MAIN_BG);
	vramSetBankC(VRAM_C_SUB_BG);
	BGCTRL[0] = BG_TILE_BASE(1) | BG_MAP_BASE(0) | BG_COLOR_256 | BG_32x32 | BG_PRIORITY(3);

	if (intro == 0)
	{
		dmaCopy(intro2Tiles, (void *)CHAR_BASE_BLOCK(1), intro2TilesLen);
		dmaCopy(intro2Map, (void *)SCREEN_BASE_BLOCK(0), intro2MapLen);
		dmaCopy(intro2Pal, BG_PALETTE, intro2PalLen);
	}
	else if (intro < 3)
	{
		dmaCopy(intro3Tiles, (void *)CHAR_BASE_BLOCK(1), intro3TilesLen);
		dmaCopy(intro3Map, (void *)SCREEN_BASE_BLOCK(0), intro3MapLen);
		dmaCopy(intro3Pal, BG_PALETTE, intro3PalLen);
	}
	else if (intro < 6)
	{
		dmaCopy(intro4Tiles, (void *)CHAR_BASE_BLOCK(1), intro4TilesLen);
		dmaCopy(intro4Map, (void *)SCREEN_BASE_BLOCK(0), intro4MapLen);
		dmaCopy(intro4Pal, BG_PALETTE, intro4PalLen);
	}
	else
	{
		dmaCopy(introTiles, (void *)CHAR_BASE_BLOCK(1), introTilesLen);
		dmaCopy(introMap, (void *)SCREEN_BASE_BLOCK(0), introMapLen);
		dmaCopy(introPal, BG_PALETTE, introPalLen);
	}

	storeOriginalPalette();
	fadeInPalette(64, 8);

mainMenu:

	videoSetMode(MODE_0_2D | DISPLAY_BG0_ACTIVE | DISPLAY_BG3_ACTIVE);

	BGCTRL[3] = BG_TILE_BASE(3) | BG_MAP_BASE(3) | BG_COLOR_256 | BG_32x32 | BG_PRIORITY(0);
	dmaCopy(fontTiles, (void *)CHAR_BASE_BLOCK(3), fontTilesLen);
	dmaFillHalfWords(0, (void *)SCREEN_BASE_BLOCK(3), 2048);

	if (intro == 0)
	{
		dmaCopy(mainscreenbg2Tiles, (void *)CHAR_BASE_BLOCK(1), mainscreenbg2TilesLen);
		dmaCopy(mainscreenbg2Map, (void *)SCREEN_BASE_BLOCK(0), mainscreenbg2MapLen);
		dmaCopy(mainscreenbg2Pal, BG_PALETTE, mainscreenbg2PalLen);
	}
	else
	{
		dmaCopy(mainscreenbgTiles, (void *)CHAR_BASE_BLOCK(1), mainscreenbgTilesLen);
		dmaCopy(mainscreenbgMap, (void *)SCREEN_BASE_BLOCK(0), mainscreenbgMapLen);
		dmaCopy(mainscreenbgPal, BG_PALETTE, mainscreenbgPalLen);
	}

	BGCTRL_SUB[0] = BG_TILE_BASE(1) | BG_MAP_BASE(0) | BG_COLOR_256 | BG_32x32 | BG_PRIORITY(3);
	dmaCopy(bgTiles, (void *)CHAR_BASE_BLOCK_SUB(1), bgTilesLen);
	dmaCopy(bgMap, (void *)SCREEN_BASE_BLOCK_SUB(0), bgMapLen);

	BGCTRL_SUB[1] = BG_TILE_BASE(2) | BG_MAP_BASE(1) | BG_COLOR_256 | BG_32x32 | BG_PRIORITY(2);
	dmaCopy(mainscreenuiTiles, (void *)CHAR_BASE_BLOCK_SUB(2), mainscreenuiTilesLen);
	dmaCopy(mainscreenuiMap, (void *)SCREEN_BASE_BLOCK_SUB(1), mainscreenuiMapLen);

	int bg2 = bgInitSub(3, BgType_ExRotation, BgSize_ER_512x512, 3, 3);
	bgWrapOn(bg2);
	bgSetPriority(bg2, 2);
	dmaCopy(tilemapTiles, bgGetGfxPtr(bg2), tilemapTilesLen);
	dmaFillHalfWords(TILE_AIR, bgGetMapPtr(bg2), 64 * 64 * 2);

	bg2Map = (u16 *)bgGetMapPtr(bg2); // Make sure this is defined before using generateMap or setGameTerrain so it doesn't tap into an undefined pointer and freeze

	dmaCopy(bgPal, BG_PALETTE_SUB, bgPalLen);

	printSmart(0, 17, titleSplashes[rando(0, sizeof(titleSplashes) / sizeof(titleSplashes[0]) - 1)]);

	int x = 0;

	while (1)
	{
		swiWaitForVBlank();
		mmStreamUpdate();

		REG_BG0HOFS_SUB = x / 2;

		scanKeys();
		int pressed = keysDown();

		if (pressed & KEY_START)
		{
			return 0;
		}

		if (pressed & KEY_TOUCH)
		{
			touchRead(&touch);
			if (touch.px >= 56 && touch.px <= 199 && touch.py >= 56 && touch.py <= 87)
			{
				changeTextBackground();
				clearPrint();

				// Let player choose a world
				int chosenIndex = 0;
				while (1)
				{
					swiWaitForVBlank();
					mmStreamUpdate();
					clearPrint();
					printDirect("Select a world size:\nPress B to return.\n\n");
					for (int i = 0; i < 4; i++)
					{
						if (i == chosenIndex)
							printDirect("> ");
						else
							printDirect("  ");
						switch (i)
						{
						case 0:
							printDirect("Tiny (128x64)");
							break;
						case 1:
							printDirect("Small (256x64)");
							break;
						case 2:
							printDirect("Medium (512x128)");
							break;
						case 3:
							printDirect("Large (1024x256)");
							break;
						}
						printDirect("\n");
					}

					scanKeys();
					int down = keysDown();

					if (down & KEY_UP)
					{
						chosenIndex--;
						if (chosenIndex < 0)
							chosenIndex = 3;
					}
					else if (down & KEY_DOWN)
					{
						chosenIndex++;
						if (chosenIndex >= 4)
							chosenIndex = 0;
					}
					else if (down & KEY_A)
					{
						break; // World size chosen
					}
					else if (down & KEY_B)
					{
						goto mainMenu;
					}
				}
				switch (chosenIndex)
				{
				case 0:
					mapWidth = 128;
					mapHeight = 64;
					break;
				case 1:
					mapWidth = 256;
					mapHeight = 64;
					break;
				case 2:
					mapWidth = 512;
					mapHeight = 128;
					break;
				case 3:
					mapWidth = 1024;
					mapHeight = 256;
					break;
				}
				mmStreamClose();
				clearPrint();
				generateMap();
				break;
			}
			else if (touch.px >= 56 && touch.px <= 199 && touch.py >= 104 && touch.py <= 135)
			{
				changeTextBackground();
				clearPrint();
				if (fatInitDefault())
				{
					// Player wants to load a world:
					// We open the local ./terrarias/ folder and list every .ter file (world)

					char worldFiles[10][64];
					int worldFileCount = 0;
					// List all .ter files in the terrarias directory
					DIR *dir;
					struct dirent *ent;
					if ((dir = opendir("terrarias/")) != NULL)
					{
						// printDirect("Found worlds:\n");
						while ((ent = readdir(dir)) != NULL)
						{
							if (strstr(ent->d_name, ".ter") != NULL)
							{
								strcpy(worldFiles[worldFileCount], ent->d_name);
								worldFileCount++;
								if (worldFileCount >= 10)
									break; // Max 10 worlds for now
							}
						}
						closedir(dir);
					}
					else
					{
						// Could not open directory
						printDirect("Could not open terrarias/ directory!");
						delay(1);
						goto mainMenu;
						break;
					}

					if (worldFileCount == 0)
					{
						printDirect("No worlds found in terrarias/ folder!");
						delay(1);
						goto mainMenu;
						break;
					}

					// Let player choose a world
					int chosenIndex = 0;
					while (1)
					{
						swiWaitForVBlank();
						mmStreamUpdate();
						clearPrint();
						printDirect("Select a world to load:\nPress B to return.\n\n");
						for (int i = 0; i < worldFileCount; i++)
						{
							if (i == chosenIndex)
								printDirect("> ");
							else
								printDirect("  ");
							printDirect(worldFiles[i]);
							printDirect("\n");
						}

						scanKeys();
						int down = keysDown();

						if (down & KEY_UP)
						{
							chosenIndex--;
							if (chosenIndex < 0)
								chosenIndex = worldFileCount - 1;
						}
						else if (down & KEY_DOWN)
						{
							chosenIndex++;
							if (chosenIndex >= worldFileCount)
								chosenIndex = 0;
						}
						else if (down & KEY_A)
						{
							break; // World chosen
						}
						else if (down & KEY_B)
						{
							goto mainMenu;
						}
					}

					if (loadMapFromFile(worldFiles[chosenIndex]))
					{
						mmStreamClose();
						clearPrint();
						printDirect("Map loaded successfully!");
						gametime = 0;
					}
					else
					{
						clearPrint();
						printDirect("Error loading map!");
						delay(1);
						goto mainMenu;
					}
				}
				else
				{
					clearPrint();
					printDirect("Error: FatInitDefault failed!");
					delay(1);
					goto mainMenu;
				}
				break;
			}
		}
		x++;
	}
	videoSetMode(MODE_0_2D | DISPLAY_BG0_ACTIVE | DISPLAY_BG1_ACTIVE | DISPLAY_BG3_ACTIVE);	   // BG0: Background (item slots, etc...), BG1: Item tiles, BG3: Text
	videoSetModeSub(MODE_3_2D | DISPLAY_BG0_ACTIVE | DISPLAY_BG1_ACTIVE | DISPLAY_BG3_ACTIVE); // BG0: Background, BG1: Black background (to darken bg0), BG3: Game terrain

	vramSetBankA(VRAM_A_MAIN_BG);
	vramSetBankB(VRAM_B_MAIN_SPRITE);
	vramSetBankC(VRAM_C_SUB_BG);	 // for backgrounds on subscreen
	vramSetBankD(VRAM_D_SUB_SPRITE); // for sprites on subscreen

	BGCTRL[0] = BG_TILE_BASE(1) | BG_MAP_BASE(0) | BG_COLOR_256 | BG_32x32 | BG_PRIORITY(3);
	dmaCopy(invTiles, (void *)CHAR_BASE_BLOCK(1), invTilesLen);
	dmaFillHalfWords(0, (void *)SCREEN_BASE_BLOCK(0), 2048);
	BGCTRL[1] = BG_TILE_BASE(2) | BG_MAP_BASE(2) | BG_COLOR_256 | BG_32x32 | BG_PRIORITY(2);
	dmaCopy(itemsTiles, (void *)CHAR_BASE_BLOCK(2), itemsTilesLen);
	dmaFillHalfWords(63, (void *)SCREEN_BASE_BLOCK(2), 2048);

	dmaCopy(invPal, BG_PALETTE, invPalLen);

	dmaFillHalfWords(0, (void *)SCREEN_BASE_BLOCK_SUB(1), 2048);  // Fill the background with tiles 0
	dmaFillHalfWords(0x6666, (void *)CHAR_BASE_BLOCK_SUB(2), 32); // Fill the tiles data with the color black (index 6 in the palette)
	BGCTRL_SUB[1] =
		BG_TILE_BASE(2) |
		BG_MAP_BASE(1) |
		BG_COLOR_16 |
		BG_32x32 |
		BG_PRIORITY(3); // behind BG0

	REG_BLDCNT_SUB =
		BLEND_ALPHA |
		BLEND_SRC_BG0 | // world background
		BLEND_DST_BG1;	// black BG

	oamInit(&oamMain, SpriteMapping_1D_128, false);

	inventorySelectionSprite = oamAllocateGfx(&oamMain, SpriteSize_32x32, SpriteColorFormat_256Color);
	dmaCopy(uiTiles, inventorySelectionSprite, uiTilesLen);

	dmaCopy(uiPal, SPRITE_PALETTE, uiPalLen);

	oamInit(&oamSub, SpriteMapping_1D_128, false);

	nullSprite = oamAllocateGfx(&oamSub, SpriteSize_16x16, SpriteColorFormat_256Color);
	dmaFillHalfWords(0, nullSprite, 16 * 16);

	player.sprite_gfx_mem = oamAllocateGfx(&oamSub, SpriteSize_32x64, SpriteColorFormat_256Color);
	dmaCopy(spritesTiles, player.sprite_gfx_mem, 32 * 64);

	itemHandSprite = oamAllocateGfx(&oamSub, SpriteSize_16x16, SpriteColorFormat_256Color);

	dmaCopy(tilemapTiles, itemHandSprite, 16 * 16);

	for (int i = 0; i < MAX_ITEMS; i++)
	{
		item[i].sprite_gfx_mem = oamAllocateGfx(&oamSub, SpriteSize_16x16, SpriteColorFormat_256Color);
		// No need to copy anything, we'll do that later when we summon the item
	}

	dmaCopy(spritesPal, SPRITE_PALETTE_SUB, spritesPalLen);

	setStreamAudio("game2.pcm");
	mmStreamOpen(&mystream);

	// Setup inventory
	inventorySetHotbar();
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
				inventorySetHotbar();
			}
			else
			{
				inventorySetFull();
			}
		}

		if (held & KEY_A && held & KEY_B && held & KEY_X && held & KEY_Y) // Debug menu
		{
			// Stop everything and show the debug menu
			inventorySetHotbar();
			clearPrint();
			print(0, 0, "Debug menu, bazinga!\n");
			printDirect("Press X to close debug menu.\n");

			int selection = 0;
			int maxSelection = 8;

			while (pmMainLoop())
			{
				// Menu selection system
				swiWaitForVBlank();
				mmStreamUpdate();
				scanKeys();

				int pressed = keysDown();
				if (pressed & KEY_X)
				{
					clearPrint();
					inventorySetHotbar();
					break;
				}

				if (pressed & KEY_DOWN)
					selection++;
				else if (pressed & KEY_UP)
					selection--;
				if (selection < 0)
					selection = maxSelection;
				if (selection > maxSelection)
					selection = 0;

				for (int i = 0; i <= maxSelection; i++)
				{
					if (i == selection)
						print(0, 2 + i, "> ");
					else
						print(0, 2 + i, "  ");

					switch (i)
					{
					case 0:
						printDirect("Spawn entity");
						break;
					case 1:
						printDirect("Get Item");
						break;
					case 2:
						printDirect("Set health to max");
						break;
					case 3:
						printDirect("Kill all entities");
						break;
					case 4:
						printDirect("Kill player");
						break;
					case 5:
						printDirect("Become [TITLE CARD] for 5 mins");
						break;
					case 6:
						printDirect("Show debug info: ");
						if (debug)
							printDirect("ON ");
						else
							printDirect("OFF");
						break;
					case 7:
						printDirect("Set time to night");
						break;
					case 8:
						printDirect("Set time to day");
						break;
					default:
						printDirect("---");
						break;
					}
				}

				if (pressed & KEY_A)
				{
					switch (selection)
					{
					case 0:
					{
						int chosenID = 0;
						while (pmMainLoop())
						{
							swiWaitForVBlank();
							mmStreamUpdate();
							scanKeys();

							int pressed = keysDown();

							clearPrint();
							print(0, 0, "Choose an entity ID: ");
							printValDirect(chosenID);
							printDirect("\n");
							printDirect(entities[chosenID].name);
							printDirect("\nPress A to spawn at player's position.\nPress X to cancel.");

							if (pressed & KEY_UP)
							{
								chosenID--;
								if (chosenID < 0)
									chosenID = ENTITIES - 1;
							}
							else if (pressed & KEY_DOWN)
							{
								chosenID++;
								if (chosenID >= ENTITIES)
									chosenID = 0;
							}
							else if (pressed & KEY_A)
							{
								spawnEntity(chosenID, player.x, player.y);
								printDirect("\nEntity spawned!");
							}
							else if (pressed & KEY_X)
							{
								clearPrint();
								print(0, 0, "Debug menu, bazinga!\n");
								printDirect("Press X to close debug menu.\n");
								break;
							}
						}
					}
					break;
					case 1:
					{
						int chosenID = 0;
						int chosenQ = 1;
						while (pmMainLoop())
						{
							swiWaitForVBlank();
							mmStreamUpdate();
							scanKeys();

							int pressed = keysDown();

							clearPrint();
							print(0, 0, "Choose an item ID: ^ ");
							printValDirect(chosenID);
							printDirect(" v\n< ");
							printValDirect(chosenQ);
							printDirect(" >\n");
							printDirect(getElementName(chosenID));
							printDirect("\nPress A to spawn at player's position.\nPress X to cancel.");

							if (pressed & KEY_UP)
							{
								chosenID--;
								if (chosenID < 0)
									chosenID = TILES - 1;
							}
							else if (pressed & KEY_DOWN)
							{
								chosenID++;
								if (chosenID >= TILES)
									chosenID = 0;
							}
							else if (pressed & KEY_LEFT)
							{
								chosenQ--;
								if (chosenQ < 1)
									chosenQ = 100;
							}
							else if (pressed & KEY_RIGHT)
							{
								chosenQ++;
								if (chosenQ > 100)
									chosenQ = 1;
							}
							else if (pressed & KEY_A)
							{
								giveInventory(chosenID, chosenQ);
								printDirect("\nItem given!");
							}
							else if (pressed & KEY_X)
							{
								clearPrint();
								print(0, 0, "Debug menu, bazinga!\n");
								printDirect("Press X to close debug menu.\n");
								break;
							}
						}
					}
					break;
					case 2:
						player.health = 400;
						printDirect("\nHealth set to max!");
						break;
					case 3:
						for (int e = 0; e < ENTITY_COUNT; e++)
						{
							if (entity[e].exists)
							{
								killEntity(e);
							}
						}
						printDirect("\nAll entities killed!");
						break;
					case 4:
						player.invincibilityFrames = 0;
						playerDamage(9999, HIT_NONE, "");
						printDirect("\nPlayer killed!");
						break;
					case 5:
						player.invincibilityFrames += 60 * 5 * 60; // 5 minutes of invincibility at 60 fps
						printDirect("\nInvincibility for 5 minutes!");
						break;
					case 6:
						debug = !debug;
						break;
					case 7:
						gametime = DAY_LENGTH / 2 - 16; // Night time
						printDirect("\nTime set to night!");
						break;
					case 8:
						gametime = DAY_LENGTH - 16; // Day time
						printDirect("\nTime set to day!");
						break;
					default:
						printDirect("\nUnknown implementation.");
						break;
					}
				}
			}
		}

		if (pressed & KEY_START) // Pause menu
		{
			// Stop everything and show the pause menu
			inventorySetHotbar();
			clearPrint();
			printDirect(worldFileName);
			printDirect("\nGame Paused\n");
			printDirect("Press START to resume.\n");

			int selection = 0;
			int maxSelection = 2;

			while (pmMainLoop())
			{
				// Menu selection system
				swiWaitForVBlank();
				mmStreamUpdate();
				scanKeys();

				int pressed = keysDown();
				if (pressed & KEY_START)
				{
					clearPrint();
					inventorySetHotbar();
					break;
				}
				if (pressed & KEY_DOWN)
					selection++;
				else if (pressed & KEY_UP)
					selection--;
				if (selection < 0)
					selection = maxSelection;
				if (selection > maxSelection)
					selection = 0;

				for (int i = 0; i <= maxSelection; i++)
				{
					if (i == selection)
						print(0, 4 + i, "> ");
					else
						print(0, 4 + i, "  ");
					switch (i)
					{
					case 0:
						printDirect("Quit to main menu");
						break;
					case 1:
						printDirect("Save Game");
						break;
					case 2:
						printDirect("Load Game");
						break;
					default:
						printDirect("---");
						break;
					}
				}

				if (pressed & KEY_A)
				{
					switch (selection)
					{
					case 0:
						clearPrint();
						for (int i = 0; i < 128; i++)
							oamSet(&oamSub, i, 0, 0, 0, 0, SpriteSize_8x8, SpriteColorFormat_256Color, nullSprite, -1, false, false, false, false, false);
						oamUpdate(&oamSub);
						goto mainMenu;
						break;
					case 1:
						saveMapToFile(worldFileName);
						break;
					case 2:
						loadMapFromFile(worldFileName);
						break;
					default:
						printDirect("\nUnknown implementation.");
						break;
					}
				}
			}
		}

		if (pressed & KEY_X)
		{
			setInventorySelection(inventorySelection + 1);
		}

		if (pressed & KEY_Y)
		{
			setInventorySelection(inventorySelection - 1);
		}

		if (held & KEY_DOWN)
			scale *= 1.01;
		if (held & KEY_UP)
			scale *= 0.99;

		if (scale > MAX_SCALE)
			scale = MAX_SCALE;
		if (scale < MIN_SCALE)
			scale = MIN_SCALE;

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
				player.fall = player.y;
			}
		}

		if (held & KEY_RIGHT)
		{
			if ((player.x + player.sizeX) / 8 < mapWidth)
			{
				player.isLookingLeft = false;
				if (player.velocityX < 1) // if player stationary or pushed to the opposite direction, he can fight it back.
					player.velocityX++;	  // But can't accelerate more if he's already going that way.
				player.animation = ANIM_WALK;
			}
		}
		else if (held & KEY_LEFT)
		{
			if (player.x - 1 >= 0)
			{
				player.isLookingLeft = true;
				if (player.velocityX > -1)
					player.velocityX--;
				player.animation = ANIM_WALK;
			}
		}
		else
			player.animation = ANIM_NONE;

		if (!player.isOnGround)
		{
			if (isTileSolid(gameTerrain[player.x / 8 + (player.y + player.velocity) / 8 * MAP_WIDTH_MAX]))
			{
				player.velocity = 0;
				player.y /= 8;
				player.y *= 8;
			}
			if (isTileSolid(gameTerrain[(player.x + player.sizeX - 1) / 8 + (player.y + player.velocity) / 8 * MAP_WIDTH_MAX]))
			{
				player.velocity = 0;
				player.y /= 8;
				player.y *= 8;
			}
		}

		if (!player.isOnGround || player.velocity < 0)
		{
			player.velocity += player.weight; // Apply gravity
			if (player.velocity > 6)
				player.velocity = 6;
			player.y += player.velocity;
			player.animation = ANIM_JUMP; // Fall and jump look the same
			if (player.y + player.sizeY > mapHeight * 8)
			{
				player.y = mapHeight * 8 - player.sizeY; // Prevent falling out of the map
				player.isOnGround = true;
				player.isJumping = false;
				player.velocity = 0; // Reset velocity when on the ground
			}
		}

		if (player.velocityX > 0)
		{
			int futureX = player.x + player.velocityX;
			int futureRX = futureX + player.sizeX - 1;
			if (futureRX < mapWidth * 8)
				if (!isTileSolid(gameTerrain[(futureRX) / 8 + player.y / 8 * MAP_WIDTH_MAX]) &&
					!isTileSolid(gameTerrain[(futureRX) / 8 + (player.y + player.sizeY / 2) / 8 * MAP_WIDTH_MAX]) &&
					!isTileSolid(gameTerrain[(futureRX) / 8 + (player.y + player.sizeY - 1) / 8 * MAP_WIDTH_MAX]))
				{
					player.x += player.velocityX;
				}
		}
		else if (player.velocityX < 0)
		{
			int futureX = player.x + player.velocityX;
			if (futureX > 0)
				if (!isTileSolid(gameTerrain[(futureX) / 8 + player.y / 8 * MAP_WIDTH_MAX]) &&
					!isTileSolid(gameTerrain[(futureX) / 8 + (player.y + player.sizeY / 2) / 8 * MAP_WIDTH_MAX]) &&
					!isTileSolid(gameTerrain[(futureX) / 8 + (player.y + player.sizeY - 1) / 8 * MAP_WIDTH_MAX]))
				{
					player.x += player.velocityX;
				}
		}

		if (player.isOnGround) // Friction
			player.velocityX += (player.velocityX > 0) ? -1 : (player.velocityX < 0) ? 1
																					 : 0;

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
							if (i < (craftableRecipesCount - craftingOffset * 16))
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
					if (touch.px >= 24 * 8 && touch.px < 32 * 8 && touch.py >= 16 * 8 && touch.py < 20 * 8) // player touched the Craftable/Available button
					{
						craftingShowCraftableOnly = !craftingShowCraftableOnly;
						renderCrafting();
					}
					if (touch.px >= 24 * 8 && touch.px < 32 * 8 && touch.py >= 20 * 8 && touch.py < 24 * 8) // player touched the Craft button
					{
						if (craftingSelection >= 0 && craftingSelection < craftableRecipesCount)
						{
							bool craftable = true;
							for (int i = 0; i < craftingRecipes[craftableRecipes[craftingSelection + craftingOffset * 16]].ingredientCount; i++)
							{
								if (!playerHasItem(craftingRecipes[craftableRecipes[craftingSelection + craftingOffset * 16]].itemsNeeded[i], craftingRecipes[craftableRecipes[craftingSelection + craftingOffset * 16]].itemsNeededQuantity[i]))
								{
									craftable = false;
								}
							}
							if (craftable)
							{
								for (int i = 0; i < craftingRecipes[craftableRecipes[craftingSelection + craftingOffset * 16]].ingredientCount; i++)
								{
									takeInventory(craftingRecipes[craftableRecipes[craftingSelection + craftingOffset * 16]].itemsNeeded[i], craftingRecipes[craftableRecipes[craftingSelection + craftingOffset * 16]].itemsNeededQuantity[i]);
								}
								giveInventory(craftingRecipes[craftableRecipes[craftingSelection + craftingOffset * 16]].item, craftingRecipes[craftableRecipes[craftingSelection + craftingOffset * 16]].quantity);
								setCraftingSelection(craftingSelection);
								renderCrafting();
								print(0, 1, "                                  ");
								print(0, 1, "Crafted ");
								printDirect(getElementName(craftingRecipes[craftableRecipes[craftingSelection + craftingOffset * 16]].item));
							}
							else
							{
								print(0, 1, "                                  ");
								print(0, 1, "Oops! You don't have enough items!");
							}
						}
					}
					if (touch.px >= 16 * 8 && touch.px < 20 * 8 && touch.py >= 20 * 8 && touch.py < 24 * 8) // player touched the < button
					{
						craftingOffset--;
						if (craftingOffset < 0)
							craftingOffset = 0;
						setCraftingSelection(0);
						renderCrafting();
					}
					if (touch.px >= 20 * 8 && touch.px < 24 * 8 && touch.py >= 20 * 8 && touch.py < 24 * 8) // player touched the > button
					{
						craftingOffset++;
						if (craftingOffset > (craftableRecipesCount) / 16)
							craftingOffset = (craftableRecipesCount) / 16;
						setCraftingSelection(0);
						renderCrafting();
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
					if (chestOpen != -1)
					{
						if (touch.px >= 23 * 8 && touch.px < 27 * 8 && touch.py >= 0 * 8 && touch.py < 4 * 8) // Switch between chest/inventory if one is opened
						{
							if (isChestOpen)
							{
								currentInventory = inventory;
								currentInventoryQuantity = inventoryQuantity;
							}
							else
							{
								currentInventory = chestInventory[chestOpen];
								currentInventoryQuantity = chestInventoryQuantity[chestOpen];
							}
							isChestOpen = !isChestOpen;
							inventorySetFull();
						}
						if (touch.px >= 19 * 8 && touch.px < 23 * 8 && touch.py >= 0 * 8 && touch.py < 4 * 8) // Move selected item to chest/inventory
						{
							if (isChestOpen)
							{
								if (chestInventory[chestOpen][inventorySelection] != 0 && chestInventoryQuantity[chestOpen][inventorySelection] != 0)
								{
									if (giveInventory(chestInventory[chestOpen][inventorySelection], chestInventoryQuantity[chestOpen][inventorySelection]))
									{
										chestInventory[chestOpen][inventorySelection] = 0;
										chestInventoryQuantity[chestOpen][inventorySelection] = 0;
										inventorySetFull();
									}
								}
							}
							else
							{
								if (inventory[inventorySelection] != 0 && inventoryQuantity[inventorySelection] != 0)
								{
									if (giveChest(chestOpen, inventory[inventorySelection], inventoryQuantity[inventorySelection]))
									{
										inventory[inventorySelection] = 0;
										inventoryQuantity[inventorySelection] = 0;
										inventorySetFull();
									}
								}
							}
						}
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
				if (itemToMove != -1 && destination != -1 && itemToMove != destination && currentInventory[itemToMove] != 0)
				{
					if (currentInventory[itemToMove] == currentInventory[destination]) // If both items are the same
					{
						int totalQuantity = currentInventoryQuantity[itemToMove] + currentInventoryQuantity[destination];
						if (totalQuantity > 99) // Limit quantity to 99
						{
							setToInventory(currentInventory, currentInventoryQuantity, destination, currentInventory[itemToMove], 99);				  // Set destination to 99
							setToInventory(currentInventory, currentInventoryQuantity, itemToMove, currentInventory[itemToMove], totalQuantity - 99); // Set itemToMove to the remaining quantity
						}
						else
						{
							setToInventory(currentInventory, currentInventoryQuantity, destination, currentInventory[itemToMove], totalQuantity); // Combine quantities
							setToInventory(currentInventory, currentInventoryQuantity, itemToMove, 0, 0);										  // Clear the itemToMove slot
						}
					}
					else // If items are different
					{
						if (currentInventory[destination] == 0) // Just move the itemToMove to the destination
						{
							setToInventory(currentInventory, currentInventoryQuantity, destination, currentInventory[itemToMove], currentInventoryQuantity[itemToMove]);
							setToInventory(currentInventory, currentInventoryQuantity, itemToMove, 0, 0); // Clear the itemToMove slot
						}
						else // Swap items
						{
							int tempItem = currentInventory[destination];
							int tempQuantity = currentInventoryQuantity[destination];
							setToInventory(currentInventory, currentInventoryQuantity, destination, currentInventory[itemToMove], currentInventoryQuantity[itemToMove]);
							setToInventory(currentInventory, currentInventoryQuantity, itemToMove, tempItem, tempQuantity); // Swap the items
						}
					}
				}
			}
		}
		else
		{ // Game world interaction on sub-screen
			if (pressed & KEY_TOUCH)
			{
				interacting = true;
				touchFrame = frame;
			}
			if (held & KEY_TOUCH)
			{
				touchRead(&touch);
				int dx = touch.px - SCREEN_WIDTH / 2;
				int dy = touch.py - SCREEN_HEIGHT / 2;

				int worldX = scrollX + SCREEN_WIDTH / 2 + (dx * scale) / 256;
				int worldY = scrollY + SCREEN_HEIGHT / 2 + (dy * scale) / 256;

				if (worldX >= 0 && worldX < mapWidth * 8 && worldY >= 0 && worldY < mapHeight * 8)
				{

					int worldTouchX = worldX / 8;
					int worldTouchY = worldY / 8;

					// Check if touch within player range
					if (0 <= worldTouchX && worldTouchX < mapWidth && 0 <= worldTouchY && worldTouchY < mapHeight &&
						worldTouchX >= TLCtileX - player.tileRange && worldTouchX <= TRCtileX + player.tileRange &&
						worldTouchY >= TLCtileY - player.tileRange && worldTouchY <= BRCtileY + player.tileRange)
					{
						if (frame - touchFrame > 10)
						{
							if (!tileProperties[inventory[inventorySelection]].isItem // Object is a tile, not an item
								&& inventoryQuantity[inventorySelection])
							{
								if (!gameTerrain[worldTouchX + worldTouchY * MAP_WIDTH_MAX])
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
								else if (isElementWall(gameTerrain[worldTouchX + worldTouchY * MAP_WIDTH_MAX]) && gameTerrain[worldTouchX + worldTouchY * MAP_WIDTH_MAX] != inventory[inventorySelection])
								{
									breakTile(worldTouchX, worldTouchY, 1);
								}
							}
							else if (tileProperties[inventory[inventorySelection]].isTool && inventoryQuantity[inventorySelection])
							{
								if (isToolCompatible(inventory[inventorySelection], gameTerrain[worldTouchX + worldTouchY * MAP_WIDTH_MAX])) // Check if the tool can break the tile
								{
									if (gameTerrain[worldTouchX + worldTouchY * MAP_WIDTH_MAX])
										breakTile(worldTouchX, worldTouchY, getItemSpeed(inventory[inventorySelection]));
								}
							}
						}
						else
						{
							if (interacting == true)
							{
								int e = detectEntity(worldX, worldY); // Check if an entity is at the touched location
								if (e != -1 && tileProperties[inventory[inventorySelection]].isTool)
								{
									mmEffect(SFX_SWING);
									damageEntity(e, getItemDamage(inventory[inventorySelection]));
									knockBackEntity(e, (player.x > entity[e].x) ? -getItemKnockback(inventory[inventorySelection]) : getItemKnockback(inventory[inventorySelection]), -getItemKnockback(inventory[inventorySelection]));
								}
								else if (inventory[inventorySelection] == TILE_MUSHROOM && inventoryQuantity[inventorySelection] && player.health < player.maxHealth)
								{
									mmEffect(SFX_MUSHROOM);
									playerHeal(10);
									setInventory(inventorySelection, inventory[inventorySelection], inventoryQuantity[inventorySelection] - 1);
								}
								else if (tileProperties[gameTerrain[worldTouchX + worldTouchY * MAP_WIDTH_MAX]].specialParam != SPECIAL_NONE)
									interact(worldTouchX, worldTouchY);
								else // Make a swing sfx so the user knows the touch is registered but their dumbass can't use it
									mmEffect(SFX_SWING);

								interacting = false;
							}
						}
					}
					else // if not in player range, we could still use consumables
					{
						if (interacting == true)
						{
							if (inventory[inventorySelection] == TILE_MUSHROOM && inventoryQuantity[inventorySelection] && player.health < player.maxHealth)
							{
								mmEffect(SFX_MUSHROOM);
								playerHeal(10);
								setInventory(inventorySelection, inventory[inventorySelection], inventoryQuantity[inventorySelection] - 1);
							}
							interacting = false;
						}
					}
				}
			}
		}
		// TODO: do not let player skip over below ground if velocity > tile size

		// Check if player is on the ground
		if (isTileSolid(gameTerrain[BLCtileX + BLCtileY * MAP_WIDTH_MAX]))
		{
			if (player.isOnGround == false && player.velocity >= 0)
			{
				player.isOnGround = true;
				player.isJumping = false;
				player.velocity = 0; // Reset velocity when on the ground
				player.y = BLCtileY * 8 - player.sizeY;
				int h = (player.y - player.fall) / 8 - 25;
				if (h > 0)
					playerDamage(10 * h, HIT_FALL, "");
			}
		}

		if (isTileSolid(gameTerrain[BRCtileX + BRCtileY * MAP_WIDTH_MAX]))
		{
			if (player.isOnGround == false && player.velocity >= 0)
			{
				player.isOnGround = true;
				player.isJumping = false;
				player.velocity = 0; // Reset velocity when on the ground
				player.y = BRCtileY * 8 - player.sizeY;
				int h = (player.y - player.fall) / 8 - 25;
				if (h > 0)
					playerDamage(10 * h, HIT_FALL, "");
			}
		}

		if (!isTileSolid(gameTerrain[BLCtileX + BLCtileY * MAP_WIDTH_MAX]) && !isTileSolid(gameTerrain[BRCtileX + BRCtileY * MAP_WIDTH_MAX]))
		{
			if (player.isOnGround)
			{
				player.isOnGround = false;
				player.isJumping = true;
				player.fall = player.y;
			}
		}

		scrollX = clamp(player.x - SCREEN_WIDTH / 2 + player.sizeX / 2, 0, mapWidth * 8 - SCREEN_WIDTH);
		scrollY = clamp(player.y - SCREEN_HEIGHT / 2 + player.sizeY / 2, 0, mapHeight * 8 - SCREEN_HEIGHT);

		// Handle entity physics
		for (int i = 0; i < ENTITY_COUNT; i++)
		{
			if (entity[i].exists == true)
			{
				if (frame % 2 == 0) // Move every 2 frames to make it slower
				{
					if (!entity[i].isOnGround || entity[i].velocity < 0)
					{
						entity[i].velocity += entity[i].weight; // Apply gravity
						if (entity[i].velocity > 7)
							entity[i].velocity = 7;
						entity[i].y += entity[i].velocity;
						entity[i].animation = ANIM_JUMP; // Fall and jump look the same
						if (entity[i].y + entity[i].sizeY > mapHeight * 8)
						{
							entity[i].y = mapHeight * 8 - entity[i].sizeY; // Prevent falling out of the map
							entity[i].isOnGround = true;
							entity[i].isJumping = false;
							entity[i].velocity = 0; // Reset velocity when on the ground
						}
					}
					else if (entity[i].isOnGround && entity[i].velocityX != 0)
					{
						entity[i].animation = ANIM_WALK;
					}
					else
					{
						entity[i].animation = ANIM_NONE;
					}

					if (entity[i].velocityX > 0)
					{
						if (!isTileSolid(gameTerrain[(entity[i].x + entity[i].sizeX - 1 + entity[i].velocityX) / 8 + entity[i].y / 8 * MAP_WIDTH_MAX]) &&
							!isTileSolid(gameTerrain[(entity[i].x + entity[i].sizeX - 1 + entity[i].velocityX) / 8 + (entity[i].y + entity[i].sizeY / 2) / 8 * MAP_WIDTH_MAX]) &&
							!isTileSolid(gameTerrain[(entity[i].x + entity[i].sizeX - 1 + entity[i].velocityX) / 8 + (entity[i].y + entity[i].sizeY - 1) / 8 * MAP_WIDTH_MAX]))
						{
							entity[i].x += entity[i].velocityX;
						}
						else if (entities[entity[i].type].AItype == ENTITY_AI_EYE) // When eye hits a wall, flip direction
						{
							entity[i].angle = 180 - entity[i].angle;
						}
					}
					else if (entity[i].velocityX < 0)
					{
						if (!isTileSolid(gameTerrain[(entity[i].x + entity[i].velocityX) / 8 + entity[i].y / 8 * MAP_WIDTH_MAX]) &&
							!isTileSolid(gameTerrain[(entity[i].x + entity[i].velocityX) / 8 + (entity[i].y + entity[i].sizeY / 2) / 8 * MAP_WIDTH_MAX]) &&
							!isTileSolid(gameTerrain[(entity[i].x + entity[i].velocityX) / 8 + (entity[i].y + entity[i].sizeY - 1) / 8 * MAP_WIDTH_MAX]))
						{
							entity[i].x += entity[i].velocityX;
						}
						else if (entities[entity[i].type].AItype == ENTITY_AI_EYE) // When eye hits a wall, flip direction
						{
							entity[i].angle = 180 - entity[i].angle;
						}
					}

					if (entity[i].isOnGround) // Friction
						entity[i].velocityX += (entity[i].velocityX > 0) ? -1 : (entity[i].velocityX < 0) ? 1
																										  : 0;

					// Collision detection with ground
					TLCx = entity[i].x;
					TLCy = entity[i].y;
					TRCx = entity[i].x + entity[i].sizeX - 1;
					TRCy = entity[i].y;
					BLCx = entity[i].x;
					BLCy = entity[i].y + entity[i].sizeY;
					BRCx = entity[i].x + entity[i].sizeX - 1;
					BRCy = entity[i].y + entity[i].sizeY;

					TLCtileX = TLCx / 8;
					TLCtileY = TLCy / 8;
					TRCtileX = TRCx / 8;
					TRCtileY = TRCy / 8;
					BLCtileX = BLCx / 8;
					BLCtileY = BLCy / 8;
					BRCtileX = BRCx / 8;
					BRCtileY = BRCy / 8;

					if (isTileSolid(gameTerrain[BLCtileX + BLCtileY * MAP_WIDTH_MAX]))
					{
						if (entity[i].isOnGround == false && entity[i].velocity >= 0)
						{
							entity[i].isOnGround = true;
							entity[i].isJumping = false;
							entity[i].velocity = 0; // Reset velocity when on the ground
							entity[i].y = BLCtileY * 8 - entity[i].sizeY;
						}
					}

					if (isTileSolid(gameTerrain[BRCtileX + BRCtileY * MAP_WIDTH_MAX]))
					{
						if (entity[i].isOnGround == false && entity[i].velocity >= 0)
						{
							entity[i].isOnGround = true;
							entity[i].isJumping = false;
							entity[i].velocity = 0; // Reset velocity when on the ground
							entity[i].y = BRCtileY * 8 - entity[i].sizeY;
						}
					}

					if (!isTileSolid(gameTerrain[BLCtileX + BLCtileY * MAP_WIDTH_MAX]) && !isTileSolid(gameTerrain[BRCtileX + BRCtileY * MAP_WIDTH_MAX]))
					{
						if (entity[i].isOnGround)
						{
							entity[i].isOnGround = false;
							entity[i].isJumping = true;
						}
					}
				}

				// Handle entity AI

				if (entityAISounds[entities[entity[i].type].AItype].idleCount > 0 && frame % 60 == 0 && rando(1, 20) == 1) // Play idle sound every second
				{
					mmEffect(entityAISounds[entities[entity[i].type].AItype].idle[rando(0, entityAISounds[entities[entity[i].type].AItype].idleCount - 1)]);
				}

				entity[i].nextTick--;
				if (entities[entity[i].type].AItype == ENTITY_AI_SLIME)
				{
					if (entity[i].nextTick <= 0 && entity[i].isOnGround) // Tick
					{
						entity[i].velocity = -7;
						entity[i].isOnGround = false;
						entity[i].isJumping = true;
						entity[i].nextTick = rando(2 * 60, 5 * 60); // Jump every 1 to 3 seconds
						if (player.x > entity[i].x)
						{
							changeEntityVelocityX(i, 1);
						}
						else
						{
							changeEntityVelocityX(i, -1);
						}
					}
					if (entity[i].isOnGround)
					{
						if (entity[i].nextTick <= 60)
						{
							entity[i].animation = ANIM_WALK;
						}
					}
				}
				else if (entities[entity[i].type].AItype == ENTITY_AI_BUNNY)
				{
					if (entity[i].nextTick <= 0 && entity[i].isOnGround) // Tick
					{
						entity[i].velocity = -7;
						entity[i].isOnGround = false;
						entity[i].isJumping = true;
						entity[i].isLookingLeft = rando(0, 1);
						entity[i].nextTick = rando(2 * 60, 7 * 60); // Jump every 2 to 7 seconds
						if (entity[i].isLookingLeft)
						{
							changeEntityVelocityX(i, -1);
						}
						else
						{
							changeEntityVelocityX(i, 1);
						}
					}
					if (entity[i].isOnGround)
					{
						if (entity[i].nextTick <= 60)
						{
							changeEntityVelocityX(i, entity[i].isLookingLeft ? -1 : 1); // Walk in the direction it's looking
						}
					}
				}
				else if (entities[entity[i].type].AItype == ENTITY_AI_ZOMBIE)
				{
					if (frame % 2 == 0)
					{
						if (gametime >= DAY_LENGTH / 2 && gametime < DAY_LENGTH - 16) // Night time: hunt the player
						{
							if (player.x > entity[i].x && entity[i].isOnGround)
							{
								changeEntityVelocityX(i, 1);
								entity[i].isLookingLeft = false;
							}
							else if (player.x < entity[i].x && entity[i].isOnGround)
							{
								changeEntityVelocityX(i, -1);
								entity[i].isLookingLeft = true;
							}
						}
						else // Day time: run for your fucking life (do zombies have lives?)
						{
							if (player.x < entity[i].x && entity[i].isOnGround)
							{
								changeEntityVelocityX(i, 1);
								entity[i].isLookingLeft = false;
							}
							else if (player.x > entity[i].x && entity[i].isOnGround)
							{
								changeEntityVelocityX(i, -1);
								entity[i].isLookingLeft = true;
							}
						}

						if (entity[i].nextTick <= 0 && entity[i].isOnGround) // Tick
						{
							if (entity[i].velocityX > 0) // Jump over obstacles
							{
								if (isTileSolid(gameTerrain[(entity[i].x + entity[i].sizeX - 1 + entity[i].velocityX) / 8 + entity[i].y / 8 * MAP_WIDTH_MAX]) ||
									isTileSolid(gameTerrain[(entity[i].x + entity[i].sizeX - 1 + entity[i].velocityX) / 8 + (entity[i].y + entity[i].sizeY / 2) / 8 * MAP_WIDTH_MAX]) ||
									isTileSolid(gameTerrain[(entity[i].x + entity[i].sizeX - 1 + entity[i].velocityX) / 8 + (entity[i].y + entity[i].sizeY - 1) / 8 * MAP_WIDTH_MAX]))
								{
									entity[i].velocity = -7;
									entity[i].isOnGround = false; // Add this line to stop friction and allow zombie to move in the air
								}
							}
							else if (entity[i].velocityX < 0)
							{
								if (isTileSolid(gameTerrain[(entity[i].x + entity[i].velocityX) / 8 + entity[i].y / 8 * MAP_WIDTH_MAX]) ||
									isTileSolid(gameTerrain[(entity[i].x + entity[i].velocityX) / 8 + (entity[i].y + entity[i].sizeY / 2) / 8 * MAP_WIDTH_MAX]) ||
									isTileSolid(gameTerrain[(entity[i].x + entity[i].velocityX) / 8 + (entity[i].y + entity[i].sizeY - 1) / 8 * MAP_WIDTH_MAX]))
								{
									entity[i].velocity = -7;
									entity[i].isOnGround = false;
								}
							}
							entity[i].nextTick = rando(1 * 60, 3 * 60); // Jump every 1 to 3 seconds
						}
					}
				}
				else if (entities[entity[i].type].AItype == ENTITY_AI_EYE)
				{
					changeEntityAngle(i, atan2(player.y - entity[i].y, player.x - entity[i].x));
					changeEntityVelocityX(i, 2 * cos(entity[i].angle));
					changeEntityVelocityY(i, 2 * sin(entity[i].angle));
				}

				if (checkPlayerCollision(entity[i].x, entity[i].y, entity[i].sizeX, entity[i].sizeY) && entities[entity[i].type].type == ENTITY_TYPE_HOSTILE)
				{
					knockBackPlayer((entity[i].x > player.x) ? -5 : 5, -5);
					playerDamage(entities[entity[i].type].damage, HIT_PVP, entities[entity[i].type].name);
				}
			}
		}

		for (int i = 0; i < MAX_ITEMS_TOTAL; i++)
		{
			if (item[i].exists)
			{
				if (frame - item[i].atFrame >= MAX_ITEM_AGE)
				{
					destroyItem(i);
					continue;
				}
				// Item falls until it hits a tile
				// TODO: if item inside a tile, try to get it to the closest side with a non solid tile
				if (!isTileSolid(gameTerrain[item[i].x / 8 + (item[i].y + 8 + item[i].velocity) / 8 * MAP_WIDTH_MAX]))
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
						if (item[i].tile >= ITEM_COPPER_COIN && item[i].tile <= ITEM_GOLD_COIN) // Round up coins
						{
							for (int j = 0; j < 8 * 4; j++)
							{
								if (inventory[j] >= ITEM_COPPER_COIN && inventory[j] <= ITEM_GOLD_COIN && inventoryQuantity[j] == 100)
								{
									setInventory(j, 0, 0);
									giveInventory(item[i].tile + 1, 1);
								}
							}
						}
					}
				}

				// Check for close items to fuse together to not waste memory
				for (int j = 0; j < MAX_ITEMS_TOTAL; j++)
				{
					if (i == j)
						continue;
					if (item[i].exists && item[j].exists && item[j].tile == item[i].tile && item[j].x >= item[i].x - 32 && item[j].x < item[i].x + 32 && item[j].y >= item[i].y - 32 && item[j].y < item[i].y + 32)
					{
						item[i].quantity += item[j].quantity;
						item[i].atFrame = frame;
						destroyItem(j);
					}
				}

				item[i].y += item[i].velocity;

				if (item[i].y > mapHeight * 8)
				{
					destroyItem(i); // Remove item if it falls out of the map
					continue;
				}

				item[i].renderX = item[i].x - scrollX;
				item[i].renderY = item[i].y - scrollY;
			}
		}

		// Heal player every 3 seconds if not in invincibility frames
		if (!player.invincibilityFrames && frame % (60 * 3) == 0)
		{
			if (player.health < player.maxHealth)
			{
				player.health += 1;
			}
		}

		if (player.invincibilityFrames > 0)
			player.invincibilityFrames--;

		// Time transitioning: 0 (for a while) -> goes slowly to 16 (full night) -> Transitions to night BG -> goes back to 0 -> stays until day -> goes to 16 -> Transitions to day BG -> goes back to 0 -> repeat

		if (frame % 60 == 0) // Every second
		{
			gametime++;
			if (gametime >= DAY_LENGTH)
				gametime = 0;

			if (gametime >= DAY_LENGTH / 2 - 16 && gametime < DAY_LENGTH / 2)
			{
				darkness = (gametime - (DAY_LENGTH / 2 - 16)); // Transition to darkness
			}
			else if (gametime == DAY_LENGTH / 2) // Copy night BG
			{
				darkness = 16;
				dmaCopy(nbgTiles, (void *)CHAR_BASE_BLOCK_SUB(1), nbgTilesLen);
				dmaCopy(nbgMap, (void *)SCREEN_BASE_BLOCK_SUB(0), nbgMapLen);
			}
			else if (gametime > DAY_LENGTH / 2 && gametime < DAY_LENGTH / 2 + 16)
			{
				darkness = (DAY_LENGTH / 2 + 16) - gametime; // Transition to no darkness (to show night bg)
			}
			else if (gametime >= DAY_LENGTH / 2 + 16 && gametime < DAY_LENGTH - 16)
			{
				darkness = 0; // Night time
			}
			else if (gametime >= DAY_LENGTH - 16 && gametime < DAY_LENGTH)
			{
				darkness = (gametime - (DAY_LENGTH - 16)); // Transition to darkness
			}
			else if (gametime == 0) // Copy day BG
			{
				darkness = 16;
				dmaCopy(bgTiles, (void *)CHAR_BASE_BLOCK_SUB(1), bgTilesLen);
				dmaCopy(bgMap, (void *)SCREEN_BASE_BLOCK_SUB(0), bgMapLen);
			}
			else if (gametime > 0 && gametime < 16)
			{
				darkness = 16 - gametime; // Transition to no darkness (to show day bg)
			}
			else if (gametime >= 16 && gametime < DAY_LENGTH / 2 - 16)
			{
				darkness = 0; // Day time
			}

			bool isDay = gametime >= 0 && gametime < DAY_LENGTH / 2;

			if (rando(0, 10) == 0)
			{
				int spawnX, spawnY;
				int attempts = 0; // A failsafe if we can't find an entity to spawn, kept it shared just in case of a very bad luck
				do
				{
					int offsetX = rando(SCREEN_WIDTH / 2, SCREEN_WIDTH / 2 + 64) * (rando(0, 1) ? 1 : -1);
					int offsetY = rando(SCREEN_HEIGHT / 2, SCREEN_HEIGHT / 2 + 64) * (rando(0, 1) ? 1 : -1);

					spawnX = clamp(scrollX + SCREEN_WIDTH / 2 + offsetX, 0, mapWidth * 8 - 1);
					spawnY = clamp(scrollY + SCREEN_HEIGHT / 2 + offsetY, 0, mapHeight * 8 - 1);
					spawnY = (getHighestTileYFrom(spawnX / 8, spawnY / 8) - 1) * 8;
				} while ((isTileSolid(gameTerrain[spawnX / 8 + (spawnY / 8) * MAP_WIDTH_MAX]) ||
						  !isTileSolid(gameTerrain[spawnX / 8 + (spawnY / 8 + 1) * MAP_WIDTH_MAX])) &&
						 attempts++ < 100);
				int entityToSpawn = -1;
				while (entityToSpawn == -1 && attempts++ < 100)
				{
					int i = rando(0, sizeof(entities) / sizeof(entities[0]) - 1);
					if ((entities[i].isUnderground && spawnY / 8 >= stoneSurface[spawnX / 8]) ||
						(entities[i].type == ENTITY_TYPE_HOSTILE && entities[i].isSurface && spawnY / 8 < stoneSurface[spawnX / 8] && !isDay) ||
						(entities[i].type == ENTITY_TYPE_PASSIVE && entities[i].isSurface && spawnY / 8 < stoneSurface[spawnX / 8] && isDay))
					{
						for (int j = 0; j < entities[i].biomeCount; j++)
						{
							if (biomeSurface[spawnX / 8] == entities[i].biomes[j])
								entityToSpawn = i;
						}
					}
				}
				if (entityToSpawn != -1 && attempts < 100)
				{
					spawnEntity(entityToSpawn,
								spawnX,
								spawnY - entities[entityToSpawn].sizeY);
				}
			}

			for (int i = 0; i < ENTITY_COUNT; i++)
			{
				if (entity[i].exists)
				{
					Entity *E = &entity[i];
					if (!isInPlayerRadius(E->x, E->y, SCREEN_WIDTH) &&
						((entities[E->type].type == ENTITY_TYPE_HOSTILE && rando(1, 15) == 1) ||
						 (entities[E->type].type == ENTITY_TYPE_PASSIVE && rando(1, 30) == 1)))
					{
						removeEntity(i);
					}
				}
			}
		}

		if (frame % 15 == 0)
		{
			craftableRecipesCount = 0;

			int nearbySpecials[SPECIALS];
			int specialCount = 0;

			int playerTX = player.x / 8, playerTY = player.y / 8;

			for (int x = 0; x < PLAYER_STATION_RANGE * 2 + 1; x++)
			{
				for (int y = 0; y < PLAYER_STATION_RANGE * 2 + 1; y++)
				{
					int tx = playerTX + x - PLAYER_STATION_RANGE;
					int ty = playerTY + y - PLAYER_STATION_RANGE;

					if (tx < 0 || tx >= mapWidth || ty < 0 || ty >= mapHeight)
						continue;

					if (tileProperties[gameTerrain[tx + ty * MAP_WIDTH_MAX]].specialParam != SPECIAL_NONE)
					{
						bool add = true;
						for (int i = 0; i < specialCount; i++)
						{
							if (nearbySpecials[i] == tileProperties[gameTerrain[tx + ty * MAP_WIDTH_MAX]].specialParam)
							{
								add = false;
								break;
							}
						}
						if (add && specialCount < SPECIALS)
							nearbySpecials[specialCount++] = tileProperties[gameTerrain[tx + ty * MAP_WIDTH_MAX]].specialParam;
					}
				}
			}
			// Refresh craftable items
			for (int i = 0; i < sizeof(craftingRecipes) / sizeof(craftingRecipes[0]); i++)
			{
				bool canCraft = false;

				if (craftingRecipes[i].specialParam == SPECIAL_NONE)
					canCraft = true;

				for (int j = 0; j < specialCount; j++)
				{
					if (craftingRecipes[i].specialParam == nearbySpecials[j])
					{
						canCraft = true;
						break;
					}
				}

				bool hasAll = true;
				bool hasAny = false;

				if (canCraft)
				{
					for (int j = 0; j < craftingRecipes[i].ingredientCount; j++)
					{
						if (!playerHasItem(craftingRecipes[i].itemsNeeded[j], craftingRecipes[i].itemsNeededQuantity[j]))
							hasAll = false;
						if (playerHasItem(craftingRecipes[i].itemsNeeded[j], 1))
							hasAny = true;
					}
				}

				if (craftingShowCraftableOnly && !hasAll)
					canCraft = false;
				else if (!craftingShowCraftableOnly && !hasAny)
					canCraft = false;

				if (canCraft)
					craftableRecipes[craftableRecipesCount++] = i;
			}

			renderCrafting();

			for (int i = 0; i < ENTITY_COUNT; i++)
			{
				if (entity[i].exists)
				{
					Entity *E = &entity[i];
					if (E->x < 0 || E->x >= mapWidth * 8 || E->y < 0 || E->y >= mapHeight * 8)
					{
						removeEntity(i);
					}
				}
			}
		}

		REG_BLDALPHA_SUB =
			(16 - darkness) | // BG0 weight
			(darkness << 8);  // BG1 weight

		// Compute screen-relative render coordinates
		player.renderX = player.x - scrollX;
		player.renderY = player.y - scrollY;

		// Rendering Part

		if (!inventoryOpen)
		{
			clearPrint();
			print(0, 3, "TerrariaDS v" VERSION "\n\
By AzizBgBoss\n\
https://github.com/AzizBgBoss/TerrariaDS");
			renderInventoryNoSound();
			setInventorySelectionNoSound(inventorySelection);

			print(0, 8, "");

			int nearestEntities[5] = {-1, -1, -1, -1, -1}; // IDs of the nearest 5 entities

			for (int i = 0; i < ENTITY_COUNT; i++)
			{
				if (entity[i].exists)
				{
					int dist = abs(player.x - entity[i].x) + abs(player.y - entity[i].y);
					for (int j = 0; j < 5; j++)
					{
						if ((nearestEntities[j] == -1 || dist < abs(player.x - entity[nearestEntities[j]].x) + abs(player.y - entity[nearestEntities[j]].y)) &&
							isInPlayerRange(entity[i].x, entity[i].y))
						{
							for (int k = 4; k > j; k--)
							{
								nearestEntities[k] = nearestEntities[k - 1];
							}
							nearestEntities[j] = i;
							break;
						}
					}
				}
			}

			for (int i = 0; i < 5; i++)
			{
				if (nearestEntities[i] != -1)
				{
					printDirect(entities[entity[nearestEntities[i]].type].name);
					printDirect(" ");
					printValDirect(entity[nearestEntities[i]].health);
					printDirect("/");
					printValDirect(entities[entity[nearestEntities[i]].type].health);
					printDirect("     \n");
				}
			}

			int nearestItems[5] = {-1, -1, -1, -1, -1}; // IDs of the nearest 5 items

			for (int i = 0; i < MAX_ITEMS; i++)
			{
				if (item[i].exists)
				{
					int dist = abs(player.x - item[i].x) + abs(player.y - item[i].y);
					for (int j = 0; j < 5; j++)
					{
						if ((nearestItems[j] == -1 || dist < abs(player.x - item[nearestItems[j]].x) + abs(player.y - item[nearestItems[j]].y)) &&
							isInPlayerRange(item[i].x, item[i].y))
						{
							for (int k = 4; k > j; k--)
							{
								nearestItems[k] = nearestItems[k - 1];
							}
							nearestItems[j] = i;
							break;
						}
					}
				}
			}

			for (int i = 0; i < 5; i++)
			{
				if (nearestItems[i] != -1)
				{
					printValDirect(item[nearestItems[i]].quantity);
					printDirect(" ");
					printDirect(getElementName(item[nearestItems[i]].tile));
					printDirect("     \n");
				}
			}
		}

		print(0, 0, "Health: ");
		printValDirect(player.health);
		printDirect("/");
		printValDirect(player.maxHealth);
		printDirect("   \n");

		if (debug)
		{
			int entityCount = 0;
			int itemCount = 0;
			int chestCount = 0;
			for (int i = 0; i < ENTITY_COUNT; i++)
			{
				if (entity[i].exists)
					entityCount++;
			}
			for (int i = 0; i < MAX_ITEMS; i++)
			{
				if (item[i].exists)
					itemCount++;
			}
			for (int i = 0; i < CHEST_COUNT; i++)
			{
				if (chestLinks[i].active)
					chestCount++;
			}

			printDirect("X, Y: ");
			printValDirect(player.x);
			printDirect(", ");
			printValDirect(player.y);
			printDirect("   \n");
			printDirect("scale: ");
			printValDirect(scale);
			printDirect("   \n");
			printDirect("Vel X, Y: ");
			printValDirect(player.velocityX);
			printDirect(", ");
			printValDirect(player.velocity);
			printDirect("   \n");
			printDirect("time: ");
			printValDirect(gametime);
			printDirect("   \n");
			printDirect("entities: ");
			printValDirect(entityCount);
			printDirect("   \n");
			printDirect("items: ");
			printValDirect(itemCount);
			printDirect("   \n");
			printDirect("chests: ");
			printValDirect(chestCount);
			printDirect("   \n");
			printDirect("invincibility: ");
			printValDirect(player.invincibilityFrames);
			printDirect("   \n");
			printDirect("Scroll, Render: ");
			printValDirect(scrollX);
			printDirect(", ");
			printValDirect(scrollY);
			printDirect(" | ");
			printValDirect(player.renderX);
			printDirect(", ");
			printValDirect(player.renderY);
			printDirect("   \n");
		}

		// Apply scroll
		bgSetCenter(bg2, player.renderX + player.sizeX / 2, player.renderY + player.sizeY / 2);
		bgSetScroll(bg2, scrollX + player.renderX + player.sizeX / 2, scrollY + player.renderY + player.sizeY / 2);
		bgSetScale(bg2, scale, scale);
		oamRotateScale(&oamSub, 0, degreesToAngle(0), scale * 2 * (player.isLookingLeft ? -1 : 1), scale * 2);
		for (int i = 0; i < ENTITY_COUNT; i++)
		{
			if (entity[i].exists)
			{
				oamRotateScale(&oamSub,
							   2 + i,
							   degreesToAngle(RAD2DEG(-entity[i].angle)),
							   scale * 2 * (entity[i].isLookingLeft ? -1 : 1),
							   scale * 2);
			}
		}
		REG_BG0HOFS_SUB = scrollX / 8;
		REG_BG0VOFS_SUB = scrollY / 8;

		// Animate player
		switch (player.animation)
		{
		case ANIM_NONE:
			setPlayerAnimFrame(0);
			break;
		case ANIM_WALK:
			setPlayerAnimFrame((frame / 4) % 8 + 1);
			break;
		case ANIM_JUMP:
			setPlayerAnimFrame(9);
			break;
		}

		for (int i = 0; i < ENTITY_COUNT; i++)
		{
			if (entity[i].exists)
			{
				if (entities[entity[i].type].AItype == ENTITY_AI_SLIME)
				{
					// Animate slime
					switch (entity[i].animation)
					{
					case ANIM_NONE:
						setEntityAnimFrame(i, (frame / 30) % 2);
						break;
					case ANIM_WALK:
						setEntityAnimFrame(i, (frame / 4) % 2);
						break;
					case ANIM_JUMP:
						setEntityAnimFrame(i, 0);
						break;
					}
				}
				if (entities[entity[i].type].AItype == ENTITY_AI_BUNNY)
				{
					// Animate bunny
					switch (entity[i].animation)
					{
					case ANIM_NONE:
						setEntityAnimFrame(i, 0);
						break;
					case ANIM_JUMP:
					case ANIM_WALK:
						setEntityAnimFrame(i, (frame / 4) % 7);
						break;
					}
				}
				if (entities[entity[i].type].AItype == ENTITY_AI_ZOMBIE)
				{
					// Animate zombie
					switch (entity[i].animation)
					{
					case ANIM_NONE:
						setEntityAnimFrame(i, 0);
						break;
					case ANIM_WALK:
						setEntityAnimFrame(i, 2 - abs(((frame / 16) % 4) - 2)); // 0, 1, 2, 1, 0, 1, 2, 1, ...
						break;
					case ANIM_JUMP:
						setEntityAnimFrame(i, 2);
						break;
					}
				}
				if (entities[entity[i].type].AItype == ENTITY_AI_EYE)
				{
					// Animate demon eye
					switch (entity[i].animation)
					{
					case ANIM_NONE:
						setEntityAnimFrame(i, 0);
						break;
					case ANIM_WALK:
						setEntityAnimFrame(i, 0);
						break;
					case ANIM_JUMP:
						setEntityAnimFrame(i, (frame / 8) % 2);
						break;
					}
				}
			}
		}

		// chunking, on both axises

		int camTileX = (scrollX / 8) - 32 + 1;
		int camTileY = (scrollY / 8) - 32 + 1;

		while (camTileX > lastCamTileX)
		{
			lastCamTileX++;
			for (int y = 0; y < 64; y++)
			{
				int mapX = ((lastCamTileX + 63) % mapWidth + mapWidth) % mapWidth;
				int mapY = ((camTileY + y) % mapHeight + mapHeight) % mapHeight;
				Bg1SetTile((63 + lastCamTileX) % 64, (y + camTileY) % 64, getElementTile(gameTerrain[mapX + mapY * MAP_WIDTH_MAX], mapX, mapY));
			}
		}
		while (camTileX < lastCamTileX)
		{
			lastCamTileX--;
			for (int y = 0; y < 64; y++)
			{
				int mapX = ((lastCamTileX) % mapWidth + mapWidth) % mapWidth;
				int mapY = ((camTileY + y) % mapHeight + mapHeight) % mapHeight;
				Bg1SetTile((lastCamTileX) % 64, (y + camTileY) % 64, getElementTile(gameTerrain[mapX + mapY * MAP_WIDTH_MAX], mapX, mapY));
			}
		}

		while (camTileY > lastCamTileY)
		{
			lastCamTileY++;
			for (int x = 0; x < 64; x++)
			{
				int mapX = ((camTileX + x) % mapWidth + mapWidth) % mapWidth;
				int mapY = ((lastCamTileY + 63) % mapHeight + mapHeight) % mapHeight;
				Bg1SetTile((x + camTileX) % 64, (63 + lastCamTileY) % 64, getElementTile(gameTerrain[mapX + mapY * MAP_WIDTH_MAX], mapX, mapY));
			}
		}
		while (camTileY < lastCamTileY)
		{
			lastCamTileY--;
			for (int x = 0; x < 64; x++)
			{
				int mapX = ((camTileX + x) % mapWidth + mapWidth) % mapWidth;
				int mapY = ((lastCamTileY) % mapHeight + mapHeight) % mapHeight;
				Bg1SetTile((x + camTileX) % 64, (lastCamTileY) % 64, getElementTile(gameTerrain[mapX + mapY * MAP_WIDTH_MAX], mapX, mapY));
			}
		}

		if (player.invincibilityFrames > 0 && (player.invincibilityFrames / 4) % 2 == 0) // If the player is [TITLE CARD], flicker every 4 frames
		{
			oamSet(&oamSub, 0, -64, -64, 1, 0, SpriteSize_32x64, SpriteColorFormat_256Color, nullSprite, 0, false, false, false, false, false);
		}
		else
			oamSet(&oamSub, 0,
				   player.renderX - (player.sizeX / 2),
				   player.renderY - (player.sizeY / 2) - 4 * (scale - 128) / 128,													 // using a proportional offset to fix my bad math
				   1, 0, SpriteSize_32x64, SpriteColorFormat_256Color, player.sprite_gfx_mem, 0, false, false, false, false, false); // Don't use flip with oamRotateScale bruh
		// oamSet(&oamSub, 1, player.renderX, player.renderY, 0, 0, SpriteSize_16x16, SpriteColorFormat_256Color, itemHandSprite, -1, false, false, player.isLookingLeft, false, false);

		// Render entities
		u8 renderedEntities = 0;

		for (int i = 0; i < ENTITY_COUNT; i++)
		{
			if (entity[i].exists)
			{
				entity[i].renderX = entity[i].x - scrollX - (entity[i].x - player.x - player.sizeX / 2) * (scale - 256) / scale - (entity[i].sizeX / 2 * (scale - 128)) / 128;
				entity[i].renderY = entity[i].y - scrollY - (entity[i].y - player.y - player.sizeY / 2) * (scale - 256) / scale - (entity[i].sizeY / 2 * (scale - 128)) / 128;
				if (entity[i].renderX + entity[i].sizeX >= 0 && entity[i].renderX < SCREEN_WIDTH && entity[i].renderY + entity[i].sizeY >= 0 && entity[i].renderY < SCREEN_HEIGHT)
				{
					oamSet(&oamSub, renderedEntities + 2,
						   entity[i].renderX,
						   entity[i].renderY,
						   1, 0, entities[entity[i].type].spriteSize, SpriteColorFormat_256Color, entity[i].sprite_gfx_mem, i + 2, false, false, false, false, false);
					renderedEntities++;
				}
			}
		}

		// Clean up unused entity slots
		for (int i = renderedEntities; i < ENTITY_COUNT; i++)
		{
			oamSet(&oamSub, i + 2, 0, 0, 0, 0, SpriteSize_8x8, SpriteColorFormat_256Color, nullSprite, -1, false, false, false, false, false);
		}

		// Render dropped items
		u8 renderedItems = 0;

		for (int i = 0; i < MAX_ITEMS_TOTAL; i++)
		{
			if (renderedItems < MAX_ITEMS)
			{
				if (item[i].exists)
				{
					item[i].renderX = item[i].x - scrollX - (item[i].x - player.x - player.sizeX / 2) * (scale - 256) / scale - 8;
					item[i].renderY = item[i].y - scrollY - (item[i].y - player.y - player.sizeY / 2) * (scale - 256) / scale - 8;
					if (item[i].renderX + 16 >= 0 && item[i].renderX < SCREEN_WIDTH && item[i].renderY + 16 >= 0 && item[i].renderY < SCREEN_HEIGHT)
					{
						oamRotateScale(&oamSub, renderedItems + ENTITY_COUNT + 2, degreesToAngle(0), scale * 2, scale * 2);
						oamSet(&oamSub, renderedItems + ENTITY_COUNT + 2, item[i].renderX, item[i].renderY, 0, 0, SpriteSize_16x16, SpriteColorFormat_256Color, item[i].sprite_gfx_mem, i + ENTITY_COUNT + 2, false, false, false, false, false);
						renderedItems++;
					}
				}
			}
		}
		// Clean up unused sprites
		for (int i = renderedItems; i < MAX_ITEMS; i++)
		{
			oamSet(&oamSub, i + ENTITY_COUNT + 2, 0, 0, 0, 0, SpriteSize_8x8, SpriteColorFormat_256Color, nullSprite, -1, false, false, false, false, false);
		}

		oamUpdate(&oamSub);
		bgUpdate();

		frame++;
	}

	return 0;
}
