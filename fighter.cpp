/*
#define APIENTRY
#include <stdio.h>
#include <GL/gl.h>

#include "l.h"
#include "camera.h"
#include "fighter.h"
#include "core.h"
#include "missile.h"
*/
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
//#include <vulkan/vk_sdk_platform.h>	//1.0では必要だった


#include <math.h>

#include "key.h"
#include "vk.h"
#include "win.h"
#include "vect.h"
#include "unit12.h"
//#include "enemy.h"
//#include "camera.h"
#include "vector.h"
#include "matrix.h"
#include "mouse.h"

#include "fighter.h"

typedef struct
{
	matrix	mat;
	int		attack_lim;

	int		t_flg;
	int		t_lim;
	matrix	t_mf;
} FIGHTER_INF;

static	FIGHTER_INF	fighter_inf;
/*
static	float vertex[][3] = {
#if 0
	{ -0.0, -0.5, -0.1 },
	{  0.0, -0.5, -0.1 },
	{  0.1,  0.5, -0.5 },
	{ -0.1,  0.5, -0.5 },
	{ -0.1, -0.5,  0.1 },
	{  0.1, -0.5,  0.1 },
	{  0.6,  0.5,  0.5 },
	{ -0.6,  0.5,  0.5 }
#else
	{ -0.5, -0.5, -0.5 },
	{  0.5, -0.5, -0.5 },
	{  0.5,  0.5, -0.5 },
	{ -0.5,  0.5, -0.5 },
	{ -0.5, -0.5,  0.5 },
	{  0.5, -0.5,  0.5 },
	{  0.5,  0.5,  0.5 },
	{ -0.5,  0.5,  0.5 }
#endif
};

static	int face[][4] = {
	{ 0, 1, 2, 3 },
	{ 1, 5, 6, 2 },
	{ 5, 4, 7, 6 },
	{ 4, 0, 3, 7 },
	{ 4, 5, 1, 0 },
	{ 3, 2, 6, 7 }
};

static	float normal[][3] = {
	{ 0.0, 0.0,-1.0 },
	{ 1.0, 0.0, 0.0 },
	{ 0.0, 0.0, 1.0 },
	{-1.0, 0.0, 0.0 },
	{ 0.0,-1.0, 0.0 },
	{ 0.0, 1.0, 0.0 }
};

static	float g_def[] = { 1.0, 1.0, 1.0, 1.0 };
static	float g_amb[] = { 0.4, 0.4, 0.4, 1.0 };
*/

