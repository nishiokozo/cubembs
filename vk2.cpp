//#define _GNU_SOURCE // M_PI が定義される
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#include <signal.h>

//#ifdef _WIN32
//#pragma comment(linker, "/subsystem:windows")	//	コンソール表示されなくする
//#endif  // _WIN32

#define M_PI	3.14159265358979323846

#define	VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>
#include <vulkan/vk_sdk_platform.h>


//#ifndef LINMATH_H
//#define LINMATH_H

#include <math.h>

#include "key.h"
#include "vk.h"

#define ASSERTW(flg,err_msg)											 \
	if ( !(flg) ) {																		 \
		wchar_t wcs[256];														\
		mbstowcs( wcs, __func__, 256 );												\
		wchar_t wcs2[256];														\
		MultiByteToWideChar( CP_UTF8, 0, err_msg,-1,wcs2,256 );\
		MessageBoxW(NULL, wcs2, wcs, MB_OK); \
		exit(1);																 \
	} 

#define ERR_EXIT(err_msg, err_class)											 \
	do {																		 \
		MessageBox(NULL, err_msg, err_class, MB_OK); \
		exit(1);																 \
	} while (0)


PFN_vkGetDeviceProcAddr g_gdpa = NULL;
const char *tex_files[] = {"lunarg.ppm"};


//-----------------------------------------------------------------------------
bool memory_type_from_properties(
//-----------------------------------------------------------------------------
	  VkPhysicalDeviceMemoryProperties* pMemory_properties
	, uint32_t typeBits
	, VkFlags requirements_mask
	, uint32_t *typeIndex
) 
{
	// Search memtypes to find first index with those properties
	for (uint32_t i = 0; i < VK_MAX_MEMORY_TYPES; i++) 
	{
		if ((typeBits & 1) == 1) 
		{
			// Type is available, does it match user properties?
			if ((pMemory_properties->memoryTypes[i].propertyFlags & requirements_mask) == requirements_mask) 
			{
				*typeIndex = i;
				return true;
			}
		}
		typeBits >>= 1;
	}
	// No memory types matched, return failure
	return false;
}




//-----------------------------------------------------------------------------
char *demo_read_spv(const char *filename, size_t *psize) 
//-----------------------------------------------------------------------------

{
	long int size;
	size_t  retval;
	void *shader_code;

	FILE *fp = fopen(filename, "rb");
	if (!fp) return NULL;

	fseek(fp, 0L, SEEK_END);
	size = ftell(fp);

	fseek(fp, 0L, SEEK_SET);

	shader_code = malloc(size);
	retval = fread(shader_code, size, 1, fp);
	assert(retval == 1);

	*psize = size;

	fclose(fp);
	return (char*)shader_code;
}

//-----------------------------------------------------------------------------
void vk_setVert(
//-----------------------------------------------------------------------------
	  const VkDevice							& 	vk_sc_device 
	, const VkPhysicalDeviceMemoryProperties	&	vk_pdmemory_properties
	, const void* 									pDataVert
	, const int 									sizeofDataVert

	, VkBuffer 									&	sir_uniform_buffer
	, VkDeviceMemory 							&	sir_uniform_memory
)
{

	//---------------------
	// バッファ作成
	//---------------------
	{
		VkBufferCreateInfo bci;
		{
			memset(&bci, 0, sizeof(bci));
			bci.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
			bci.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
			bci.size = sizeofDataVert;//sizeof(pDataVert);
		}
		VkResult  err;
		err = vkCreateBuffer(vk_sc_device, &bci, NULL, &sir_uniform_buffer);	//create22 setVert
		ASSERTW(!err,"中断します");
	}

	//---------------------
	// バッファのメモリ要求取得
	//---------------------
	{
		VkMemoryRequirements mr;
		vkGetBufferMemoryRequirements(vk_sc_device, sir_uniform_buffer, &mr);

		VkMemoryAllocateInfo mai;
		mai.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		mai.pNext = NULL;
		mai.allocationSize = mr.size;
		mai.memoryTypeIndex = 0;

		bool  flgFound = false;
		{// 列挙検索
			uint32_t 	typeBits			= mr.memoryTypeBits;
			VkFlags 	requirements_mask	= VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
			uint32_t*	typeIndex			= &mai.memoryTypeIndex;

			for (uint32_t i = 0; i < VK_MAX_MEMORY_TYPES; i++) 
			{
				if ((typeBits & 1) == 1) 
				{
					if ((vk_pdmemory_properties.memoryTypes[i].propertyFlags & requirements_mask) == requirements_mask) 
					{
						*typeIndex = i;
						flgFound = true;
						break;
					}
				}
				typeBits >>= 1;
			}
		}
		if ( flgFound == false )
		{
			ASSERTW(false, "VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT:指定した現在のモードはサポートされていません" );
		}
		//---------------------
		// メモリの確保
		//---------------------
		{
			VkResult  err;
			err = vkAllocateMemory(vk_sc_device, &mai, NULL,&sir_uniform_memory);	//create23 setVert
			ASSERTW(!err,"中断します");
		}
	}


	//---------------------
	// メモリ転送
	//---------------------
	{
		//---------------------
		// マップメモリ
		//---------------------
		uint8_t *pData;
		{
			VkResult  err;
			err = vkMapMemory(vk_sc_device, sir_uniform_memory, 0, VK_WHOLE_SIZE, 0, (void **)&pData);
			ASSERTW(!err,"中断します");
		}

		//---------------------
		// 転送
		//---------------------
		memcpy(pData, pDataVert, sizeofDataVert);

		//---------------------
		// マップ解放
		//---------------------
		vkUnmapMemory(vk_sc_device, sir_uniform_memory);
	}


	//---------------------
	// バインド
	//---------------------
	{
		VkResult  err;
		err = vkBindBufferMemory(
			  vk_sc_device
			, sir_uniform_buffer
			, sir_uniform_memory
			, 0
		);
		ASSERTW(!err,"中断します");
	}
}

