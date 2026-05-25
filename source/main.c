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

	nitroFSInit(NULL);

	mmInitDefault("nitro:/soundbank.bin");

	mmLoadEffect(SFX_IG_0);
	mmLoadEffect(SFX_IG_1);
	mmLoadEffect(SFX_IG_2);
	mmLoadEffect(SFX_RAB);
	mmLoadEffect(SFX_ENU_OPEN);
	mmLoadEffect(SFX_ENU_CLOSE);
	mmLoadEffect(SFX_ENU_TICK);
	mmLoadEffect(SFX_LAYER_HIT_0);
	mmLoadEffect(SFX_LAYER_HIT_1);
	mmLoadEffect(SFX_LAYER_HIT_2);
	mmLoadEffect(SFX_LAYER_KILLED);
	mmLoadEffect(SFX_MUSHROOM);
	mmLoadEffect(SFX_SWING);
	mmLoadEffect(SFX_HIT);
	mmLoadEffect(SFX_DOOR_OPEN);
	mmLoadEffect(SFX_DOOR_CLOSE);

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

	int intro = rando(0, 9);

	if (intro == 0)
		audioFile = fopen("nitro:/special.pcm", "rb");
	else
		audioFile = fopen("nitro:/1.pcm", "rb");

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
		f = fopen("nitro:/intro2.img.bin", "rb");
		fread((void *)CHAR_BASE_BLOCK(1), 1, intro2TilesLen, f);
		fclose(f);
		f = fopen("nitro:/intro2.map.bin", "rb");
		fread((void *)SCREEN_BASE_BLOCK(0), 1, intro2MapLen, f);
		fclose(f);
		f = fopen("nitro:/intro2.pal.bin", "rb");
		fread((void *)BG_PALETTE, 1, intro2PalLen, f);
		fclose(f);
	}
	else if (intro < 3)
	{
		f = fopen("nitro:/intro3.img.bin", "rb");
		fread((void *)CHAR_BASE_BLOCK(1), 1, intro3TilesLen, f);
		fclose(f);
		f = fopen("nitro:/intro3.map.bin", "rb");
		fread((void *)SCREEN_BASE_BLOCK(0), 1, intro3MapLen, f);
		fclose(f);
		f = fopen("nitro:/intro3.pal.bin", "rb");
		fread((void *)BG_PALETTE, 1, intro3PalLen, f);
		fclose(f);
	}
	else if (intro < 6)
	{
		f = fopen("nitro:/intro4.img.bin", "rb");
		fread((void *)CHAR_BASE_BLOCK(1), 1, intro4TilesLen, f);
		fclose(f);
		f = fopen("nitro:/intro4.map.bin", "rb");
		fread((void *)SCREEN_BASE_BLOCK(0), 1, intro4MapLen, f);
		fclose(f);
		f = fopen("nitro:/intro4.pal.bin", "rb");
		fread((void *)BG_PALETTE, 1, intro4PalLen, f);
		fclose(f);
	}
	else
	{
		f = fopen("nitro:/intro.img.bin", "rb");
		fread((void *)CHAR_BASE_BLOCK(1), 1, introTilesLen, f);
		fclose(f);
		f = fopen("nitro:/intro.map.bin", "rb");
		fread((void *)SCREEN_BASE_BLOCK(0), 1, introMapLen, f);
		fclose(f);
		f = fopen("nitro:/intro.pal.bin", "rb");
		fread((void *)BG_PALETTE, 1, introPalLen, f);
		fclose(f);
	}

	storeOriginalPalette();
	fadeInPalette(64, 8);

