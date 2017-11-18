//#define _GNU_SOURCE // M_PI が定義される
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#include <signal.h>

//#ifdef _WIN32
//#pragma comment(linker, "/subsystem:windows")	//	コンソール表示されなくする
//#endif  // _WIN32

#define M_PI	3.14159265358979323846

#define	VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>
#include <vulkan/vk_sdk_platform.h>


//#ifndef LINMATH_H
//#define LINMATH_H

#include <math.h>

#include "key.h"
#include "vk.h"
#include "win.h"




//-----------------------------------------------------------------------------
int main(int argc, char *argv[])
//-----------------------------------------------------------------------------
{
	VkInf* pVk = 0;
	WinInf* pWin = new WinInf();

	pWin->create( "msb", 256, 256 );

	//-----------------------------------------------------
	// メインループ
	//-----------------------------------------------------
	MSG msg;   // message
	msg.wParam = 0;

	key_init(argc,argv);

	int lim = 0;

	while (true) 
	{
		PeekMessage(&msg, NULL, 0, 0, PM_REMOVE);
		if (msg.message == WM_QUIT) // check for a flgQuit message
		{
			printf("quit1 %d\n",msg.message);
			break;
		} else 
		{
			/* Translate and dispatch to event queue*/
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		if (msg.message != WM_PAINT) continue;

		if ( key.hi._1 )
		{
			pVk = new VkInf( pWin->hInstance, pWin->hWin, pWin->win_width, pWin->win_height );
		}
		
		if ( key.hi._2 )
		{
			delete pVk;
			pVk = 0;
		}

		if ( key.hi._3 )
		{
			lim = 10;
		}

		if ( lim )
		{
			delete pVk;
			pVk = new VkInf( pWin->hInstance, pWin->hWin, pWin->win_width, pWin->win_height );
			printf("init-v_release %d\n",lim );
			lim--;
		}
		
		//-----------------------------------------------------
		// 描画
		//-----------------------------------------------------
		if ( pVk ) pVk->v_draw();

		key_update();

		RedrawWindow(pWin->hWin, NULL, NULL, RDW_INTERNALPAINT);
	}

	//-----------------------------------------------------
	// 終了
	//-----------------------------------------------------
	if ( pVk ) delete pVk;
	if ( pWin ) delete pWin;

	return (int)msg.wParam;
}
//#endif
