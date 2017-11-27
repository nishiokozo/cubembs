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


struct vktexcube_vs_uniform 
{
	// Must start with MVP
	float mvp[4][4];
	float position[12 * 3][4];
	float attr[12 * 3][4];
};


static const	int	unit_MAX = 5;
static vktexcube_vs_uniform*	dataVert			= new vktexcube_vs_uniform[unit_MAX];
static VkBuffer** 				sc_uniform_buffer	= new VkBuffer*[unit_MAX];
static VkDeviceMemory** 		sc_uniform_memory	= new VkDeviceMemory*[unit_MAX];
static VkDescriptorSet** 		sc_descriptor_set	= new VkDescriptorSet*[unit_MAX];
static vect44*					mvp					= new vect44[unit_MAX];
static vect44* 					g_model				= new vect44[unit_MAX];
static vect44 g_view;
static int unit_cnt=0;

static vktexcube_vs_uniform 	dataVert0 = 
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
const char *tex_files[] = {"lunarg.ppm"};

//-----------------------------------------------------------------------------
int main(int argc, char *argv[])
//-----------------------------------------------------------------------------
{

	for ( int u = 0; u < unit_MAX; u++ )
	{
		memcpy( &dataVert[u], &dataVert0,  sizeof(vktexcube_vs_uniform) );
		unit_cnt++;
	}

	//---------------------------------------------------------
	// 透視変換行列の作成
	//---------------------------------------------------------
	{
		for ( int u = 0; u < unit_cnt; u++ )
		{
			g_model[u].identity();
			g_model[u].translate( (-(unit_cnt-1)/2)+u*1.0,0,0);
		}
		g_view.identity();
		g_view.translate(0,0,-5);
	}

	{
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
	}


	//---
	WinInf* pWin = new WinInf( "msb", 128, 128 );
	VkInf* pVk = new VkInf( pWin->hInstance, pWin->hWin, pWin->win_width, pWin->win_height );

	if ( pVk ) 
	{
//			if ( pVk->flgSetModel== false )
			{
					vk2_create( pVk->vk
						, pWin->win_width
						, pWin->win_height
						, unit_MAX
						, "s-phong-vert.spv"
						, "s-phong-frag.spv"
						, tex_files
					);
					for ( int u = 0 ; u < unit_cnt ; u++ )
					{
							vk2_loadModel( pVk->vk
								, (void*)&dataVert[u]
								, sizeof(struct vktexcube_vs_uniform)
								, sc_uniform_buffer[u]
								, sc_uniform_memory[u]
								, sc_descriptor_set[u]
							);
					}
							pVk->flgSetModel = true;
			}
	}
	//-----------------------------------------------------
	// メインループ
	//-----------------------------------------------------
	MSG msg;   // message
	msg.wParam = 0;

	key_init(argc,argv);

	int lim1 = 0;
	int lim2 = 0;


	while (true) 
	{
		PeekMessage(&msg, NULL, 0, 0, PM_REMOVE);
		if (msg.message == WM_QUIT) // check for a flgQuit message
		{
			break;
		} else 
		{
			/* Translate and dispatch to event queue*/
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			RedrawWindow(pWin->hWin, NULL, NULL, RDW_INTERNALPAINT);
		}
//		if (msg.message != WM_PAINT) continue;

		
		if ( key.hi._6 )
		{
			lim2 = 100;
		}
		if ( lim2 )
		{
			if ( pVk != 0 ) 
			{
				if ( pVk->flgSetModel== true )
				{
					vk2_release( pVk->vk );
					pVk->flgSetModel = false;
				}
				if ( pVk->flgSetModel== false )
				{
					vk2_create( pVk->vk
						, pWin->win_width
						, pWin->win_height
						, unit_MAX
						, "s-phong-vert.spv"
						, "s-phong-frag.spv"
						, tex_files
					);

					for ( int u = 0 ; u < unit_cnt ; u++ )
					{
						vk2_loadModel( pVk->vk
							, (void*)&dataVert[u]
							, sizeof(struct vktexcube_vs_uniform)
							, sc_uniform_buffer[u]
							, sc_uniform_memory[u]
							, sc_descriptor_set[u]
						);
					}
					pVk->flgSetModel = true;
				}
				lim2--;
				printf("%d ",lim2 );
			}
		}
		
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
		//-----------------------------------------------------
		// 描画
		//-----------------------------------------------------
		if ( pVk ) 
		{
			if ( pVk->flgSetModel )
			{
				vk2_updateBegin( pVk->vk, pWin->win_width, pWin->win_height );

				for ( int u =0 ; u < unit_cnt ; u++ )
				{
					vk2_drawPolygon( pVk->vk
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

				vk2_updateEnd( pVk->vk );
			}
		}

		key_update();

	}

	//-----------------------------------------------------
	// 終了
	//-----------------------------------------------------
	if ( pVk ) 
	{
		if ( pVk->flgSetModel== true )
		{
			vk2_release( pVk->vk );
			pVk->flgSetModel = false;
		}
		delete pVk;pVk=0;
	}
	delete	[] dataVert;
	delete	[] sc_uniform_buffer;
	delete 	[] sc_uniform_memory;
	delete 	[] sc_descriptor_set;
	delete	[] mvp;
	delete	[] g_model;


	if ( pWin ) delete pWin;

	return (int)msg.wParam;
}
//#endif