static Unit12::vk_vert12 	dataVertConst0 = 
{
	{//mvp
		0,0,0,0,
		0,0,0,0,
		0,0,0,0,
		0,0,0,0,
	},
	{//xyzw

		{-0.4f,-0.4f,-0.4f, 1.0f},  // -X side
		{-0.4f,-0.4f, 0.4f, 1.0f},
		{-0.4f, 0.4f, 0.4f, 1.0f},
		{-0.4f, 0.4f, 0.4f, 1.0f},
		{-0.4f, 0.4f,-0.4f, 1.0f},
		{-0.4f,-0.4f,-0.4f, 1.0f},

		{-0.4f,-0.4f,-0.4f, 1.0f},  // -Z side
		{ 0.4f, 0.4f,-0.4f, 1.0f},
		{ 0.4f,-0.4f,-0.4f, 1.0f},
		{-0.4f,-0.4f,-0.4f, 1.0f},
		{-0.4f, 0.4f,-0.4f, 1.0f},
		{ 0.4f, 0.4f,-0.4f, 1.0f},

		{-0.4f,-0.4f,-0.4f, 1.0f},  // -Y side
		{ 0.4f,-0.4f,-0.4f, 1.0f},
		{ 0.4f,-0.4f, 0.4f, 1.0f},
		{-0.4f,-0.4f,-0.4f, 1.0f},
		{ 0.4f,-0.4f, 0.4f, 1.0f},
		{-0.4f,-0.4f, 0.4f, 1.0f},

		{-0.4f, 0.4f,-0.4f, 1.0f},  // +Y side
		{-0.4f, 0.4f, 0.4f, 1.0f},
		{ 0.4f, 0.4f, 0.4f, 1.0f},
		{-0.4f, 0.4f,-0.4f, 1.0f},
		{ 0.4f, 0.4f, 0.4f, 1.0f},
		{ 0.4f, 0.4f,-0.4f, 1.0f},

		{ 0.4f, 0.4f,-0.4f, 1.0f},  // +X side
		{ 0.4f, 0.4f, 0.4f, 1.0f},
		{ 0.4f,-0.4f, 0.4f, 1.0f},
		{ 0.4f,-0.4f, 0.4f, 1.0f},
		{ 0.4f,-0.4f,-0.4f, 1.0f},
		{ 0.4f, 0.4f,-0.4f, 1.0f},

		{-0.4f, 0.4f, 0.4f, 1.0f},  // +Z side
		{-0.4f,-0.4f, 0.4f, 1.0f},
		{ 0.4f, 0.4f, 0.4f, 1.0f},
		{-0.4f,-0.4f, 0.4f, 1.0f},
		{ 0.4f,-0.4f, 0.4f, 1.0f},
		{ 0.4f, 0.4f, 0.4f, 1.0f},
	},
	{//uv
		{0.0f, 1.0f, 0.0f, 0.0f},  // -X side
		{1.0f, 1.0f, 0.0f, 0.0f},
		{1.0f, 0.0f, 0.0f, 0.0f},
		{1.0f, 0.0f, 0.0f, 0.0f},
		{0.0f, 0.0f, 0.0f, 0.0f},
		{0.0f, 1.0f, 0.0f, 0.0f},

		{1.0f, 1.0f, 0.0f, 0.0f},  // -Z side
		{0.0f, 0.0f, 0.0f, 0.0f},
		{0.0f, 1.0f, 0.0f, 0.0f},
		{1.0f, 1.0f, 0.0f, 0.0f},
		{1.0f, 0.0f, 0.0f, 0.0f},
		{0.0f, 0.0f, 0.0f, 0.0f},

		{1.0f, 0.0f, 0.0f, 0.0f},  // -Y side
		{1.0f, 1.0f, 0.0f, 0.0f},
		{0.0f, 1.0f, 0.0f, 0.0f},
		{1.0f, 0.0f, 0.0f, 0.0f},
		{0.0f, 1.0f, 0.0f, 0.0f},
		{0.0f, 0.0f, 0.0f, 0.0f},

		{1.0f, 0.0f, 0.0f, 0.0f},  // +Y side
		{0.0f, 0.0f, 0.0f, 0.0f},
		{0.0f, 1.0f, 0.0f, 0.0f},
		{1.0f, 0.0f, 0.0f, 0.0f},
		{0.0f, 1.0f, 0.0f, 0.0f},
		{1.0f, 1.0f, 0.0f, 0.0f},

		{1.0f, 0.0f, 0.0f, 0.0f},  // +X side
		{0.0f, 0.0f, 0.0f, 0.0f},
		{0.0f, 1.0f, 0.0f, 0.0f},
		{0.0f, 1.0f, 0.0f, 0.0f},
		{1.0f, 1.0f, 0.0f, 0.0f},
		{1.0f, 0.0f, 0.0f, 0.0f},

		{0.0f, 0.0f, 0.0f, 0.0f},  // +Z side
		{0.0f, 1.0f, 0.0f, 0.0f},
		{1.0f, 0.0f, 0.0f, 0.0f},
		{0.0f, 1.0f, 0.0f, 0.0f},
		{1.0f, 1.0f, 0.0f, 0.0f},
		{1.0f, 0.0f, 0.0f, 0.0f},
		

	}
};
static Unit12::vk_vert12 	dataVertPhong0 = 
{
	{//mvp
		0,0,0,0,
		0,0,0,0,
		0,0,0,0,
		0,0,0,0,
	},
	{//xyzw
		{ 0.00f ,  0.60f , -0.50f, 0.0f},  // -X side
		{ 0.00f ,  0.00f ,  1.50f, 0.0f},
		{ 0.00f ,  0.00f ,  1.50f, 0.0f},
		{ 0.00f ,  0.00f ,  1.50f, 1.0f},
		{ 0.50f ,  0.00f , -0.50f, 1.0f},
		{ 0.00f ,  0.60f , -0.50f, 1.0f},

		{ 0.00f ,  0.60f , -0.50f, 0.0f},  // -Z side
		{-0.50f ,  0.00f , -0.50f, 0.0f},
		{-0.00f ,  0.60f , -0.50f, 0.0f},
		{ 0.00f ,  0.60f , -0.50f, 1.0f},
		{ 0.50f ,  0.00f , -0.50f, 1.0f},
		{-0.50f ,  0.00f , -0.50f, 1.0f},

		{-0.00f , -0.10f , -0.00f, 0.0f},  // -Y side
		{ 0.00f , -0.10f , -0.00f, 0.0f},
		{ 0.00f , -0.10f ,  0.00f, 0.0f},
		{-0.00f , -0.10f , -0.00f, 0.0f},
		{ 0.00f , -0.10f ,  0.00f, 0.0f},
		{-0.00f , -0.10f ,  0.00f, 0.0f},

		{ 0.50f ,  0.00f , -0.50f, 0.0f},  // +Y side
		{ 0.00f ,  0.00f ,  1.50f, 0.0f},
		{-0.00f ,  0.00f ,  1.50f, 0.0f},
		{ 0.50f ,  0.00f , -0.50f, 1.0f},
		{-0.00f ,  0.00f ,  1.50f, 1.0f},
		{-0.50f ,  0.00f , -0.50f, 1.0f},

		{ 0.00f ,  0.00f ,  0.00f, 0.0f},  // +X side
		{ 0.00f ,  0.00f ,  0.00f, 0.0f},
		{ 0.00f , -0.00f ,  0.00f, 0.0f},
		{-0.00f , -0.00f ,  1.50f, 1.0f},
		{-0.00f ,  0.60f , -0.50f, 1.0f},
		{-0.50f , -0.00f , -0.50f, 1.0f},

		{-0.00f ,  0.00f ,  1.50f, 0.0f},  // +Z side
		{-0.00f , -0.00f ,  1.50f, 0.0f},
		{ 0.00f ,  0.00f ,  1.50f, 0.0f},
		{-0.00f , -0.00f ,  1.50f, 0.0f},
		{ 0.00f , -0.00f ,  1.50f, 0.0f},
		{ 0.00f ,  0.00f ,  1.50f, 0.0f},
	},
	{//normal
		{ 0.0f , 1.0f },   // -X side
		{ 1.0f , 1.0f }, 
		{ 1.0f , 0.0f }, 
		{ 0.75f, 0.63f, 0.19f }, 
		{ 0.75f, 0.63f, 0.19f }, 
		{ 0.75f, 0.63f, 0.19f }, 
		     
		{ 1.0f , 1.0f },   // -Z side
		{ 0.0f , 0.0f }, 
		{ 0.0f , 1.0f }, 
		{ 0.0f , 0.0f,-1.0f }, 
		{ 0.0f , 0.0f,-1.0f }, 
		{ 0.0f , 0.0f,-1.0f }, 
		     
		{ 1.0f , 0.0f },   // -Y side
		{ 1.0f , 1.0f }, 
		{ 0.0f , 1.0f }, 
		{ 1.0f , 0.0f }, 
		{ 0.0f , 1.0f }, 
		{ 0.0f , 0.0f }, 
		     
		{ 1.0f , 0.0f },   // +Y side
		{ 0.0f , 0.0f }, 
		{ 0.0f , 1.0f }, 
		{ 0.0f ,-1.0f, 0.0f }, 
		{ 0.0f ,-1.0f, 0.0f }, 
		{ 0.0f ,-1.0f, 0.0f }, 
		     
		{ 1.0f , 0.0f },   // +X side
		{ 0.0f , 0.0f }, 
		{ 0.0f , 1.0f }, 
		{-0.75f, 0.63f, 0.19f }, 
		{-0.75f, 0.63f, 0.19f }, 
		{-0.75f, 0.63f, 0.19f }, 
		     
		{ 0.0f , 0.0f },   // +Z side
		{ 0.0f , 1.0f }, 
		{ 1.0f , 0.0f }, 
		{ 0.0f , 1.0f }, 
		{ 1.0f , 1.0f }, 
		{ 1.0f , 0.0f }, 
	}
};
static Unit12*				g_unit;
extern	vect44 g_view,g_proj;
static const char 		*tex_files[] = {"lunarg.ppm"};


