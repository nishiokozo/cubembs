//#define _GNU_SOURCE // M_PI が定義される
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#include <signal.h>

#define M_PI	3.14159265358979323846

#define	VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>
#include <vulkan/vk_sdk_platform.h>


#include <math.h>

#include "key.h"
#include "vk.h"
#include "win.h"
#include "vect.h"
#include "enemy.h"

vect44 g_view;

	VkInf* g_pVk;
const char *tex_files[] = {"lunarg.ppm"};

//-----------------------------------------------------------------------------
int main(int argc, char *argv[])
//-----------------------------------------------------------------------------
{
	g_view.identity();
	g_view.translate(0,0,-5);

	//---
	WinInf* pWin = new WinInf( "msb", 128, 128 );
	g_pVk = new VkInf( 
		  pWin->hInstance
		, pWin->hWin
		, pWin->win_width
		, pWin->win_height
		, 200//unit_MAX
		, 1//DEMO_TEXTURE_COUNT
	);

	enemy_create();

	//-----------------------------------------------------
	// メインループ
	//-----------------------------------------------------
	MSG msg;   // message
	msg.wParam = 0;

	key_init(argc,argv);

	int lim1 = 0;
	int lim2 = 0;


	while (true) 
	{
		PeekMessage(&msg, NULL, 0, 0, PM_REMOVE);
		if (msg.message == WM_QUIT) // check for a flgQuit message
		{
			break;
		} else 
		{
			/* Translate and dispatch to event queue*/
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			RedrawWindow(pWin->hWin, NULL, NULL, RDW_INTERNALPAINT);
		}
		if (msg.message != WM_PAINT) continue;

		
		if ( key.hi._6 )
		{
			lim2 = 30;
		}
		if ( lim2 )
		{
			enemy_remove();
			enemy_create();
		
			lim2--;
			printf("%d ",lim2 );
		}
		
		//-----------------------------------------------------
		// 描画
		//-----------------------------------------------------
		if ( g_pVk ) 
		{
			vk2_updateBegin( g_pVk->vk, pWin->win_width, pWin->win_height );

			enemy_update();
			
			vk2_updateEnd( g_pVk->vk );

		}

		key_update();

	}

	//-----------------------------------------------------
	// 終了
	//-----------------------------------------------------
	if ( g_pVk ) 
	{
		enemy_remove();
		delete g_pVk;g_pVk=0;
	}


	if ( pWin ) delete pWin;

	return (int)msg.wParam;
}
//#endif
