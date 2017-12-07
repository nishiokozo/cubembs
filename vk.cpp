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


static PFN_vkGetDeviceProcAddr g_gdpa = NULL;


VKAPI_ATTR VkBool32 VKAPI_CALL BreakCallback(VkFlags msgFlags, VkDebugReportObjectTypeEXT objType, uint64_t srcObject,
											 size_t location, int32_t msgCode, const char *pLayerPrefix, const char *pMsg,
											 void *pUserData) 
											 {
//#ifndef WIN32
//	raise(SIGTRAP);
//#else
	DebugBreak();
//#endif

	return false;
}


//-----------------------------------------------------------------------------
bool loadTexture(
//-----------------------------------------------------------------------------
	const char *filename, uint8_t *rgba_data,
	VkSubresourceLayout *layout, int32_t *width, int32_t *height) 

{

	FILE *fPtr = fopen(filename, "rb");
	char header[256], *cPtr, *tmp;

	if (!fPtr)
		return false;

	cPtr = fgets(header, 256, fPtr); // P6
	if (cPtr == NULL || strncmp(header, "P6\n", 3)) 
	{
		fclose(fPtr);
		return false;
	}

	do 
	{
		cPtr = fgets(header, 256, fPtr);
		if (cPtr == NULL) 
		{
			fclose(fPtr);
			return false;
		}
	} while (!strncmp(header, "#", 1));

	sscanf(header, "%u %u", width, height);
	if (rgba_data == NULL) 
	{
		fclose(fPtr);
		return true;
	}
	tmp = fgets(header, 256, fPtr); // Format
	(void)tmp;
	if (cPtr == NULL || strncmp(header, "255\n", 3)) 
	{
		fclose(fPtr);
		return false;
	}

	for (int y = 0; y < *height; y++) 
	{
		uint8_t *rowPtr = rgba_data;
		for (int x = 0; x < *width; x++) 
		{
			size_t s = fread(rowPtr, 3, 1, fPtr);
			(void)s;
			rowPtr[3] = 255; /* Alpha of 1 */
			rowPtr += 4;
		}
		rgba_data += layout->rowPitch;
	}
	fclose(fPtr);
	return true;
}
//-----------------------------------------------------------------------------
void demo_prepare_texture_image(
//-----------------------------------------------------------------------------
	VkDevice 							device,
	VkPhysicalDeviceMemoryProperties* 	pMemory_properties,
	const char *						filename,
	struct texture_object *				tex_obj,
	VkImageTiling			 			tiling,
	VkImageUsageFlags					usage,
	VkFlags				 				required_props
) 
{
	//-----------------------------------------------------
	// 
	//-----------------------------------------------------
	int32_t tex_width;
	int32_t tex_height;
	{

		if (!loadTexture(filename, NULL, NULL, &tex_width, &tex_height)) 
		{
			ASSERTW(false,"Failed to load textures Load Texture Failure");
		}

		tex_obj->tex_width = tex_width;
		tex_obj->tex_height = tex_height;
	}

	//-----------------------------------------------------
	// イメージの作成
	//-----------------------------------------------------
	{
		const VkFormat tex_format = VK_FORMAT_R8G8B8A8_UNORM;
		const VkImageCreateInfo ici = 
		{
			.sType 					= VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
			.pNext 					= NULL,
			.flags 					= 0,
			.imageType 				= VK_IMAGE_TYPE_2D,
			.format 				= tex_format,
			.extent 				= 
			{ 
				  tex_width
				, tex_height
				, 1
			},
			.mipLevels 				= 1,
			.arrayLayers 			= 1,
			.samples 				= VK_SAMPLE_COUNT_1_BIT,
			.tiling 				= tiling,
			.usage 					= usage,
			.sharingMode			= VK_SHARING_MODE_EXCLUSIVE,
			.queueFamilyIndexCount	= 0,
			.pQueueFamilyIndices	= 0,
			.initialLayout 			= VK_IMAGE_LAYOUT_PREINITIALIZED,

		};
		VkResult  err;
		err = vkCreateImage(device, &ici, NULL, &tex_obj->image);		//create13	setup
		assert(!err);
	}

	//---------------------------------------------------------
	// 
	//---------------------------------------------------------
	{
		VkMemoryRequirements mr;
		{
			//---------------------------------------------------------
			// イメージメモリ要求の取得
			//---------------------------------------------------------
			vkGetImageMemoryRequirements(device, tex_obj->image, &mr);

			tex_obj->mem_alloc.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			tex_obj->mem_alloc.pNext = NULL;
			tex_obj->mem_alloc.allocationSize = mr.size;
			tex_obj->mem_alloc.memoryTypeIndex = 0;

			//-----------------------------------------------------
			// プロパティーの取得
			//-----------------------------------------------------
			bool  pass = false;
			{
				uint32_t 							typeBits			= mr.memoryTypeBits;
				VkFlags 							requirements_mask	= required_props;
				uint32_t*							typeIndex			= &tex_obj->mem_alloc.memoryTypeIndex;

				// Search memtypes to find first index with those properties
				for (uint32_t i = 0; i < VK_MAX_MEMORY_TYPES; i++) 
				{
					if ((typeBits & 1) == 1) 
					{
						// Type is available, does it match user properties?
						if ((pMemory_properties->memoryTypes[i].propertyFlags & requirements_mask) == requirements_mask) 
						{
							*typeIndex = i;
							pass = true;
							break;
						}
					}
					typeBits >>= 1;
				}
			}
			assert(pass);
		}
	}

	/* allocate memory */
	//---------------------------------------------------------
	// メモリの確保
	//---------------------------------------------------------
	{
		VkResult  err;
		err = vkAllocateMemory(device, &tex_obj->mem_alloc, NULL, &(tex_obj->devmem));	//create14	setup
		assert(!err);
	}

	/* bind memory */
	//---------------------------------------------------------
	// イメージメモリのバインド
	//---------------------------------------------------------
	{
		VkResult  err;
		err = vkBindImageMemory(device, tex_obj->image, tex_obj->devmem, 0);
		assert(!err);
	}

	//---------------------------------------------------------
	// 
	//---------------------------------------------------------
	if (required_props & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) 
	{
		//---------------------------------------------------------
		// イメージサブリソースレイアウトの取得
		//---------------------------------------------------------
		VkSubresourceLayout layout;
		{
			const VkImageSubresource subres = 
			{
				.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
				.mipLevel = 0,
				.arrayLayer = 0,
			};

			vkGetImageSubresourceLayout(device, tex_obj->image, &subres, &layout);

		}

		//---------------------------------------------------------
		// マップメモリ
		//---------------------------------------------------------
		{
			void *data;
			VkResult  err;
			err = vkMapMemory(device, tex_obj->devmem, 0, tex_obj->mem_alloc.allocationSize, 0, &data);
			assert(!err);

			//---------------------------------------------------------
			// テクスチャ転送
			//---------------------------------------------------------
			if ( !loadTexture( filename, (uint8_t*)data, &layout, &tex_width, &tex_height ) )
			{
				fprintf(stderr, "Error loading texture: %s\n", filename);
			}
		}

		//---------------------------------------------------------
		// マップ解除
		//---------------------------------------------------------
		{
			vkUnmapMemory(device, tex_obj->devmem);
		}
	}

	tex_obj->imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
}

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
//	if (!fp) return NULL;
	if (!fp) 
	{
		char	chr[256];
		sprintf( chr, "ファイルがありません [%s]",filename );
		ASSERTW(false, chr );
	}

	fseek(fp, 0L, SEEK_END);
	size = ftell(fp);

	fseek(fp, 0L, SEEK_SET);

	shader_code = malloc(size);						//createTmp
	retval = fread(shader_code, size, 1, fp);
	assert(retval == 1);

	*psize = size;

	fclose(fp);
	return (char*)shader_code;
}

