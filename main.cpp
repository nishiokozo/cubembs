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

const static int WIN_NAME_LEN = 80;
struct WindowInf
{

	HINSTANCE hInstance;
	POINT win_minsize;				// minimum window size
	HWND hWin;				  // hWnd - window handle
	char win_name[WIN_NAME_LEN];  // Name to put on the window/icon

	int	win_width;
	int	win_height;
} ;


WindowInf win;


//-----------------------------------------------------------------------------
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) 
//-----------------------------------------------------------------------------
{
	static int validation_error = 0;

	switch (uMsg) 
	{
	case WM_CLOSE:
		PostQuitMessage(validation_error);
		break;
	case WM_PAINT:
		break;
	case WM_GETMINMAXINFO:	 // set window's minimum size
		return 0;
	case WM_SIZE:
		break;
	default:
		break;
	}
	return (DefWindowProc(hWnd, uMsg, wParam, lParam));
}


#include <shellapi.h>

//-----------------------------------------------------------------------------
void	win_init( WindowInf& win, const char* name, int width, int height  )
//-----------------------------------------------------------------------------
{
	win.win_width = width;
	win.win_height = height;

	win.hInstance		= GetModuleHandle( NULL );

	{
		strncpy(win.win_name, "cube", WIN_NAME_LEN);

		WNDCLASSEX win_class;

		// Initialize the window class structure:
		win_class.cbSize = sizeof(WNDCLASSEX);
		win_class.style = CS_HREDRAW | CS_VREDRAW;
		win_class.lpfnWndProc = WndProc;
		win_class.cbClsExtra = 0;
		win_class.cbWndExtra = 0;
		win_class.hInstance = win.hInstance;
		win_class.hIcon = LoadIcon(NULL, IDI_APPLICATION);
		win_class.hCursor = LoadCursor(NULL, IDC_ARROW);
		win_class.hbrBackground = (HBRUSH)( COLOR_WINDOW + 1 );
		win_class.lpszMenuName = NULL;
		win_class.lpszClassName = win.win_name;
		win_class.hIconSm = LoadIcon(NULL, IDI_WINLOGO);
		// Register window class:
		if (!RegisterClassEx(&win_class)) 
		{
			// It didn't work, so try to give a useful error:
			printf("Unexpected error trying to start the application!\n");
			fflush(stdout);
			exit(1);
		}
		// Create window with the registered class:
		RECT wr = {0, 0, win.win_width, win.win_height};
		AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);

		win.hWin = CreateWindowEx(
			0,
			win.win_name,		   			// class name
			win.win_name,		   			// app name
			WS_OVERLAPPEDWINDOW | 		// window style
			WS_VISIBLE | WS_SYSMENU,
			100, 100,		   			// x/y coords
			wr.right - wr.left, 		// width
			wr.bottom - wr.top, 		// height
			NULL,			   			// handle to parent
			NULL,			   			// handle to menu
			win.hInstance,
			NULL					  	// no extra parameters
		);
	
		if (!win.hWin) 
		{
			// It didn't work, so try to give a useful error:
			printf("Cannot create a window in which to draw!\n");
			fflush(stdout);
			exit(1);
		}
		// Window client area size must be at least 1 pixel high, to prevent crash.
		win.win_minsize.x = GetSystemMetrics(SM_CXMINTRACK);
		win.win_minsize.y = GetSystemMetrics(SM_CYMINTRACK)+1;
	}
}

VkInf* pVk = 0;
//-----------------------------------------------------------------------------
int main(int argc, char *argv[])
//-----------------------------------------------------------------------------
{
	win_init( win , "msb", 256, 256 );

	//-----------------------------------------------------
	// メインループ
	//-----------------------------------------------------
	MSG msg;   // message
	msg.wParam = 0;
	bool done; // flag saying when app is complete
	done = false; // initialize loop condition variable

	key_init(argc,argv);

	while (!done) 
	{
		PeekMessage(&msg, NULL, 0, 0, PM_REMOVE);
		if (msg.message == WM_QUIT) // check for a flgQuit message
		{
			done = true; // if found, flgQuit app
		} else 
		{
			/* Translate and dispatch to event queue*/
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		
		if ( key.hi._1 )
		{
			pVk = new VkInf( win.hInstance, win.hWin, win.win_width, win.win_height );
		}
		
		if ( key.hi._2 )
		{
			delete pVk;
			pVk = 0;
		}
		
		//-----------------------------------------------------
		// 描画
		//-----------------------------------------------------
		if ( pVk ) pVk->v_draw();

		key_update();

		RedrawWindow(win.hWin, NULL, NULL, RDW_INTERNALPAINT);
	}

	//-----------------------------------------------------
	// 終了
	//-----------------------------------------------------

	return (int)msg.wParam;
}
//#endif