mainMenu:

	videoSetMode(MODE_0_2D | DISPLAY_BG0_ACTIVE | DISPLAY_BG3_ACTIVE);

	BGCTRL[3] = BG_TILE_BASE(3) | BG_MAP_BASE(3) | BG_COLOR_256 | BG_32x32 | BG_PRIORITY(0);
	f = fopen("nitro:/font.img.bin", "rb");
	fread((void *)CHAR_BASE_BLOCK(3), 1, fontTilesLen, f);
	fclose(f);
	dmaFillHalfWords(0, (void *)SCREEN_BASE_BLOCK(3), 2048);

	if (intro == 0)
	{
		f = fopen("nitro:/mainscreenbg2.img.bin", "rb");
		fread((void *)CHAR_BASE_BLOCK(1), 1, mainscreenbg2TilesLen, f);
		fclose(f);
		f = fopen("nitro:/mainscreenbg2.map.bin", "rb");
		fread((void *)SCREEN_BASE_BLOCK(0), 1, mainscreenbg2MapLen, f);
		fclose(f);
		f = fopen("nitro:/mainscreenbg2.pal.bin", "rb");
		fread((void *)BG_PALETTE, 1, mainscreenbg2PalLen, f);
		fclose(f);
	}
	else
	{
		f = fopen("nitro:/mainscreenbg.img.bin", "rb");
		fread((void *)CHAR_BASE_BLOCK(1), 1, mainscreenbgTilesLen, f);
		fclose(f);
		f = fopen("nitro:/mainscreenbg.map.bin", "rb");
		fread((void *)SCREEN_BASE_BLOCK(0), 1, mainscreenbgMapLen, f);
		fclose(f);
		f = fopen("nitro:/mainscreenbg.pal.bin", "rb");
		fread((void *)BG_PALETTE, 1, mainscreenbgPalLen, f);
		fclose(f);
	}

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
	fclose(audioFile);

	videoSetMode(MODE_0_2D | DISPLAY_BG0_ACTIVE | DISPLAY_BG1_ACTIVE | DISPLAY_BG3_ACTIVE);	   // BG0: Background (item slots, etc...), BG1: Item tiles, BG3: Text
	videoSetModeSub(MODE_3_2D | DISPLAY_BG0_ACTIVE | DISPLAY_BG1_ACTIVE | DISPLAY_BG3_ACTIVE); // BG0: Background, BG1: Black background (to darken bg0), BG3: Game terrain

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
	f = fopen("nitro:/ui.img.bin", "rb");
	fread(inventorySelectionSprite, 1, uiTilesLen, f);
	fclose(f);

	f = fopen("nitro:/ui.pal.bin", "rb");
	fread((void *)SPRITE_PALETTE, 1, uiPalLen, f);
	fclose(f);

	oamInit(&oamSub, SpriteMapping_1D_128, false);

	nullSprite = oamAllocateGfx(&oamSub, SpriteSize_16x16, SpriteColorFormat_256Color);
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
			int maxSelection = 7;

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
						printDirect("Set health to max");
						break;
					case 2:
						printDirect("Kill all entities");
						break;
					case 3:
						printDirect("Kill player");
						break;
					case 4:
						printDirect("Become [TITLE CARD] for 5 mins");
						break;
					case 5:
						printDirect("Show debug info: ");
						if (debug)
							printDirect("ON ");
						else
							printDirect("OFF");
						break;
					case 6:
						printDirect("Set time to night");
						break;
					case 7:
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
								clearPrint();
								print(0, 0, "Debug menu, bazinga!\n");
								printDirect("Press X to close debug menu.\n");
								break;
							}
							else if (pressed & KEY_X)
							{
								clearPrint();
								print(0, 0, "Debug menu, bazinga!\n");
								printDirect("Press X to close debug menu.\n");
								break;
							}
						}
						break;
					case 1:
						player.health = 400;
						printDirect("\nHealth set to max!");
						break;
					case 2:
						for (int e = 0; e < ENTITY_COUNT; e++)
						{
							if (entity[e].exists)
							{
								killEntity(e);
							}
						}
						printDirect("\nAll entities killed!");
						break;
					case 3:
						playerDamage(9999);
						printDirect("\nPlayer killed!");
						break;
					case 4:
						player.invincibilityFrames += 60 * 5 * 60; // 5 minutes of invincibility at 60 fps
						printDirect("\nInvincibility for 5 minutes!");
						break;
					case 5:
						debug = !debug;
						break;
					case 6:
						gametime = DAY_LENGTH / 2; // Night time
						printDirect("\nTime set to night!");
						break;
					case 7:
						gametime = 0; // Day time
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
			if (!isTileSolid(gameTerrain[(player.x + player.sizeX - 1 + player.velocityX) / 8 + player.y / 8 * MAP_WIDTH_MAX]) &&
				!isTileSolid(gameTerrain[(player.x + player.sizeX - 1 + player.velocityX) / 8 + (player.y + player.sizeY / 2) / 8 * MAP_WIDTH_MAX]) &&
				!isTileSolid(gameTerrain[(player.x + player.sizeX - 1 + player.velocityX) / 8 + (player.y + player.sizeY - 1) / 8 * MAP_WIDTH_MAX]))
			{
				player.x += player.velocityX;
			}
		}
		else if (player.velocityX < 0)
		{
			if (!isTileSolid(gameTerrain[(player.x + player.velocityX) / 8 + player.y / 8 * MAP_WIDTH_MAX]) &&
				!isTileSolid(gameTerrain[(player.x + player.velocityX) / 8 + (player.y + player.sizeY / 2) / 8 * MAP_WIDTH_MAX]) &&
				!isTileSolid(gameTerrain[(player.x + player.velocityX) / 8 + (player.y + player.sizeY - 1) / 8 * MAP_WIDTH_MAX]))
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
			// TODO: add entity damaging with swords wow
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
							if (inventory[inventorySelection] >= 1 && inventory[inventorySelection] < 100 // Object is a tile, not an item
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
							else if (inventory[inventorySelection] >= 100 && inventory[inventorySelection] < 200 // Object is an item, not a tile
									 && inventoryQuantity[inventorySelection])
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
								if (e != -1 && inventory[inventorySelection] >= 100 && inventory[inventorySelection] < 200)
								{
									mmEffect(SFX_SWING);
									damageEntity(e, 5);
									knockBackEntity(e, (player.x > entity[e].x) ? -5 : 5, -5);
								}
								else if (inventory[inventorySelection] == TILE_MUSHROOM && inventoryQuantity[inventorySelection] && player.health < player.maxHealth)
								{
									mmEffect(SFX_MUSHROOM);
									playerHeal(10);
									setInventory(inventorySelection, inventory[inventorySelection], inventoryQuantity[inventorySelection] - 1);
								}
								else if (gameTerrain[worldTouchX + worldTouchY * MAP_WIDTH_MAX] != 0)
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
					playerDamage(10 * h);
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
					playerDamage(10 * h);
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
				if (entity[i].health <= 0)
				{
					killEntity(i);
					continue;
				}
				if ((0 <= entity[i].type && entity[i].type < ENTITIES) && frame % 2 == 0) // Move every 2 frames to make it slower
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
					}
					else if (entity[i].velocityX < 0)
					{
						if (!isTileSolid(gameTerrain[(entity[i].x + entity[i].velocityX) / 8 + entity[i].y / 8 * MAP_WIDTH_MAX]) &&
							!isTileSolid(gameTerrain[(entity[i].x + entity[i].velocityX) / 8 + (entity[i].y + entity[i].sizeY / 2) / 8 * MAP_WIDTH_MAX]) &&
							!isTileSolid(gameTerrain[(entity[i].x + entity[i].velocityX) / 8 + (entity[i].y + entity[i].sizeY - 1) / 8 * MAP_WIDTH_MAX]))
						{
							entity[i].x += entity[i].velocityX;
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
				else if (entity[i].type == ENTITY_RED_SLIME)
				{
					if (frame % 2 == 0)
					{
						if (player.x > entity[i].x)
						{
							entity[i].x++;
						}
						else
						{
							entity[i].x--;
						}
						if (player.y > entity[i].y)
						{
							entity[i].y++;
						}
						else
						{
							entity[i].y--;
						}
					}
				}

				// Handle entity AI
				entity[i].nextTick--;
				if (entity[i].type == ENTITY_GREEN_SLIME)
				{
					if (entity[i].nextTick <= 0 && entity[i].isOnGround) // Tick
					{
						entity[i].velocity = -7;
						entity[i].isOnGround = false;
						entity[i].isJumping = true;
						entity[i].nextTick = rando(1 * 60, 3 * 60); // Jump every 1 to 3 seconds
						if (player.x > entity[i].x)
						{
							if (entity[i].velocityX < 1)
								entity[i].velocityX++;
						}
						else
						{
							if (entity[i].velocityX > -1)
								entity[i].velocityX--;
						}
					}

					if (checkPlayerCollision(entity[i].x, entity[i].y, entity[i].sizeX, entity[i].sizeY))
					{
						knockBackPlayer((entity[i].x > player.x) ? -5 : 5, -5);
						playerDamage(5);
					}
				}
				else if (entity[i].type == ENTITY_BUNNY)
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
							if (entity[i].velocityX > -1)
								entity[i].velocityX--;
						}
						else
						{
							if (entity[i].velocityX < 1)
								entity[i].velocityX++;
						}
					}
				}
				else if (entity[i].type == ENTITY_ZOMBIE)
				{
					if (frame % 2 == 0)
					{
						if (gametime >= DAY_LENGTH / 2 && gametime < DAY_LENGTH - 16) // Night time: hunt the player
						{
							if (player.x > entity[i].x && entity[i].isOnGround)
							{
								if (entity[i].velocityX < 1)
									entity[i].velocityX++;
								entity[i].isLookingLeft = false;
								entity[i].animation = ANIM_WALK;
							}
							else if (player.x < entity[i].x && entity[i].isOnGround)
							{
								if (entity[i].velocityX > -1)
									entity[i].velocityX--;
								entity[i].isLookingLeft = true;
								entity[i].animation = ANIM_WALK;
							}
						}
						else // Day time: run for your fucking life (do zombies have lives?)
						{
							if (player.x < entity[i].x && entity[i].isOnGround)
							{
								if (entity[i].velocityX < 1)
									entity[i].velocityX++;
								entity[i].isLookingLeft = false;
								entity[i].animation = ANIM_WALK;
							}
							else if (player.x > entity[i].x && entity[i].isOnGround)
							{
								if (entity[i].velocityX > -1)
									entity[i].velocityX--;
								entity[i].isLookingLeft = true;
								entity[i].animation = ANIM_WALK;
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

					if (checkPlayerCollision(entity[i].x, entity[i].y, entity[i].sizeX, entity[i].sizeY))
					{
						knockBackPlayer((entity[i].x > player.x) ? -5 : 5, -5);
						playerDamage(10);
					}
				}
			}
		}

		for (int i = 0; i < MAX_ITEMS_TOTAL; i++)
		{
			if (item[i].exists)
			{
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

		// Time transitioning: 0 (for a while) -> goes slowly to 16 (full night) -> stays at 16 (for a while) -> goes slowly back to 0 -> repeat
		// One whole day is about 10 minutes (600 seconds)
		// TODO: when background reaches full darkness, change bg to a night one and transition back to 100% opacity

		if (frame % 60 == 0) // Every second
		{
			gametime++;
			if (gametime >= DAY_LENGTH)
				gametime = 0;

			if (gametime < DAY_LENGTH / 2 - 16)
				darkness = 0; // Day time
			else if (gametime >= DAY_LENGTH / 2 - 16 && gametime < DAY_LENGTH / 2)
				darkness = (gametime - (DAY_LENGTH / 2 - 16)); // Transition to night
			else if (gametime >= DAY_LENGTH / 2 && gametime < DAY_LENGTH - 16)
				darkness = 16; // Night time
			else if (gametime >= DAY_LENGTH - 16 && gametime < DAY_LENGTH)
				darkness = 16 - (gametime - (DAY_LENGTH - 16)); // Transition to day

			if (gametime >= DAY_LENGTH / 2 && gametime < DAY_LENGTH - 16)
			{ // Night time
				if (rando(0, 15) == 0)
				{
					int entityToSpawn = -1;
					while (entityToSpawn == -1)
					{
						int i = rando(0, sizeof(entities) / sizeof(entities[0]) - 1);
						if (entities[i].type == ENTITY_TYPE_HOSTILE)
						{
							entityToSpawn = i;
						}
					}
					int spawnX = rando(0, 1) ? rando(-SCREEN_WIDTH * 2, -SCREEN_WIDTH) : rando(SCREEN_WIDTH, SCREEN_WIDTH * 2); // Spawn on left or right side of the screen
					spawnEntity(entityToSpawn,
								scrollX + SCREEN_WIDTH / 2 + spawnX,
								getHighestTileY(spawnX / 8) - entities[entityToSpawn].sizeY);
				}
			}
			else
			{ // Day time
				if (rando(0, 20) == 0)
				{
					int entityToSpawn = -1;
					while (entityToSpawn == -1)
					{
						int i = rando(0, sizeof(entities) / sizeof(entities[0]) - 1);
						if (entities[i].type == ENTITY_TYPE_PASSIVE)
						{
							entityToSpawn = i;
						}
					}
					int spawnX = rando(0, 1) ? rando(-SCREEN_WIDTH * 2, -SCREEN_WIDTH) : rando(SCREEN_WIDTH, SCREEN_WIDTH * 2); // Spawn on left or right side of the screen
					spawnEntity(entityToSpawn,
								scrollX + SCREEN_WIDTH / 2 + spawnX,
								getHighestTileY(spawnX / 8) - entities[entityToSpawn].sizeY);
				}
			}

			REG_BLDALPHA_SUB =
				(16 - darkness) | // BG0 weight
				(darkness << 8);  // BG1 weight
		}

		// Rendering Part

		print(0, 0, "Health: ");
		printValDirect(player.health);
		printDirect("/");
		printValDirect(player.maxHealth);
		printDirect("   \n");

		if (debug)
		{
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
		}

		// Compute screen-relative render coordinates
		player.renderX = player.x - scrollX;
		player.renderY = player.y - scrollY;

		// Apply scroll
		bgSetCenter(bg2, player.renderX + player.sizeX / 2, player.renderY + player.sizeY / 2);
		bgSetScroll(bg2, scrollX + player.renderX + player.sizeX / 2, scrollY + player.renderY + player.sizeY / 2);
		bgSetScale(bg2, scale, scale);
		oamRotateScale(&oamSub, 0, degreesToAngle(0), scale * 2 * (player.isLookingLeft ? -1 : 1), scale * 2);
		for (int i = 0; i < ENTITY_COUNT; i++)
		{
			if (entity[i].exists)
			{
				oamRotateScale(&oamSub, 2 + i, degreesToAngle(0), scale * 2 * (entity[i].isLookingLeft ? -1 : 1), scale * 2);
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
				if (entity[i].type <= ENTITY_BLUE_SLIME && entity[i].type >= ENTITY_GREEN_SLIME)
				{
					// Animate slime
					switch (entity[i].animation)
					{
					case ANIM_NONE:
						setEntityAnimFrame(i, 16);
						break;
					case ANIM_JUMP:
						setEntityAnimFrame(i, 0);
						break;
					}
				}
				if (entity[i].type == ENTITY_BUNNY)
				{
					// Animate bunny
					switch (entity[i].animation)
					{
					case ANIM_NONE:
						setEntityAnimFrame(i, 0);
						break;
					case ANIM_JUMP:
						setEntityAnimFrame(i, (frame / 4) % 7);
						break;
					}
				}
				if (entity[i].type == ENTITY_ZOMBIE)
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
			}
		}

		// chunking, on both axises

		int camTileX = (scrollX / 8) - 32 + 1;
		int camTileY = (scrollY / 8) - 32 + 1;

		if (camTileX > lastCamTileX)
		{
			lastCamTileX++;
			for (int y = 0; y < 64; y++)
			{
				int mapX = ((lastCamTileX + 63) % mapWidth + mapWidth) % mapWidth;
				int mapY = ((camTileY + y) % mapHeight + mapHeight) % mapHeight;
				Bg1SetTile((63 + lastCamTileX) % 64, (y + camTileY) % 64, getElementTile(gameTerrain[mapX + mapY * MAP_WIDTH_MAX], mapX, mapY));
			}
		}
		else if (camTileX < lastCamTileX)
		{
			lastCamTileX--;
			for (int y = 0; y < 64; y++)
			{
				int mapX = ((lastCamTileX) % mapWidth + mapWidth) % mapWidth;
				int mapY = ((camTileY + y) % mapHeight + mapHeight) % mapHeight;
				Bg1SetTile((lastCamTileX) % 64, (y + camTileY) % 64, getElementTile(gameTerrain[mapX + mapY * MAP_WIDTH_MAX], mapX, mapY));
			}
		}

		if (camTileY > lastCamTileY)
		{
			lastCamTileY++;
			for (int x = 0; x < 64; x++)
			{
				int mapX = ((camTileX + x) % mapWidth + mapWidth) % mapWidth;
				int mapY = ((lastCamTileY + 63) % mapHeight + mapHeight) % mapHeight;
				Bg1SetTile((x + camTileX) % 64, (63 + lastCamTileY) % 64, getElementTile(gameTerrain[mapX + mapY * MAP_WIDTH_MAX], mapX, mapY));
			}
		}
		else if (camTileY < lastCamTileY)
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
				   player.renderY - (player.sizeY / 2) - 4 * (scale - 128) / 128, // using a proportional offset to fix my bad math
				   1, 0, SpriteSize_32x64, SpriteColorFormat_256Color, player.sprite_gfx_mem, 0, false, false, player.isLookingLeft, false, false);
		// oamSet(&oamSub, 1, player.renderX, player.renderY, 0, 0, SpriteSize_16x16, SpriteColorFormat_256Color, itemHandSprite, -1, false, false, player.isLookingLeft, false, false);

		// Render entities
		u8 renderedEntities = 0;

		for (int i = 0; i < ENTITY_COUNT; i++)
		{
			if (entity[i].exists)
			{
				entity[i].renderX = entity[i].x - scrollX - (entity[i].x - player.x - player.sizeX / 2) * (scale - 256) / scale - (entity[i].sizeX / 2 * (scale - 128)) / 128;
				entity[i].renderY = entity[i].y - scrollY - (entity[i].y - player.y - player.sizeY / 2) * (scale - 256) / scale - (entity[i].sizeY / 2 * (scale - 128)) / 128;
				if (entity[i].renderX >= 0 && entity[i].renderX < SCREEN_WIDTH && entity[i].renderY >= 0 && entity[i].renderY < SCREEN_HEIGHT)
				{
					oamSet(&oamSub, renderedEntities + 2,
						   entity[i].renderX,
						   entity[i].renderY,
						   1, 0, entities[entity[i].type].spriteSize, SpriteColorFormat_256Color, entity[i].sprite_gfx_mem, i + 2, false, false, entity[i].isLookingLeft, false, false);
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
					if (item[i].renderX >= 0 && item[i].renderX < SCREEN_WIDTH && item[i].renderY >= 0 && item[i].renderY < SCREEN_HEIGHT)
					{
						oamRotateScale(&oamSub, renderedItems + ENTITY_COUNT + 2, degreesToAngle(0), scale * 2, scale * 2);
						oamSet(&oamSub, renderedItems + ENTITY_COUNT + 2, item[i].renderX, item[i].renderY, 0, 0, SpriteSize_16x16, SpriteColorFormat_256Color, item[i].sprite_gfx_mem, renderedItems + 2, false, false, false, false, false);
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
