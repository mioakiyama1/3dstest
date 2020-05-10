#include <3ds.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char **argv)
{
	gfxInitDefault();
	
	u8 param[0x300];
	u8 hmac[0x20];

	PrintConsole bWindow, rightbWindow, leftbWindow, leftWindow, rightWindow;
	consoleInit(GFX_TOP, &leftWindow);
	consoleInit(GFX_TOP, &rightWindow);
	consoleInit(GFX_TOP, &rightbWindow);
	consoleInit(GFX_TOP, &leftbWindow);
	consoleInit(GFX_BOTTOM, &bWindow);

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
		
		if (kDown & KEY_B)
		{
		memset(param, 0, sizeof(param));
		memset(hmac, 0, sizeof(hmac));
		APT_PrepareToDoApplicationJump(0, 0x0004000E00021800, 0);
		APT_DoApplicationJump(param, sizeof(param), hmac);
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
