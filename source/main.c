/*
	Multiple Windows Text example made by Aurelio Mannara for ctrulib
	This code was modified for the last time on: 12/12/2014 23:50 UTC+1

*/

#include <3ds.h>
#include <stdio.h>

int main(int argc, char **argv)
{
	// leave this to initialize
	gfxInitDefault();

	//ADD ALL NEW WINDOWS HERE
	PrintConsole bWindow, rightbWindow, leftbWindow, leftWindow, rightWindow;

	//Initialize console for each window
	consoleInit(GFX_TOP, &leftWindow);
	consoleInit(GFX_TOP, &rightWindow);
	consoleInit(GFX_TOP, &rightbWindow);
	consoleInit(GFX_TOP, &leftbWindow);
	consoleInit(GFX_BOTTOM, &bWindow);

	//Now we specify the window position and dimension for each console window using consoleSetWindow(PrintConsole* console, int x, int y, int width, int height);
	//x, y, width and height are in terms of cells, not pixel, where each cell is composed by 8x8 pixels.
	consoleSetWindow(&leftWindow, 1, 1, 23, 14);
	consoleSetWindow(&rightWindow, 26, 1, 23, 14);
	consoleSetWindow(&leftbWindow, 1, 17, 23, 14);
	consoleSetWindow(&rightbWindow, 26, 17, 23, 14);
	consoleSetWindow(&bWindow, 1, 1, 30, 20);

	consoleSelect(&leftWindow);
	printf("05-10-2020\n");
	printf("Hi mom\n");
	
	consoleSelect(&bWindow);
	printf("press START to exit\n");
	printf("press A to get more text\n");
	printf("this text is \x1b[31mred\x1b[0m\n");
	//x1b is ANSI escape, ["xx"m where xx is color identifier


	consoleSelect(&rightWindow);
	printf("Happy mother's day\n");

	// Main loop
	while (aptMainLoop())
	{
		//Scan all the inputs once for each frame
		hidScanInput();

		//hidKeysDown returns information about which buttons have been just pressed (and they weren't in the previous frame)
		u32 kDown = hidKeysDown();
		
		if (kDown & KEY_A)
		{
		consoleSelect(&leftbWindow);
		printf("A input works...");
		}

		if (kDown & KEY_START)
		{
		consoleSelect(&rightbWindow);
		printf("Exiting...");		
		break;
		}

		// Flush and swap framebuffers
		gfxFlushBuffers();
		gfxSwapBuffers();

		//Wait for VBlank
		gspWaitForVBlank();
	}


	gfxExit();
	
	return 0;
}