//-----------------------------------------------------------------------------
static void	vk_createDescripterPool( 
	  const  VkDevice			&	vk_sc_device
	, const  int 					tex_count
	, const uint32_t 				sc_cntImage

	, VkDescriptorPool 			&	vk_desc_pool
)
//-----------------------------------------------------------------------------
{
	//=========================================================================
	// デスクリプタ・作成
	//=========================================================================
	{
		VkDescriptorPoolSize dps[2] = 
		{
			[0] =
				{
				 .type 				= VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
				 .descriptorCount 	= sc_cntImage
				},
			[1] =
				{
				 .type 				= VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
				 .descriptorCount 	= sc_cntImage * tex_count
				},
		};
		VkDescriptorPoolCreateInfo dpci = 
		{
			.sType 					= VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
			.pNext 					= NULL,
			.flags 					= 0,
			.maxSets 				= sc_cntImage,
			.poolSizeCount 			= 2,
			.pPoolSizes 			= dps
		};
		VkResult  err;
		err = vkCreateDescriptorPool(vk_sc_device, &dpci, NULL, &vk_desc_pool);
		ASSERTW(!err,"中断します");
	}
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
static void	vk_createDescriptionLayout( 
	  const VkDevice& vk_sc_device
	, const int tex_count

	, VkDescriptorSetLayout 				&vk_desc_layout
)
{
	//=========================================================================
	// ディスクリプションレイアウト・作成
	//=========================================================================
	{
		//---------------------
		//ディスクリプションレイアウト・情報取得
		//---------------------
		VkDescriptorSetLayoutBinding dslb[2] = 
		{
			[0] =
				
				{
				 .binding 				= 0,
				 .descriptorType 		= VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
				 .descriptorCount 		= 1,
				 .stageFlags 			= VK_SHADER_STAGE_VERTEX_BIT,
				 .pImmutableSamplers 	= NULL
				},
			[1] =
				
				{
				 .binding = 1,
				 .descriptorType 		= VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
				 .descriptorCount 		= tex_count,
				 .stageFlags 			= VK_SHADER_STAGE_FRAGMENT_BIT,
				 .pImmutableSamplers 	= NULL
				},
		};
		{
			VkDescriptorSetLayoutCreateInfo dslci = 
			{
				.sType 					= VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
				.pNext 					= NULL,
				.flags					= 0,
				.bindingCount			= 2,
				.pBindings				= dslb
			};
			VkResult  err;
			err = vkCreateDescriptorSetLayout(vk_sc_device, &dslci, NULL, &vk_desc_layout);
			ASSERTW(!err,"中断します");
		}
	}
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
static void	vk_createPipelineLayout( 
	  const VkDevice				&	vk_sc_device
	, const VkDescriptorSetLayout 	&	vk_desc_layout

	,  VkPipelineLayout				&	vk_pipeline_layout
)
{
	//=========================================================================
	// パイプラインレイアウト・作成
	//=========================================================================
	{
		VkPipelineLayoutCreateInfo plci = 
		{
			.sType 						= VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
			.pNext 						= NULL,
			.flags						= 0,
			.setLayoutCount 			= 1,
			.pSetLayouts 				= &vk_desc_layout
		};

		VkResult  err;
		err = vkCreatePipelineLayout(vk_sc_device, &plci, NULL, &vk_pipeline_layout);
		ASSERTW(!err,"中断します");
	}

}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
static void	vk_createRenderPass( 
	  const VkDevice	& 	vk_sc_device
	, const VkFormat 	&	vk_surface_format
	, const VkFormat 		vk_depth_format

	, VkRenderPass 		&	vk_render_pass
)
{
	//=========================================================================
	// レンダーパスの作成
	//=========================================================================
	{

		// レンダリングの開始時には、その内容は気にしないので、colorおよびdepth_inf添付ファイルの初期レイアウトはLAYOUT_UNDEFINEDになります。
		// サブパスの開始時に、カラーアタッチメントのレイアウトはLAYOUT_COLOR_ATTACHMENT_OPTIMALに移行され、
		// depth_infステンシルアタッチメントのレイアウトはLAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMALに移行されます。
		// レンダーパスの最後に、カラーアタッチメントのレイアウトがLAYOUT_PRESENT_SRC_KHRに移行され、表示されます。
		// これはすべてレンダーパスの一部として行われ、バリアは必要ありません。

		VkAttachmentDescription ad[2] = 
		{
			[0] =
				{
				 .flags 			= 0,
				 .format			= vk_surface_format,
				 .samples 			= VK_SAMPLE_COUNT_1_BIT,
				 .loadOp 			= VK_ATTACHMENT_LOAD_OP_CLEAR,
				 .storeOp 			= VK_ATTACHMENT_STORE_OP_STORE,
				 .stencilLoadOp 	= VK_ATTACHMENT_LOAD_OP_DONT_CARE,
				 .stencilStoreOp	= VK_ATTACHMENT_STORE_OP_DONT_CARE,
				 .initialLayout 	= VK_IMAGE_LAYOUT_UNDEFINED,
				 .finalLayout 		= VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
				},
			[1] =
				
				{
				 .flags 			= 0,
				 .format 			= vk_depth_format,
				 .samples 			= VK_SAMPLE_COUNT_1_BIT,
				 .loadOp 			= VK_ATTACHMENT_LOAD_OP_CLEAR,
				 .storeOp 			= VK_ATTACHMENT_STORE_OP_DONT_CARE,
				 .stencilLoadOp 	= VK_ATTACHMENT_LOAD_OP_DONT_CARE,
				 .stencilStoreOp 	= VK_ATTACHMENT_STORE_OP_DONT_CARE,
				 .initialLayout 	= VK_IMAGE_LAYOUT_UNDEFINED,
				 .finalLayout		= VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
				},
		};

		//---
		
		const VkAttachmentReference color_reference = 
		{
			.attachment = 0, .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
		};
		const VkAttachmentReference depth_reference = 
		{
			.attachment = 1,
			.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
		};
		const VkSubpassDescription subpass = 
		{
			.flags 						= 0,
			.pipelineBindPoint 			= VK_PIPELINE_BIND_POINT_GRAPHICS,
			.inputAttachmentCount 		= 0,
			.pInputAttachments 			= NULL,
			.colorAttachmentCount 		= 1,
			.pColorAttachments 			= &color_reference,
			.pResolveAttachments 		= NULL,
			.pDepthStencilAttachment 	= &depth_reference,
			.preserveAttachmentCount 	= 0,
			.pPreserveAttachments 		= NULL
		};

		//---------------------
		// レンダーパスの取得
		//---------------------
		{
			VkRenderPassCreateInfo rp_info = 
			{
				.sType 				= VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
				.pNext 				= NULL,
				.flags 				= 0,
				.attachmentCount 	= 2,
				.pAttachments 		= ad,
				.subpassCount 		= 1,
				.pSubpasses 		= &subpass,
				.dependencyCount 	= 0,
				.pDependencies 		= NULL
			};

			VkResult  err;
			err = vkCreateRenderPass(vk_sc_device, &rp_info, NULL, &vk_render_pass);
			ASSERTW(!err,"中断します");
		}
	}

}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
static void	vk_CreatePipelineCache( 
	  const VkDevice			&	vk_sc_device
	, const VkRenderPass 		&	vk_render_pass

	,  VkPipelineCache 			&	vk_pipelineCache

)
{

	//---------------------
	// パイプライン・キャッシュの作成
	//---------------------
	{
		VkPipelineCacheCreateInfo ppc;
		memset(&ppc, 0, sizeof(ppc));
		ppc.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;

		VkResult  err;
		err = vkCreatePipelineCache(vk_sc_device, &ppc, NULL, &vk_pipelineCache);
		ASSERTW(!err,"中断します");
	}
}//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
static void	vk_CreateGraphicsPipelines( 
	  const VkDevice			&	vk_sc_device
	, const VkPipelineLayout	&	vk_pipeline_layout
	, const VkRenderPass 		&	vk_render_pass
	, const void*					vcode
	, const size_t					vsize
	, const void*					fcode
	, const size_t					fsize
	, const VkPipelineCache 	&	vk_pipelineCache

	,  VkPipeline 				&	vk_pipeline_graphics

)
{

	//=========================================================================
	// グラフィックパイプラインの作成
	//=========================================================================
	{
		//---------------------
		// パイプライン・バーテックス入力状態作成情報
		//---------------------
		VkPipelineVertexInputStateCreateInfo vi;
		{
			memset(&vi, 0, sizeof(vi));
			vi.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		}

		//---------------------
		// パイプライン・入力アセンブリ状態作成情報
		//---------------------
		VkPipelineInputAssemblyStateCreateInfo ia;
		{
			memset(&ia, 0, sizeof(ia));
			ia.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
			ia.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		}

		//---------------------
		// パイプライン・ラスタライゼーション状態作成情報
		//---------------------
		VkPipelineRasterizationStateCreateInfo rs;
		{
			memset(&rs, 0, sizeof(rs));
			rs.sType 					= VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
			rs.polygonMode 				= VK_POLYGON_MODE_FILL;
			rs.cullMode 				= VK_CULL_MODE_BACK_BIT;
			rs.frontFace 				= VK_FRONT_FACE_COUNTER_CLOCKWISE;
			rs.depthClampEnable 		= VK_FALSE;
			rs.rasterizerDiscardEnable 	= VK_FALSE;
			rs.depthBiasEnable 			= VK_FALSE;
			rs.lineWidth 				= 1.0f;
		}

		//---------------------
		// パイプライン・カラーブレンド状態作成情報
		//---------------------
		VkPipelineColorBlendStateCreateInfo pcbsc;
		{
			memset(&pcbsc, 0, sizeof(pcbsc));
			pcbsc.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
			VkPipelineColorBlendAttachmentState att_state[1];
			memset(att_state, 0, sizeof(att_state));
			att_state[0].colorWriteMask = 0xf;
			att_state[0].blendEnable = VK_FALSE;
			pcbsc.attachmentCount = 1;
			pcbsc.pAttachments = att_state;
		}

		//---------------------
		// パイプライン・デプスステンシル状態作成情報
		//---------------------
		VkPipelineDepthStencilStateCreateInfo dsci;
		{
			memset(&dsci, 0, sizeof(dsci));
			dsci.sType 					= VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
			dsci.depthTestEnable 		= VK_TRUE;
			dsci.depthWriteEnable 		= VK_TRUE;
			dsci.depthCompareOp 		= VK_COMPARE_OP_LESS_OR_EQUAL;
			dsci.depthBoundsTestEnable 	= VK_FALSE;
			dsci.back.failOp 			= VK_STENCIL_OP_KEEP;
			dsci.back.passOp 			= VK_STENCIL_OP_KEEP;
			dsci.back.compareOp			= VK_COMPARE_OP_ALWAYS;
			dsci.stencilTestEnable 		= VK_FALSE;
			dsci.front 					= dsci.back;
		}
		
		//---------------------
		// パイプライン・ビューポート状態作成情報
		//---------------------
		VkPipelineViewportStateCreateInfo vp;
		{
			memset(&vp, 0, sizeof(vp));
			vp.sType 			= VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
			vp.viewportCount 	= 1;
			vp.scissorCount 	= 1;
		}

		//---------------------
		// パイプライン・マルチサンプル状態作成情報
		//---------------------
		VkPipelineMultisampleStateCreateInfo ms;
		{
			memset(&ms, 0, sizeof(ms));
			ms.sType 				= VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
			ms.pSampleMask 			= NULL;
			ms.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		}

		//---------------------
		// パイプライン・ダイナミック状態作成情報
		//---------------------
		VkDynamicState ds[VK_DYNAMIC_STATE_RANGE_SIZE];
		VkPipelineDynamicStateCreateInfo pdsci;
		{
			memset(&pdsci, 0, sizeof pdsci);
			pdsci.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;

			memset(ds, 0, sizeof ds);
			pdsci.pDynamicStates = ds;
			ds[pdsci.dynamicStateCount++] = VK_DYNAMIC_STATE_VIEWPORT;
			ds[pdsci.dynamicStateCount++] = VK_DYNAMIC_STATE_SCISSOR;
		}

		//---------------------
		// グラフィックパイプラインの作成
		//---------------------
		{
			VkShaderModule vert_sm;
			VkShaderModule flag_sm;
			//---------------------
			// シェーダーモジュールの作成
			//---------------------
			VkPipelineShaderStageCreateInfo pssci[2];
			{
				memset(&pssci, 0, 2 * sizeof(VkPipelineShaderStageCreateInfo));

				//---------------------
				// バーテックスシェーダー読み込みモジュールの作成
				//---------------------
				{
					pssci[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
					pssci[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
					{
						VkShaderModuleCreateInfo smci;
						smci.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
						smci.pNext = NULL;
						smci.codeSize = vsize;
						smci.pCode = (uint32_t*)vcode;
						smci.flags = 0;
						{
							VkResult  err;
							err = vkCreateShaderModule(vk_sc_device, &smci, NULL, &vert_sm);
							ASSERTW(!err,"中断します");
						}
						pssci[0].module = vert_sm;
					}
					pssci[0].pName = "main";
				}

				//---------------------
				// フラグメントシェーダーモジュールの作成
				//---------------------
				{
					pssci[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
					pssci[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
					{
						VkShaderModuleCreateInfo smci;
						smci.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
						smci.pNext = NULL;
						smci.codeSize = fsize;
						smci.pCode = (uint32_t*)fcode;
						smci.flags = 0;
						{
							VkResult  err;
							err = vkCreateShaderModule(vk_sc_device, &smci, NULL, &flag_sm);
							ASSERTW(!err,"中断します");
						}
						pssci[1].module = flag_sm;
					}
					pssci[1].pName = "main";
				}
			}

			//---------------------
			// グラフィックパイプラインの生成
			//---------------------
			{
				VkGraphicsPipelineCreateInfo gpci;
				memset(&gpci, 0, sizeof(gpci));
				gpci.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
				gpci.layout = vk_pipeline_layout;
				gpci.stageCount = 2; // Two stages: vs and fs

				gpci.pVertexInputState 		= &vi;
				gpci.pInputAssemblyState 	= &ia;
				gpci.pRasterizationState 	= &rs;
				gpci.pColorBlendState 		= &pcbsc;
				gpci.pMultisampleState 		= &ms;
				gpci.pViewportState 		= &vp;
				gpci.pDepthStencilState 	= &dsci;
				gpci.pStages 				= pssci;
				gpci.renderPass 			= vk_render_pass;
				gpci.pDynamicState 			= &pdsci;
				gpci.renderPass 			= vk_render_pass;

				VkResult  err;
				err = vkCreateGraphicsPipelines(vk_sc_device, vk_pipelineCache, 1, &gpci, NULL, &vk_pipeline_graphics);
				ASSERTW(!err,"中断します");
			}

			//---------------------
			// フラグメントシェーダーモジュールの廃棄
			//---------------------
			vkDestroyShaderModule(vk_sc_device, flag_sm, NULL);

			//---------------------
			// バーテックスシェーダーモジュールの廃棄
			//---------------------
			vkDestroyShaderModule(vk_sc_device, vert_sm, NULL);
		}

	}
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
static void vk_AllocateCommandBuffers( 
	  const VkDevice			&	vk_device 
	, const VkCommandPool 		&	vk_cmd_pool

	, VkCommandBuffer 			&	sir_cmdbuf
)
{
//	for (uint32_t i = 0; i < vk.swapchainImageCount; i++) 
	{
		const VkCommandBufferAllocateInfo cmai = 
		{
			.sType 				= VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
			.pNext 				= NULL,
			.commandPool 		= vk_cmd_pool,
			.level 				= VK_COMMAND_BUFFER_LEVEL_PRIMARY,
			.commandBufferCount = 1,
		};
		VkResult  err;
		err = vkAllocateCommandBuffers(vk_device, &cmai, &sir_cmdbuf);	//create21	setPipeline
		assert(!err);
	}
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
static void	vk_AllocateDescriptorSets(
	  const VkDevice				&	vk_sc_device
	, const VkDescriptorSetLayout 	&	vk_desc_layout
	, const VkDescriptorPool 		&	vk_desc_pool

	, VkDescriptorSet 				&	descriptor_set
)
{
	//---------------------
	// ディスクリプターセット・確保
	//---------------------
	{
		VkDescriptorSetAllocateInfo dsai = 
		{
			.sType				= VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
			.pNext 				= NULL,
			.descriptorPool 	= vk_desc_pool,
			.descriptorSetCount = 1,
			.pSetLayouts 		= &vk_desc_layout
		};
		VkResult  err;
		err = vkAllocateDescriptorSets(vk_sc_device, &dsai, &descriptor_set);
		{
			char	chr[256];
			sprintf( chr, "中断します。 err=(%x)\n",err);
			ASSERTW(!err, chr);
		}
	}

}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
static void	vk_UpdateDescriptorSets(
	  const VkDevice				&	vk_sc_device
	, const int 						tex_count
	, const texture_object* 			pTex
	, const int							sizeofStructDataVert
	, const VkBuffer 				&	uniform_buffer

	, VkDescriptorSet 				&	descriptor_set
)
{
	//---------------------
	// ディスクリプターセット・更新
	//---------------------
	{
		//---------------------
		// ディスクリプターセット・バッファ設定
		//---------------------
		VkDescriptorBufferInfo dbi;
		dbi.offset 				= 0;
		dbi.range 				= sizeofStructDataVert;
		dbi.buffer 				= uniform_buffer;

		//---------------------
		// ディスクリプターセット・イメージ設定
		//---------------------
		VkDescriptorImageInfo dif[tex_count];
		memset(&dif, 0, sizeof(dif));
		for (unsigned int i = 0; i < tex_count; i++) 
		{
			dif[i].sampler		= pTex[i].sampler;
			dif[i].imageView 	= pTex[i].imgview;
			dif[i].imageLayout 	= VK_IMAGE_LAYOUT_GENERAL;
		}

		VkWriteDescriptorSet wds[2];
		memset(&wds, 0, sizeof(wds));

		wds[0].sType 			= VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		wds[0].pNext 			= 0;
		wds[0].dstSet 			= descriptor_set;
		wds[0].dstBinding 		= 0;
		wds[0].dstArrayElement	= 0;
		wds[0].descriptorCount 	= 1;
		wds[0].descriptorType 	= VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		wds[0].pImageInfo 		= NULL;
		wds[0].pBufferInfo 		= &dbi;
		wds[0].pTexelBufferView	= 0;

		wds[1].sType 			= VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		wds[1].pNext 			= 0;
		wds[1].dstSet 			= descriptor_set;
		wds[1].dstBinding 		= 1;
		wds[1].dstArrayElement	= 0;
		wds[1].descriptorCount 	= tex_count;
		wds[1].descriptorType 	= VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		wds[1].pImageInfo 		= dif;
		wds[1].pBufferInfo 		= NULL;
		wds[1].pTexelBufferView	= 0;

		vkUpdateDescriptorSets(vk_sc_device, 2, wds, 0, NULL);
	}
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
static void	vk_createFramebuffer( 
	  const VkDevice				& 	vk_sc_device
	, const VkImageView 			&	sc_imgview
	, const VkRenderPass 			&	vk_render_pass
	, const VkImageView				& 	vk_depth_imgview
	, const int 						_width
	, const int 						_height

	, VkFramebuffer 				&	sc_framebuffer

)
{

	//---------------------
	// フレームバッファの作成
	//---------------------
	{
		VkImageView attachments[2];
		attachments[1] = vk_depth_imgview;
		attachments[0] = sc_imgview;

		VkFramebufferCreateInfo fci = 
		{
			.sType 				= VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
			.pNext 				= NULL,
			.flags 				= 0,
			.renderPass 		= vk_render_pass,
			.attachmentCount 	= 2,
			.pAttachments 		= attachments,
			.width 				= _width,
			.height 			= _height,
			.layers 			= 1
		};
		VkResult  err;
		err = vkCreateFramebuffer(vk_sc_device, &fci, NULL, &sc_framebuffer);
		ASSERTW(!err,"中断します");
	}
}

//-----------------------------------------------------------------------------
static void	vk_CmdBindPipeline( 
//-----------------------------------------------------------------------------
	  const VkPipeline 					&	vk_pipeline_graphics
	, const	VkCommandBuffer				& 	sc_cmdbuf

)
{
	//---------------------
	// 描画コマンド・パイプラインのバインド
	//---------------------
	vkCmdBindPipeline(sc_cmdbuf, VK_PIPELINE_BIND_POINT_GRAPHICS, vk_pipeline_graphics );
}
//-----------------------------------------------------------------------------
static void	vk_CmdBindDescriptorSets( 
//-----------------------------------------------------------------------------
	  const VkPipelineLayout			&	vk_pipeline_layout
	, const	VkDescriptorSet				& 	sc_descriptor_set
	, const	VkCommandBuffer				& 	sc_cmdbuf
)
{
	//---------------------
	// 描画コマンド・ディスクリプターのバインド
	//---------------------
	vkCmdBindDescriptorSets(
		 sc_cmdbuf
		,VK_PIPELINE_BIND_POINT_GRAPHICS
		,vk_pipeline_layout
		,0
		,1
		,&sc_descriptor_set//sc_descriptor_set[i]
		,0
		,NULL
	);
}
//-----------------------------------------------------------------------------
static void	vk_EndCommandBuffer( 
//-----------------------------------------------------------------------------
	  const	VkCommandBuffer				& 	sc_cmdbuf
)
{
	//---------------------
	// 描画コマンドバッファ終了
	//---------------------
	{
		VkResult  err;
		err = vkEndCommandBuffer(sc_cmdbuf);
		ASSERTW(!err,"中断します");
	}
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
static void	vk_CmdBeginRenderPass( 
	  const VkRenderPass 				&	vk_render_pass
	, const	VkFramebuffer				& 	sc_framebuffer
	, const	VkCommandBuffer				& 	sc_cmdbuf
	, const int 							_width
	, const int 							_height
)
{
	//---------------------
	// 描画コマンド・レンダーパスの開始
	//---------------------
	{
		VkClearValue cv[2] = 
		{
			[0] = 
			{
				.color =
				{
//					.float32 = {0.2f, 0.2f, 0.2f, 0.2f}
					.float32 = {1.0f, 1.0f, 1.0f, 1.0f}
				},
			}
			,
			[1] = 
			{
				.depthStencil = {1.0f, 0}
			}
		};

		VkRenderPassBeginInfo rpbi = 
		{
			.sType 						= VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
			.pNext 						= NULL,
			.renderPass 				= vk_render_pass,
			.framebuffer 				= sc_framebuffer, //sc_framebuffer[i],
			.renderArea =
			{
				.offset 		= {0,0},
				.extent		 	= {_width,_height},
			},
			.clearValueCount 			= 2,
			.pClearValues 				= cv
		};
//printf("cmbuf %d before vkCmdBeginRenderPass-st \n",__LINE__);
		vkCmdBeginRenderPass(sc_cmdbuf, &rpbi, VK_SUBPASS_CONTENTS_INLINE);
//printf("cmbuf %d before vkCmdBeginRenderPass-en \n",__LINE__);

	}
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
static void	vk_BeginCommandBuffer( 
	  const	VkCommandBuffer				& 	sir_cmdbuf
)
{
	const VkCommandBufferBeginInfo cbbi = 
	{
		.sType 				= VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
		.pNext 				= NULL,
		.flags 				= VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT,
		.pInheritanceInfo 	= NULL,
	};
	VkResult  err;
	err = vkBeginCommandBuffer(sir_cmdbuf, &cbbi);
	assert(!err);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
static void	vk_CmdSetViewport( 
	  const	VkCommandBuffer				& 	sc_cmdbuf
	, const int 							_width
	, const int 							_height

)
{
	//---------------------
	// 描画コマンド・ビューポートの設定
	//---------------------
	{
		VkViewport viewport;
		memset(&viewport, 0, sizeof(viewport));
		viewport.height = (float)_height;
		viewport.width = (float)_width;
		viewport.minDepth = (float)0.0f;
		viewport.maxDepth = (float)1.0f;
		vkCmdSetViewport(sc_cmdbuf, 0, 1, &viewport);
	}
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
static void	vk_CmdSetScissor( 
	  const	VkCommandBuffer				& 	sc_cmdbuf
	, const int 							_width
	, const int 							_height

)
{

	//---------------------
	// 描画コマンド・シザリングエリアの設定
	//---------------------
	{
		VkRect2D scissor;
		memset(&scissor, 0, sizeof(scissor));
		scissor.extent.width = _width;
		scissor.extent.height = _height;
		scissor.offset.x = 0;
		scissor.offset.y = 0;
		vkCmdSetScissor(sc_cmdbuf, 0, 1, &scissor);
	}
}
//-----------------------------------------------------------------------------
void vk2_create( VulkanInf& vk, int _width, int _height
//-----------------------------------------------------------------------------
//	,void* pDataVert
//	,int sizeofStructDataVert
	, int unit_cnt
 )
{


	//-----------------------------------------------------
	// パイプライン作成
	//-----------------------------------------------------
	{

//		vk_setPipeline( vk, _width, _height, _vertexCount, _instanceCount, _firstVertex, _firstInstance
//			, sizeofStructDataVert
//		 );
		{
			//---------------------
			// ディスクリプターレイアウト作成
			//---------------------
				vk_createDescriptionLayout(
					  vk.device
					, DEMO_TEXTURE_COUNT

					, vk.desc_layout	//create11
				);

			//-----------------------------------------------------
			// デスクリプタプールの作成
			//-----------------------------------------------------
			{
				vk_createDescripterPool(									
					  vk.device
					, DEMO_TEXTURE_COUNT
					, vk.swapchainImageCount * unit_cnt

					, vk.desc_pool	//create6
				);
			}

			//-----------------------------------------------------
			// パイプラインレイアウトの作成
			//-----------------------------------------------------
				vk_createPipelineLayout(
					  vk.device 
					, vk.desc_layout

					, vk.pipeline_layout	//create10
				);

			//-----------------------------------------------------
			// レンダーパスの作成
			//-----------------------------------------------------
				vk_createRenderPass(
					  vk.device 
					, vk.format
					, vk.depth_inf.format

					, vk.render_pass	//create9
				);
		

			//-----------------------------------------------------
			// パイプラインキャッシュの作成
			//-----------------------------------------------------
			{
				vk_CreatePipelineCache(
					  vk.device 
					, vk.render_pass

					, vk.pipelineCache	//create8
				);
			}
			//---------------------
			// グラフィックパイプライン作成
			//---------------------
			const char* fn_vert = "s-phong-vert.spv";
			const char* fn_frag = "s-phong-frag.spv";
//			const char* fn_vert = "s-const-tex-vert.spv";
//			const char* fn_frag = "s-const-tex-frag.spv";
			{
				void *vcode;
				size_t vsize;
				void *fcode;
				size_t fsize;
				vcode = demo_read_spv( fn_vert, &vsize);
				fcode = demo_read_spv( fn_frag, &fsize);
				vk_CreateGraphicsPipelines(
					  vk.device 
					, vk.pipeline_layout
					, vk.render_pass
					, vcode
					, vsize 
					, fcode
					, fsize 
					, vk.pipelineCache

					, vk.pipeline		//create7
				);
				free(vcode);
				free(fcode);
			}

			//-----------------------------------------------------
			// コマンドバッファの作成
			//-----------------------------------------------------
			for (uint32_t i = 0; i < vk.swapchainImageCount; i++) 
			{
				vk_AllocateCommandBuffers(
					  vk.device
					, vk.cmd_pool

					, vk.sir_cmdbuf[i]
				);
			}

			
			//-----------------------------------------------------
			// フレームバッファの作成
			//-----------------------------------------------------
			for (int i = 0; i < vk.swapchainImageCount; i++) 
			{
				vk_createFramebuffer( 
					  vk.device
					, vk.sir_imgview[i]
					, vk.render_pass
					, vk.depth_inf.imgview
					, _width
					, _height

					, vk.sir_framebuffer[i]	//create5
				);
			}
			

		}
	}

	//-----------------------------------------------------
	// 終了待ち
	//-----------------------------------------------------
	{
		/*
		 * Prepare functions above may generate pipeline commands
		 * that need to be flushed before beginning the render loop.
		 */

		// This function could get called twice if the texture uses a staging buffer
		// In that case the second call should be ignored
		if (vk.cmdbuf == VK_NULL_HANDLE)
		{
		//	return;
		}
		else
		{
			//-----------------------------------------------------
			// コマンドバッファ終了
			//-----------------------------------------------------
			{
				VkResult  err;
				err = vkEndCommandBuffer(vk.cmdbuf);
				assert(!err);
			}

			//-----------------------------------------------------
			// フェンス処理
			//-----------------------------------------------------
			{	
				//-----------------------------------------------------
				// フェンスの作成
				//-----------------------------------------------------
				VkFence fence;
				{
					VkFenceCreateInfo fci = 
					{
						.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
						.pNext = NULL,
						.flags = 0
					};
					VkResult  err;
					err = vkCreateFence(vk.device, &fci, NULL, &fence);	//createTmp1
					assert(!err);
				}

				//-----------------------------------------------------
				// フェンス待ち
				//-----------------------------------------------------
				{
					const VkCommandBuffer cmd_bufs[] = {vk.cmdbuf};
					{
						VkSubmitInfo si = 
						{
							.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
							.pNext = NULL,
							.waitSemaphoreCount = 0,
							.pWaitSemaphores = NULL,
							.pWaitDstStageMask = NULL,
							.commandBufferCount = 1,
							.pCommandBuffers = cmd_bufs,
							.signalSemaphoreCount = 0,
							.pSignalSemaphores = NULL
						};
						VkResult  err;
						err = vkQueueSubmit(vk.graphics_queue, 1, &si, fence);
						assert(!err);
					}

					{
						VkResult  err;
						err = vkWaitForFences(vk.device, 1, &fence, VK_TRUE, UINT64_MAX);
						assert(!err);
					}

					vkFreeCommandBuffers(vk.device, vk.cmd_pool, 1, cmd_bufs);
				}

				//-----------------------------------------------------
				// フェンスの廃棄
				//-----------------------------------------------------
				vkDestroyFence(vk.device, fence, NULL);		//createTmp1

			}
			vk.cmdbuf = VK_NULL_HANDLE;
		}

		//-----------------------------------------------------
		// 
		//-----------------------------------------------------
		if (vk.staging_texture.image) 
		{
			//demo_destroy_texture_image(&vk, &vk.staging_texture);
			{
				/* clean up staging resources */
				vkFreeMemory(vk.device, vk.staging_texture.devmem, NULL);
				vkDestroyImage(vk.device, vk.staging_texture.image, NULL);
			}
		}

		//-----------------------------------------------------
		// 
		//-----------------------------------------------------
		{
			vk.current_buffer = 0;
		}
	}
//============
}
//-----------------------------------------------------------------------------
void vk2_loadModel( VulkanInf& vk
//-----------------------------------------------------------------------------
	,void* pDataVert
	,int sizeofStructDataVert

	, VkBuffer* 				&	sir_uniform_buffer
	, VkDeviceMemory* 			&	sir_uniform_memory
	, VkDescriptorSet* 			&	sir_descriptor_set
)
{

	sir_uniform_buffer 	= (VkBuffer *)			malloc(sizeof(VkBuffer) 		* vk.swapchainImageCount);
	sir_uniform_memory 	= (VkDeviceMemory *)	malloc(sizeof(VkDeviceMemory) 	* vk.swapchainImageCount);
	sir_descriptor_set 	= (VkDescriptorSet *)	malloc(sizeof(VkDescriptorSet) 	* vk.swapchainImageCount);

	for ( int i = 0; i < vk.swapchainImageCount; i++) 
	{
		//-----------------------------------------------------
		// モデルデータの作成
		//-----------------------------------------------------
		vk_setVert(
			  vk.device
			, vk.memory_properties
			, pDataVert
			, sizeofStructDataVert

			, sir_uniform_buffer[i]
			, sir_uniform_memory[i]
		);

		//-----------------------------------------------------
		// ディスクリプターの作成
		//-----------------------------------------------------
		vk_AllocateDescriptorSets(
			  vk.device
  			, vk.desc_layout
			, vk.desc_pool

			, sir_descriptor_set[i]
		);

		vk_UpdateDescriptorSets(
			  vk.device
			, DEMO_TEXTURE_COUNT
			, vk.textures 
			, sizeofStructDataVert
			, sir_uniform_buffer[i]
			, sir_descriptor_set[i]
		);
	}
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
static void	vk_AcquireNextImage(
	  const VkDevice				&	vk_sc_device
	, const VkSwapchainKHR			&	vk_sc_base
	, const VkSemaphore 			&	vk_s_image_acquired_semaphores	//[vk_FRAME_LAG]

	, uint32_t 						&	vk_current_buffer
)
{

	//=========================================================================================
	// 次のイメージと入れ替え
	//=========================================================================================
	{
		VkResult  err;
		do 
		{
			// Get the index of the next available vk_sc_base image:
			err = vkAcquireNextImageKHR(
				  vk_sc_device
				, vk_sc_base
				, UINT64_MAX
				, vk_s_image_acquired_semaphores	//[vk_frame_index]
				, VK_NULL_HANDLE
				, &vk_current_buffer
			);
			if (err == VK_ERROR_OUT_OF_DATE_KHR) 
			{
				// vk_sc_base is out of date (e.g. the window was resized) and
				// must be recreated:
				//demo_resize(&vk);
			} else if (err == VK_SUBOPTIMAL_KHR) 
			{
				// vk_sc_base is not as optimal as it could be, but the platform's
				// presentation engine will still present the image correctly.
				break;
			} else 
			{
				ASSERTW(!err,"中断します");
			}
		} while (err != VK_SUCCESS);
	}
}
//-----------------------------------------------------------------------------
void	vk2_updateBegin( VulkanInf& vk
	, const int 		_width
	, const int 		_height
)
//-----------------------------------------------------------------------------
{
	// Ensure no more than FRAME_LAG renderings are outstanding
	vkWaitForFences(vk.device, 1, &vk.fences[vk.frame_index], VK_TRUE, UINT64_MAX);
	vkResetFences(vk.device, 1, &vk.fences[vk.frame_index]);

	vk_AcquireNextImage(
		  vk.device
		, vk.swapchain
		, vk.image_acquired_semaphores[vk.frame_index]

		, vk.current_buffer
	);
/*
	//---------------------------------------------------------
	// 
	//---------------------------------------------------------
	{
		VkResult  err;
		do 
		{
			// Get the index of the next available swapchain image:
			err = vkAcquireNextImageKHR(vk.device, vk.swapchain, UINT64_MAX,
											  vk.image_acquired_semaphores[vk.frame_index],
											  VK_NULL_HANDLE, &vk.current_buffer);

			if (err == VK_ERROR_OUT_OF_DATE_KHR) 
			{
				// vk.swapchain is out of date (e.g. the window was resized) and
				// must be recreated:
				//demo_resize(&vk);
			} else if (err == VK_SUBOPTIMAL_KHR) 
			{
				// vk.swapchain is not as optimal as it could be, but the platform's
				// presentation engine will still present the image correctly.
				break;
			} else 
			{
				assert(!err);
			}
		} while (err != VK_SUCCESS);
	}
*/


	//-----------------------------------------------------
	// 描画コマンドバッファの作成
	//-----------------------------------------------------
//	for (uint32_t i = 0; i < vk.swapchainImageCount; i++) 
	{
int i = vk.current_buffer;
//		vk.current_buffer = i;
		
		//-----------------------------------------------------
		// コマンドバッファの開始
		//-----------------------------------------------------
		vk_BeginCommandBuffer(
			  vk.sir_cmdbuf[i]
		);

		//-----------------------------------------------------
		// コマンド・レンダーパスの開始
		//-----------------------------------------------------
		vk_CmdBeginRenderPass(
			  vk.render_pass
			, vk.sir_framebuffer[i]
			, vk.sir_cmdbuf[i]
			, _width
			, _height
		);
		
		//-----------------------------------------------------
		// コマンド・パイプラインのバインド
		//-----------------------------------------------------
		vk_CmdBindPipeline(
			  vk.pipeline
			, vk.sir_cmdbuf[i]
		);

		
		//-----------------------------------------------------
		// ビューポートの設定
		//-----------------------------------------------------
		vk_CmdSetViewport(
			  vk.sir_cmdbuf[i]
			, _width
			, _height
		);

		//-----------------------------------------------------
		// シザリングエリアの設定
		//-----------------------------------------------------
		vk_CmdSetScissor(
			  vk.sir_cmdbuf[i]
			, _width
			, _height
		);
	}
}
//-----------------------------------------------------------------------------
void	vk2_drawPolygon( VulkanInf& vk
,const void* pMVP
,int matrixSize
, VkDeviceMemory* 			&	sir_uniform_memory
	,int _vertexCount		//	= 12*3;
	,int _instanceCount		//	= 1;
	,int _firstVertex		//	= 0;
	,int _firstInstance		// = 0;
	, VkDescriptorSet* 			&	sir_descriptor_set
)
{
	//-----------------------------------------------------
	// 描画コマンドバッファの作成
	//-----------------------------------------------------
//	for (uint32_t i = 0; i < vk.swapchainImageCount; i++) 
	{
int i = vk.current_buffer;
		//-----------------------------------------------------
		// コマンド・ディスクリプターのバインド
		//-----------------------------------------------------
		vk_CmdBindDescriptorSets(
			  vk.pipeline_layout
			, sir_descriptor_set[vk.current_buffer]
			, vk.sir_cmdbuf[i]
		);

		//-----------------------------------------------------
		// 描画コマンド発行
		//-----------------------------------------------------
		vkCmdDraw(vk.sir_cmdbuf[i], _vertexCount, _instanceCount, _firstVertex, _firstInstance);
	}

	{

		uint8_t *pData;

		//---------------------------------------------------------
		// マップメモリ
		//---------------------------------------------------------
		{
			VkResult  err;
			err = vkMapMemory( 
				vk.device
				, sir_uniform_memory[vk.current_buffer]
				, 0
				, VK_WHOLE_SIZE
				, 0
				, (void **)&pData
			);
			assert(!err);
		}

		//---------------------------------------------------------
		// マトリクスのコピー
		//---------------------------------------------------------
		memcpy(pData, pMVP, matrixSize);

		//---------------------------------------------------------
		// マップ解除
		//---------------------------------------------------------
		vkUnmapMemory(vk.device, sir_uniform_memory[vk.current_buffer]);
	}
}
//-----------------------------------------------------------------------------
void	vk2_updateEnd( VulkanInf& vk)
//-----------------------------------------------------------------------------
{
	//-----------------------------------------------------
	// 描画コマンドバッファの作成
	//-----------------------------------------------------
//	for (uint32_t i = 0; i < vk.swapchainImageCount; i++) 
	{
int i = vk.current_buffer;
		//-----------------------------------------------------
		// レンダーパス終了
		//-----------------------------------------------------
		// Note that ending the renderpass changes the image's layout from
		// COLOR_ATTACHMENT_OPTIMAL to PRESENT_SRC_KHR
		vkCmdEndRenderPass(vk.sir_cmdbuf[i]);

		//-----------------------------------------------------
		// コマンドバッファ終了
		//-----------------------------------------------------
		vk_EndCommandBuffer(
			vk.sir_cmdbuf[i]
		);
	}


	// Wait for the image acquired semaphore to be signaled to ensure
	// that the image won't be rendered to until the presentation
	// engine has fully released ownership to the application, and it is
	// okay to render to the image.

	//---------------------------------------------------------
	// グラフィックキューを登録
	//---------------------------------------------------------
	{
		VkPipelineStageFlags psf;
		psf = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

		{
			VkSubmitInfo si;
			si.sType 				= VK_STRUCTURE_TYPE_SUBMIT_INFO;
			si.pNext 				= NULL;
			si.pWaitDstStageMask 	= &psf;
			si.waitSemaphoreCount 	= 1;
			si.pWaitSemaphores 		= &vk.image_acquired_semaphores[vk.frame_index];
			si.commandBufferCount 	= 1;
			si.pCommandBuffers 		= &vk.sir_cmdbuf[vk.current_buffer];
			si.signalSemaphoreCount = 1;
			si.pSignalSemaphores 	= &vk.draw_complete_semaphores[vk.frame_index];
			{
				VkResult  err;
				err = vkQueueSubmit(vk.graphics_queue, 1, &si, vk.fences[vk.frame_index]);
				assert(!err);
			}
		}
	}

	//---------------------------------------------------------
	// 描画キューを登録
	//---------------------------------------------------------
/*
	if (vk.flg_separate_present_queue) 
	{
		VkPipelineStageFlags psf;
		psf = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		{

			// If we are using separate queues, change image ownership to the
			// present queue before presenting, waiting for the draw complete
			// semaphore and signalling the ownership released semaphore when finished
			VkSubmitInfo si;
			si.sType 				= VK_STRUCTURE_TYPE_SUBMIT_INFO;
			si.pNext 				= NULL;
			si.pWaitDstStageMask 	= &psf;
			si.waitSemaphoreCount 	= 1;
			si.pWaitSemaphores 		= &vk.draw_complete_semaphores[vk.frame_index];
			si.commandBufferCount 	= 1;
			si.pCommandBuffers 		= &vk.sir_graphics_to_present_cmdbuf[vk.current_buffer];
			si.signalSemaphoreCount = 1;
			si.pSignalSemaphores 	= &vk.image_ownership_semaphores[vk.frame_index];

			{
				VkFence nullFence = VK_NULL_HANDLE;
				VkResult  err;
				err = vkQueueSubmit(vk.present_queue, 1, &si, nullFence);
				assert(!err);
			}
		
		}
	}
*/

	// If we are using separate queues we have to wait for image ownership,
	// otherwise wait for draw complete
	{
		VkPresentInfoKHR present = 
		{
			.sType 				= VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
			.pNext 				= NULL,
			.waitSemaphoreCount = 1,
/*
			.pWaitSemaphores 	= (vk.flg_separate_present_queue)
								   ? &vk.image_ownership_semaphores[vk.frame_index]
								   : &vk.draw_complete_semaphores[vk.frame_index],
*/
			.pWaitSemaphores 	=  &vk.draw_complete_semaphores[vk.frame_index],
			.swapchainCount 		= 1,
			.pSwapchains 		= &vk.swapchain,
			.pImageIndices 		= &vk.current_buffer,
		};


		//---------------------------------------------------------
		// 描画キック
		//---------------------------------------------------------
		{
			VkResult  err;
			err = vkQueuePresentKHR(vk.present_queue, &present);
			vk.frame_index += 1;
			vk.frame_index %= FRAME_LAG;

			if (err == VK_ERROR_OUT_OF_DATE_KHR) 
			{
				// vk.swapchain is out of date (e.g. the window was resized) and
				// must be recreated:
				//demo_resize(&vk);
			} else if (err == VK_SUBOPTIMAL_KHR) 
			{
				// vk.swapchain is not as optimal as it could be, but the platform's
				// presentation engine will still present the image correctly.
			} else 
			{
				assert(!err);
			}
		}
	}
}
//-----------------------------------------------------------------------------
void	vk2_release( VulkanInf& vk )
//-----------------------------------------------------------------------------
{
	//---------------------------------------------------------
	// 終了
	//---------------------------------------------------------
	uint32_t i;
	vkDeviceWaitIdle(vk.device);

	for (i = 0; i < vk.swapchainImageCount; i++) 
	{
		vkDestroyFramebuffer(vk.device, vk.sir_framebuffer[i], NULL);	//create5	*	vk2_create
	}

	vkDestroyPipeline(vk.device, vk.pipeline, NULL);					//create7	*	vk2_create
	vkDestroyPipelineCache(vk.device, vk.pipelineCache, NULL);			//create8	*	vk2_create
	vkDestroyRenderPass(vk.device, vk.render_pass, NULL);				//create9	*	vk2_create
	vkDestroyPipelineLayout(vk.device, vk.pipeline_layout, NULL)	;	//create10	*	vk2_create
	vkDestroyDescriptorSetLayout(vk.device, vk.desc_layout, NULL);		//create11	*	vk2_create

	for (i = 0; i < vk.swapchainImageCount; i++) 
	{
		vkFreeCommandBuffers(vk.device, vk.cmd_pool, 1, &vk.sir_cmdbuf[i]);	//create21	*	vk2_create
	}


	vkDestroyDescriptorPool(vk.device, vk.desc_pool, NULL);				//create6	*	vk2_create
/*
	if ( vk.sir_uniform_buffer )
	{
		for (int i = 0; i < vk.swapchainImageCount; i++) 
		{
			vkDestroyBuffer(vk.device, vk.sir_uniform_buffer[i], NULL);			//create22	*	vk3_create
		}
		free(vk.sir_uniform_buffer);
	}

	if ( vk.sir_uniform_memory )
	for (int i = 0; i < vk.swapchainImageCount; i++) 
	{

		vkFreeMemory(vk.device, vk.sir_uniform_memory[i], NULL);			//create23	*	vk3_create
		free(vk.sir_uniform_memory);
	}

	if ( vk.sir_descriptor_set )
	{
		free(vk.sir_descriptor_set);
	}
*/
}
