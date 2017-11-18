#include <windows.h>
#include <stdio.h>
#include "win.h"

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
void	win_init( WinInf& win, const char* name, int width, int height  )
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
//		win.win_minsize.x = GetSystemMetrics(SM_CXMINTRACK);
//		win.win_minsize.y = GetSystemMetrics(SM_CYMINTRACK)+1;
	}
}

//-----------------------------------------------------------------------------
void WinInf::create(  const char* name, int width, int height )
//-----------------------------------------------------------------------------
{
	win_init( *this , "msb", 256, 256 );
}
