/*
#define	key_map \
	KEY_PRESS	a;\
	KEY_PRESS	b;\
	KEY_PRESS	c;\
	KEY_PRESS	d;\
	KEY_PRESS	e;\
	KEY_PRESS	f;\
	KEY_PRESS	g;\
	KEY_PRESS	h;\
	KEY_PRESS	i;\
	KEY_PRESS	j;\
	KEY_PRESS	k;\
	KEY_PRESS	l;\
	KEY_PRESS	m;\
	KEY_PRESS	n;\
	KEY_PRESS	o;\
	KEY_PRESS	p;\
	KEY_PRESS	q;\
	KEY_PRESS	r;\
	KEY_PRESS	s;\
	KEY_PRESS	t;\
	KEY_PRESS	u;\
	KEY_PRESS	v;\
	KEY_PRESS	w;\
	KEY_PRESS	x;\
	KEY_PRESS	y;\
	KEY_PRESS	z;\
	KEY_PRESS	_1;\
	KEY_PRESS	_2;\
	KEY_PRESS	_3;\
	KEY_PRESS	_4;\
	KEY_PRESS	_5;\
	KEY_PRESS	_6;\
	KEY_PRESS	_7;\
	KEY_PRESS	_8;\
	KEY_PRESS	_9;\
	KEY_PRESS	_0;\
\
	KEY_PRESS	f1;\
	KEY_PRESS	f2;\
	KEY_PRESS	f3;\
	KEY_PRESS	f4;\
	KEY_PRESS	f5;\
	KEY_PRESS	f6;\
	KEY_PRESS	f7;\
	KEY_PRESS	f8;\
	KEY_PRESS	f9;\
	KEY_PRESS	f10;\
	KEY_PRESS	f11;\
	KEY_PRESS	f12;\
	KEY_PRESS	f13;\
	KEY_PRESS	f14;\
	KEY_PRESS	f15;\
	KEY_PRESS	f16;\
\
	KEY_PRESS	esc;\
	KEY_PRESS	tab;\
	KEY_PRESS	capsLock;\
	KEY_PRESS	enter;\
	KEY_PRESS	space;\
	KEY_PRESS	shift;\
	KEY_PRESS	shift_l;\
	KEY_PRESS	shift_r;\
	KEY_PRESS	ctrl;\
	KEY_PRESS	ctrl_l;\
	KEY_PRESS	ctrl_r;\
	KEY_PRESS	alt;\
	KEY_PRESS	alt_l;\
	KEY_PRESS	alt_r;\
\
	KEY_PRESS	scrollLock;\
	KEY_PRESS	pause;\
\
	KEY_PRESS	pageUp;\
	KEY_PRESS	pageDown;\
	KEY_PRESS	end;\
	KEY_PRESS	home;\
	KEY_PRESS	insert;\
	KEY_PRESS	del;\
\
	KEY_PRESS	left;\
	KEY_PRESS	up;\
	KEY_PRESS	right;\
	KEY_PRESS	down;\
\
	KEY_PRESS	numLock;\
	KEY_PRESS	num_0;\
	KEY_PRESS	num_1;\
	KEY_PRESS	num_2;\
	KEY_PRESS	num_3;\
	KEY_PRESS	num_4;\
	KEY_PRESS	num_5;\
	KEY_PRESS	num_6;\
	KEY_PRESS	num_7;\
	KEY_PRESS	num_8;\
	KEY_PRESS	num_9;\
	KEY_PRESS	num_multiply;\
	KEY_PRESS	num_add;\
	KEY_PRESS	num_enter;\
	KEY_PRESS	num_sub;\
	KEY_PRESS	num_dot;\
	KEY_PRESS	num_devide;\

typedef struct 
{
	int	on;
	int	hi;
	int	lo;
	int	rep;
	
} KEY_PRESS;
*/
typedef struct
{
	int	a;
	int	b;
	int	c;
	int	d;
	int	e;
	int	f;
	int	g;
	int	h;
	int	i;
	int	j;
	int	k;
	int	l;
	int	m;
	int	n;
	int	o;
	int	p;
	int	q;
	int	r;
	int	s;
	int	t;
	int	u;
	int	v;
	int	w;
	int	x;
	int	y;
	int	z;
	int	_1;
	int	_2;
	int	_3;
	int	_4;
	int	_5;
	int	_6;
	int	_7;
	int	_8;
	int	_9;
	int	_0;

	int	f1;
	int	f2;
	int	f3;
	int	f4;
	int	f5;
	int	f6;
	int	f7;
	int	f8;
	int	f9;
	int	f10;
	int	f11;
	int	f12;
	int	f13;
	int	f14;
	int	f15;
	int	f16;

	int	esc;
	int	tab;
	int	capsLock;
	int	enter;
	int	space;
	int	shift;
	int	shift_l;
	int	shift_r;
	int	ctrl;
	int	ctrl_l;
	int	ctrl_r;
	int	alt;
	int	alt_l;
	int	alt_r;

	int	scrollLock;
	int	pause;

	int	pageUp;
	int	pageDown;
	int	end;
	int	home;
	int	insert;
	int	del;

	int	left;
	int	up;
	int	right;
	int	down;

	int	numLock;
	int	num_0;
	int	num_1;
	int	num_2;
	int	num_3;
	int	num_4;
	int	num_5;
	int	num_6;
	int	num_7;
	int	num_8;
	int	num_9;
	int	num_multiply;
	int	num_add;
	int	num_enter;
	int	num_sub;
	int	num_dot;
	int	num_devide;

} KEY_TYPE;

typedef struct
{
	KEY_TYPE	on;
	KEY_TYPE	hi;
	KEY_TYPE	lo;
	KEY_TYPE	rep;
} KEY_INF;


extern	KEY_INF	key;

/*
enum
{
	KEY_OFF		= (1<<0),
	KEY_ON		= (1<<1),
	KEY_HI		= (1<<2),
	KEY_LOW		= (1<<3),
	KEY_REPEAT	= (1<<4),
};
*/
int	key_update();
int key_init( int argc, char *argv[] );
