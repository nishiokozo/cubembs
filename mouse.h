//int mouse_update();
int mouse_update( float win_x, float win_y, float win_w, float win_h );
int mouse_init();
int mouse_mouse(int button, int state, int x, int y);
int	 mosue_resize(int w, int h);

typedef struct 
{
	int	l;
	int	r;
} MOUSE_TYPE;

typedef struct
{
	float	sx;
	float	sy;
	float	mx;
	float	my;
	float	cx;
	float	cy;
	float	dx;
	float	dy;
	float	screen_w;
	float	screen_h;
//	int	flgDrag;

	vector	vd;
	vector	vm;	//	移動量 //
	vector	vw;	//	位置 //
	vector	vf;	//	位置（アンチチャタリング）


	MOUSE_TYPE	on;
	MOUSE_TYPE	hi;
	MOUSE_TYPE	lo;
	MOUSE_TYPE	rep;
/*
	int	l;
	int	r;
	int	hi_l;
	int	hi_r;
	int	lo_l;
	int	lo_r;
*/
	int	button;
	int	state;
} MOUSE_INF;

extern	MOUSE_INF	mouse;



/*class	Mouse
{
public:

	void	update();
	Mouse();
	~Mouse();
};
*/
