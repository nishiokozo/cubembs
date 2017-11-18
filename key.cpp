#include <windows.h>
#include <stdio.h>
#include "key.h"

static	BYTE g_state[256];

KEY_INF	key;

static	int	g_lim_on;

//-----------------------------------------------------------------------------
int key_init( int argc, char *argv[] )
//-----------------------------------------------------------------------------
{
	memset( &key, 0, sizeof(key)) ;
	return	true;
}
//-----------------------------------------------------------------------------
int	key_update()
//-----------------------------------------------------------------------------
{

//最上位ビットが 1 のときはキーが押されていることを、0 のときはキーが押されていないことを示します。最下位ビットが 1 のときはキーがトグル状態にあることを、0 のときはトグルが解除されていることを示します。たとえば、CapsLock キーが ON になっているときは、トグル状態になります。
	GetKeyboardState( g_state );


#if 0
int i;
for ( i = 0 ; i < 255 ; i++ )
{
	if ( g_state[i] & 0x80 )
	{
		font_printf("key %d\n", i );
	}
}
#endif

	int	flg_on_a             =  ( g_state[(int)'A']    &0x80 )	!= 0;
	int	flg_on_b             =  ( g_state[(int)'B']    &0x80 )	!= 0;
	int	flg_on_c             =  ( g_state[(int)'C']    &0x80 )	!= 0;
	int	flg_on_d             =  ( g_state[(int)'D']    &0x80 )	!= 0;
	int	flg_on_e             =  ( g_state[(int)'E']    &0x80 )	!= 0;
	int	flg_on_f             =  ( g_state[(int)'F']    &0x80 )	!= 0;
	int	flg_on_g             =  ( g_state[(int)'G']    &0x80 )	!= 0;
	int	flg_on_h             =  ( g_state[(int)'H']    &0x80 )	!= 0;
	int	flg_on_i             =  ( g_state[(int)'I']    &0x80 )	!= 0;
	int	flg_on_j             =  ( g_state[(int)'J']    &0x80 )	!= 0;
	int	flg_on_k             =  ( g_state[(int)'K']    &0x80 )	!= 0;
	int	flg_on_l             =  ( g_state[(int)'L']    &0x80 )	!= 0;
	int	flg_on_m             =  ( g_state[(int)'M']    &0x80 )	!= 0;
	int	flg_on_n             =  ( g_state[(int)'N']    &0x80 )	!= 0;
	int	flg_on_o             =  ( g_state[(int)'O']    &0x80 )	!= 0;
	int	flg_on_p             =  ( g_state[(int)'P']    &0x80 )	!= 0;
	int	flg_on_q             =  ( g_state[(int)'Q']    &0x80 )	!= 0;
	int	flg_on_r             =  ( g_state[(int)'R']    &0x80 )	!= 0;
	int	flg_on_s             =  ( g_state[(int)'S']    &0x80 )	!= 0;
	int	flg_on_t             =  ( g_state[(int)'T']    &0x80 )	!= 0;
	int	flg_on_u             =  ( g_state[(int)'U']    &0x80 )	!= 0;
	int	flg_on_v             =  ( g_state[(int)'V']    &0x80 )	!= 0;
	int	flg_on_w             =  ( g_state[(int)'W']    &0x80 )	!= 0;
	int	flg_on_x             =  ( g_state[(int)'X']    &0x80 )	!= 0;
	int	flg_on_y             =  ( g_state[(int)'Y']    &0x80 )	!= 0;
	int	flg_on_z             =  ( g_state[(int)'Z']    &0x80 )	!= 0;
	int	flg_on__1            =  ( g_state[(int)'1']    &0x80 )	!= 0;
	int	flg_on__2            =  ( g_state[(int)'2']    &0x80 )	!= 0;
	int	flg_on__3            =  ( g_state[(int)'3']    &0x80 )	!= 0;
	int	flg_on__4            =  ( g_state[(int)'4']    &0x80 )	!= 0;
	int	flg_on__5            =  ( g_state[(int)'5']    &0x80 )	!= 0;
	int	flg_on__6            =  ( g_state[(int)'6']    &0x80 )	!= 0;
	int	flg_on__7            =  ( g_state[(int)'7']    &0x80 )	!= 0;
	int	flg_on__8            =  ( g_state[(int)'8']    &0x80 )	!= 0;
	int	flg_on__9            =  ( g_state[(int)'9']    &0x80 )	!= 0;
	int	flg_on__0            =  ( g_state[(int)'0']    &0x80 )	!= 0;
	int	flg_on_tab           =  ( g_state[VK_TAB]      &0x80 )	!= 0;
	int	flg_on_esc           =  ( g_state[VK_ESCAPE]   &0x80 )	!= 0;
	int	flg_on_space         =  ( g_state[VK_SPACE]    &0x80 )	!= 0;
	int	flg_on_enter         =  ( g_state[VK_RETURN]   &0x80 )	!= 0;
	int	flg_on_shift_l       =  ( g_state[VK_LSHIFT]   &0x80 )	!= 0;
	int	flg_on_shift_r       =  ( g_state[VK_RSHIFT]   &0x80 )	!= 0;
	int	flg_on_shift         =  ( g_state[VK_SHIFT]    &0x80 )	!= 0;
	int	flg_on_ctrl_l        =  ( g_state[VK_LCONTROL] &0x80 )	!= 0;
	int	flg_on_ctrl_r        =  ( g_state[VK_LCONTROL] &0x80 )	!= 0;
	int	flg_on_ctrl          =  ( g_state[VK_CONTROL]  &0x80 )	!= 0;
	int	flg_on_alt_l         =  ( g_state[VK_LMENU]    &0x80 )	!= 0;
	int	flg_on_alt_r         =  ( g_state[VK_RMENU]    &0x80 )	!= 0;
	int	flg_on_alt           =  ( g_state[VK_MENU]     &0x80 )	!= 0;
	int	flg_on_pause         =  ( g_state[VK_PAUSE]    &0x80 )	!= 0;
	int	flg_on_capsLock      =  ( g_state[VK_CAPITAL]  &0x80 )	!= 0;
	int	flg_on_pageUp        =  ( g_state[VK_PRIOR]    &0x80 )	!= 0;
	int	flg_on_pageDown      =  ( g_state[VK_NEXT]     &0x80 )	!= 0;
	int	flg_on_end           =  ( g_state[VK_END]      &0x80 )	!= 0;
	int	flg_on_home          =  ( g_state[VK_HOME]     &0x80 )	!= 0;
	int	flg_on_left          =  ( g_state[VK_LEFT]     &0x80 )	!= 0;
	int	flg_on_up            =  ( g_state[VK_UP]       &0x80 )	!= 0;
	int	flg_on_right         =  ( g_state[VK_RIGHT]    &0x80 )	!= 0;
	int	flg_on_down          =  ( g_state[VK_DOWN]     &0x80 )	!= 0;
	int	flg_on_insert        =  ( g_state[VK_INSERT]   &0x80 )	!= 0;
	int	flg_on_del           =  ( g_state[VK_DELETE]   &0x80 )	!= 0;
	int	flg_on_scrollLock    =  ( g_state[VK_SCROLL]   &0x80 )	!= 0;
	int	flg_on_f1            =  ( g_state[VK_F1]       &0x80 )	!= 0;
	int	flg_on_f2            =  ( g_state[VK_F2]       &0x80 )	!= 0;
	int	flg_on_f3            =  ( g_state[VK_F3]       &0x80 )	!= 0;
	int	flg_on_f4            =  ( g_state[VK_F4]       &0x80 )	!= 0;
	int	flg_on_f5            =  ( g_state[VK_F5]       &0x80 )	!= 0;
	int	flg_on_f6            =  ( g_state[VK_F6]       &0x80 )	!= 0;
	int	flg_on_f7            =  ( g_state[VK_F7]       &0x80 )	!= 0;
	int	flg_on_f8            =  ( g_state[VK_F8]       &0x80 )	!= 0;
	int	flg_on_f9            =  ( g_state[VK_F9]       &0x80 )	!= 0;
	int	flg_on_f10           =  ( g_state[VK_F10]      &0x80 )	!= 0;
	int	flg_on_f11           =  ( g_state[VK_F11]      &0x80 )	!= 0;
	int	flg_on_f12           =  ( g_state[VK_F12]      &0x80 )	!= 0;
	int	flg_on_f13           =  ( g_state[VK_F13]      &0x80 )	!= 0;
	int	flg_on_f14           =  ( g_state[VK_F14]      &0x80 )	!= 0;
	int	flg_on_f15           =  ( g_state[VK_F15]      &0x80 )	!= 0;
	int	flg_on_f16           =  ( g_state[VK_F16]      &0x80 )	!= 0;
	int	flg_on_numLock       =  ( g_state[VK_NUMLOCK]  &0x80 )	!= 0;
	int	flg_on_num_0         =  ( g_state[VK_NUMPAD0]  &0x80 )	!= 0;
	int	flg_on_num_1         =  ( g_state[VK_NUMPAD1]  &0x80 )	!= 0;
	int	flg_on_num_2         =  ( g_state[VK_NUMPAD2]  &0x80 )	!= 0;
	int	flg_on_num_3         =  ( g_state[VK_NUMPAD3]  &0x80 )	!= 0;
	int	flg_on_num_4         =  ( g_state[VK_NUMPAD4]  &0x80 )	!= 0;
	int	flg_on_num_5         =  ( g_state[VK_NUMPAD5]  &0x80 )	!= 0;
	int	flg_on_num_6         =  ( g_state[VK_NUMPAD6]  &0x80 )	!= 0;
	int	flg_on_num_7         =  ( g_state[VK_NUMPAD7]  &0x80 )	!= 0;
	int	flg_on_num_8         =  ( g_state[VK_NUMPAD8]  &0x80 )	!= 0;
	int	flg_on_num_9         =  ( g_state[VK_NUMPAD9]  &0x80 )	!= 0;
	int	flg_on_num_multiply  =  ( g_state[VK_MULTIPLY] &0x80 )	!= 0;
	int	flg_on_num_add       =  ( g_state[VK_ADD]      &0x80 )	!= 0;
	int	flg_on_num_enter     =  ( g_state[VK_SEPARATOR]&0x80 )	!= 0;
	int	flg_on_num_sub       =  ( g_state[VK_SUBTRACT] &0x80 )	!= 0;
	int	flg_on_num_dot       =  ( g_state[VK_DECIMAL]  &0x80 )	!= 0;
	int	flg_on_num_devide    =  ( g_state[VK_DIVIDE]   &0x80 )	!= 0;

//	key.hi.space = ( key.on.space == false )  && flg_on_space;
	key.hi.a = ( key.on.a == false ) && flg_on_a;
	key.hi.b = ( key.on.b == false ) && flg_on_b;
	key.hi.c = ( key.on.c == false ) && flg_on_c;
	key.hi.d = ( key.on.d == false ) && flg_on_d;
	key.hi.e = ( key.on.e == false ) && flg_on_e;
	key.hi.f = ( key.on.f == false ) && flg_on_f;
	key.hi.g = ( key.on.g == false ) && flg_on_g;
	key.hi.h = ( key.on.h == false ) && flg_on_h;
	key.hi.i = ( key.on.i == false ) && flg_on_i;
	key.hi.j = ( key.on.j == false ) && flg_on_j;
	key.hi.k = ( key.on.k == false ) && flg_on_k;
	key.hi.l = ( key.on.l == false ) && flg_on_l;
	key.hi.m = ( key.on.m == false ) && flg_on_m;
	key.hi.n = ( key.on.n == false ) && flg_on_n;
	key.hi.o = ( key.on.o == false ) && flg_on_o;
	key.hi.p = ( key.on.p == false ) && flg_on_p;
	key.hi.q = ( key.on.q == false ) && flg_on_q;
	key.hi.r = ( key.on.r == false ) && flg_on_r;
	key.hi.s = ( key.on.s == false ) && flg_on_s;
	key.hi.t = ( key.on.t == false ) && flg_on_t;
	key.hi.u = ( key.on.u == false ) && flg_on_u;
	key.hi.v = ( key.on.v == false ) && flg_on_v;
	key.hi.w = ( key.on.w == false ) && flg_on_w;
	key.hi.x = ( key.on.x == false ) && flg_on_x;
	key.hi.y = ( key.on.y == false ) && flg_on_y;
	key.hi.z = ( key.on.z == false ) && flg_on_z;
	key.hi._1 = ( key.on._1 == false ) && flg_on__1;
	key.hi._2 = ( key.on._2 == false ) && flg_on__2;
	key.hi._3 = ( key.on._3 == false ) && flg_on__3;
	key.hi._4 = ( key.on._4 == false ) && flg_on__4;
	key.hi._5 = ( key.on._5 == false ) && flg_on__5;
	key.hi._6 = ( key.on._6 == false ) && flg_on__6;
	key.hi._7 = ( key.on._7 == false ) && flg_on__7;
	key.hi._8 = ( key.on._8 == false ) && flg_on__8;
	key.hi._9 = ( key.on._9 == false ) && flg_on__9;
	key.hi._0 = ( key.on._0 == false ) && flg_on__0;
	key.hi.tab = ( key.on.tab == false ) && flg_on_tab;
	key.hi.esc = ( key.on.esc == false ) && flg_on_esc;
	key.hi.space = ( key.on.space == false ) && flg_on_space;
	key.hi.enter = ( key.on.enter == false ) && flg_on_enter;
	key.hi.shift_l = ( key.on.shift_l == false ) && flg_on_shift_l;
	key.hi.shift_r = ( key.on.shift_r == false ) && flg_on_shift_r;
	key.hi.shift = ( key.on.shift == false ) && flg_on_shift;
	key.hi.ctrl_l = ( key.on.ctrl_l == false ) && flg_on_ctrl_l;
	key.hi.ctrl_r = ( key.on.ctrl_r == false ) && flg_on_ctrl_r;
	key.hi.ctrl = ( key.on.ctrl == false ) && flg_on_ctrl;
	key.hi.alt_l = ( key.on.alt_l == false ) && flg_on_alt_l;
	key.hi.alt_r = ( key.on.alt_r == false ) && flg_on_alt_r;
	key.hi.alt = ( key.on.alt == false ) && flg_on_alt;
	key.hi.pause = ( key.on.pause == false ) && flg_on_pause;
	key.hi.capsLock = ( key.on.capsLock == false ) && flg_on_capsLock;
	key.hi.pageUp = ( key.on.pageUp == false ) && flg_on_pageUp;
	key.hi.pageDown = ( key.on.pageDown == false ) && flg_on_pageDown;
	key.hi.end = ( key.on.end == false ) && flg_on_end;
	key.hi.home = ( key.on.home == false ) && flg_on_home;
	key.hi.left = ( key.on.left == false ) && flg_on_left;
	key.hi.up = ( key.on.up == false ) && flg_on_up;
	key.hi.right = ( key.on.right == false ) && flg_on_right;
	key.hi.down = ( key.on.down == false ) && flg_on_down;
	key.hi.insert = ( key.on.insert == false ) && flg_on_insert;
	key.hi.del = ( key.on.del == false ) && flg_on_del;
	key.hi.scrollLock = ( key.on.scrollLock == false ) && flg_on_scrollLock;
	key.hi.f1 = ( key.on.f1 == false ) && flg_on_f1;
	key.hi.f2 = ( key.on.f2 == false ) && flg_on_f2;
	key.hi.f3 = ( key.on.f3 == false ) && flg_on_f3;
	key.hi.f4 = ( key.on.f4 == false ) && flg_on_f4;
	key.hi.f5 = ( key.on.f5 == false ) && flg_on_f5;
	key.hi.f6 = ( key.on.f6 == false ) && flg_on_f6;
	key.hi.f7 = ( key.on.f7 == false ) && flg_on_f7;
	key.hi.f8 = ( key.on.f8 == false ) && flg_on_f8;
	key.hi.f9 = ( key.on.f9 == false ) && flg_on_f9;
	key.hi.f10 = ( key.on.f10 == false ) && flg_on_f10;
	key.hi.f11 = ( key.on.f11 == false ) && flg_on_f11;
	key.hi.f12 = ( key.on.f12 == false ) && flg_on_f12;
	key.hi.f13 = ( key.on.f13 == false ) && flg_on_f13;
	key.hi.f14 = ( key.on.f14 == false ) && flg_on_f14;
	key.hi.f15 = ( key.on.f15 == false ) && flg_on_f15;
	key.hi.f16 = ( key.on.f16 == false ) && flg_on_f16;
	key.hi.numLock = ( key.on.numLock == false ) && flg_on_numLock;
	key.hi.num_0 = ( key.on.num_0 == false ) && flg_on_num_0;
	key.hi.num_1 = ( key.on.num_1 == false ) && flg_on_num_1;
	key.hi.num_2 = ( key.on.num_2 == false ) && flg_on_num_2;
	key.hi.num_3 = ( key.on.num_3 == false ) && flg_on_num_3;
	key.hi.num_4 = ( key.on.num_4 == false ) && flg_on_num_4;
	key.hi.num_5 = ( key.on.num_5 == false ) && flg_on_num_5;
	key.hi.num_6 = ( key.on.num_6 == false ) && flg_on_num_6;
	key.hi.num_7 = ( key.on.num_7 == false ) && flg_on_num_7;
	key.hi.num_8 = ( key.on.num_8 == false ) && flg_on_num_8;
	key.hi.num_9 = ( key.on.num_9 == false ) && flg_on_num_9;
	key.hi.num_multiply = ( key.on.num_multiply == false ) && flg_on_num_multiply;
	key.hi.num_add = ( key.on.num_add == false ) && flg_on_num_add;
	key.hi.num_enter = ( key.on.num_enter == false ) && flg_on_num_enter;
	key.hi.num_sub = ( key.on.num_sub == false ) && flg_on_num_sub;
	key.hi.num_dot = ( key.on.num_dot == false ) && flg_on_num_dot;
	key.hi.num_devide = ( key.on.num_devide == false ) && flg_on_num_devide;

	key.on.a = flg_on_a;
	key.on.b = flg_on_b;
	key.on.c = flg_on_c;
	key.on.d = flg_on_d;
	key.on.e = flg_on_e;
	key.on.f = flg_on_f;
	key.on.g = flg_on_g;
	key.on.h = flg_on_h;
	key.on.i = flg_on_i;
	key.on.j = flg_on_j;
	key.on.k = flg_on_k;
	key.on.l = flg_on_l;
	key.on.m = flg_on_m;
	key.on.n = flg_on_n;
	key.on.o = flg_on_o;
	key.on.p = flg_on_p;
	key.on.q = flg_on_q;
	key.on.r = flg_on_r;
	key.on.s = flg_on_s;
	key.on.t = flg_on_t;
	key.on.u = flg_on_u;
	key.on.v = flg_on_v;
	key.on.w = flg_on_w;
	key.on.x = flg_on_x;
	key.on.y = flg_on_y;
	key.on.z = flg_on_z;
	key.on._1 = flg_on__1;
	key.on._2 = flg_on__2;
	key.on._3 = flg_on__3;
	key.on._4 = flg_on__4;
	key.on._5 = flg_on__5;
	key.on._6 = flg_on__6;
	key.on._7 = flg_on__7;
	key.on._8 = flg_on__8;
	key.on._9 = flg_on__9;
	key.on._0 = flg_on__0;
	key.on.tab = flg_on_tab;
	key.on.esc = flg_on_esc;
	key.on.space = flg_on_space;
	key.on.enter = flg_on_enter;
	key.on.shift_l = flg_on_shift_l;
	key.on.shift_r = flg_on_shift_r;
	key.on.shift = flg_on_shift;
	key.on.ctrl_l = flg_on_ctrl_l;
	key.on.ctrl_r = flg_on_ctrl_r;
	key.on.ctrl = flg_on_ctrl;
	key.on.alt_l = flg_on_alt_l;
	key.on.alt_r = flg_on_alt_r;
	key.on.alt = flg_on_alt;
	key.on.pause = flg_on_pause;
	key.on.capsLock = flg_on_capsLock;
	key.on.pageUp = flg_on_pageUp;
	key.on.pageDown = flg_on_pageDown;
	key.on.end = flg_on_end;
	key.on.home = flg_on_home;
	key.on.left = flg_on_left;
	key.on.up = flg_on_up;
	key.on.right = flg_on_right;
	key.on.down = flg_on_down;
	key.on.insert = flg_on_insert;
	key.on.del = flg_on_del;
	key.on.scrollLock = flg_on_scrollLock;
	key.on.f1 = flg_on_f1;
	key.on.f2 = flg_on_f2;
	key.on.f3 = flg_on_f3;
	key.on.f4 = flg_on_f4;
	key.on.f5 = flg_on_f5;
	key.on.f6 = flg_on_f6;
	key.on.f7 = flg_on_f7;
	key.on.f8 = flg_on_f8;
	key.on.f9 = flg_on_f9;
	key.on.f10 = flg_on_f10;
	key.on.f11 = flg_on_f11;
	key.on.f12 = flg_on_f12;
	key.on.f13 = flg_on_f13;
	key.on.f14 = flg_on_f14;
	key.on.f15 = flg_on_f15;
	key.on.f16 = flg_on_f16;
	key.on.numLock = flg_on_numLock;
	key.on.num_0 = flg_on_num_0;
	key.on.num_1 = flg_on_num_1;
	key.on.num_2 = flg_on_num_2;
	key.on.num_3 = flg_on_num_3;
	key.on.num_4 = flg_on_num_4;
	key.on.num_5 = flg_on_num_5;
	key.on.num_6 = flg_on_num_6;
	key.on.num_7 = flg_on_num_7;
	key.on.num_8 = flg_on_num_8;
	key.on.num_9 = flg_on_num_9;
	key.on.num_multiply = flg_on_num_multiply;
	key.on.num_add = flg_on_num_add;
	key.on.num_enter = flg_on_num_enter;
	key.on.num_sub = flg_on_num_sub;
	key.on.num_dot = flg_on_num_dot;
	key.on.num_devide = flg_on_num_devide;

	{
		int	flg_rep = false;
		key.rep.space = false;

		if ( key.hi.space )
		{
			g_lim_on = 10;
			flg_rep = true;
		}
		if ( flg_on_space )
		{
			g_lim_on--;
			if ( g_lim_on <= 0 )
			{
				flg_rep = true;
				g_lim_on = 2;
			}
		}
		if ( flg_rep )
		{
			key.rep.space = true;
		}
	}

	return	true;
}