//-----------------------------------------------------------------------------
void vk_vert_CreateBuffer(
//-----------------------------------------------------------------------------
	  const VkDevice							& 	vk_sc_device
//	, const VkPhysicalDeviceMemoryProperties	&	vk_pdmemory_properties
//	, const void* 									pDataVert
	, const int 									sizeofDataVert

	, VkBuffer 									&	sir_uniform_buffer
//	, VkDeviceMemory 							&	sir_uniform_memory
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
}
//-----------------------------------------------------------------------------
void vk_vert_AllocateMemory(
//-----------------------------------------------------------------------------
	  const VkDevice							& 	vk_sc_device
	, const VkPhysicalDeviceMemoryProperties	&	vk_pdmemory_properties
//	, const void* 									pDataVert
//	, const int 									sizeofDataVert
	, VkBuffer 									&	sir_uniform_buffer

	, VkDeviceMemory 							&	sir_uniform_memory
)
{

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
}
//-----------------------------------------------------------------------------
void vk_vert_MapMemory(
//-----------------------------------------------------------------------------
	  const VkDevice							& 	vk_sc_device 
//	, const VkPhysicalDeviceMemoryProperties	&	vk_pdmemory_properties
	, const void* 									pDataVert
	, const int 									sizeofDataVert
//	, VkBuffer 									&	sir_uniform_buffer
	, const VkDeviceMemory 						&	sir_uniform_memory
)
{


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
}
//-----------------------------------------------------------------------------
void vk_vert_BindBufferMemory(
//-----------------------------------------------------------------------------
	  const VkDevice							& 	vk_sc_device 
//	, const VkPhysicalDeviceMemoryProperties	&	vk_pdmemory_properties
//	, const void* 									pDataVert
//	, const int 									sizeofDataVert

	, VkBuffer 									&	sir_uniform_buffer
	, VkDeviceMemory 							&	sir_uniform_memory
)
{


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
static void	vkCreateDescriptorPool( 
//-----------------------------------------------------------------------------
	  const  VkDevice			&	vk_sc_device
	, const  int 					tex_count
	, const uint32_t 				sc_cntImage

	, VkDescriptorPool 			&	vk_desc_pool
)
{
	//---------------------
	// デスクリプタ・作成
	//---------------------
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

//-----------------------------------------------------------------------------
static void	vk_CreateDescriptorSetLayout( 
//-----------------------------------------------------------------------------
	  const VkDevice				&	vk_sc_device
	, const int 						tex_count

	, VkDescriptorSetLayout 		&	vk_desc_layout
)
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

//-----------------------------------------------------------------------------
static void	vk_CreatePipelineLayout( 
//-----------------------------------------------------------------------------
	  const VkDevice				&	vk_sc_device
	, const VkDescriptorSetLayout 	&	vk_desc_layout

	,  VkPipelineLayout				&	vk_pipeline_layout
)
{
	//---------------------
	// パイプラインレイアウト・作成
	//---------------------
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
static void	vk_CreateRenderPass( 
//-----------------------------------------------------------------------------
	  const VkDevice	& 	vk_sc_device
	, const VkFormat 	&	vk_surface_format
	, const VkFormat 		vk_depth_format

	, VkRenderPass 		&	vk_render_pass
)
{
	//---------------------
	// レンダーパスの作成
	//---------------------
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

//-----------------------------------------------------------------------------
static void	vk_CreatePipelineCache( 
//-----------------------------------------------------------------------------
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
}

//-----------------------------------------------------------------------------
static void	vk_CreateGraphicsPipelines( 
//-----------------------------------------------------------------------------
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
						ASSERTW(!err,"中断します v-shader ");
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
						ASSERTW(!err,"中断します f-shader");
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
			ASSERTW(!err,"中断します pipelines");
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
//-----------------------------------------------------------------------------
static void vk_AllocateCommandBuffers( 
//-----------------------------------------------------------------------------
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
		err = vkAllocateCommandBuffers(vk_device, &cmai, &sir_cmdbuf);	//upper
		assert(!err);
	}
}
//-----------------------------------------------------------------------------
static void	vk_AllocateDescriptorSets(
//-----------------------------------------------------------------------------
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
		err = vkAllocateDescriptorSets(vk_sc_device, &dsai, &descriptor_set); 	//create_AllocateDescriptorSet
		{
			char	chr[256];
			sprintf( chr, "中断します。 err=(%x)\n",err);
			ASSERTW(!err, chr);
		}
	}

}
//-----------------------------------------------------------------------------
static void	vk_UpdateDescriptorSets(
//-----------------------------------------------------------------------------
	  const VkDevice				&	vk_sc_device
	, const int 						tex_count
	, const texture_object* 			pTex
	, const int							sizeofStructDataVert
	, const VkBuffer 				&	uniform_buffer

	, VkDescriptorSet 				&	descriptor_set
)
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

//-----------------------------------------------------------------------------
static void	vk_createFramebuffer( 
//-----------------------------------------------------------------------------
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
static void	vk_CmdBeginRenderPass( 
//-----------------------------------------------------------------------------
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
	vkCmdBeginRenderPass(sc_cmdbuf, &rpbi, VK_SUBPASS_CONTENTS_INLINE);

}

//-----------------------------------------------------------------------------
static void	vk_BeginCommandBuffer( 
//-----------------------------------------------------------------------------
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
static void	vk_CmdSetViewport( 
//-----------------------------------------------------------------------------
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
static void	vk_CmdSetScissor( 
//-----------------------------------------------------------------------------
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
static void vk_cmdPipelineBarrier(
//-----------------------------------------------------------------------------
	  const VkCommandBuffer 					&	vk_vk_cmdbuf
	, const texture_object				& 	vktex
)
{
	//---------------------
	// テクスチャイメージ・コマンド・パイプラインバリアの作成
	//---------------------
	VkImage 				image				= vktex.image;
	VkImageAspectFlags 		aspectMask			= VK_IMAGE_ASPECT_COLOR_BIT;
	VkImageLayout 			old_image_layout	= VK_IMAGE_LAYOUT_PREINITIALIZED;
	VkImageLayout 			new_image_layout	= vktex.imageLayout;
	VkAccessFlags 			srcAccessMask		= VK_ACCESS_HOST_WRITE_BIT;
	VkPipelineStageFlags 	src_stages			= VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
	VkPipelineStageFlags 	dest_stages			= VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;

//		ASSERTW(vk_vk_cmdbuf,"中断します");

	VkImageMemoryBarrier imb = 
	{
		  .sType 				= VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER
		, .pNext 				= NULL
		, .srcAccessMask 		= srcAccessMask
		, .dstAccessMask 		= 0
		, .oldLayout 			= old_image_layout
		, .newLayout 			= new_image_layout
		, .srcQueueFamilyIndex	= 0
		, .dstQueueFamilyIndex	= 0
		, .image 				= image
		, .subresourceRange 	= {aspectMask, 0, 1, 0, 1}
	};

	switch (new_image_layout) 
	{
	case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:				imb.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;										break;
	case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:			imb.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;								break;
	case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:	imb.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT; 						break;
	case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:			imb.dstAccessMask = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_INPUT_ATTACHMENT_READ_BIT;	break;
	case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL: 				imb.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;										break;
	case VK_IMAGE_LAYOUT_PRESENT_SRC_KHR:					imb.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;											break;
	default:												imb.dstAccessMask = 0;																	break;
	}
	vkCmdPipelineBarrier(vk_vk_cmdbuf, src_stages, dest_stages, 0, 0, NULL, 0, NULL, 1, &imb);
}

//-----------------------------------------------------------------------------
void vk_CreateSampler(
//-----------------------------------------------------------------------------
	  const VkDevice				&	vk_sc_device
	, VkSampler				& 	sampler
)
{
	VkSamplerCreateInfo sci = 
	{
		  .sType					= VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO
		, .pNext 					= NULL
		, .flags 					= 0
		, .magFilter 				= VK_FILTER_NEAREST
		, .minFilter 				= VK_FILTER_NEAREST
		, .mipmapMode 				= VK_SAMPLER_MIPMAP_MODE_NEAREST
		, .addressModeU 			= VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE
		, .addressModeV				= VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE
		, .addressModeW				= VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE
		, .mipLodBias 				= 0.0f
		, .anisotropyEnable 		= VK_FALSE
		, .maxAnisotropy 			= 1
		, .compareEnable 			= VK_FALSE
		, .compareOp 				= VK_COMPARE_OP_NEVER
		, .minLod 					= 0.0f
		, .maxLod 					= 0.0f
		, .borderColor 				= VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE
		, .unnormalizedCoordinates	= VK_FALSE
	};

	VkResult  err;
	err = vkCreateSampler(vk_sc_device, &sci, NULL, &sampler);
	ASSERTW(!err,"中断します");

}
//-----------------------------------------------------------------------------
void vk_tex_CreateImageView(
//-----------------------------------------------------------------------------
	  const VkDevice			&	vk_sc_device
	, const VkFormat			&	tex_format
	, const VkImage				&	tx_image
	, VkImageView				& 	tx_imgview
)
{	//---------------------
	// テクスチャイメージ・ビューの作成
	//---------------------
	VkImageViewCreateInfo ivc = 
	{
		  .sType 				= VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO
		, .pNext 				= NULL
		, .flags 				= 0
		, .image 				= tx_image
		, .viewType 			= VK_IMAGE_VIEW_TYPE_2D
		, .format 				= tex_format
		, .components 			=
			{
				VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G,
				VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A
			}
		, .subresourceRange 	= {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1}
	};
	VkResult  err;
	err = vkCreateImageView(vk_sc_device, &ivc, NULL, &tx_imgview);
	ASSERTW(!err,"中断します");
}

//-----------------------------------------------------------------------------
void vk2_loadTexture( VulkanInf& vk
//-----------------------------------------------------------------------------
	, const char** tex_files
	, const int tex_cnt 
)
{
	
	//---------------------------------------------------------
	// コマンドバッファの確保
	//---------------------------------------------------------
	vk_AllocateCommandBuffers(	//createNN_AllocateCommandBuffers
		  vk.device
		, vk.cmd_pool

		, vk.nor_cmdbuf
	);

	//---------------------------------------------------------
	// コマンドバッファの開始
	//---------------------------------------------------------
	{
		VkCommandBufferBeginInfo cb = 
		{
			.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
			.pNext = NULL,
			.flags = 0,
			.pInheritanceInfo = NULL,
		};
		VkResult  err;
		err = vkBeginCommandBuffer(vk.nor_cmdbuf, &cb);
		assert(!err);
	}

	{
		const VkFormat tex_format = VK_FORMAT_R8G8B8A8_UNORM;
		VkFormatProperties props;
		uint32_t i;

		vkGetPhysicalDeviceFormatProperties(vk.gpu, tex_format, &props);

//			for (i = 0; i < tex_cnt; i++) 
		{

			if ((props.linearTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT) ) 
			{
			}
			else 
			{
				// Can't support VK_FORMAT_R8G8B8A8_UNORM !? 
				assert(!"No support for R8G8B8A8_UNORM as texture image format");
			}

		for (i = 0; i < tex_cnt; i++) 
		{
//printf("1 tex %d: %x %x\n", i, (int)props.linearTilingFeatures, (int)VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT);
				/* Device can texture using linear textures */
				demo_prepare_texture_image(
					  vk.device
					, &vk.memory_properties
					, tex_files[i]
					, &vk.textures[i]
					, VK_IMAGE_TILING_LINEAR
					, VK_IMAGE_USAGE_SAMPLED_BIT
					, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
				);
		}
		for (i = 0; i < tex_cnt; i++) 
		{
			vk_cmdPipelineBarrier(
				  vk.nor_cmdbuf
				, vk.textures[i]
			);
			} 
		}
		for (i = 0; i < tex_cnt; i++) 
		{

			//-----------------------------------------------------
			// サンプラーの作成
			//-----------------------------------------------------
			vk_CreateSampler(
				  vk.device
				, vk.textures[i].sampler
			);
		}
		for (i = 0; i < tex_cnt; i++) 
		{
			vk_tex_CreateImageView(
				  vk.device
				, tex_format
				, vk.textures[i].image

				, vk.textures[i].imgview
			);
			
		}
	}

	//-----------------------------------------------------
	// コマンドバッファ終了
	//-----------------------------------------------------
	{
		VkResult  err;
		err = vkEndCommandBuffer(vk.nor_cmdbuf);
		assert(!err);
	}

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
		const VkCommandBuffer cmd_bufs[] = {vk.nor_cmdbuf};
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

		vkFreeCommandBuffers(vk.device, vk.cmd_pool, 1, cmd_bufs);	//createNN_AllocateCommandBuffers
	}

	//-----------------------------------------------------
	// フェンスの廃棄
	//-----------------------------------------------------
	vkDestroyFence(vk.device, fence, NULL);		//createTmp1


}

//-----------------------------------------------------------------------------
void vk2_loadModel( VulkanInf& vk
//-----------------------------------------------------------------------------
	,void* pDataVert
	,int sizeofStructDataVert

	, VkBuffer* 				&	sir_uniform_buffer
	, VkDeviceMemory* 			&	sir_uniform_memory
	, VkDescriptorSet* 			&	sir_descriptor_set

	, const char* fn_vert
	, const char* fn_frag
	, const char** tex_files
	, const int		tex_cnt
)
{
}

//-----------------------------------------------------------------------------
void	vk2_removeModel( VulkanInf& vk
//-----------------------------------------------------------------------------
	, VkBuffer* 				&	sir_uniform_buffer
	, VkDeviceMemory* 			&	sir_uniform_memory
	, VkDescriptorSet* 			&	sir_descriptor_set
)
{
}

//-----------------------------------------------------------------------------
static void	vk_AcquireNextImage(
//-----------------------------------------------------------------------------
	  const VkDevice				&	vk_sc_device
	, const VkSwapchainKHR			&	vk_sc_base
	, const VkSemaphore 			&	vk_s_image_acquired_semaphores	//[FRAME_LAG]

	, uint32_t 						&	vk_current_buffer
)
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

//-----------------------------------------------------------------------------
void	vk2_updateBegin( VulkanInf& vk
//-----------------------------------------------------------------------------
	, const int 		_width
	, const int 		_height
)
{
	//-----------------------------------------------------
	// Ensure no more than FRAME_LAG renderings are outstanding
	//-----------------------------------------------------
	vkWaitForFences(vk.device, 1, &vk.fences[vk.frame_index], VK_TRUE, UINT64_MAX);
	vkResetFences(vk.device, 1, &vk.fences[vk.frame_index]);

	//-----------------------------------------------------
	// 次のイメージと入れ替え
	//-----------------------------------------------------
	vk_AcquireNextImage(
		  vk.device
		, vk.swapchain
		, vk.image_acquired_semaphores[vk.frame_index]

		, vk.current_buffer
	);

	//-----------------------------------------------------
	// コマンドバッファの開始
	//-----------------------------------------------------
	vk_BeginCommandBuffer(
		  vk.sir_cmdbuf[vk.current_buffer]
	);

	//-----------------------------------------------------
	// コマンド・レンダーパスの開始
	//-----------------------------------------------------
	vk_CmdBeginRenderPass(
		  vk.render_pass
		, vk.sir_framebuffer[vk.current_buffer]
		, vk.sir_cmdbuf[vk.current_buffer]
		, _width
		, _height
	);
	
	//-----------------------------------------------------
	// コマンド・パイプラインのバインド
	//-----------------------------------------------------
	vk_CmdBindPipeline(
		  vk.pipeline
		, vk.sir_cmdbuf[vk.current_buffer]
	);

	//-----------------------------------------------------
	// ビューポートの設定
	//-----------------------------------------------------
	vk_CmdSetViewport(
		  vk.sir_cmdbuf[vk.current_buffer]
		, _width
		, _height
	);

	//-----------------------------------------------------
	// シザリングエリアの設定
	//-----------------------------------------------------
	vk_CmdSetScissor(
		  vk.sir_cmdbuf[vk.current_buffer]
		, _width
		, _height
	);
}

//-----------------------------------------------------------------------------
void	vk2_drawPolygon( VulkanInf& vk
//-----------------------------------------------------------------------------
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
}
//-----------------------------------------------------------------------------
void	vk2_updateEnd( VulkanInf& vk)
//-----------------------------------------------------------------------------
{
	//-----------------------------------------------------
	// レンダーパス終了
	//-----------------------------------------------------
	// Note that ending the renderpass changes the image's layout from
	// COLOR_ATTACHMENT_OPTIMAL to PRESENT_SRC_KHR
	vkCmdEndRenderPass(vk.sir_cmdbuf[vk.current_buffer]);

	//-----------------------------------------------------
	// コマンドバッファ終了
	//-----------------------------------------------------
	vk_EndCommandBuffer(
		vk.sir_cmdbuf[vk.current_buffer]
	);


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


	// If we are using separate queues we have to wait for image ownership,
	// otherwise wait for draw complete
	//---------------------------------------------------------
	// 描画キック
	//---------------------------------------------------------
	{
		VkPresentInfoKHR present = 
		{
			.sType 				= VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
			.pNext 				= NULL,
			.waitSemaphoreCount = 1,
			.pWaitSemaphores 	=  &vk.draw_complete_semaphores[vk.frame_index],
			.swapchainCount 		= 1,
			.pSwapchains 		= &vk.swapchain,
			.pImageIndices 		= &vk.current_buffer,
		};


		{
			VkResult  err;
//			err = vkQueuePresentKHR(vk.present_queue, &present);
			err = vkQueuePresentKHR(vk.graphics_queue, &present);
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
static void	vk_createInstance( VkInstance& vk_inst)
//-----------------------------------------------------------------------------
{
	uint32_t 		cntName = 0;
	const char*		tblName[64];

	int vk_enabled_layer_count = 0; // レイヤー数はゼロ

	//-------------------------
	// インスタンス名の取得
	//-------------------------
	memset(tblName, 0, sizeof(char*)*64);
	{
		VkBool32 flgFound_surface = 0;
		VkBool32 flgFound_win32_surface = 0;
		uint32_t cntEp = 0;
		//-------------------------
		// インスタンス・数の取得
		//-------------------------
		{
			VkResult err;
			err = vkEnumerateInstanceExtensionProperties( NULL, &cntEp, NULL);
			ASSERTW(!err,"中断します");
		}

		ASSERTW( (cntEp > 0), "Vulkanインスタンスが取得できませんでしたれました。" );

		{
			VkExtensionProperties* tblEp = (VkExtensionProperties*)malloc(sizeof(VkExtensionProperties) * cntEp); 	//createTmp

			//-------------------------
			// インスタンス・列挙選択
			//-------------------------
			{
				VkResult err;
				err = vkEnumerateInstanceExtensionProperties( NULL, &cntEp, tblEp);
				ASSERTW(!err,"中断します");
			}
			for (uint32_t i = 0; i < cntEp; i++) 
			{
				if (!strcmp(VK_KHR_SURFACE_EXTENSION_NAME, tblEp[i].extensionName)) 
				{
					flgFound_surface = 1;
					tblName[cntName++] = VK_KHR_SURFACE_EXTENSION_NAME;
				}
				if (!strcmp(VK_KHR_WIN32_SURFACE_EXTENSION_NAME, tblEp[i].extensionName)) 
				{
					flgFound_win32_surface = 1;
					tblName[cntName++] = VK_KHR_WIN32_SURFACE_EXTENSION_NAME;
				}

				ASSERTW(cntName < 64,"中断します");
			}

			free(tblEp);	 	//createTmp
		}

		if (!flgFound_surface) 
		{
			ASSERTW(false,
			 "vkEnumerateInstanceExtensionPropertiesは、" VK_KHR_SURFACE_EXTENSION_NAME "拡張を見つけることができませんでした。"
			  "互換性のあるVulkanドライバ(ICD)をインストールしていますか?詳細については、『スタートアップガイド』を参照してください。"
			);
		}
		if (!flgFound_win32_surface) 
		{
			ASSERTW(false,
			 "vkEnumerateInstanceExtensionPropertiesは、" VK_KHR_WIN32_SURFACE_EXTENSION_NAME "拡張を見つけることができませんでした。"
			  "互換性のあるVulkanドライバ(ICD)をインストールしていますか?詳細については、『スタートアップガイド』を参照してください。"
			);
		}
	}

	//-------------------------
	// インスタンスの作成
	//-------------------------
	{
		const VkApplicationInfo app = 
		{
			.sType 					= VK_STRUCTURE_TYPE_APPLICATION_INFO,
			.pNext 					= NULL,
			.pApplicationName 		= "cube",
			.applicationVersion 	= 0,
			.pEngineName 			= "cube",
			.engineVersion 			= 0,
			.apiVersion 			= VK_API_VERSION_1_0
		};

		char **	names = NULL;
		VkInstanceCreateInfo inst_info = 
		{
			  .sType 					= VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO
			, .pNext 					= NULL
			, .flags					= 0
			, .pApplicationInfo 		= &app
			, .enabledLayerCount 		= vk_enabled_layer_count
			, .ppEnabledLayerNames 		= (const char *const *)names
			, .enabledExtensionCount 	= cntName
			, .ppEnabledExtensionNames 	= (const char *const *)tblName
		};

		VkResult err;
		err = vkCreateInstance(&inst_info, NULL, &vk_inst);
		if (err == VK_ERROR_INCOMPATIBLE_DRIVER) 
		{
			ASSERTW(false,
				"互換性のあるVulkanインストール可能クライアントドライバ(ICD)が見つかりません。"
				"詳細については、『スタートアップガイド』を参照してください。"
			);
		} 
		else 
		if (err == VK_ERROR_EXTENSION_NOT_PRESENT) 
		{
			ASSERTW(false,
				"指定された拡張ライブラリが見つかりません。"
				"レイヤパスが適切に設定されていることを確認します。"
			);
		} 
		else 
		if (err) 
		{
			ASSERTW(false,
				"vkCreateInstanceに失敗しました。"
				"互換性のあるVulkanドライバ(ICD)をインストールしていますか?"
				"詳細については、『スタートアップガイド』を参照してください。"
			);
		}
	}
}

//-----------------------------------------------------------------------------
static void	vk_enumeratePhysicalDevices( 
//-----------------------------------------------------------------------------
	  const VkInstance& 	vk_inst

	, VkPhysicalDevice& 	vk_gpuDevice 
)
{

	int				num = 0;	// 最初のGPU
	uint32_t 		cntPd = 0;

	//-------------------------
	// GPU数を取得
	//-------------------------
	{
		VkResult err;
		err = vkEnumeratePhysicalDevices(vk_inst, &cntPd, NULL);
		ASSERTW(!err && cntPd > 0,"中断します");
	}

	ASSERTW( (cntPd>num), "GPUデバイスが取得できませんでしたれました。" );


	//-------------------------
	// GPUデバイスを列挙
	//-------------------------
	{
		VkPhysicalDevice* 	pd = (VkPhysicalDevice*)malloc(sizeof(VkPhysicalDevice) * cntPd); 	//createTmp
		{
			VkResult err;
			err = vkEnumeratePhysicalDevices(vk_inst, &cntPd, pd);
			ASSERTW(!err,"中断します");
		}
		vk_gpuDevice = pd[num];
		free(pd); 	//createTmp
	}


}

//-----------------------------------------------------------------------------
void vk_CreateWin32Surface(
//-----------------------------------------------------------------------------
	  const HINSTANCE 				hInstance
	, const HWND 					hWin
	, const VkInstance			&	vk_inst

	, VkSurfaceKHR				& 	vk_surface
){
	//---------------------------------------------------------
	// windowsハンドルから、vkサーフェスを取得する
	//---------------------------------------------------------
	VkWin32SurfaceCreateInfoKHR createInfo;

	createInfo.sType 		= VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	createInfo.pNext 		= NULL;
	createInfo.flags 		= 0;
	createInfo.hinstance 	= hInstance;
	createInfo.hwnd 		= hWin;

	VkResult  err;
	err = vkCreateWin32SurfaceKHR(vk_inst, &createInfo, NULL, &vk_surface);		//create30s
	assert(!err);
}

//-----------------------------------------------------------------------------
static void vk_get_graphics_queue_family_index( 
//-----------------------------------------------------------------------------
	  const VkPhysicalDevice& 			vk_gpuDevice
	, const VkSurfaceKHR& 				vk_surface_base 

	, uint32_t&							vk_q_idxQueue_graphics
)
{
	VkQueueFamilyProperties*			tblQf;
	uint32_t 							cntQf;


	//-------------------------
	// キューファミリー・数取得
	//-------------------------
	{
		vkGetPhysicalDeviceQueueFamilyProperties(vk_gpuDevice, &cntQf, NULL);
		ASSERTW(cntQf >= 1,"中断します");
	}

	//-------------------------
	// キューファミリー・確保
	//-------------------------
	tblQf = (VkQueueFamilyProperties *)malloc( cntQf * sizeof(VkQueueFamilyProperties)); 	//createTmp

	//-------------------------
	// キューファミリー・列挙
	//-------------------------
	vkGetPhysicalDeviceQueueFamilyProperties( vk_gpuDevice, &cntQf, tblQf);

	//-------------------------
	// キュー・サポートフラグ列挙
	//-------------------------
	VkBool32 *qflg = (VkBool32 *)malloc(cntQf * sizeof(VkBool32)); 	//createTmp
	for (uint32_t i = 0; i < cntQf; i++) 
	{
		vkGetPhysicalDeviceSurfaceSupportKHR(vk_gpuDevice, i, vk_surface_base, &qflg[i]);
	}
	//-------------------------
	// キュー・検索・選択
	//-------------------------
	{
		uint32_t gq = UINT32_MAX;
		{
			//キューファミリの配列内のグラフィックスと描画のキューを検索し、両方をサポートするものを検索します。
			bool flgFound = false;
			for (uint32_t i = 0; i < cntQf; i++) 
			{

				// 1 VK_QUEUE_GRAPHICS_BIT 			キューファミリのキューがグラフィックス操作をサポートすることを示します。
				// 2 VK_QUEUE_COMPUTE_BIT 			キューファミリのキューが計算操作をサポートすることを示します。
				// 4 VK_QUEUE_TRANSFER_BIT 			キューファミリのキューが転送操作をサポートすることを示します。
				// 8 VK_QUEUE_SPARSE_BINDING_BIT 	キューファミリのキューが疎メモリ管理をサポートすることを示します。
				if ((tblQf[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0) 
				{
					if (qflg[i] == VK_TRUE) 
					{
						flgFound = true;
						gq = i;
						break;
					}
				}
			}
			if (flgFound == false) 
			{
				ASSERTW(false, "graphicsとpresentの両方をサポートするキューが見つかりませんでした。" );
			}
		}
		vk_q_idxQueue_graphics = gq;
	}

	free(qflg); 	//createTmp

	free( tblQf ); 	//createTmp
}

//-----------------------------------------------------------------------------
static void vk_CreateDevice( 
//-----------------------------------------------------------------------------
	  const VkPhysicalDevice& 	vk_gpuDevice
	, const uint32_t& 			vk_q_idxQueue_graphics

	, VkDevice& 				vk_sc_device 
)
{
	uint32_t 							cntName = 0;
	const char*							tblName[64];
	//-------------------------
	// 列挙デバイス情報初期化
	//-------------------------
	{
		memset(tblName, 0, 64*sizeof(char*));
	}

	//-------------------------
	// 列挙デバイス情報取得
	//-------------------------
	{
		uint32_t cntDevice = 0;
		{
			VkResult err;
			err = vkEnumerateDeviceExtensionProperties(vk_gpuDevice, NULL, &cntDevice, NULL);
			ASSERTW(!err, "中断します");
		}

		{
			bool flgFound = false;
			if (cntDevice > 0) 
			{
				//-------------------------
				// 列挙デバイス・バッファ確保
				//-------------------------
				VkExtensionProperties* tblDevice = (VkExtensionProperties*)malloc(sizeof(VkExtensionProperties) * cntDevice); 	//createTmp

				//-------------------------
				// 列挙デバイス・名取得
				//-------------------------
				{
					VkResult err;
					err = vkEnumerateDeviceExtensionProperties( vk_gpuDevice, NULL, &cntDevice, tblDevice);
					ASSERTW(!err,"中断します");
				}

				//-------------------------
				// 列挙デバイス・検索：スワップチェイン
				//-------------------------
				for (uint32_t i = 0; i < cntDevice; i++) 
				{
printf("extDevice %d %s\n", i , tblDevice[i].extensionName );
					if (!strcmp(VK_KHR_SWAPCHAIN_EXTENSION_NAME, tblDevice[i].extensionName)) 
					{
						flgFound = true;
						tblName[cntName++] = (char*)VK_KHR_SWAPCHAIN_EXTENSION_NAME;
					}
					ASSERTW(cntName < 64,"中断します");
				}

				free(tblDevice); 	//createTmp
			}
			if ( flgFound == false ) 
			{
				ASSERTW(false, "vkEnumerateDeviceExtensionPropertiesが" VK_KHR_SWAPCHAIN_EXTENSION_NAME "拡張を見つけられませんでした。" );
			}
		}
printf("cntDevice %d\n", (int)cntDevice );
printf("cntName %d\n", cntName );
	}

	//-------------------------
	// デバイスの作成：スワップチェイン
	//-------------------------
	{
		VkDeviceQueueCreateInfo dqci[1];
		memset(dqci,0,sizeof(dqci));
		float qp[1] 					= {0.0};
		{
			dqci[0].sType 				= VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			dqci[0].pNext 				= NULL;
			dqci[0].queueFamilyIndex 	= vk_q_idxQueue_graphics;
			dqci[0].queueCount 			= 1;
			dqci[0].pQueuePriorities 	= qp;
			dqci[0].flags 				= 0;
		}
		VkDeviceCreateInfo dci = 
		{
			  .sType 					= VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO
			, .pNext 					= NULL
			, .flags					= 0
			, .queueCreateInfoCount 	= 1
			, .pQueueCreateInfos 		= dqci
			, .enabledLayerCount		= 0
			, .ppEnabledLayerNames 		= NULL
			, .enabledExtensionCount 	= cntName
			, .ppEnabledExtensionNames 	= (const char *const *)tblName
			, .pEnabledFeatures 		= NULL			 // If specific features are required, pass them in here
		};

		VkResult  err;
		err = vkCreateDevice(vk_gpuDevice, &dci, NULL, &vk_sc_device);
		ASSERTW(!err,"中断します");
	}
}

//-----------------------------------------------------------------------------
static void vk_surface_get_format_color_space( 
//-----------------------------------------------------------------------------
	  const VkPhysicalDevice	& 	vk_gpuDevice
	, const VkSurfaceKHR		& 	vk_surface_base

	, VkFormat 					&	vk_surface_format
	, VkColorSpaceKHR 			&	vk_surface_colorspace
)
{

	//-------------------------
	// サーフェースフォーマット・数量取得
	//-------------------------
	uint32_t cntFormat;
	{
		VkResult  err;
		err = vkGetPhysicalDeviceSurfaceFormatsKHR(vk_gpuDevice, vk_surface_base, &cntFormat, NULL);
		ASSERTW(!err,"中断します");
	}

	//-------------------------
	// サーフェースフォーマット・バッファを確保
	//-------------------------
	VkSurfaceFormatKHR *tblFormat = (VkSurfaceFormatKHR *)malloc(cntFormat * sizeof(VkSurfaceFormatKHR)); 	//createTmp

	//-------------------------
	// サーフェースフォーマット・列挙
	//-------------------------
	{
		VkResult  err;
		err = vkGetPhysicalDeviceSurfaceFormatsKHR(vk_gpuDevice, vk_surface_base, &cntFormat, tblFormat);
		ASSERTW(!err,"中断します");
		ASSERTW(cntFormat >= 1,"中断します");
	}
	//-------------------------
	// サーフェースフォーマット・検索 
	//-------------------------
	{
		// フォーマットリストにVK_FORMAT_UNDEFINEDのエントリが1つだけ含まれている場合
		// サーフェスには優先フォーマットがありません。それ以外の場合は、少なくとも1つのサポートされているフォーマットが返されます。

    	// VK_FORMAT_B8G8R8A8_UNORM = 44,
		// VK_FORMAT_B8G8R8A8_SRGB = 50,
		if (cntFormat == 1 && tblFormat[0].format == VK_FORMAT_UNDEFINED) 
		{
			vk_surface_format = VK_FORMAT_B8G8R8A8_UNORM;
		} else 
		{
			vk_surface_format = tblFormat[0].format;
		}
	
		// VK_COLOR_SPACE_SRGB_NONLINEAR_KHR = 0,
		vk_surface_colorspace	= tblFormat[0].colorSpace;
	}
#if 1
	for ( int i = 0 ; i < cntFormat ; i++ )
	{
		printf("SurfaceFormats %d %d colorSpace %d\n",i,tblFormat[i].format,tblFormat[i].colorSpace  );

	}
#endif
	free( tblFormat ); 	//createTmp
}

//-----------------------------------------------------------------------------
static void vk_semaphore_CreateFence( 
//-----------------------------------------------------------------------------
	  const VkDevice& 			vk_sc_device 

	, VkFence 					vk_s_fences[FRAME_LAG]
)
{

	//-------------------------
	// Create fences that we can use to throttle if we get too far
	// ahead of the image presents
	//-------------------------
	for (uint32_t i = 0; i < FRAME_LAG; i++) 
	{
		VkFenceCreateInfo fci = 
		{
			.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
			.pNext = NULL,
			.flags = VK_FENCE_CREATE_SIGNALED_BIT
		};
		VkResult  err;
		err = vkCreateFence(vk_sc_device, &fci, NULL, &vk_s_fences[i]);
		ASSERTW(!err,"中断します");
	}
}
//-----------------------------------------------------------------------------
static void vk_CreateSemaphore_acquired( 
//-----------------------------------------------------------------------------
	  const VkDevice& 			vk_sc_device 

	, VkSemaphore 				vk_s_image_acquired_semaphores[FRAME_LAG]
)
{
	// Create semaphores to synchronize acquiring presentable buffers before
	// rendering and waiting for drawing to be complete before presenting
	//-------------------------
	// セマフォの作成：イメージアキュワイド
	//-------------------------
	for (uint32_t i = 0; i < FRAME_LAG; i++) 
	{
		VkSemaphoreCreateInfo sci = 
		{
			.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
			.pNext = NULL,
			.flags = 0,
		};

		VkResult  err;
		err = vkCreateSemaphore(vk_sc_device, &sci, NULL, &vk_s_image_acquired_semaphores[i]);
		ASSERTW(!err,"中断します");
	}
}
//-----------------------------------------------------------------------------
static void vk_CreateSemaphore_complete( 
//-----------------------------------------------------------------------------
	  const VkDevice& 			vk_sc_device 

	, VkSemaphore 				vk_s_draw_complete_semaphores[FRAME_LAG]
)
{

	//-------------------------
	// セマフォの作成：描画完了
	//-------------------------
	for (uint32_t i = 0; i < FRAME_LAG; i++) 
	{
		VkSemaphoreCreateInfo sci = 
		{
			.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
			.pNext = NULL,
			.flags = 0,
		};

		VkResult  err;
		err = vkCreateSemaphore(vk_sc_device, &sci, NULL, &vk_s_draw_complete_semaphores[i]);
		ASSERTW(!err,"中断します");
	}

}
//-----------------------------------------------------------------------------
static void vk_createCommandPool( 
//-----------------------------------------------------------------------------
	  const VkDevice& 			vk_sc_device 
	, const uint32_t 			&vk_q_idxQueue_graphics

	, VkCommandPool		 		&vk_cmd_pool
)
{

	//-------------------------
	// コマンドプールの作成
	//-------------------------
	VkCommandPoolCreateInfo cpci = 
	{
		.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
		.pNext = NULL,
		.flags = 0,
		.queueFamilyIndex = vk_q_idxQueue_graphics,
	};

	VkResult  err;
	err = vkCreateCommandPool(vk_sc_device, &cpci, NULL, &vk_cmd_pool);
	ASSERTW(!err,"中断します");

}
//-----------------------------------------------------------------------------
static void vk_CreateSwapchain(
//-----------------------------------------------------------------------------
	  const VkPhysicalDevice	& 	vk_gpuDevice
	, const VkSurfaceKHR		& 	vk_surface_base
	, const VkFormat			&	vk_surface_format
	, const VkColorSpaceKHR		&	vk_surface_colorspace
	, const VkDevice			& 	vk_sc_device 
	, const int 					_width
	, const int 					_height

	, VkSwapchainKHR			&	vk_sc_base
)
{
	//-------------------------
	// スワップチェイン・描画モード値
	//-------------------------
	VkPresentModeKHR pm = VK_PRESENT_MODE_FIFO_KHR;

	//-------------------------
	// スワップチェイン・描画モードの数の取得
	//-------------------------
	uint32_t pmc;
	{
		VkResult  err;
		err = vkGetPhysicalDeviceSurfacePresentModesKHR( vk_gpuDevice, vk_surface_base, &pmc, NULL);
		ASSERTW(!err,"中断します");
	}
	//-------------------------
	// スワップチェイン・描画モードバッファの確保
	//-------------------------
	VkPresentModeKHR *tblPm;
	{
		tblPm = (VkPresentModeKHR *)malloc(pmc * sizeof(VkPresentModeKHR)); 	//createTmp
		ASSERTW(tblPm,"中断します");

		VkResult  err;
		err = vkGetPhysicalDeviceSurfacePresentModesKHR( vk_gpuDevice, vk_surface_base, &pmc, tblPm);
		ASSERTW(!err,"中断します");
	}

	//-------------------------
	// スワップチェイン・描画モードを検索
	//-------------------------
	{
		bool	flgFound = false;
		for (size_t i = 0; i < pmc; ++i) 
		{
			if (tblPm[i] == pm) 
			{
				flgFound = true;
				break;
			}
		}
		if (flgFound == false) 
		{
			ASSERTW(false, "VK_PRESENT_MODE_FIFO_KHR:指定した現在のモードはサポートされていません" );
		}
	}

	//-------------------------
	// スワップチェイン・描画モードバッファの解放
	//-------------------------
	if (NULL != tblPm) 
	{
		free(tblPm); 	//createTmp
	}

	//-------------------------
	// スワップチェイン・サーフェス情報の取得
	//-------------------------
	VkSurfaceCapabilitiesKHR pdsc;
	{
		VkResult  err;
		err = vkGetPhysicalDeviceSurfaceCapabilitiesKHR( vk_gpuDevice, vk_surface_base, &pdsc);
		ASSERTW(!err,"中断します");
	}

	//-------------------------
	// スワップチェイン作成
	//-------------------------
	{
		uint32_t mic = 2; //ダブルバッファ
		if (mic > pdsc.minImageCount ) 
		{
			ASSERTW(false, "minImageCount:指定した現在のモードはサポートされていません" );
		}
		if (mic > pdsc.maxImageCount ) 
		{
	//				ASSERTW(false, "maxImageCount:指定した現在のモードはサポートされていません" );
		}

		VkSurfaceTransformFlagBitsKHR stfb = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
		if ( !(pdsc.supportedTransforms & stfb) ) 
		{
			ASSERTW(false, "VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR:指定した現在のモードはサポートされていません" );
		}

		VkCompositeAlphaFlagBitsKHR caf = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		if ( !(pdsc.supportedCompositeAlpha & caf) ) 
		{
			ASSERTW(false, "VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR:指定した現在のモードはサポートされていません" );
		}

		VkExtent2D ext;
		if (pdsc.currentExtent.width == 0xFFFFFFFF) 
		{
			ASSERTW(false, "currentExtent:指定した現在のモードはサポートされていません" );
		}

		VkSwapchainKHR oldSwapchain = vk_sc_base;

		{
			VkSwapchainCreateInfoKHR sci = 
			{
				  .sType 					= VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR
				, .pNext 					= NULL
				, .flags 					= 0
				, .surface 					= vk_surface_base
				, .minImageCount 			= mic
				, .imageFormat 				= vk_surface_format
				, .imageColorSpace 			= vk_surface_colorspace
				, .imageExtent 				= 
					{ 
					  _width
					, _height
					}
				, .imageArrayLayers 		= 1
				, .imageUsage 				= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
				, .imageSharingMode 		= VK_SHARING_MODE_EXCLUSIVE
				, .queueFamilyIndexCount 	= 0
				, .pQueueFamilyIndices 		= NULL
				, .preTransform 			= stfb
				, .compositeAlpha 			= caf
				, .presentMode 				= pm
				, .clipped 					= true
				, .oldSwapchain 			= oldSwapchain
			};

			VkResult  err;
printf("line %d\n",__LINE__ );//g++だとここでSegment Fault
			err = vkCreateSwapchainKHR(vk_sc_device, &sci, NULL, &vk_sc_base);
printf("line %d\n",__LINE__ );
			ASSERTW(!err,"中断します");
		}
		//-------------------------
		// 旧スワップチェインの廃棄
		//-------------------------
		{
			// 既存のvk_sc_baseを再作成したばかりの場合は、この時点で古いvk_sc_baseを破棄する必要があります。
			// 注：vk_sc_baseを破棄すると、プラットフォームが完了したら、それに関連するすべての表示可能なイメージも消去されます。
			if (oldSwapchain != VK_NULL_HANDLE) 
			{
				vkDestroySwapchainKHR(vk_sc_device, oldSwapchain, NULL);
			}
		}
	}
}

//-----------------------------------------------------------------------------
static void vk_depth_CreateImage0(
//-----------------------------------------------------------------------------
	  const VkDevice& 								vk_sc_device 
	, const int 									_width
	, const int 									_height
	, const VkFormat&								vk_depth_format

	, VkImage&										vk_depth_image
)
{	
	//---------------------
	// デプスバッファ・イメージ作成
	//---------------------
	VkImageCreateInfo ic = 
	{
		.sType 			= VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
		.pNext 			= NULL,
		.flags 			= 0,
		.imageType 		= VK_IMAGE_TYPE_2D,
		.format 		= vk_depth_format,
		.extent 		= {_width, _height, 1},
		.mipLevels 		= 1,
		.arrayLayers 	= 1,
		.samples 		= VK_SAMPLE_COUNT_1_BIT,
		.tiling 		= VK_IMAGE_TILING_OPTIMAL,
		.usage 			= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
	};

	VkResult  err;
	err = vkCreateImage(vk_sc_device, &ic, NULL, &vk_depth_image);
	ASSERTW(!err,"中断します");
}
//-----------------------------------------------------------------------------
static void vk_depth_AllocateMemory(
//-----------------------------------------------------------------------------
	  const VkDevice& 								vk_sc_device 
	, const VkPhysicalDeviceMemoryProperties	&	vk_pdmemory_properties
	, const VkImage		&							vk_depth_image

	, VkDeviceMemory		&						vk_depth_devmem
)
{	

	VkMemoryRequirements mr;
	vkGetImageMemoryRequirements(vk_sc_device, vk_depth_image, &mr);

	VkMemoryAllocateInfo mai;
	mai.sType 				= VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	mai.pNext 				= NULL;
	mai.allocationSize 		= mr.size;
	mai.memoryTypeIndex 	= 0;

	//---------------------
	// デプスバッファ・メモリ列挙判定
	//---------------------
	{
		bool  flgFound = false;
		{
			uint32_t 		typeBits			= mr.memoryTypeBits;
			VkFlags 		requirements_mask	= VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
			uint32_t*		typeIndex			= &mai.memoryTypeIndex;

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
		ASSERTW(flgFound,"VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT:未対応のフォーマットです");
	}

	//---------------------
	// デプスバッファ・メモリ確保
	//---------------------
	{ 
		VkResult  err;
		err = vkAllocateMemory(vk_sc_device, &mai, NULL, &vk_depth_devmem); 	//create19_AllocateMemory
		ASSERTW(!err,"中断します");
	}
}
//-----------------------------------------------------------------------------
static void vk_depth_BindImageMemory(
//-----------------------------------------------------------------------------
	  const VkDevice& 								vk_sc_device 
	, const VkImage		&							vk_depth_image
	, const VkDeviceMemory	&							vk_depth_devmem
)
{	

	//---------------------
	// デプスバッファ・イメージメモリのバインド
	//---------------------
	VkResult  err;
	err = vkBindImageMemory(
		 vk_sc_device
		, vk_depth_image
		, vk_depth_devmem
		, 0
	);
	ASSERTW(!err,"中断します");
}
//-----------------------------------------------------------------------------
static void vk_depth_CreateImageView(
//-----------------------------------------------------------------------------
	  const VkDevice	& 	vk_sc_device 
	, const VkFormat	&	vk_depth_format
	, const VkImage		&	vk_depth_image

	, VkImageView		&	vk_depth_imgview
)
{	

	//---------------------
	// デプスバッファ・イメージビューの作成
	//---------------------
	VkImageViewCreateInfo ivci = 
	{
		.sType 						= VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
		.pNext 						= NULL,
		.flags 						= 0,
		.image 						= vk_depth_image,
		.viewType 					= VK_IMAGE_VIEW_TYPE_2D,
		.format 					= vk_depth_format,
		.components 				= 
			{
				  VK_COMPONENT_SWIZZLE_R
				, VK_COMPONENT_SWIZZLE_G
				, VK_COMPONENT_SWIZZLE_B
				, VK_COMPONENT_SWIZZLE_A
			},
		.subresourceRange = 
			{
				  .aspectMask 		= VK_IMAGE_ASPECT_DEPTH_BIT
				, .baseMipLevel 	= 0
				, .levelCount 		= 1
				, .baseArrayLayer 	= 0
				, .layerCount 		= 1
			},
	};
	VkResult  err;
	err = vkCreateImageView(
		  vk_sc_device
		, &ivci
		, NULL
		, &vk_depth_imgview
	);
	ASSERTW(!err,"中断します");
}

//-----------------------------------------------------------------------------
static void vk_sir_CreateImageView(
//-----------------------------------------------------------------------------
	  const VkDevice			& 	vk_sc_device 
	, const VkFormat			&	vk_surface_format
	, const VkImage				&	sci

	, VkImageView 				&	sc_imgview
)
{
	//-------------------------
	// スワップチェインイメージ・ビューの作成
	//-------------------------
	VkImageViewCreateInfo ivci = 
	{
		.sType 			= VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
		.pNext 			= NULL,
		.flags 			= 0,
		.image 			= sci,
		.viewType 		= VK_IMAGE_VIEW_TYPE_2D,
		.format 		= vk_surface_format,
		.components 	=
		{
			.r = VK_COMPONENT_SWIZZLE_R,
			.g = VK_COMPONENT_SWIZZLE_G,
			.b = VK_COMPONENT_SWIZZLE_B,
			.a = VK_COMPONENT_SWIZZLE_A,
		},
		.subresourceRange = 
		{
			.aspectMask 	= VK_IMAGE_ASPECT_COLOR_BIT,
			.baseMipLevel 	= 0,
			.levelCount 	= 1,
			.baseArrayLayer = 0,
			.layerCount 	= 1
		},
	};

	{
		VkResult  err;
		err = vkCreateImageView(
			  vk_sc_device
			, &ivci
			, NULL
			, &sc_imgview
		);
		ASSERTW(!err,"中断します");
	}
}
//-----------------------------------------------------------------------------
void VkInf::loadModel(
//-----------------------------------------------------------------------------
	 void* pDataVert
	,int sizeofStructDataVert

//	, VkBuffer* 				&	sc_uniform_buffer
//	, VkDeviceMemory* 			&	sc_uniform_memory
//	, VkDescriptorSet* 			&	sc_descriptor_set
	, const char* fn_vert
	, const char* fn_frag
	, const char** 	tex_files
	, const int		tex_cnt

		, Vkunit	&	vkunit

)
{
	//-----------------------------------------------------
	// 
	//-----------------------------------------------------
	vk2_loadTexture( vk, tex_files, tex_cnt );


	//-----------------------------------------------------
	// 
	//-----------------------------------------------------
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
	// 
	//-----------------------------------------------------

	vkunit.uniform_buffer 	= (VkBuffer *)			malloc(sizeof(VkBuffer) 		* vk.swapchainImageCount);	//create37_malloc
	vkunit.uniform_memory 	= (VkDeviceMemory *)	malloc(sizeof(VkDeviceMemory) 	* vk.swapchainImageCount);	//create38_malloc
	vkunit.descriptor_set 	= (VkDescriptorSet *)	malloc(sizeof(VkDescriptorSet) 	* vk.swapchainImageCount);	//create39_malloc

	for ( int i = 0; i < vk.swapchainImageCount; i++) 
	{
		//-----------------------------------------------------
		// モデルデータの作成
		//-----------------------------------------------------
		vk_vert_CreateBuffer(
			  vk.device
			, sizeofStructDataVert

			, vkunit.uniform_buffer[i]
		);
		vk_vert_AllocateMemory(
			  vk.device
			, vk.memory_properties
			, vkunit.uniform_buffer[i]

			, vkunit.uniform_memory[i]
		);
		vk_vert_MapMemory(
			  vk.device
			, pDataVert
			, sizeofStructDataVert
			, vkunit.uniform_memory[i]
		);

		vk_vert_BindBufferMemory(
			  vk.device
			, vkunit.uniform_buffer[i]
			, vkunit.uniform_memory[i]
		);

		//-----------------------------------------------------
		// ディスクリプターの作成
		//-----------------------------------------------------
		vk_AllocateDescriptorSets(
			  vk.device
  			, vk.desc_layout
			, vk.desc_pool

			, vkunit.descriptor_set[i]
		);

		vk_UpdateDescriptorSets(
			  vk.device
			, DEMO_TEXTURE_COUNT
			, vk.textures 
			, sizeofStructDataVert
			, vkunit.uniform_buffer[i]
			, vkunit.descriptor_set[i]
		);
	}
}
//-----------------------------------------------------------------------------
void	VkInf::unloadModel(
//-----------------------------------------------------------------------------
		 Vkunit	&	vkunit
//	  VkBuffer* 				&	sir_uniform_buffer
//	, VkDeviceMemory* 			&	sir_uniform_memory
//	, VkDescriptorSet* 			&	sir_descriptor_set
)
{

	for (int i = 0; i < vk.swapchainImageCount; i++) 
	{
		vkDestroyBuffer(vk.device, vkunit.uniform_buffer[i], NULL);			//create22	*	vk3_create
		vkFreeMemory(vk.device, vkunit.uniform_memory[i], NULL);			//create23	*	vk3_create

		vkFreeDescriptorSets( 	//create_AllocateDescriptorSet
			  vk.device				//	VkDevice device
			, vk.desc_pool			//	VkDescriptorPool descriptorPool
			, 1						//	uint32_t descriptorSetCount
			, &vkunit.descriptor_set[i]	//	const VkDescriptorSet* pDescriptorSets
		);
	}


		free(vkunit.uniform_buffer); 	//create37_malloc
		free(vkunit.uniform_memory); 	//create38_malloc
		free(vkunit.descriptor_set); 	//create39_malloc
}
//-----------------------------------------------------------------------------
void	VkInf::drawModel(
//-----------------------------------------------------------------------------
	 const void* pMVP
	,int matrixSize
//	, VkDeviceMemory* 			&	sir_uniform_memory
	,int _vertexCount		//	= 12*3;
	,int _instanceCount		//	= 1;
	,int _firstVertex		//	= 0;
	,int _firstInstance		// = 0;
//	, VkDescriptorSet* 			&	sir_descriptor_set
	, Vkunit	&	vkunit

)
{
	//-----------------------------------------------------
	// コマンド・ディスクリプターのバインド
	//-----------------------------------------------------
	vk_CmdBindDescriptorSets(
		  vk.pipeline_layout
		, vkunit.descriptor_set[vk.current_buffer]
		, vk.sir_cmdbuf[vk.current_buffer]
	);

	//-----------------------------------------------------
	// 描画コマンド発行
	//-----------------------------------------------------
	vkCmdDraw(vk.sir_cmdbuf[vk.current_buffer], _vertexCount, _instanceCount, _firstVertex, _firstInstance);

	//---------------------------------------------------------
	// マトリクスのコピー
	//---------------------------------------------------------
	{
		uint8_t *pData;

		VkResult  err;
		err = vkMapMemory( 
			vk.device
			, vkunit.uniform_memory[vk.current_buffer]
			, 0
			, VK_WHOLE_SIZE
			, 0
			, (void **)&pData
		);
		assert(!err);

		memcpy(pData, pMVP, matrixSize);
	}

	//---------------------------------------------------------
	// マップ解除
	//---------------------------------------------------------
	vkUnmapMemory(vk.device, vkunit.uniform_memory[vk.current_buffer]);
}
//-----------------------------------------------------------------------------
void	VkInf::drawBegin( int _width, int _height )
//-----------------------------------------------------------------------------
{
	vk2_updateBegin( vk, _width, _height );
}
//-----------------------------------------------------------------------------
void	VkInf::drawEnd()
//-----------------------------------------------------------------------------
{
	vk2_updateEnd( vk );
}
//-----------------------------------------------------------------------------
VkInf::VkInf( HINSTANCE hInstance, HWND hWin, int _width, int _height, int unit_max, int tex_max )
//-----------------------------------------------------------------------------
{
	memset(&vk, 0, sizeof(vk));
	vk.presentMode = VK_PRESENT_MODE_FIFO_KHR;
	vk.enabled_extension_count = 0;
	vk.enabled_layer_count = 0;

	//---------------------
	// 拡張インスタンス情報の取得
	//---------------------
	vk_createInstance( vk.inst );

	//---------------------
	// GPUデバイスの取得
	//---------------------
	vk_enumeratePhysicalDevices( 
		  vk.inst

		, vk.gpu
	 );

	//---------------------
	// サーフェイスの取得
	//---------------------
	vk_CreateWin32Surface(
		  hInstance
		, hWin
		, vk.inst
	
	    , vk.surface
	);

	//---------------------
	// グラフィックキューインデックスの取得
	//---------------------
	vk_get_graphics_queue_family_index( 
		  vk.gpu
		, vk.surface 

		, vk.q_graphics_queue_family_index
	);

	//---------------------
	// デバイスの生成：スワップチェイン
	//---------------------
	vk_CreateDevice( 
		  vk.gpu
		, vk.q_graphics_queue_family_index

		, vk.device 
	);

	//---------------------
	// デバイスキュー取得
	//---------------------
	vkGetDeviceQueue(
		  vk.device
		, vk.q_graphics_queue_family_index
		, 0

		, &vk.graphics_queue
	);

	//---------------------
	// デバイスサーフェースフォーマットの取得
	//---------------------
	vk_surface_get_format_color_space( 
	      vk.gpu
	    , vk.surface
	
		, vk.format
		, vk.color_space
	);

	//---------------------
	// フェンスの作成
	//---------------------
	vk_semaphore_CreateFence( 
	      vk.device 

		, vk.fences
	);

	//---------------------
	// セマフォの作成 acwuired
	//---------------------
	vk_CreateSemaphore_acquired( 
	      vk.device 

		, vk.image_acquired_semaphores
	);

	//---------------------
	// セマフォの作成 complete
	//---------------------
	vk_CreateSemaphore_complete( 
	      vk.device 

		, vk.draw_complete_semaphores
	);

	//---------------------
	// コマンドプールの生成
	//---------------------
	vk_createCommandPool(
	     vk.device 
		,vk.q_graphics_queue_family_index

		,vk.cmd_pool
	);

	//---------------------------------------------------------
	// サーフェス情報の取得
	//---------------------------------------------------------
	VkSurfaceCapabilitiesKHR surfCapabilities;
	{
		VkResult  err;
		err = vkGetPhysicalDeviceSurfaceCapabilitiesKHR( 
			  vk.gpu
			, vk.surface

			, &surfCapabilities
		);
		assert(!err);
	}

	//---------------------------------------------------------
	// スワップチェインの切り替え
	//---------------------------------------------------------
	vk_CreateSwapchain(
		  vk.gpu
		, vk.surface
		, vk.format
		, vk.color_space
		, vk.device 
		, _width
		, _height

		, vk.swapchain
	);

	//---------------------------------------------------------
	// スワップチェイン・イメージ数の取得
	//---------------------------------------------------------
	{
		VkResult  err;
		err = vkGetSwapchainImagesKHR(
			  vk.device
			, vk.swapchain

			, &vk.swapchainImageCount
			, NULL
		);
		assert(!err);
	}

	//---------------------------------------------------------
	// スワップチェイン・イメージバッファの確保
	//---------------------------------------------------------
	VkImage *sci = (VkImage *)malloc(vk.swapchainImageCount * sizeof(VkImage)); 	//createTmp
	assert(sci);
	{
		VkResult  err;
		err = vkGetSwapchainImagesKHR(
			  vk.device
			, vk.swapchain

			, &vk.swapchainImageCount
			, sci
		);
		assert(!err);
	}

	//---------------------------------------------------------
	// イメージビューの作成
	//---------------------------------------------------------
//	vk.swapchain_image_resources = (SwapchainImageResources *)malloc(sizeof(SwapchainImageResources) * vk.swapchainImageCount);	//create24s
//	assert(vk.swapchain_image_resources);
	vk.sir_image						=(VkImage* 			)malloc(sizeof(VkImage			) * vk.swapchainImageCount);		//create32
	vk.sir_cmdbuf						=(VkCommandBuffer* 	)malloc(sizeof(VkCommandBuffer	) * vk.swapchainImageCount);		//create33
	vk.sir_graphics_to_present_cmdbuf	=(VkCommandBuffer* 	)malloc(sizeof(VkCommandBuffer	) * vk.swapchainImageCount);		//create34
	vk.sir_imgview						=(VkImageView* 		)malloc(sizeof(VkImageView		) * vk.swapchainImageCount);		//create35
//	vk.sir_uniform_buffer				=(VkBuffer* 		)malloc(sizeof(VkBuffer			) * vk.swapchainImageCount);
//	vk.sir_uniform_memory				=(VkDeviceMemory* 	)malloc(sizeof(VkDeviceMemory	) * vk.swapchainImageCount);
	vk.sir_framebuffer					=(VkFramebuffer* 	)malloc(sizeof(VkFramebuffer	) * vk.swapchainImageCount);		//create36
//	vk.sir_descriptor_set				=(VkDescriptorSet* 	)malloc(sizeof(VkDescriptorSet	) * vk.swapchainImageCount);

	{
		VkImage *sci = (VkImage *)malloc(vk.swapchainImageCount * sizeof(VkImage));
		ASSERTW(sci,"中断します");

		//-------------------------
		// スワップチェインイメージ・バッファの確保
		//-------------------------
		{
			VkResult  err;
			uint32_t a = vk.swapchainImageCount;
			err = vkGetSwapchainImagesKHR(
				  vk.device
				, vk.swapchain
				, &a
				, sci
			);
			ASSERTW(!err,"中断します");
		}

		//-------------------------
		// スワップチェインイメージ・バッファの確保
		//-------------------------
		for (uint32_t i = 0; i < vk.swapchainImageCount; i++) 
		{
			vk_sir_CreateImageView(
				  vk.device 
				, vk.format
				, sci[i]

				, vk.sir_imgview[i]
			);
		}
		free( sci ); 	//createTmp
	}

	//---------------------------------------------------------
	// 物理デバイスメモリプロパティを取得
	//---------------------------------------------------------
	vkGetPhysicalDeviceMemoryProperties(
		  vk.gpu

		, &vk.memory_properties
	);

	//---------------------
	// デプスイメージの取得
	//---------------------
	vk.depth_inf.format = VK_FORMAT_D16_UNORM;

	vk_depth_CreateImage0(
		  vk.device 
		, _width
		, _height
		, vk.depth_inf.format

		, vk.depth_inf.image
	);

	vk_depth_AllocateMemory(
		  vk.device 
		, vk.memory_properties
		, vk.depth_inf.image

		, vk.depth_inf.devmem
	);

	vk_depth_BindImageMemory(
		  vk.device 
		, vk.depth_inf.image
		, vk.depth_inf.devmem
	);

	vk_depth_CreateImageView(
		  vk.device 
		, vk.depth_inf.format
		, vk.depth_inf.image

		, vk.depth_inf.imgview
	);


	//---------------------
	// ディスクリプターセットレイアウト作成
	//---------------------
	vk_CreateDescriptorSetLayout(
		  vk.device
		, tex_max			//DEMO_TEXTURE_COUNT

		, vk.desc_layout	//create11
	);

	//-----------------------------------------------------
	// デスクリプタプールの作成
	//-----------------------------------------------------
	{
	vkCreateDescriptorPool(									
		  vk.device
		, tex_max		//DEMO_TEXTURE_COUNT
		, vk.swapchainImageCount * unit_max

		, vk.desc_pool	//create6
	);
	}

	//-----------------------------------------------------
	// パイプラインレイアウトの作成
	//-----------------------------------------------------
	vk_CreatePipelineLayout(
		  vk.device 
		, vk.desc_layout

		, vk.pipeline_layout	//create10
	);

	//-----------------------------------------------------
	// レンダーパスの作成
	//-----------------------------------------------------
	vk_CreateRenderPass(
		  vk.device 
		, vk.format
		, vk.depth_inf.format

		, vk.render_pass	//create9
	);


	//-----------------------------------------------------
	// パイプラインキャッシュの作成
	//-----------------------------------------------------
	vk_CreatePipelineCache(
		  vk.device 
		, vk.render_pass

		, vk.pipelineCache	//create8
	);

	//-----------------------------------------------------
	// コマンドバッファの作成
	//-----------------------------------------------------
	for (uint32_t i = 0; i < vk.swapchainImageCount; i++) 
	{
		vk_AllocateCommandBuffers(	//create21_AllocateCommandBuffers
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


	//-----------------------------------------------------
	// 
	//-----------------------------------------------------
	vk.current_buffer = 0;
	vk.frame_index = 0;

	printf("vk create \n");

}

//-----------------------------------------------------------------------------
VkInf::~VkInf()
//-----------------------------------------------------------------------------
{
	//---------------------------------------------------------
	// 終了
	//---------------------------------------------------------
	uint32_t i;
	vkDeviceWaitIdle(vk.device);

	for (i = 0; i < vk.swapchainImageCount; i++) 
	{
		vkDestroyFramebuffer(vk.device, vk.sir_framebuffer[i], NULL);	//create5	*	vk2 create
	}

	vkDestroyPipeline(vk.device, vk.pipeline, NULL);					//create7	*	vk2 create
	vkDestroyPipelineCache(vk.device, vk.pipelineCache, NULL);			//create8	*	vk2 create
	vkDestroyRenderPass(vk.device, vk.render_pass, NULL);				//create9	*	vk2 create
	vkDestroyPipelineLayout(vk.device, vk.pipeline_layout, NULL)	;	//create10	*	vk2 create
	vkDestroyDescriptorSetLayout(vk.device, vk.desc_layout, NULL);		//create11	*	vk2 create

	for (i = 0; i < vk.swapchainImageCount; i++) 
	{
		vkFreeCommandBuffers(vk.device, vk.cmd_pool, 1, &vk.sir_cmdbuf[i]);	//create21_AllocateCommandBuffers
	}


	vkDestroyDescriptorPool(vk.device, vk.desc_pool, NULL);				//create6	*	vk2 create

	// Wait for fences from present operations
	for (int i = 0; i < FRAME_LAG; i++) 
	{
		vkWaitForFences(vk.device, 1, &vk.fences[i], VK_TRUE, UINT64_MAX);
		vkDestroyFence(vk.device, vk.fences[i], NULL);										//create1	*	setup
		vkDestroySemaphore(vk.device, vk.image_acquired_semaphores[i], NULL)	;			//create2	*	setup
		vkDestroySemaphore(vk.device, vk.draw_complete_semaphores[i], NULL);				//create3	*	setup
	}

	for (int i = 0; i < DEMO_TEXTURE_COUNT; i++) 
	{
		vkDestroyImageView(vk.device, vk.textures[i].imgview, NULL);	//create12	*	setup
		vkDestroyImage(vk.device, vk.textures[i].image, NULL);			//create13	*	setup
		vkFreeMemory(vk.device, vk.textures[i].devmem, NULL);			//create14	*	setup
		vkDestroySampler(vk.device, vk.textures[i].sampler, NULL);		//create15	*	setup
	}
	vkDestroySwapchainKHR(vk.device, vk.swapchain, NULL);				//create16	*	setup

	vkDestroyImageView(vk.device, vk.depth_inf.imgview, NULL);			//create17	*	setup
	vkDestroyImage(vk.device, vk.depth_inf.image, NULL);				//create18	*	setup
	vkFreeMemory(vk.device, vk.depth_inf.devmem, NULL);					//create19_AllocateMemory

	for (int i = 0; i < vk.swapchainImageCount; i++) 
	{
		vkDestroyImageView(vk.device, vk.sir_imgview[i], NULL);				//create20	*	setup
	}

//	free(vk.swapchain_image_resources);									//create24	*	setup
	free(vk.sir_image);													//create32
	free(vk.sir_cmdbuf);												//create33
	free(vk.sir_graphics_to_present_cmdbuf);							//create34
	free(vk.sir_imgview);												//create35
//	free(vk.sir_uniform_buffer);
//	free(vk.sir_uniform_memory);
	free(vk.sir_framebuffer);											//create36	
//	free(vk.sir_descriptor_set);

	vkDestroyCommandPool(vk.device, vk.cmd_pool, NULL);					//create26	*	setup

	vkDeviceWaitIdle(vk.device);
	vkDestroyDevice(vk.device, NULL);									//create29	*	setup

	vkDestroySurfaceKHR(vk.inst, vk.surface, NULL);						//create30	*	setup

// init
	free(vk.queue_props);												//create25	*	init
	vkDestroyInstance(vk.inst, NULL);									//create31	*	init

	printf("vk released\n");

}



