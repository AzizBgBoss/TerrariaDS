// Background utilities

#include "defs.h"

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