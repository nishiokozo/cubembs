#define APIENTRY
#include <stdio.h>
#include <math.h>

#include <windows.h>
#include <stdio.h>
#include <string.h>
#include "vector.h"
#include "mouse.h"
//#include "mouse.h"
//#include "font.h"

MOUSE_INF	mouse;

//-----------------------------------------------------------------------------
int mouse_init()
//-----------------------------------------------------------------------------
{
	memset( &mouse, 0, sizeof(mouse) ); 

	POINT pos;

	GetCursorPos( &pos );

	mouse.sx = (float)pos.x;
	mouse.sy = (float)pos.y;

	mouse.mx = 0;
	mouse.my = 0;

	return true;
}
//-----------------------------------------------------------------------------
int mouse_update()
//-----------------------------------------------------------------------------
{
	POINT pos;

	GetCursorPos( &pos );

	int	l = GetAsyncKeyState(VK_LBUTTON);
	int r = GetAsyncKeyState(VK_RBUTTON);
// 	int	n = GetSystemMetrics( SM_CMOUSEBUTTONS );
// 	int	w = GetSystemMetrics( SM_XVIRTUALSCREEN );

	int	on_l = (l&0x8000)!=0;
	int	on_r = (r&0x8000)!=0;

	mouse.hi.l = !mouse.on.l &&  on_l;
	mouse.hi.r = !mouse.on.r &&  on_r;

	mouse.lo.l =  mouse.on.l && !on_l;
	mouse.lo.r =  mouse.on.r && !on_r;

	mouse.mx = (float)pos.x - mouse.sx;
	mouse.my = (float)pos.y - mouse.sy;
	mouse.vm.x =   (float)pos.x - mouse.sx;
	mouse.vm.y = -((float)pos.y - mouse.sy);
	mouse.on.l = on_l;
	mouse.on.r = on_r;

	mouse.sx = (float)pos.x;
	mouse.sy = (float)pos.y;

	mouse.dx = (mouse.sx - mouse.cx) * mouse.screen_w;
	mouse.dy = (mouse.sy - mouse.cy) * mouse.screen_h;

//	mouse.vw.x =  (((float)pos.x - framework_getX())/ framework_getW())*2.0f-1.0f;
//	mouse.vw.y =  (((float)pos.y - framework_getY())/ framework_getH())*2.0f-1.0f;
//	mouse.vw.x =  (((float)pos.x - framework_getX())/ framework_getW())*2.0f-1.0f;
//	mouse.vw.y =  (((float)pos.y - framework_getY())/ framework_getH())*2.0f-1.0f;


//	mouse.vf.x = 0; 
//	mouse.vf.y = 0; 
//	if ( mouse.vw.x >  0.1f ) mouse.vf.x = (mouse.vw.x-0.1f)/0.9f;
//	if ( mouse.vw.x < -0.1f ) mouse.vf.x = (mouse.vw.x+0.1f)/0.9f;
//	if ( mouse.vw.y >  0.1f ) mouse.vf.y = (mouse.vw.y-0.1f)/0.9f;
//	if ( mouse.vw.y < -0.1f ) mouse.vf.y = (mouse.vw.y+0.1f)/0.9f;

//	if ( mouse.vf.x >  1.0f ) mouse.vf.x =  1.0f;
//	if ( mouse.vf.x < -1.0f ) mouse.vf.x = -1.0f;
//	if ( mouse.vf.y >  1.0f ) mouse.vf.y =  1.0f;
//	if ( mouse.vf.y < -1.0f ) mouse.vf.y = -1.0f;
//	

//	if ( mouse.vw.x < -1 ) mouse.vw.x = -1;
//	if ( mouse.vw.y < -1 ) mouse.vw.y = -1;
//	if ( mouse.vw.x >  1 ) mouse.vw.x =  1;
//	if ( mouse.vw.y >  1 ) mouse.vw.y =  1;

	mouse.vd.x =  (mouse.sx - mouse.cx) * mouse.screen_w;
	mouse.vd.y = -(mouse.sy - mouse.cy) * mouse.screen_h;
 
//	font_printf( "mouse %f %f %d %d %d %d \n", mouse.mx, mouse.my, mouse.on.l, mouse.on.r, n, w );

	return	false;
}


//-----------------------------------------------------------------------------
int	 mosue_resize(int w, int h)
//-----------------------------------------------------------------------------
{
	/* マウスポインタ位置のウィンドウ内の相対的位置への換算用 */
	mouse.screen_w = 1.0 / (float)w;
	mouse.screen_h = 1.0 / (float)h;


	return	true;
}
//-----------------------------------------------------------------------------
int mouse_mouse(int button, int state, int x, int y)
//-----------------------------------------------------------------------------
{
return true;
	mouse.button = button;
	mouse.state = state;
	if ( button != 0xffff )
	{
#if 0
		switch (button) {
		case GLUT_LEFT_BUTTON:
			switch (state) {
			case GLUT_DOWN:

				// ドラッグ開始点を記録
				mouse.cx = mouse.sx;
				mouse.cy = mouse.sy;

//						mouse.on.l = true;
//						mouse.flgDrag = true;

				break;
			case GLUT_UP:

//						mouse.on.l = false;
//						mouse.flgDrag = false;

				break;
			default:
				break;
			}
			break;
		default:
			break;
		}
#endif
	}
	else
	{
		
		mouse.sx = x;
		mouse.sy = y;

	}
		
	return	true;
}

void	Mouse::update()
{
	mouse_update();
}

Mouse::Mouse()
{
	mouse_init();
}
Mouse::~Mouse()
{
}