//-----------------------------------------------------------------------------
matrix	fighter_getMatrix()
//-----------------------------------------------------------------------------
{
	return	fighter_inf.mat;
}
//-----------------------------------------------------------------------------
static	void drawBody( matrix m, float size )
//-----------------------------------------------------------------------------
{
	memcpy( g_unit->mat_model.m, m.m, sizeof(matrix));
/*
	{
		g_unit->mvp.identity();
		g_unit->mvp.perspectiveGL( 45, 512.0/512.0,0.1,100		 );
		g_unit->mvp.m[1][1] *= -1; // GL to Vulkan
		g_unit->mvp =  g_unit->mat_model * g_view * g_unit->mvp;
	}
*/
		g_unit->mvp =  g_unit->mat_model * g_view * g_proj;
//printf("fi %f %f %f\n", g_unit->mat_model.m[3][0], g_unit->mat_model.m[3][1], g_unit->mat_model.m[3][2] );
	// 描画
	{
		g_unit->drawModel();
	}

/*
	int i;
	int j;

	glPushMatrix();

		glMultMatrixf( (float*)m.m );

		glEnable(GL_LIGHTING);
		glEnable(GL_DEPTH_TEST);
		glBegin(GL_QUADS);
		{
			glMaterialfv(GL_FRONT, GL_DIFFUSE, g_def);
			glMaterialfv(GL_FRONT, GL_AMBIENT, g_amb);
		}

		for (j = 0; j < 6; ++j) {
			glNormal3fv(normal[j]);
			for (i = 0; i < 4; ++i) {
				float	v[3];
				v[0] = vertex[face[j][i]][0] * size;
				v[1] = vertex[face[j][i]][1] * size;
				v[2] = vertex[face[j][i]][2] * size;
				glVertex3fv( v );
			}
		}
		glEnd();

	glPopMatrix();
*/
}

