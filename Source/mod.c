#include <3ds.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
	gfxInitDefault();

	PrintConsole bWindow, rightbWindow, leftbWindow, leftWindow, rightWindow;
	consoleInit(GFX_TOP, &leftWindow);
	consoleInit(GFX_TOP, &rightWindow);
	consoleInit(GFX_TOP, &rightbWindow);
	consoleInit(GFX_TOP, &leftbWindow);
	consoleInit(GFX_BOTTOM, &bWindow);

	//x, y, width and height are in terms of cells where each cell is composed by 8x8 pixels.
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
	printf("What you can do here: \n\n");
	printf("press X to boot an applet\n\n");
	printf("press A to add more text\n\n");
	printf("press B to attempt to boot another title (UNDER CONSTRUCTION)\n\n");	

	consoleSelect(&rightWindow);
	printf("3DSAPP v0.05");

	// loops once a frame
	while (aptMainLoop())
	{
		hidScanInput();

		//which buttons pressed
		u32 kDown = hidKeysDown();
		
		if (kDown & KEY_A)
		{
			consoleSelect(&leftbWindow);
			printf("A input works...\n");
		}
		
		if (kDown & KEY_B)
		{
			gfxInitDefault();
			u8 param[0x300];
			u8 hmac[0x20];

			
			PrintConsole bWindow, tWindow;
			consoleInit(GFX_BOTTOM, &bWindow);
			consoleInit(GFX_TOP, &tWindow);
			consoleSetWindow(&tWindow, 1, 1, 40, 30);
			consoleSetWindow(&bWindow, 1, 1, 40, 30);
		
			gfxFlushBuffers();
			gfxSwapBuffers();
			gspWaitForVBlank();
		
			consoleSelect(&tWindow);
			printf("Welcome to title launcher (v0.05)\n");
			printf("Below, there are options for what app to launch\n");
			printf("More will be added later");
		
			consoleSelect(&bWindow);
			printf("A - settings\n");
			printf("B - download play\n");
			printf("X - eShop\n");
			printf("Y - Home Menu\n\n\n");
		
			while(aptMainLoop)
			{	
				hidScanInput();
				u32 kDown = hidKeysDown();
				
				gfxFlushBuffers();
				gfxSwapBuffers();
				gspWaitForVBlank();
				if(kDown & KEY_A || kDown & KEY_B || kDown & KEY_X || kDown & KEY_Y)
				{
				consoleSelect(&bWindow);
				printf("opening...");
				}
				if (kDown & KEY_A)
					{
				memset(param, 0, sizeof(param));
				memset(hmac, 0, sizeof(hmac));
				APT_PrepareToDoApplicationJump(0, 0x0004001000021000, 0);
				APT_DoApplicationJump(param, sizeof(param), hmac);
					}
				else if (kDown & KEY_B)
					{
				memset(param, 0, sizeof(param));
				memset(hmac, 0, sizeof(hmac));
				APT_PrepareToDoApplicationJump(0, 0x0004001000021100, 0);
				APT_DoApplicationJump(param, sizeof(param), hmac);
					}
				else if (kDown & KEY_X)
					{
				memset(param, 0, sizeof(param));
				memset(hmac, 0, sizeof(hmac));
				APT_PrepareToDoApplicationJump(0, 0x0004001000021900, 0);
				APT_DoApplicationJump(param, sizeof(param), hmac);
					}		
				else if (kDown & KEY_Y)
					{
				memset(param, 0, sizeof(param));
				memset(hmac, 0, sizeof(hmac));
				APT_PrepareToDoApplicationJump(0, 0x0004003000008F02, 0);
				APT_DoApplicationJump(param, sizeof(param), hmac);
					}
				else continue;
			}
		}

		if (kDown & KEY_START)
		{
		consoleSelect(&rightbWindow);
		printf("Exiting...");		
		break;
		}
		
		if (kDown & KEY_X) //loads new screen
		{
			bool debug = true;
			void _aptDebug(int a, int b)
			{
				if (debug) 
				{
					printf("_aptDebug(%d,%x)\n", a, b);
				}
			}
			
			gfxInitDefault();
			
			PrintConsole bWindow, tWindow;
			consoleInit(GFX_BOTTOM, &bWindow);
			consoleInit(GFX_TOP, &tWindow);
			consoleSetWindow(&tWindow, 1, 1, 40, 30);
			consoleSetWindow(&bWindow, 1, 1, 40, 30);
			
			u32 aptbuf[0x400/4];
			
			gfxFlushBuffers();
			gfxSwapBuffers();
			gspWaitForVBlank();
			
			consoleSelect(&tWindow);
			printf("Welcome to alt app launcher (v0.05)\n\n");
			printf("A will launch error\n\n");
			printf("B will launch memolib\n\n");
			printf("X will launch keyboard\n\n");
			printf("START will not exit, press HOME to do so...");
				while(aptMainLoop())
				{
				hidScanInput();
				gfxFlushBuffers();
				gfxSwapBuffers();
				gspWaitForVBlank();
				u32 kDown = hidKeysDown();
				
					if(kDown)
					{
					NS_APPID appId;
						if(kDown & KEY_A || KEY_B || KEY_X)
							{
							consoleSelect(&bWindow);
							printf("launching app...\n");
							}
						if(kDown & KEY_A)
							{
							appId = APPID_ERROR;
							//break;
							}
						else if(kDown & KEY_B)
							{
							appId = APPID_MEMOLIB;
							//break;
							}
						else if(kDown & KEY_X)
							{
							appId = APPID_SOFTWARE_KEYBOARD;
							//break;
							}
						else continue;
						
						memset(aptbuf, 0, sizeof(aptbuf));
						if (!aptLaunchLibraryApplet(appId, aptbuf, sizeof(aptbuf), 0))
							{
							break;
							}
					}

				}
				
		}
		
		// Flush and swap framebuffers, there are two. swap at each frame
		//framebuffers store egb info abt. each pixel
		gfxFlushBuffers();
		gfxSwapBuffers();
		gspWaitForVBlank();
	}
gfxExit();
return 0;
}

/*future app launcher plans:
each app has a integer, keyboard opens to ask which one, integer is the array placement of the id that will then boot*/
