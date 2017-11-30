//#define _GNU_SOURCE // M_PI が定義される
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
#include <vulkan/vk_sdk_platform.h>


#include <math.h>

#include "key.h"
#include "vk.h"
#include "win.h"
#include "vect.h"
#include "enemy.h"
//#include <GL/gl.h>
//#include "l.h"

//#include "camera.h"
//#include "fighter.h"
#include <math.h>
#include "vector.h"
#include "matrix.h"
#include "unit12.h"
#include "core.h"

static	matrix	g_mat;

extern	VkInf* g_pVk;
extern	vect44 g_view;

/*
struct vk_texcube_vs_uniform 
{
	// Must start with MVP
	float mvp[4][4];
	float position[12 * 3][4];
	float attr[12 * 3][4];
};
*/

static	float vertex[][3] = {

	{ -0.0, -0.4, 1.5 },
	{  0.0, -0.4, 1.5 },
	{  0.0, -0.4, 1.5 },
	{ -0.0, -0.4, 1.5 },

	{ -0.05,  0.4, -0.5 },
	{  0.05,  0.4, -0.5 },
	{  0.7, -0.4, -0.5 },
	{ -0.7, -0.4, -0.5 }
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

static const char 		*tex_files[] = {"lunarg.ppm"};

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

/*
static const	int	unit_MAX = 1;
static int unit_cnt=0;
static vk_texcube_vs_uniform*	dataVert			;
static VkBuffer** 				sc_uniform_buffer	;
static VkDeviceMemory** 		sc_uniform_memory	;
static VkDescriptorSet** 		sc_descriptor_set	;
static vect44*					mvp					;
static vect44* 					g_model				;
*/

static	Unit12*	g_unit;
//static int g_cube;

static	float g_col[] = { 0.8, 0.0, 0.0, 1.0 };
//-----------------------------------------------------------------------------
matrix	core_getMatrix()
//-----------------------------------------------------------------------------
{
	return	g_mat;
}
//-----------------------------------------------------------------------------
static	void drawCube( matrix m, float size )
//-----------------------------------------------------------------------------
{

	memcpy( g_unit->mat_model.m, m.m,sizeof(float[16]));
	g_unit->mat_model.m[3][2] = -g_unit->mat_model.m[3][2];
//	printf("pos1 %f %f %f\n", g_unit->mat_model.m[3][0], g_unit->mat_model.m[3][1], g_unit->mat_model.m[3][2] );

//		g_unit->mat_model.identity();
//		g_unit->mat_model.translate( (-(unit_cnt-1)/2)+0*1.0,0,0);

//	printf("pos2 %f %f %f\n", g_unit->mat_model.m[3][0], g_unit->mat_model.m[3][1], g_unit->mat_model.m[3][2] );

	vect44 mvp;
	{
		mvp.identity();
		mvp.perspectiveGL( 45, 512.0/512.0,0.1,100		 );
		mvp.m[1][1] *= -1; // GL to Vulkan
		mvp =  g_unit->mat_model * g_view * mvp;
	}

		g_unit->drawModel();

/*
//	for ( int u =0 ; u < unit_cnt ; u++ )
	{
		vk2_drawPolygon( g_pVk->vk
			, mvp.m//g_unit->mvp.m
			, sizeof(vect44)
			, sc_uniform_memory[u]
			, 12*3	//_vertexCount
			, 1		//_instanceCount
			, 0		//_firstVertex
			, 0		//_firstInstance
			, sc_descriptor_set[u]
		);
	}
*/

/*
	glPushMatrix();

		glMultMatrixf( (float*)m.m );

		glCallList(g_cube);

	glPopMatrix();
*/
}

//-----------------------------------------------------------------------------
void	core_create( )
//-----------------------------------------------------------------------------
{
/*
	dataVert			= new vk_texcube_vs_uniform[unit_MAX];
	sc_uniform_buffer	= new VkBuffer*[unit_MAX];
	sc_uniform_memory	= new VkDeviceMemory*[unit_MAX];
	sc_descriptor_set	= new VkDescriptorSet*[unit_MAX];
	mvp					= new vect44[unit_MAX];
	g_model				= new vect44[unit_MAX];

*/
	g_unit = new Unit12;

	//---------------------------------------------------------
	//	
	//---------------------------------------------------------
//	unit_cnt=0;
	//---------------------------------------------------------
	//	モデルコピー 
	//---------------------------------------------------------
/*
	for ( int u = 0; u < unit_MAX; u++ )
	{
//		memcpy( &dataVert[u], &dataVertConst,  sizeof(vk_texcube_vs_uniform) );
		memcpy( &dataVert[u], &dataVertConst,  sizeof(vk_texcube_vs_uniform) );
		unit_cnt++;
	}
*/

	//---------------------------------------------------------
	// 透視変換行列の作成
	//---------------------------------------------------------
//	for ( int u = 0; u < unit_cnt; u++ )
//	{
		g_unit->mat_model.identity();
//		g_unit->mat_model.translate( (-(unit_cnt-1)/2)+u*1.0,0,0);
//	}

//	for ( int u = 0; u < unit_cnt; u++ )
//	{
//		 	g_unit->mat_model.rotY(RAD(45.2));
//	}
//	for ( int u = 0; u < unit_cnt; u++ )
//	{
		g_unit->mvp.identity();
		g_unit->mvp.perspectiveGL( 45, 512.0/512.0,0.1,100		 );
		g_unit->mvp.m[1][1] *= -1; // GL to Vulkan
		g_unit->mvp =  g_unit->mat_model * g_view * g_unit->mvp;
//	}


	//---------------------------------------------------------
	//	モデル登録 
	//---------------------------------------------------------
		g_unit->loadModel(
			  &dataVertConst0, "s-const-tex-vert.spv", "s-const-tex-frag.spv"
//			  &dataVertPhong0, "s-phong-vert.spv", "s-phong-frag.spv"
			, tex_files
			, 1
		);

/*
	for ( int u = 0 ; u < unit_cnt ; u++ )
	{
		vk2_loadModel( g_pVk->vk
			
			, (void*)&dataVert[u]
			, sizeof(struct vk_texcube_vs_uniform)
			, sc_uniform_buffer[u]
			, sc_uniform_memory[u]
			, sc_descriptor_set[u]
			, "s-const-tex-vert.spv", "s-const-tex-frag.spv"
//			, "s-phong-vert.spv", "s-phong-frag.spv"
			, tex_files
			, 1
		);
	}
*/
/*
	//	モデル作成
	g_cube = glGenLists(1);

	glNewList(g_cube, GL_COMPILE);

		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, g_col);
		glBegin(GL_QUADS);
		{
			int i;
			int j;
			float	size = 1.0f;
			for (j = 0; j < 6; ++j) 
			{
				glNormal3fv(normal[j]);
				for (i = 0; i < 4; ++i) 
				{
					float	v[3];
					v[0] = vertex[face[j][i]][0] * size;
					v[1] = vertex[face[j][i]][1] * size;
					v[2] = vertex[face[j][i]][2] * size;
					glVertex3fv( v );
				}
			}
		}
		glEnd();

	glEndList();

	//	初期位置
*/
	g_mat = mtrans( 0,10,0 );

//	return	true;
}
//-----------------------------------------------------------------------------
static	void updateBody( matrix* pM1, vector v1, vector pt, vector vs )
//-----------------------------------------------------------------------------
{
	vector vx = pM1->vx();
	vector vy = pM1->vy();
	vector vz = pM1->vz();
	vector vc = vnormal(vcross(  vz, v1 ));

	float	f = fdot( vz, v1 );
	float	rc = acos(f) / 30.0f;



	matrix	mr = mrotv( vc, rc );

	vector p = pM1->vpos() +  pM1->mrot() * vs ;

#if 0
//	姿勢制御

	vector	p0 = pM1->vpos();

	vector	vt = vnormal(pt-p0);

	float	rz = acos(fdot( vy, vt ));
	
	if ( rz > rad(10) )
	{

	  #if 0
		line_request( p0, pt, vector(1,1,0));
		line_request( p0, p0+vy, vector(1,0,0));
	  #endif

		if ( fdot( vx, vt ) > 0.0f ) rz = -rz;

		(*pM1) = mrotz(rz/20) * (*pM1) * mr ;
	}
	else
	{
		(*pM1) = (*pM1) * mr ;
	}
#else
	(*pM1) = (*pM1) * mr ;
#endif

	pM1->m[3][0] = p.x;
	pM1->m[3][1] = p.y;
	pM1->m[3][2] = p.z;


}
//-----------------------------------------------------------------------------
int	core_update()
//-----------------------------------------------------------------------------
{
	vector pt;
	vector vs;

	static	int flgKakusan = false;
	static	int limKakusan = 600;

//	if ( key.hi.k ) flgKakusan = !flgKakusan;

	if ( limKakusan-- <= 0 )
	{
		limKakusan = 1000;
		flgKakusan = ! flgKakusan;
	}
	

	if ( flgKakusan )
	{
		vs = vector( 0, 0, 5.2f );
	}
	else
	{
//		pt = fighter_getMatrix().vpos();
		pt = vector( 0, 0, 0 );
		vs = vector( 0, 0, 1.3f );
	}
	
	

	vector p0 = g_mat.vpos();


	vector vm = vnormal( pt - p0 );

//	printf("vm %f %f %f\n", vm.x, vm.y, vm.z );

	updateBody( &g_mat, vm, pt, vs );

	return	true;
}

//-----------------------------------------------------------------------------
int	core_draw()
//-----------------------------------------------------------------------------
{

	g_mat	= mnormal( g_mat );

	drawCube( g_mat, 1.0f );

	return	true;
}
