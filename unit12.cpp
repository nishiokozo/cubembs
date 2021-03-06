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
//#include <vulkan/vk_sdk_platform.h>	//1.0では必要だった


#include <math.h>

#include "key.h"
#include "vk.h"
#include "win.h"
#include "vect.h"
#include "unit12.h"



//=============
extern	VkInf* g_pVk;

//-----------------------------------------------------------------------------
void Unit12::loadModel(
//-----------------------------------------------------------------------------
	 vk_vert12* pDataVert12

	, const char* fn_vert
	, const char* fn_frag
	, const char** 	tex_files
	, const int		tex_cnt
)
{
	g_pVk->loadModel(
		  (void*)pDataVert12
		, sizeof(struct vk_vert12)
//		, vkunit.uniform_buffer
//		, vkunit.uniform_memory
//		, vkunit.descriptor_set
		, fn_vert
		, fn_frag
		, tex_files
		, tex_cnt

		, vkunit
	);

}

//-----------------------------------------------------------------------------
void Unit12::unloadModel()
//-----------------------------------------------------------------------------
{
	g_pVk->unloadModel(
//		  vkunit.uniform_buffer
//		, vkunit.uniform_memory
//		, vkunit.descriptor_set
		 vkunit
	);
}

//-----------------------------------------------------------------------------
void Unit12::drawModel()
//-----------------------------------------------------------------------------
{
	g_pVk->drawModel(
		  mvp.m
		, sizeof(vect44)
		, 12*3	//_vertexCount
		, 1		//_instanceCount
		, 0		//_firstVertex
		, 0		//_firstInstance

		, vkunit
	);
}

