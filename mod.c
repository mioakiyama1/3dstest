#include <3ds.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <citro3d.h>
#include "vshader_shbin.h"

#define CLEAR_COLOR 0x68B0D8FF


#define DISPLAY_TRANSFER_FLAGS \
	(GX_TRANSFER_FLIP_VERT(0) | GX_TRANSFER_OUT_TILED(0) | GX_TRANSFER_RAW_COPY(0) | \
	GX_TRANSFER_IN_FORMAT(GX_TRANSFER_FMT_RGBA8) | GX_TRANSFER_OUT_FORMAT(GX_TRANSFER_FMT_RGB8) | \
	GX_TRANSFER_SCALING(GX_TRANSFER_SCALE_NO))

typedef struct { float x, y, z; } vertex;

static const vertex vertex_list[] = 
{
	{    0.0f, 100.0f, 0.5f },
	{ -80.0f,  25.0f, 0.5f },
	{ +80.0f,  25.0f, 0.5f },
};

#define vertex_list_count (sizeof(vertex_list)/sizeof(vertex_list[0]))

static DVLB_s* vshader_dvlb;
static shaderProgram_s program;
static int uLoc_projection;
static C3D_Mtx projectionTop, projectionBot;
static void* vbo_data;

static void sceneInit(void)
{
	// Load the vertex shader, create a shader program and bind it
	vshader_dvlb = DVLB_ParseFile((u32*)vshader_shbin, vshader_shbin_size);
	shaderProgramInit(&program);
	shaderProgramSetVsh(&program, &vshader_dvlb->DVLE[0]);
	C3D_BindProgram(&program);

	// Get the location of the uniforms
	uLoc_projection = shaderInstanceGetUniformLocation(program.vertexShader, "projection");

	// Configure attributes for use with the vertex shader
	C3D_AttrInfo* attrInfo = C3D_GetAttrInfo();
	AttrInfo_Init(attrInfo);
	AttrInfo_AddLoader(attrInfo, 0, GPU_FLOAT, 3); // v0=position
	AttrInfo_AddFixed(attrInfo, 1); // v1=color

	// Set the fixed attribute (color) to solid white
	C3D_FixedAttribSet(1, 1.0, 1.0, 1.0, 1.0);

	// Compute the projection matrix
	Mtx_OrthoTilt(&projectionTop, -200.0f, 200.0f, 0.0f, 240.0f, 0.0f, 1.0f, true);
	Mtx_OrthoTilt(&projectionBot, -160.0f, 160.0f, 0.0f, 240.0f, 0.0f, 1.0f, true);

	// Create the VBO (vertex buffer object)
	vbo_data = linearAlloc(sizeof(vertex_list));
	memcpy(vbo_data, vertex_list, sizeof(vertex_list));

	// Configure buffers
	C3D_BufInfo* bufInfo = C3D_GetBufInfo();
	BufInfo_Init(bufInfo);
	BufInfo_Add(bufInfo, vbo_data, sizeof(vertex), 1, 0x0);

	// Configure the first fragment shading substage to just pass through the vertex color
	// See https://www.opengl.org/sdk/docs/man2/xhtml/glTexEnv.xml for more insight
	C3D_TexEnv* env = C3D_GetTexEnv(0);
	C3D_TexEnvInit(env);
	C3D_TexEnvSrc(env, C3D_Both, GPU_PRIMARY_COLOR, 0, 0);
	C3D_TexEnvFunc(env, C3D_Both, GPU_REPLACE);
}

static void sceneRender(float a, bool top)
{
	float b = (cosf(C3D_Angle(a)) + 1.0f) / 2.0f;
	if (!top) b = 1.0f - b;

	C3D_FixedAttribSet(1, b, b, b, 1.0);
	// Update the uniforms
	C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, uLoc_projection, top ? &projectionTop : &projectionBot);

	// Draw the VBO
	C3D_DrawArrays(GPU_TRIANGLES, 0, vertex_list_count);
}

static void sceneExit(void)
{
	// Free the VBO
	linearFree(vbo_data);

	// Free the shader program
	shaderProgramFree(&program);
	DVLB_Free(vshader_dvlb);
}