//-----------------------------------------------------------------------------
void	fighter_remove()
//-----------------------------------------------------------------------------
{
	delete	g_unit;
}
//-----------------------------------------------------------------------------
int	fighter_create()
//-----------------------------------------------------------------------------
{
//	fighter_inf.mat = mrotx(-0.0001f)* mroty(-0.0001f)*mrotz(0.00f)*mtrans( 0,	5.0f,	15 );
//	fighter_inf.mat = mrotx(rad(-25.0001f))* mroty(rad(45))*mrotz(0.00f)*mtrans(45,	45.0f,	45 );
	fighter_inf.mat = mtrans(0,10,35 ) * mrotx(rad(-25.0001f))* mroty(rad(45))*mrotz(0.00f);

	g_unit = new Unit12;

	//---------------------------------------------------------
	// 透視変換行列の作成
	//---------------------------------------------------------
	{
		g_unit->mat_model.identity();
		g_unit->mat_model.translate( 0,-2,0);
	}

	{
		g_unit->mat_model.rotY(RAD(45.2));
	}

/*
	{
		g_unit->mvp.identity();
		g_unit->mvp.perspectiveGL( 45, 512.0/512.0,0.1,100		 );
		g_unit->mvp.m[1][1] *= -1; // GL to Vulkan
		g_unit->mvp =  g_unit->mat_model * g_view * g_unit->mvp;
	}
*/
		g_unit->mvp =  g_unit->mat_model * g_view * g_proj;

	//---------------------------------------------------------
	// モデル読み込み
	//---------------------------------------------------------
	{
		g_unit->loadModel(
			  &dataVertConst0, "s-const-tex-vert.spv", "s-const-tex-frag.spv", tex_files, 1
//			  &dataVertPhong0, "s-phong-vert.spv", "s-phong-frag.spv", tex_files, 0
		);
	}

	return	true;
}



