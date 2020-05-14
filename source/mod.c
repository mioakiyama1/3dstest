#include <3ds.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char **argv)
{
	gfxInitDefault();
	//unsigned 8 bit
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
	printf("Hello\n");
	printf("this text is \x1b[31mred\x1b[0m\n");
	//x1b is ANSI escape, ["xx"m where xx is color identifier
	
	consoleSelect(&bWindow);
	printf("What you can do here\n\n");
	printf("press X for experimental menu (0.04)");
	printf("press A to get more text\n");
	printf("press B to attempt to boot another title (depreciated)\n");	



	consoleSelect(&rightWindow);
	printf("3dsapp v0.04");

	// loops once a frame
	while (aptMainLoop())
	{
		//Scan all the inputs once for each frame
		hidScanInput();

		//hidKeysDown returns information about which buttons have been just pressed (and they weren't in the previous frame)
		u32 kDown = hidKeysDown();
		
		if (kDown & KEY_A)
		{
		consoleSelect(&leftbWindow);
		printf("A input works...\n");
		}
		
		if (kDown & KEY_B)
		{
		memset(param, 0, sizeof(param));
		memset(hmac, 0, sizeof(hmac));
		APT_PrepareToDoApplicationJump(0, 0x0004000E00021800LL, 0);
		APT_DoApplicationJump(param, sizeof(param), hmac);
		}

		if (kDown & KEY_START)
		{
		consoleSelect(&rightbWindow);
		printf("Exiting...");		
		break;
		}
		
		if (kDown & KEY_X) //loads new screen
		{
			gfxInitDefault();
			PrintConsole bWindow, tWindow;
			consoleInit(GFX_BOTTOM, &bWindow);
			consoleInit(GFX_TOP, &tWindow);
			consoleSetWindow(&tWindow, 1, 1, 40, 30);
			consoleSetWindow(&bWindow, 1, 1, 40, 30);
			
			consoleSelect(&tWindow);
			printf("Welcome to alt menu *experimental*\n");
			printf("You can't exit from here yet. Press HOME to...")
				while(aptMainLoop())
				{
				hidScanInput();
				u32 kDown = hidKeysDown();
				
					if(kDown & KEY_START)
					{
					gfxExit();
					break;
					}
				gfxFlushBuffers();
				gfxSwapBuffers();
				gspWaitForVBlank();
				}
		}
		
		// Flush and swap framebuffers, there are two. swap at each frame
		//framebuffers store egb info abt. each pixel
		gfxFlushBuffers();
		gfxSwapBuffers();

		
	

		//Wait for VBlank
		gspWaitForVBlank();
	}


	gfxExit();
	
	return 0;
}