int main(int argc, char **argv)
{
	gfxInitDefault();
	C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);

	// Initialize the render target
	C3D_RenderTarget* top = C3D_RenderTargetCreate(240, 400, GPU_RB_RGBA8, GPU_RB_DEPTH24_STENCIL8);
	C3D_RenderTargetSetOutput(top, GFX_TOP, GFX_LEFT, DISPLAY_TRANSFER_FLAGS);
	C3D_RenderTarget* bot = C3D_RenderTargetCreate(240, 320, GPU_RB_RGBA8, GPU_RB_DEPTH24_STENCIL8);
	C3D_RenderTargetSetOutput(bot, GFX_BOTTOM, GFX_LEFT, DISPLAY_TRANSFER_FLAGS);

	//PrintConsole bWindow, rightbWindow, leftbWindow, leftWindow, rightWindow;
	//consoleInit(GFX_TOP, &leftWindow);
	//consoleInit(GFX_TOP, &rightWindow);
	//consoleInit(GFX_TOP, &rightbWindow);
	//consoleInit(GFX_TOP, &leftbWindow);
	//consoleInit(GFX_BOTTOM, &bWindow);

	//x, y, width and height are in terms of cells where each cell is composed by 8x8 pixels.
	//consoleSetWindow(&leftWindow, 1, 1, 23, 14);
	//consoleSetWindow(&rightWindow, 26, 1, 23, 14);
	//consoleSetWindow(&leftbWindow, 1, 17, 23, 14);
	//consoleSetWindow(&rightbWindow, 26, 17, 23, 14);
	//consoleSetWindow(&bWindow, 1, 1, 40, 30);

	//consoleSelect(&leftWindow);
	//printf("Hello\n");
	//printf("this text is \x1b[31mred\x1b[0m\n");
	//x1b is ANSI escape, ["xx"m where xx is color identifier
	
	//consoleSelect(&bWindow);
	//printf("What you can do here: \n\n");
	//printf("press X to boot an applet\n\n");
	//printf("press B to attempt to boot another title \n(UNDER CONSTRUCTION)\n\n");	
	//printf("press A to add more text\n\n");
	//printf("press Y for credits\n\n");
	
	//consoleSelect(&rightWindow);
	//printf("3DSAPP v0.05");

	// loops once a frame
	float count = 0.0f;
	while (aptMainLoop())
	{
		sceneInit();
		hidScanInput();

		//which buttons pressed
		u32 kDown = hidKeysDown();
		
		// Render the scene
		C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
		C3D_RenderTargetClear(top, C3D_CLEAR_ALL, CLEAR_COLOR, 0);
		C3D_FrameDrawOn(top);
		sceneRender(count, true);
		C3D_RenderTargetClear(bot, C3D_CLEAR_ALL, CLEAR_COLOR, 0);
		C3D_FrameDrawOn(bot);
		sceneRender(count, false);
		C3D_FrameEnd(0);
		count += 1/128.0f;
		
		
		//if (kDown & KEY_A)
		//{
		//	consoleSelect(&leftbWindow);
		//	printf("A input works...\n");
		//}
		
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
			printf("Welcome to title launcher (v0.05)\n\n");
			printf("Below, there are options for what app to launch\n\n");
			printf("More will be added later\n\n");
		
			consoleSelect(&bWindow);
			printf("A - settings\n\n");
			printf("B - download play\n\n");
			printf("X - eShop\n\n");
			printf("Y - Home Menu\n\n");
			printf("START can't exit yet\n\n");		
			while(aptMainLoop)
			{	
				hidScanInput();
				u32 kDown = hidKeysDown();
				
				gfxFlushBuffers();
				gfxSwapBuffers();
				gspWaitForVBlank();
				if (kDown & KEY_A)
					{
					consoleSelect(&bWindow);
					printf("opening...");
				
					memset(param, 0, sizeof(param));
					memset(hmac, 0, sizeof(hmac));
					APT_PrepareToDoApplicationJump(0, 0x0004001000021000, 0);
					APT_DoApplicationJump(param, sizeof(param), hmac);
						}
				else if (kDown & KEY_B)
					{
					consoleSelect(&bWindow);
					printf("opening...");
				
					memset(param, 0, sizeof(param));
					memset(hmac, 0, sizeof(hmac));
					APT_PrepareToDoApplicationJump(0, 0x0004001000021100, 0);
					APT_DoApplicationJump(param, sizeof(param), hmac);
					}
				else if (kDown & KEY_X)
					{
					consoleSelect(&bWindow);
					printf("opening...");
				
					memset(param, 0, sizeof(param));
					memset(hmac, 0, sizeof(hmac));
					APT_PrepareToDoApplicationJump(0, 0x0004001000021900, 0);
					APT_DoApplicationJump(param, sizeof(param), hmac);
					}		
				else if (kDown & KEY_Y)
					{
					consoleSelect(&bWindow);
					printf("opening...");
				
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
		//consoleSelect(&rightbWindow);
		//printf("Exiting...");	
		gfxExit();
		return 0;
		//break;
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
							}
						else if(kDown & KEY_B)
							{
							appId = APPID_MEMOLIB;
							}
						else if(kDown & KEY_X)
							{
							appId = APPID_SOFTWARE_KEYBOARD;
							}
						else continue;
							//define elseif for exiting	
						memset(aptbuf, 0, sizeof(aptbuf));
						if (!aptLaunchLibraryApplet(appId, aptbuf, sizeof(aptbuf), 0))
							{
							break;
							}
					}

				}
				
		}
		
		if (kDown & KEY_Y)
			{
			gfxInitDefault();
			
			PrintConsole tWindow, bWindow;
			consoleInit(GFX_TOP, &tWindow);
			consoleInit(GFX_BOTTOM, &bWindow);
			consoleSetWindow(&tWindow, 1, 1, 40, 30);
			consoleSetWindow(&bWindow, 1, 1, 40, 30);	
		
			consoleSelect(&tWindow);
			printf("Welcome!\n\n");
			printf("Credits:\n\n\n\n\n");
			printf("Made with devkitpro, msys2, and programmer's notepad");
				
				while(aptMainLoop)
					{
					hidScanInput();
					gfxFlushBuffers();
					gfxSwapBuffers();
					gspWaitForVBlank();
					u32 kDown = hidKeysDown();
					//double n = 0.00;
					//char a[] = {"Initial version", "adds color", "Added SMDH support", "adds Remote launching (but not functional)", "adds a new screen", "adds applet support", "revised remote launcher"};

					//for (n = 0.00; n < 0.06; n += 0.01)
						//{
						//int b = n * 100;
						//consoleSelect(&bWindow);
						//printf("Version number ");
						//printf(n);
						//printf(": ");
						//printf(a[b]);
						//n++;
						//}
					
					}
			}
		// Flush and swap framebuffers, there are two. swap at each frame
		//framebuffers store egb info abt. each pixel
		gfxFlushBuffers();
		gfxSwapBuffers();
		gspWaitForVBlank();
	}
gfxExit();
sceneExit();
C3D_Fini();
return 0;
}

/*future app launcher plans:
each app has a integer, keyboard opens to ask which one, integer is the array placement of the id that will then boot*/
