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
#include "vk2.h"
#include "win.h"
#include "vect.h"
#include "enemy.h"

extern	VkInf* g_pVk;
extern	vect44 g_view;

struct vktexcube_vs_uniform 
{
	// Must start with MVP
	float mvp[4][4];
	float position[12 * 3][4];
	float attr[12 * 3][4];
};


static const	int	unit_MAX = 5;
static int unit_cnt=0;
static vktexcube_vs_uniform*	dataVert			;//= new vktexcube_vs_uniform[unit_MAX];
static VkBuffer** 				sc_uniform_buffer	;//= new VkBuffer*[unit_MAX];
static VkDeviceMemory** 		sc_uniform_memory	;//= new VkDeviceMemory*[unit_MAX];
static VkDescriptorSet** 		sc_descriptor_set	;//= new VkDescriptorSet*[unit_MAX];
static vect44*					mvp					;//= new vect44[unit_MAX];
static vect44* 					g_model				;//= new vect44[unit_MAX];

static vktexcube_vs_uniform 	dataVertDelta = 
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
static vktexcube_vs_uniform 	dataVertConst = 
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
static const char *tex_files[] = {"lunarg.ppm"};


//-----------------------------------------------------------------------------
void enemy_create()
//-----------------------------------------------------------------------------
{

	dataVert			= new vktexcube_vs_uniform[unit_MAX];
	sc_uniform_buffer	= new VkBuffer*[unit_MAX];
	sc_uniform_memory	= new VkDeviceMemory*[unit_MAX];
	sc_descriptor_set	= new VkDescriptorSet*[unit_MAX];
	mvp					= new vect44[unit_MAX];
	g_model				= new vect44[unit_MAX];

	//---------------------------------------------------------
	//	
	//---------------------------------------------------------
	unit_cnt=0;
	//---------------------------------------------------------
	//	モデルコピー 
	//---------------------------------------------------------
	for ( int u = 0; u < unit_MAX; u++ )
	{
//		memcpy( &dataVert[u], &dataVertConst,  sizeof(vktexcube_vs_uniform) );
		memcpy( &dataVert[u], &dataVertDelta,  sizeof(vktexcube_vs_uniform) );
		unit_cnt++;
	}

	//---------------------------------------------------------
	// 透視変換行列の作成
	//---------------------------------------------------------
	for ( int u = 0; u < unit_cnt; u++ )
	{
		g_model[u].identity();
		g_model[u].translate( (-(unit_cnt-1)/2)+u*1.0,0,0);
	}

	for ( int u = 0; u < unit_cnt; u++ )
	{
//		 	g_model[u].rotX(RAD(0.1));
	 	g_model[u].rotY(RAD(80));
	}
	for ( int u = 0; u < unit_cnt; u++ )
	{
		mvp[u].identity();
		mvp[u].perspectiveGL( 45, 512.0/512.0,0.1,100		 );
		mvp[u].m[1][1] *= -1; // GL to Vulkan
		mvp[u] =  g_model[u] * g_view * mvp[u];
	}


	//---------------------------------------------------------
	//	モデル登録 
	//---------------------------------------------------------
	for ( int u = 0 ; u < unit_cnt ; u++ )
	{
		vk2_loadModel( g_pVk->vk
			, (void*)&dataVert[u]
			, sizeof(struct vktexcube_vs_uniform)
			, sc_uniform_buffer[u]
			, sc_uniform_memory[u]
			, sc_descriptor_set[u]
//						, "s-const-tex-vert.spv", "s-const-tex-frag.spv"
						, "s-phong-vert.spv", "s-phong-frag.spv"
						, tex_files
						, 1
		);
	}
}
//-----------------------------------------------------------------------------
void enemy_remove()
//-----------------------------------------------------------------------------
{

	//---------------------------------------------------------
	//	モデル 
	//---------------------------------------------------------
	for ( int u = 0 ; u < unit_cnt ; u++ )
	{
		vk2_removeModel( g_pVk->vk
			, sc_uniform_buffer[u]
			, sc_uniform_memory[u]
			, sc_descriptor_set[u]
		);
	}

	delete	[] dataVert;
	delete	[] sc_uniform_buffer;
	delete 	[] sc_uniform_memory;
	delete 	[] sc_descriptor_set;
	delete	[] mvp;
	delete	[] g_model;

}
//-----------------------------------------------------------------------------
void enemy_update()
//-----------------------------------------------------------------------------
{
		for ( int u = 0; u < unit_cnt; u++ )
		{
		 	g_model[u].rotX(RAD(0.1));
		 	g_model[u].rotY(RAD(1));
		}
		for ( int u = 0; u < unit_cnt; u++ )
		{
			mvp[u].identity();
			mvp[u].perspectiveGL( 45, 512.0/512.0,0.1,100		 );
			mvp[u].m[1][1] *= -1; // GL to Vulkan
			mvp[u] =  g_model[u] * g_view * mvp[u];
		}

				for ( int u =0 ; u < unit_cnt ; u++ )
				{
					vk2_drawPolygon( g_pVk->vk
						, mvp[u].m
						, sizeof(vect44)
						, sc_uniform_memory[u]
						, 12*3	//_vertexCount
						, 1		//_instanceCount
						, 0		//_firstVertex
						, 0		//_firstInstance
						, sc_descriptor_set[u]
					);
				}


}
