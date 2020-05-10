#include <3ds.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <citro2d.h>

C2D_TextBuf g_staticBuf;
C2D_Text g_staticText[2];
C2D_Font font[2];

static void sceneInit(void)
{
	g_staticBuf  = C2D_TextBufNew(1024);
	font[0] = C2D_FontLoadSystem(CFG_REGION_USA);
	font[1] = C2D_FontLoad("romfs:/liberationitalic.bcfnt");

	// parse the strings
	C2D_TextFontParse(&g_staticText[0], font[0], g_staticBuf, "normaltext");
	C2D_TextFontParse(&g_staticText[1], font[1], g_staticBuf, "libtext");
	C2D_TextOptimize(&g_staticText[0]);
	C2D_TextOptimize(&g_staticText[1]);
}

float size = 0.5f;
static void sceneRender(float size)
{
	// draw static strings
	float text2PosX = 384.0f - g_staticText[0].width*0.75f; // right-justify
	C2D_DrawText(&g_staticText[0], 0, 8.0f, 8.0f, 0.5f, size, size);
	C2D_DrawText(&g_staticText[1], C2D_AtBaseline, 108.0f, 36.0f, 0.5f, size, size);
}

static void sceneExit(void)
{
	// delete buffers
	C2D_TextBufDelete(g_staticBuf);
	C2D_FontFree(font[0]);
	C2D_FontFree(font[1]);
}
int main(int argc, char **argv)
{
	
	gfxInitDefault();
	romfsInit();
	cfguInit(); // load font load sym
	// libraries
	C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
	C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
	C2D_Prepare();
		
	C3D_RenderTarget* top = C2D_CreateScreenTarget(GFX_TOP, GFX_BOTTOM);
	
	sceneInit();
	sceneRender(size);

	//PrintConsole bWindow, rightbWindow, leftbWindow, leftWindow, rightWindow;


	/*consoleInit(GFX_TOP, &leftWindow);
	consoleInit(GFX_TOP, &rightWindow);
	consoleInit(GFX_TOP, &rightbWindow);
	consoleInit(GFX_TOP, &leftbWindow);
	consoleInit(GFX_BOTTOM, &bWindow);

//x coord, y coord, length in x-dir, length in y-dir
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
	*/


	while (aptMainLoop())
	{

		hidScanInput();
		u32 kDown = hidKeysDown();
		
		//if (kDown & KEY_A)
		//{
		//consoleSelect(&leftbWindow);
		//printf("A input works...");
		//}

		if (kDown & KEY_START)
		{
		//consoleSelect(&rightbWindow);
		//printf("Exiting...");
		sceneExit();
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
