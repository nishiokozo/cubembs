enum
{
	ENEMY_STAT_NONE,
	ENEMY_STAT_ACTIVE,
	ENEMY_STAT_DOWN,
};
int	enemy_draw();
int	enemy_update();
int	enemy_init( int argc, char *argv[] );
int	enemy_create();
int	enemy_remove();
typedef struct
{
	matrix	mat;
	int		limAttack;
	int		stat;
} ENEMY_INF;
ENEMY_INF*	enemy_getPtr();
int	enemy_getCnt();