//-----------------------------------------------------------------------------
int	fighter_update()
//-----------------------------------------------------------------------------
{
	matrix	matMove = mident();
	matrix	matRot = mident();

#define	SPD	0.4f

	if ( key.on.a )		matMove *= mtrans( SPD,0,0);
	if ( key.on.d )		matMove *= mtrans(-SPD,0,0);
	if ( key.on.w )		matMove *= mtrans( 0.0, SPD,0);
	if ( key.on.s )		matMove *= mtrans( 0.0,-SPD,0);
	if ( key.on.shift )	matMove *= mtrans( 0.0,0.0,  SPD);
	if ( key.on.ctrl )	matMove *= mtrans( 0.0,0.0, -SPD);

	if ( key.on.left )	matRot *= mroty(rad( 1));
	if ( key.on.right )	matRot *= mroty(rad(-1));
	if ( key.on.up )	matRot *= mrotx(rad(-1));
	if ( key.on.down )	matRot *= mrotx(rad( 1));

	if ( key.hi.r )
	{
		matMove = mident();
		fighter_inf.mat = mident();
	}

	if ( fighter_inf.attack_lim == 0 )
	{
//		if ( key.on.z )		
		if ( mouse.on.l ||  mouse.on.r )		
		{
//			missile_request( fighter_inf.mat, MISSLE_TYPE_FIGHTER,2.0f );
			fighter_inf.attack_lim = 1;
		}
	}
	if ( fighter_inf.attack_lim > 0 ) 		fighter_inf.attack_lim--;



//	fighter_inf.mat = mroty(rad(-mouse.vf.x*2)) * mrotx(rad(mouse.vf.y*2)) * fighter_inf.mat;	




	if ( key.hi.t )
	{
		{
			fighter_inf.t_flg  = true;
			vector va = fighter_inf.mat.vy();
			vector vb = vector(0,1,0);
			float	r = acos(fdot(va,vb));

			float	n = r *10;

			fighter_inf.t_lim = 50;

			fighter_inf.t_mf = mrotv( vnormal(vcross(  va, vb )), r/50 );

			printf("r=%f\n", n );

		}
	}
//			font_printf("fflg=%d %d\n", fighter_inf.t_flg, fighter_inf.t_lim );

	if ( fighter_inf.t_flg )
	{
		if ( fighter_inf.t_lim > 0 ) 
		{
			fighter_inf.t_lim--;
			if ( fighter_inf.t_lim == 0 )
			{
				fighter_inf.t_flg = false ;
			}
		}
	}

	if ( fighter_inf.t_flg )
	{
//			font_printf("a\n" );

			vector vpos = fighter_inf.mat.vpos();
			fighter_inf.mat = fighter_inf.mat * fighter_inf.t_mf;
			fighter_inf.mat.m[3][0] = vpos.x;
			fighter_inf.mat.m[3][1] = vpos.y;
			fighter_inf.mat.m[3][2] = vpos.z;
				
	}

		fighter_inf.mat = matMove * matRot * fighter_inf.mat;
		fighter_inf.mat = mnormal( fighter_inf.mat );






	
	return	true;
}

//-----------------------------------------------------------------------------
int	fighter_draw()
//-----------------------------------------------------------------------------
{
	drawBody( fighter_inf.mat, 1.0f );

	return	true;
}
