const static int WIN_NAME_LEN = 80;
class WinInf
{
public:
	HINSTANCE hInstance;
//	POINT win_minsize;				// minimum window size
	HWND hWin;				  // hWnd - window handle
	char win_name[WIN_NAME_LEN];  // Name to put on the window/icon

	int	win_x;
	int	win_y;
	int	win_width;
	int	win_height;

	WinInf( const char* name, int width, int height );

};


