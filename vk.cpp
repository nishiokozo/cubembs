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
static const char *tex_files[] = {"lunarg.ppm"};


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


// On MS-Windows, make this a global, so it's available to WndProc()



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
	VkDevice device,
	VkPhysicalDeviceMemoryProperties* pMemory_properties,
	const char *filename,
	struct texture_object *tex_obj,
	VkImageTiling tiling,
	VkImageUsageFlags usage,
	VkFlags required_props
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
			ERR_EXIT("Failed to load textures", "Load Texture Failure");
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
void	vk_init( VulkanInf& vk )
//-----------------------------------------------------------------------------
{
	memset(&vk, 0, sizeof(vk));
	vk.presentMode = VK_PRESENT_MODE_FIFO_KHR;
	vk.enabled_extension_count = 0;
	vk.enabled_layer_count = 0;

	//---------------------------------------------------------
	// 拡張インスタンス情報の取得
	//---------------------------------------------------------
	{
		VkBool32 surfaceExtFound = 0;
		VkBool32 platformSurfaceExtFound = 0;
		
		memset(vk.extension_names, 0, sizeof(vk.extension_names));

		{
			uint32_t instance_extension_count = 0;
			//---------------------------------------------------------
			// 
			//---------------------------------------------------------
			{
				VkResult err;
				err = vkEnumerateInstanceExtensionProperties( NULL, &instance_extension_count, NULL);
				assert(!err);
			}

			if (instance_extension_count > 0) 
			{
				VkExtensionProperties* instance_extensions = (VkExtensionProperties*)malloc(sizeof(VkExtensionProperties) * instance_extension_count);

				//---------------------------------------------------------
				// 
				//---------------------------------------------------------
				{
					VkResult err;
					err = vkEnumerateInstanceExtensionProperties( NULL, &instance_extension_count, instance_extensions);
					assert(!err);
				}
				for (uint32_t i = 0; i < instance_extension_count; i++) 
				{
					if (!strcmp(VK_KHR_SURFACE_EXTENSION_NAME, instance_extensions[i].extensionName)) 
					{
						surfaceExtFound = 1;
						vk.extension_names[vk.enabled_extension_count++] = VK_KHR_SURFACE_EXTENSION_NAME;
					}
					if (!strcmp(VK_KHR_WIN32_SURFACE_EXTENSION_NAME, instance_extensions[i].extensionName)) 
					{
						platformSurfaceExtFound = 1;
						vk.extension_names[vk.enabled_extension_count++] = VK_KHR_WIN32_SURFACE_EXTENSION_NAME;
					}

					assert(vk.enabled_extension_count < 64);
				}

				free(instance_extensions);
			}
		}

		if (!surfaceExtFound) 
		{
			ERR_EXIT("vkEnumerateInstanceExtensionProperties failed to find "
					 "the " VK_KHR_SURFACE_EXTENSION_NAME
					 " extension.\n\nDo you have a compatible "
					 "Vulkan installable client driver (ICD) installed?\nPlease "
					 "look at the Getting Started guide for additional "
					 "information.\n",
					 "vkCreateInstance Failure");
		}
		if (!platformSurfaceExtFound) 
		{
			ERR_EXIT("vkEnumerateInstanceExtensionProperties failed to find "
					 "the " VK_KHR_WIN32_SURFACE_EXTENSION_NAME
					 " extension.\n\nDo you have a compatible "
					 "Vulkan installable client driver (ICD) installed?\nPlease "
					 "look at the Getting Started guide for additional "
					 "information.\n",
					 "vkCreateInstance Failure");
		}
	}

	//---------------------------------------------------------
	// インスタンスの作成
	//---------------------------------------------------------
	{
		const VkApplicationInfo app = 
		{
			.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
			.pNext = NULL,
			.pApplicationName = APP_SHORT_NAME,
			.applicationVersion = 0,
			.pEngineName = APP_SHORT_NAME,
			.engineVersion = 0,
			.apiVersion = VK_API_VERSION_1_0,
		};
		{
			char **instance_validation_layers = NULL;
			VkInstanceCreateInfo inst_info = 
			{
				.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
				.pNext = NULL,
				.pApplicationInfo = &app,
				.enabledLayerCount = vk.enabled_layer_count,
				.ppEnabledLayerNames = (const char *const *)instance_validation_layers,
				.enabledExtensionCount = vk.enabled_extension_count,
				.ppEnabledExtensionNames = (const char *const *)vk.extension_names,
			};


			//---------------------------------------------------------
			// インスタンスの作成
			//---------------------------------------------------------
			{
				VkResult err;
				err = vkCreateInstance(&inst_info, NULL, &vk.inst);			//create31
				if (err == VK_ERROR_INCOMPATIBLE_DRIVER) 
				{
					ERR_EXIT("Cannot find a compatible Vulkan installable client driver "
							 "(ICD).\n\nPlease look at the Getting Started guide for "
							 "additional information.\n",
							 "vkCreateInstance Failure");
				} else if (err == VK_ERROR_EXTENSION_NOT_PRESENT) 
				{
					ERR_EXIT("Cannot find a specified extension library"
							 ".\nMake sure your layers path is set appropriately.\n",
							 "vkCreateInstance Failure");
				} else if (err) 
				{
					ERR_EXIT("vkCreateInstance failed.\n\nDo you have a compatible Vulkan "
							 "installable client driver (ICD) installed?\nPlease look at "
							 "the Getting Started guide for additional information.\n",
							 "vkCreateInstance Failure");
				}
			}
		}
	}

	//---------------------------------------------------------
	// GPU数を取得
	//---------------------------------------------------------
	uint32_t gpu_count;
	{
		VkResult err;
		err = vkEnumeratePhysicalDevices(vk.inst, &gpu_count, NULL);
		assert(!err && gpu_count > 0);
	}

	//---------------------------------------------------------
	// GPUデバイスを列挙
	//---------------------------------------------------------
	if (gpu_count > 0) 
	{
		VkPhysicalDevice*	physical_devices = (VkPhysicalDevice*)malloc(sizeof(VkPhysicalDevice) * gpu_count);
		{
			VkResult err;
			err = vkEnumeratePhysicalDevices(vk.inst, &gpu_count, physical_devices);
			assert(!err);
		}
		/* For cube &vk we just grab the first physical device */
		vk.gpu = physical_devices[0];
		free(physical_devices);
	} else 
	{
		ERR_EXIT("vkEnumeratePhysicalDevices reported zero accessible devices.\n\n"
				 "Do you have a compatible Vulkan installable client driver (ICD) "
				 "installed?\nPlease look at the Getting Started guide for "
				 "additional information.\n",
				 "vkEnumeratePhysicalDevices Failure");
	}

	//---------------------------------------------------------
	// 列挙デバイス情報初期化
	//---------------------------------------------------------
	{
		vk.enabled_extension_count = 0;
		memset(vk.extension_names, 0, sizeof(vk.extension_names));
	}

	//---------------------------------------------------------
	// 列挙デバイス情報取得
	//---------------------------------------------------------
	{
		uint32_t device_extension_count = 0;
		{
			VkResult err;
			err = vkEnumerateDeviceExtensionProperties(vk.gpu, NULL, &device_extension_count, NULL);
			assert(!err);
		}

		{
			VkBool32 swapchainExtFound = 0;
			if (device_extension_count > 0) 
			{
				VkExtensionProperties* device_extensions = (VkExtensionProperties*)malloc(sizeof(VkExtensionProperties) * device_extension_count);
				//---------------------------------------------------------
				// 列挙デバイス情報取得
				//---------------------------------------------------------
				{
					VkResult err;
					err = vkEnumerateDeviceExtensionProperties( vk.gpu, NULL, &device_extension_count, device_extensions);
					assert(!err);
				}

				for (uint32_t i = 0; i < device_extension_count; i++) 
				{
					if (!strcmp(VK_KHR_SWAPCHAIN_EXTENSION_NAME, device_extensions[i].extensionName)) 
					{
						swapchainExtFound = 1;
						vk.extension_names[vk.enabled_extension_count++] = (char*)VK_KHR_SWAPCHAIN_EXTENSION_NAME;
					}
					assert(vk.enabled_extension_count < 64);
				}

				free(device_extensions);
			}
			if (!swapchainExtFound) 
			{
				ERR_EXIT("vkEnumerateDeviceExtensionProperties failed to find "
						 "the " VK_KHR_SWAPCHAIN_EXTENSION_NAME
						 " extension.\n\nDo you have a compatible "
						 "Vulkan installable client driver (ICD) installed?\nPlease "
						 "look at the Getting Started guide for additional "
						 "information.\n",
						 "vkCreateInstance Failure");
			}
		}
	}



	//---------------------------------------------------------
	// GPUデバイス情報取得
	//---------------------------------------------------------
	vkGetPhysicalDeviceProperties(vk.gpu, &vk.gpu_props);

	//---------------------------------------------------------
	// デバイスキューファミリー情報取得
	//---------------------------------------------------------
	{
		vkGetPhysicalDeviceQueueFamilyProperties(vk.gpu,&vk.queue_family_count, NULL);
		assert(vk.queue_family_count >= 1);
	}

	//---------------------------------------------------------
	// キューファミリー情報バッファ確保
	//---------------------------------------------------------
	vk.queue_props = (VkQueueFamilyProperties *)malloc( vk.queue_family_count * sizeof(VkQueueFamilyProperties));	//create25

	//---------------------------------------------------------
	// キューファミリー情報取得
	//---------------------------------------------------------
	vkGetPhysicalDeviceQueueFamilyProperties( vk.gpu, &vk.queue_family_count, vk.queue_props);

	//---------------------------------------------------------
	// デバイス機能情報の取得
	//---------------------------------------------------------
	{
		// Query fine-grained feature support for this device.
		//  If app has specific feature requirements it should check supported
		//  features based on this query
		VkPhysicalDeviceFeatures physDevFeatures;
		vkGetPhysicalDeviceFeatures(vk.gpu, &physDevFeatures);
	}

/*
	//---------------------------------------------------------
	// 拡張関数取得
	//---------------------------------------------------------
	{
		vkGetPhysicalDeviceSurfaceSupportKHR = (PFN_vkGetPhysicalDeviceSurfaceSupportKHR)vkGetInstanceProcAddr(vk.inst, "vk" "GetPhysicalDeviceSurfaceSupportKHR");
		if (vkGetPhysicalDeviceSurfaceSupportKHR == NULL) 
		{
			ERR_EXIT("vkGetInstanceProcAddr failed to find vk" "GetPhysicalDeviceSurfaceSupportKHR", "vkGetInstanceProcAddr Failure");
		}

		vkGetPhysicalDeviceSurfaceCapabilitiesKHR = (PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR)vkGetInstanceProcAddr(vk.inst, "vk" "GetPhysicalDeviceSurfaceCapabilitiesKHR");
		if (vkGetPhysicalDeviceSurfaceCapabilitiesKHR == NULL) 
		{
			ERR_EXIT("vkGetInstanceProcAddr failed to find vk" "GetPhysicalDeviceSurfaceCapabilitiesKHR", "vkGetInstanceProcAddr Failure");
		}

		vkGetPhysicalDeviceSurfaceFormatsKHR = (PFN_vkGetPhysicalDeviceSurfaceFormatsKHR)vkGetInstanceProcAddr(vk.inst, "vk" "GetPhysicalDeviceSurfaceFormatsKHR");
		if (vkGetPhysicalDeviceSurfaceFormatsKHR == NULL) 
		{
			ERR_EXIT("vkGetInstanceProcAddr failed to find vk" "GetPhysicalDeviceSurfaceFormatsKHR", "vkGetInstanceProcAddr Failure");
		}

		vkGetPhysicalDeviceSurfacePresentModesKHR = (PFN_vkGetPhysicalDeviceSurfacePresentModesKHR)vkGetInstanceProcAddr(vk.inst, "vk" "GetPhysicalDeviceSurfacePresentModesKHR");
		if (vkGetPhysicalDeviceSurfacePresentModesKHR == NULL) 
		{
			ERR_EXIT("vkGetInstanceProcAddr failed to find vk" "GetPhysicalDeviceSurfacePresentModesKHR", "vkGetInstanceProcAddr Failure");
		}

		vkGetSwapchainImagesKHR = (PFN_vkGetSwapchainImagesKHR)vkGetInstanceProcAddr(vk.inst, "vk" "GetSwapchainImagesKHR");
		if (vkGetSwapchainImagesKHR == NULL) 
		{
			ERR_EXIT("vkGetInstanceProcAddr failed to find vk" "GetSwapchainImagesKHR", "vkGetInstanceProcAddr Failure");
		}
	}
*/

}


//-----------------------------------------------------------------------------
void vk_setup( VulkanInf& vk, HINSTANCE hInstance, HWND hWin, int _width, int _height )
//-----------------------------------------------------------------------------
{
	//---------------------------------------------------------
	// windowsハンドルから、vkサーフェスを取得する
	//---------------------------------------------------------
	{
		VkWin32SurfaceCreateInfoKHR createInfo;
	
		createInfo.sType 		= VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
		createInfo.pNext 		= NULL;
		createInfo.flags 		= 0;
		createInfo.hinstance 	= hInstance;
		createInfo.hwnd 		= hWin;

		VkResult  err;
		err = vkCreateWin32SurfaceKHR(vk.inst, &createInfo, NULL, &vk.surface);		//create30s
		assert(!err);
	}

	//---------------------------------------------------------
	// キューを作成
	//---------------------------------------------------------
	{
		// Iterate over each queue to learn whether it supports presenting:
		VkBool32 *supportsPresent = (VkBool32 *)malloc(vk.queue_family_count * sizeof(VkBool32));
		for (uint32_t i = 0; i < vk.queue_family_count; i++) 
		{
			vkGetPhysicalDeviceSurfaceSupportKHR(vk.gpu, i, vk.surface, &supportsPresent[i]);
		}

		// Search for a graphics and a present queue in the array of queue
		// families, try to find one that supports both
		uint32_t graphicsQueueFamilyIndex = UINT32_MAX;
		uint32_t presentQueueFamilyIndex = UINT32_MAX;
		for (uint32_t i = 0; i < vk.queue_family_count; i++) 
		{
			if ((vk.queue_props[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0) 
			{
				if (graphicsQueueFamilyIndex == UINT32_MAX) 
				{
					graphicsQueueFamilyIndex = i;
				}

				if (supportsPresent[i] == VK_TRUE) 
				{
					graphicsQueueFamilyIndex = i;
					presentQueueFamilyIndex = i;
					break;
				}
			}
		}

		if (presentQueueFamilyIndex == UINT32_MAX) 
		{
			// If didn't find a queue that supports both graphics and present, then
			// find a separate present queue.
			for (uint32_t i = 0; i < vk.queue_family_count; ++i) 
			{
				if (supportsPresent[i] == VK_TRUE) 
				{
					presentQueueFamilyIndex = i;
					break;
				}
			}
		}

		// Generate error if could not find both a graphics and a present queue
		if (graphicsQueueFamilyIndex == UINT32_MAX || presentQueueFamilyIndex == UINT32_MAX) 
		{
			ERR_EXIT("Could not find both graphics and present queues\n",
					 "Swapchain Initialization Failure");
		}

		vk.q_graphics_queue_family_index = graphicsQueueFamilyIndex;
		vk.q_present_queue_family_index = presentQueueFamilyIndex;
//		vk.flg_separate_present_queue = (vk.q_graphics_queue_family_index != vk.q_present_queue_family_index);

		free(supportsPresent);
	}

	//demo_create_device(&vk);
	//---------------------------------------------------------
	// 
	//---------------------------------------------------------
	{
		float queue_priorities[1] = {0.0};
		VkDeviceQueueCreateInfo queues[2];
		queues[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queues[0].pNext = NULL;
		queues[0].queueFamilyIndex = vk.q_graphics_queue_family_index;
		queues[0].queueCount = 1;
		queues[0].pQueuePriorities = queue_priorities;
		queues[0].flags = 0;

		VkDeviceCreateInfo device = 
		{
			.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
			.pNext = NULL,
			.queueCreateInfoCount = 1,
			.pQueueCreateInfos = queues,
			.enabledLayerCount = 0,
			.ppEnabledLayerNames = NULL,
			.enabledExtensionCount = vk.enabled_extension_count,
			.ppEnabledExtensionNames = (const char *const *)vk.extension_names,
			.pEnabledFeatures =
				NULL, // If specific features are required, pass them in here
		};
/*
		if (vk.flg_separate_present_queue) 
		{
			queues[1].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queues[1].pNext = NULL;
			queues[1].queueFamilyIndex = vk.q_present_queue_family_index;
			queues[1].queueCount = 1;
			queues[1].pQueuePriorities = queue_priorities;
			queues[1].flags = 0;
			device.queueCreateInfoCount = 2;
		}
*/
		VkResult  err;
		err = vkCreateDevice(vk.gpu, &device, NULL, &vk.device);			//create29s
		assert(!err);
	}

/*
	//---------------------------------------------------------
	// 
	//---------------------------------------------------------
	{
		{
			if (!g_gdpa) g_gdpa = (PFN_vkGetDeviceProcAddr)vkGetInstanceProcAddr(vk.inst, "vkGetDeviceProcAddr");
			vkCreateSwapchainKHR = (PFN_vkCreateSwapchainKHR)g_gdpa(vk.device, "vk" "CreateSwapchainKHR");
			if (vkCreateSwapchainKHR == NULL) 
			{
				ERR_EXIT("vkGetDeviceProcAddr failed to find vk" "CreateSwapchainKHR", "vkGetDeviceProcAddr Failure");
			}
		}
		{
			if (!g_gdpa) g_gdpa = (PFN_vkGetDeviceProcAddr)vkGetInstanceProcAddr(vk.inst, "vkGetDeviceProcAddr");
			vkDestroySwapchainKHR = (PFN_vkDestroySwapchainKHR)g_gdpa(vk.device, "vk" "DestroySwapchainKHR");
			if (vkDestroySwapchainKHR == NULL) 
			{
				ERR_EXIT("vkGetDeviceProcAddr failed to find vk" "DestroySwapchainKHR", "vkGetDeviceProcAddr Failure");
			}
		}
		{
			if (!g_gdpa) g_gdpa = (PFN_vkGetDeviceProcAddr)vkGetInstanceProcAddr(vk.inst, "vkGetDeviceProcAddr");
			vkGetSwapchainImagesKHR = (PFN_vkGetSwapchainImagesKHR)g_gdpa(vk.device, "vk" "GetSwapchainImagesKHR");
			if (vkGetSwapchainImagesKHR == NULL) 
			{
				ERR_EXIT("vkGetDeviceProcAddr failed to find vk" "GetSwapchainImagesKHR", "vkGetDeviceProcAddr Failure");
			}
		}
		{
			if (!g_gdpa) g_gdpa = (PFN_vkGetDeviceProcAddr)vkGetInstanceProcAddr(vk.inst, "vkGetDeviceProcAddr");
			vkAcquireNextImageKHR = (PFN_vkAcquireNextImageKHR)g_gdpa(vk.device, "vk" "AcquireNextImageKHR");
			if (vkAcquireNextImageKHR == NULL) 
			{
				ERR_EXIT("vkGetDeviceProcAddr failed to find vk" "AcquireNextImageKHR", "vkGetDeviceProcAddr Failure");
			}
		}
		{
			if (!g_gdpa) g_gdpa = (PFN_vkGetDeviceProcAddr)vkGetInstanceProcAddr(vk.inst, "vkGetDeviceProcAddr");
			vkQueuePresentKHR = (PFN_vkQueuePresentKHR)g_gdpa(vk.device, "vk" "QueuePresentKHR");
			if (vkQueuePresentKHR == NULL) 
			{
				ERR_EXIT("vkGetDeviceProcAddr failed to find vk" "QueuePresentKHR", "vkGetDeviceProcAddr Failure");
			}
		}
	}
*/

	//---------------------------------------------------------
	// 
	//---------------------------------------------------------
	vkGetDeviceQueue(vk.device, vk.q_graphics_queue_family_index, 0, &vk.graphics_queue);

	//---------------------------------------------------------
	// 
	//---------------------------------------------------------
/*
	if (!vk.flg_separate_present_queue) 
	{
		vk.present_queue = vk.graphics_queue;
	} else 
*/
	{
		vkGetDeviceQueue(vk.device, vk.q_present_queue_family_index, 0, &vk.present_queue);
	}

	//---------------------------------------------------------
	// フォーマット情報を取得
	//---------------------------------------------------------
	{	
		//---------------------------------------------------------
		// フォーム数を取得
		//---------------------------------------------------------
		uint32_t formatCount;
		{
			VkResult  err;
			err = vkGetPhysicalDeviceSurfaceFormatsKHR(vk.gpu, vk.surface, &formatCount, NULL);
			assert(!err);
		}

		//---------------------------------------------------------
		// フォーム数分バッファを確保
		//---------------------------------------------------------
		VkSurfaceFormatKHR *surfFormats = (VkSurfaceFormatKHR *)malloc(formatCount * sizeof(VkSurfaceFormatKHR));

		//---------------------------------------------------------
		// サーフェースフォーマットを取得
		//---------------------------------------------------------
		{
			VkResult  err;
			err = vkGetPhysicalDeviceSurfaceFormatsKHR(vk.gpu, vk.surface, &formatCount, surfFormats);
			assert(!err);
		}

		//---------------------------------------------------------
		// 
		//---------------------------------------------------------
		{
			// If the format list includes just one entry of VK_FORMAT_UNDEFINED,
			// the surface has no preferred format.  Otherwise, at least one
			// supported format will be returned.
			if (formatCount == 1 && surfFormats[0].format == VK_FORMAT_UNDEFINED) 
			{
				vk.format = VK_FORMAT_B8G8R8A8_UNORM;
			} else 
			{
				assert(formatCount >= 1);
				vk.format = surfFormats[0].format;
			}
		
			vk.color_space	= surfFormats[0].colorSpace;
//			vk.flgQuit			= false;
//			vk.curFrame		= 0;
		}
	}


	for (uint32_t i = 0; i < FRAME_LAG; i++) 
	{
		//---------------------------------------------------------
		// 
		//---------------------------------------------------------
		{
			// Create fences that we can use to throttle if we get too far
			// ahead of the image presents
			VkFenceCreateInfo fci = 
			{
				.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
				.pNext = NULL,
				.flags = VK_FENCE_CREATE_SIGNALED_BIT
			};
			VkResult  err;
			err = vkCreateFence(vk.device, &fci, NULL, &vk.fences[i]);		//create1s
			assert(!err);
		}

		{
			// Create semaphores to synchronize acquiring presentable buffers before
			// rendering and waiting for drawing to be complete before presenting
			VkSemaphoreCreateInfo sci = 
			{
				.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
				.pNext = NULL,
				.flags = 0,
			};
			//---------------------------------------------------------
			// セマフォの作成：イメージアキュワイド
			//---------------------------------------------------------
			{
				VkResult  err;
				err = vkCreateSemaphore(vk.device, &sci, NULL, &vk.image_acquired_semaphores[i]);	//create2s
				assert(!err);
			}

			//---------------------------------------------------------
			// セマフォの作成：描画完了
			//---------------------------------------------------------
			{
				VkResult  err;
				err = vkCreateSemaphore(vk.device, &sci, NULL, &vk.draw_complete_semaphores[i]);	//create3s
				assert(!err);
			}

			//---------------------------------------------------------
			// セマフォの作成：イメージオーナーシップ
			//---------------------------------------------------------
/*
			if (vk.flg_separate_present_queue) 
			{
				VkResult  err;
				err = vkCreateSemaphore(vk.device, &sci, NULL, &vk.image_ownership_semaphores[i]);	//create4s
				assert(!err);
			}
*/
		}
	}



	//---------------------------------------------------------
	// コマンドプールの作成
	//---------------------------------------------------------
	{

		const VkCommandPoolCreateInfo cmd_pool_info = 
		{
			.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
			.pNext = NULL,
			.queueFamilyIndex = vk.q_graphics_queue_family_index,
			.flags = 0,
		};

		VkResult  err;
		err = vkCreateCommandPool(vk.device, &cmd_pool_info, NULL, &vk.cmd_pool);	//create26s
		assert(!err);
	}



	//---------------------------------------------------------
	// コマンドバッファの確保
	//---------------------------------------------------------
	{
		const VkCommandBufferAllocateInfo cmai = 
		{
			.sType 				= VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
			.pNext 				= NULL,
			.commandPool 		= vk.cmd_pool,
			.level 				= VK_COMMAND_BUFFER_LEVEL_PRIMARY,
			.commandBufferCount = 1,
		};
		VkResult  err;
		err = vkAllocateCommandBuffers(vk.device, &cmai, &vk.cmdbuf);
		assert(!err);
	}

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
		err = vkBeginCommandBuffer(vk.cmdbuf, &cb);
		assert(!err);
	}


	//---------------------------------------------------------
	// 描画モード値
	//---------------------------------------------------------
	VkPresentModeKHR swapchainPresentMode = VK_PRESENT_MODE_FIFO_KHR;
	{
		// The FIFO present mode is guaranteed by the spec to be supported
		// and to have no tearing.  It's a great default present mode to use.

		//  There are times when you may wish to use another present mode.  The
		//  following code shows how to select them, and the comments provide some
		//  reasons you may wish to use them.
		//
		// It should be noted that Vulkan 1.0 doesn't provide a method for
		// synchronizing rendering with the presentation engine's display.  There
		// is a method provided for throttling rendering with the display, but
		// there are some presentation engines for which this method will not work.
		// If an application doesn't throttle its rendering, and if it renders much
		// faster than the refresh rate of the display, this can waste power on
		// mobile devices.  That is because power is being spent rendering images
		// that may never be seen.

		// VK_PRESENT_MODE_IMMEDIATE_KHR is for applications that don't care about
		// tearing, or have some way of synchronizing their rendering with the
		// display.
		// VK_PRESENT_MODE_MAILBOX_KHR may be useful for applications that
		// generally render a new presentable image every refresh cycle, but are
		// occasionally early.  In this case, the application wants the new image
		// to be displayed instead of the previously-queued-for-presentation image
		// that has not yet been displayed.
		// VK_PRESENT_MODE_FIFO_RELAXED_KHR is for applications that generally
		// render a new presentable image every refresh cycle, but are occasionally
		// late.  In this case (perhaps because of stuttering/latency concerns),
		// the application wants the late image to be immediately displayed, even
		// though that may mean some tearing.
		//---------------------------------------------------------
		// 描画モードの数の取得
		//---------------------------------------------------------
		uint32_t presentModeCount;
		{
			VkResult  err;
			err = vkGetPhysicalDeviceSurfacePresentModesKHR( vk.gpu, vk.surface, &presentModeCount, NULL);
			assert(!err);
		}

		//---------------------------------------------------------
		// 描画モードバッファの確保
		//---------------------------------------------------------
		VkPresentModeKHR *presentModes;
		{
			presentModes = (VkPresentModeKHR *)malloc(presentModeCount * sizeof(VkPresentModeKHR));
			assert(presentModes);

			VkResult  err;
			err = vkGetPhysicalDeviceSurfacePresentModesKHR( vk.gpu, vk.surface, &presentModeCount, presentModes);
			assert(!err);
		}

		//---------------------------------------------------------
		// 描画モードを検索
		//---------------------------------------------------------
		if (vk.presentMode !=  swapchainPresentMode) 
		{

			for (size_t i = 0; i < presentModeCount; ++i) 
			{
				if (presentModes[i] == vk.presentMode) 
				{
					swapchainPresentMode = vk.presentMode;
					break;
				}
			}
		}
		if (swapchainPresentMode != vk.presentMode) 
		{
			ERR_EXIT("Present mode specified is not supported\n", "Present mode unsupported");
		}

		//---------------------------------------------------------
		// 描画モードバッファの解放
		//---------------------------------------------------------
		if (NULL != presentModes) 
		{
			free(presentModes);
		}
	}


	//---------------------------------------------------------
	// サーフェス情報の取得
	//---------------------------------------------------------
	VkSurfaceCapabilitiesKHR surfCapabilities;
	{
		VkResult  err;
		err = vkGetPhysicalDeviceSurfaceCapabilitiesKHR( vk.gpu, vk.surface, &surfCapabilities);
		assert(!err);
	}

	//---------------------------------------------------------
	// 最小イメージ数値
	//---------------------------------------------------------
	uint32_t mic = 3;
	{
		// Determine the number of VkImages to use in the swap chain.
		// Application desires to acquire 3 images at a time for triple
		// buffering
		if (mic < surfCapabilities.minImageCount) 
		{
			mic = surfCapabilities.minImageCount;
		}
		// If maxImageCount is 0, we can ask for as many images as we want;
		// otherwise we're limited to maxImageCount
		if ((surfCapabilities.maxImageCount > 0) && (mic > surfCapabilities.maxImageCount)) 
		{
			// Application must settle for fewer images than desired:
			mic = surfCapabilities.maxImageCount;
		}
	}

	//---------------------------------------------------------
	// サーフェストランスフォーム値
	//---------------------------------------------------------
//	VkSurfaceTransformFlagsKHR preTransform;
	VkSurfaceTransformFlagBitsKHR preTransform;
	{
		if ( surfCapabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR ) 
		{
			preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
		} else 
		{
			preTransform = surfCapabilities.currentTransform;
		}
	}

	//---------------------------------------------------------
	// コンポジットアルファフラグ値
	//---------------------------------------------------------
	VkCompositeAlphaFlagBitsKHR caf = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	{
		// Find a supported composite alpha mode - one of these is guaranteed to be set
		VkCompositeAlphaFlagBitsKHR compositeAlphaFlags[4] = 
		{
			VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
			VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR,
			VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR,
			VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR,
		};
		for (uint32_t i = 0; i < sizeof(compositeAlphaFlags); i++) 
		{
			if (surfCapabilities.supportedCompositeAlpha & compositeAlphaFlags[i]) 
			{
				caf = compositeAlphaFlags[i];
				break;
			}
		}
	}

	//---------------------------------------------------------
	// スワップチェインの切り替え
	//---------------------------------------------------------
	{
		//---------------------------------------------------------
		// スクリーンサイズ値
		//---------------------------------------------------------
		VkExtent2D ext;
		{
			// width and height are either both 0xFFFFFFFF, or both not 0xFFFFFFFF.
			if (surfCapabilities.currentExtent.width == 0xFFFFFFFF) 
			{
				// If the surface size is undefined, the size is set to the size
				// of the images requested, which must fit within the minimum and
				// maximum values.
				ext.width = _width;
				ext.height =_height;

				if (ext.width < surfCapabilities.minImageExtent.width) 
				{
					ext.width = surfCapabilities.minImageExtent.width;
				} 
				else 
				if (ext.width > surfCapabilities.maxImageExtent.width) 
				{
					ext.width = surfCapabilities.maxImageExtent.width;
				}
				
				if (ext.height < surfCapabilities.minImageExtent.height) 
				{
					ext.height = surfCapabilities.minImageExtent.height;
				} 
				else 
				if (ext.height > surfCapabilities.maxImageExtent.height) 
				{
					ext.height = surfCapabilities.maxImageExtent.height;
				}
			} else 
			{
				// If the surface size is defined, the swap chain size must match
				ext = surfCapabilities.currentExtent;
	//			_width = surfCapabilities.currentExtent.width;
	//			_height = surfCapabilities.currentExtent.height;
			}
		}

		//---------------------------------------------------------
		// スワップチェインの保存
		//---------------------------------------------------------
		VkSwapchainKHR oldSwapchain = vk.swapchain;

		//---------------------------------------------------------
		// スワップチェインの作成
		//---------------------------------------------------------
		{
			VkSwapchainCreateInfoKHR sci = 
			{
				.sType 					= VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
				.pNext 					= NULL,
				.surface 				= vk.surface,
				.minImageCount 			= mic,
				.imageFormat 			= vk.format,
				.imageColorSpace 		= vk.color_space,
				.imageExtent 			=
					{
					   .width = ext.width
					 , .height = ext.height
					},
				.imageUsage 			= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
				.preTransform 			= preTransform,
				.compositeAlpha 		= caf,
				.imageArrayLayers 		= 1,
				.imageSharingMode 		= VK_SHARING_MODE_EXCLUSIVE,
				.queueFamilyIndexCount 	= 0,
				.pQueueFamilyIndices 	= NULL,
				.presentMode 			= swapchainPresentMode,
				.oldSwapchain 			= oldSwapchain,
				.clipped 				= true,
			};
			VkResult  err;
			err = vkCreateSwapchainKHR(vk.device, &sci, NULL, &vk.swapchain);	//create16s
			assert(!err);
		}
		//---------------------------------------------------------
		// 旧スワップチェインの廃棄
		//---------------------------------------------------------
		{
			// If we just re-created an existing swapchain, we should destroy the old
			// swapchain at this point.
			// Note: destroying the swapchain also cleans up all its associated
			// presentable images once the platform is done with them.
			if (oldSwapchain != VK_NULL_HANDLE) 
			{
				vkDestroySwapchainKHR(vk.device, oldSwapchain, NULL);
			}
		}
	}

	//---------------------------------------------------------
	// スワップチェイン・イメージ数の取得
	//---------------------------------------------------------
	{
		VkResult  err;
		err = vkGetSwapchainImagesKHR(vk.device, vk.swapchain, &vk.swapchainImageCount, NULL);
		assert(!err);
	}

	{
		//---------------------------------------------------------
		// スワップチェイン・イメージバッファの確保
		//---------------------------------------------------------
		VkImage *sci = (VkImage *)malloc(vk.swapchainImageCount * sizeof(VkImage));
		assert(sci);
		{
			VkResult  err;
			err = vkGetSwapchainImagesKHR(vk.device, vk.swapchain, &vk.swapchainImageCount, sci);
			assert(!err);
		}

		//---------------------------------------------------------
		// イメージビューの作成
		//---------------------------------------------------------
		{
//			vk.swapchain_image_resources = (SwapchainImageResources *)malloc(sizeof(SwapchainImageResources) * vk.swapchainImageCount);	//create24s
//			assert(vk.swapchain_image_resources);
			vk.sir_image						=(VkImage* 			)malloc(sizeof(VkImage			) * vk.swapchainImageCount);
			vk.sir_cmdbuf						=(VkCommandBuffer* 	)malloc(sizeof(VkCommandBuffer	) * vk.swapchainImageCount);
			vk.sir_graphics_to_present_cmdbuf	=(VkCommandBuffer* 	)malloc(sizeof(VkCommandBuffer	) * vk.swapchainImageCount);
			vk.sir_imgview						=(VkImageView* 		)malloc(sizeof(VkImageView		) * vk.swapchainImageCount);
//			vk.sir_uniform_buffer				=(VkBuffer* 		)malloc(sizeof(VkBuffer			) * vk.swapchainImageCount);
//			vk.sir_uniform_memory				=(VkDeviceMemory* 	)malloc(sizeof(VkDeviceMemory	) * vk.swapchainImageCount);
			vk.sir_framebuffer					=(VkFramebuffer* 	)malloc(sizeof(VkFramebuffer	) * vk.swapchainImageCount);
//			vk.sir_descriptor_set				=(VkDescriptorSet* 	)malloc(sizeof(VkDescriptorSet	) * vk.swapchainImageCount);

			for (uint32_t i = 0; i < vk.swapchainImageCount; i++) 
			{
				VkImageViewCreateInfo ivci = 
				{
					.sType 			= VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
					.pNext 			= NULL,
					.format 		= vk.format,
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
					.viewType = VK_IMAGE_VIEW_TYPE_2D,
					.flags = 0,
					.image = sci[i],
				};

				vk.sir_image[i] = sci[i];

				{
					VkResult  err;
					err = vkCreateImageView(vk.device, &ivci, NULL, &vk.sir_imgview[i]);	//create20s
					assert(!err);
				}
			}
		}

	}
	
	//---------------------------------------------------------
	// 物理デバイスメモリプロパティを取得
	//---------------------------------------------------------
	{
		vk.frame_index = 0;
		// Get Memory information and properties
		vkGetPhysicalDeviceMemoryProperties(vk.gpu, &vk.memory_properties);
	}
	
	//-----------------------------------------------------
	// フレームバッファの作成
	//-----------------------------------------------------
	{//demo_prepare_depth(&vk);
		const VkFormat depth_format = VK_FORMAT_D16_UNORM;
		const VkImageCreateInfo image = 
		{
			.sType 			= VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
			.pNext 			= NULL,
			.imageType 		= VK_IMAGE_TYPE_2D,
			.format 		= depth_format,
			.extent 		= {_width, _height, 1},
			.mipLevels 		= 1,
			.arrayLayers 	= 1,
			.samples 		= VK_SAMPLE_COUNT_1_BIT,
			.tiling 		= VK_IMAGE_TILING_OPTIMAL,
			.usage 			= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
			.flags 			= 0,
		};



		//-----------------------------------------------------
		// デプス作成
		//-----------------------------------------------------
		{
			VkMemoryRequirements mr;
			vk.depth_inf.format = depth_format;

			//-----------------------------------------------------
			// イメージの作成
			//-----------------------------------------------------
			{
				/* create image */
				VkResult  err;
				err = vkCreateImage(vk.device, &image, NULL, &vk.depth_inf.image);	//create18s
				assert(!err);
			}

			//-----------------------------------------------------
			// イメージメモリ情報の取得
			//-----------------------------------------------------
			{
				vkGetImageMemoryRequirements(vk.device, vk.depth_inf.image, &mr);
			}

			vk.depth_inf.mem_alloc.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			vk.depth_inf.mem_alloc.pNext = NULL;
			vk.depth_inf.mem_alloc.allocationSize = mr.size;
			vk.depth_inf.mem_alloc.memoryTypeIndex = 0;

			//-----------------------------------------------------
			// 
			//-----------------------------------------------------
			{
				bool  pass = false;
				{
					VkPhysicalDeviceMemoryProperties* 	pMemory_properties 	= &vk.memory_properties;
					uint32_t 							typeBits			= mr.memoryTypeBits;
					VkFlags 							requirements_mask	= VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
					uint32_t*							typeIndex			= &vk.depth_inf.mem_alloc.memoryTypeIndex;

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

			//-----------------------------------------------------
			// メモリ確保：デプス 
			//-----------------------------------------------------
			{ 
				/* allocate memory */
				VkResult  err;
				err = vkAllocateMemory(vk.device, &vk.depth_inf.mem_alloc, NULL, &vk.depth_inf.devmem);	//create19s
				assert(!err);
			}

			//-----------------------------------------------------
			// イメージメモリのバインド
			//-----------------------------------------------------
			{
				/* bind memory */
				VkResult  err;
				err = vkBindImageMemory(vk.device, vk.depth_inf.image, vk.depth_inf.devmem, 0);
				assert(!err);
			}
		}

		//-----------------------------------------------------
		// イメージビューの作成
		//-----------------------------------------------------
		{
			VkImageViewCreateInfo ivci = 
			{
				.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
				.pNext = NULL,
				.image = VK_NULL_HANDLE,
				.format = depth_format,
				.subresourceRange = 
				{.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT,
									 .baseMipLevel = 0,
									 .levelCount = 1,
									 .baseArrayLayer = 0,
									 .layerCount = 1},
				.flags = 0,
				.viewType = VK_IMAGE_VIEW_TYPE_2D,
			};
			/* create image imgview */
			ivci.image = vk.depth_inf.image;
			VkResult  err;
			err = vkCreateImageView(vk.device, &ivci, NULL, &vk.depth_inf.imgview);	//create17s
			assert(!err);
		}
	}

	//-----------------------------------------------------
	// テクスチャイメージビューの作成
	//-----------------------------------------------------
	//demo_prepare_textures(&vk);
	{
		const VkFormat tex_format = VK_FORMAT_R8G8B8A8_UNORM;
		VkFormatProperties props;
		uint32_t i;

		vkGetPhysicalDeviceFormatProperties(vk.gpu, tex_format, &props);

		for (i = 0; i < DEMO_TEXTURE_COUNT; i++) 
		{

			if ((props.linearTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT) ) 
			{
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

				// Nothing in the pipeline needs to be complete to start, and don't allow fragment
				// shader to run until layout transition completes
				//-----------------------------------------------------
				//
				//-----------------------------------------------------
				{
					VkImage 				image				= vk.textures[i].image;
					VkImageAspectFlags 		aspectMask			= VK_IMAGE_ASPECT_COLOR_BIT;
					VkImageLayout 			old_image_layout	= VK_IMAGE_LAYOUT_PREINITIALIZED;
					VkImageLayout 			new_image_layout	= vk.textures[i].imageLayout;
					VkAccessFlagBits 		srcAccessMask		= VK_ACCESS_HOST_WRITE_BIT;
					VkPipelineStageFlags 	src_stages			= VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
					VkPipelineStageFlags 	dest_stages			= VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;

					assert(vk.cmdbuf);

					VkImageMemoryBarrier imb = 
					{
						.sType 				= VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
						.pNext 				= NULL,
						.srcAccessMask 		= srcAccessMask,
						.dstAccessMask 		= 0,
						.oldLayout 			= old_image_layout,
						.newLayout 			= new_image_layout,
						.image 				= image,
						.subresourceRange 	= {aspectMask, 0, 1, 0, 1}
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
					vkCmdPipelineBarrier(vk.cmdbuf, src_stages, dest_stages, 0, 0, NULL, 0, NULL, 1, &imb);
				}

				vk.staging_texture.image = 0;
			} 
			else 
			if (props.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT) 
			{
				/* Must use staging buffer to copy linear texture to optimized */

				memset(&vk.staging_texture, 0, sizeof(vk.staging_texture));
				demo_prepare_texture_image(
					  vk.device
					, &vk.memory_properties
					, tex_files[i]
					, &vk.staging_texture
					, VK_IMAGE_TILING_LINEAR
					, VK_IMAGE_USAGE_TRANSFER_SRC_BIT
					, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
				);

				demo_prepare_texture_image(
					  vk.device
					, &vk.memory_properties
					, tex_files[i]
					, &vk.textures[i]
					, VK_IMAGE_TILING_OPTIMAL
					,(VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT)
					,VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
				);				

				//-----------------------------------------------------
				//
				//-----------------------------------------------------
				{
					VkImage 				image				= vk.staging_texture.image;
					VkImageAspectFlags 		aspectMask 			= VK_IMAGE_ASPECT_COLOR_BIT;
					VkImageLayout 			old_image_layout 	= VK_IMAGE_LAYOUT_PREINITIALIZED;
					VkImageLayout 			new_image_layout 	= VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
					VkAccessFlagBits 		srcAccessMask		= VK_ACCESS_HOST_WRITE_BIT;
					VkPipelineStageFlags 	src_stages			= VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
					VkPipelineStageFlags 	dest_stages			= VK_PIPELINE_STAGE_TRANSFER_BIT;
					assert(vk.cmdbuf);
					VkImageMemoryBarrier imb = 
					{
						.sType 				= VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
						.pNext 				= NULL,
						.srcAccessMask 		= srcAccessMask,
						.dstAccessMask 		= 0,
						.oldLayout 			= old_image_layout,
						.newLayout 			= new_image_layout,
						.image 				= image,
						.subresourceRange 	= {aspectMask, 0, 1, 0, 1}
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
					vkCmdPipelineBarrier(vk.cmdbuf, src_stages, dest_stages, 0, 0, NULL, 0, NULL, 1, &imb);
				}

				//-----------------------------------------------------
				//
				//-----------------------------------------------------
				{
					VkImage 				image				= vk.textures[i].image;
					VkImageAspectFlags 		aspectMask 			= VK_IMAGE_ASPECT_COLOR_BIT;
					VkImageLayout 			old_image_layout 	= VK_IMAGE_LAYOUT_PREINITIALIZED;
					VkImageLayout 			new_image_layout 	= VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
					VkAccessFlagBits 		srcAccessMask		= VK_ACCESS_HOST_WRITE_BIT;
					VkPipelineStageFlags 	src_stages			= VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
					VkPipelineStageFlags 	dest_stages			= VK_PIPELINE_STAGE_TRANSFER_BIT;
					assert(vk.cmdbuf);
					VkImageMemoryBarrier imb = 
					{
						.sType 				= VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
						.pNext 				= NULL,
						.srcAccessMask 		= srcAccessMask,
						.dstAccessMask 		= 0,
						.oldLayout 			= old_image_layout,
						.newLayout 			= new_image_layout,
						.image 				= image,
						.subresourceRange 	= {aspectMask, 0, 1, 0, 1}
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
					vkCmdPipelineBarrier(vk.cmdbuf, src_stages, dest_stages, 0, 0, NULL, 0, NULL, 1, &imb);
				}

				//-----------------------------------------------------
				//
				//-----------------------------------------------------
				{
					VkImageCopy ic = 
					{
						.srcSubresource 	= {VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1},
						.srcOffset 			= {0, 0, 0},
						.dstSubresource 	= {VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1},
						.dstOffset 			= {0, 0, 0},
						.extent 			= {vk.staging_texture.tex_width,vk.staging_texture.tex_height, 1},
					};
					vkCmdCopyImage(
						  vk.cmdbuf
						, vk.staging_texture.image
						, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL
						, vk.textures[i].image
						, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
						, 1
						, &ic
					);
				}

				
				//-----------------------------------------------------
				//
				//-----------------------------------------------------
				{
					VkImage 				image				= vk.textures[i].image;
					VkImageAspectFlags 		aspectMask 			= VK_IMAGE_ASPECT_COLOR_BIT;
					VkImageLayout 			old_image_layout 	= VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
					VkImageLayout 			new_image_layout 	= vk.textures[i].imageLayout;
					VkAccessFlagBits 		srcAccessMask		= VK_ACCESS_TRANSFER_WRITE_BIT;
					VkPipelineStageFlags 	src_stages			= VK_PIPELINE_STAGE_TRANSFER_BIT;
					VkPipelineStageFlags 	dest_stages			= VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
					assert(vk.cmdbuf);
					VkImageMemoryBarrier imb = 
					{
						.sType 				= VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
						.pNext 				= NULL,
						.srcAccessMask 		= srcAccessMask,
						.dstAccessMask 		= 0,
						.oldLayout 			= old_image_layout,
						.newLayout		 	= new_image_layout,
						.image 				= image,
						.subresourceRange 	= {aspectMask, 0, 1, 0, 1}
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
					vkCmdPipelineBarrier(vk.cmdbuf, src_stages, dest_stages, 0, 0, NULL, 0, NULL, 1, &imb);
				}

			} 
			else 
			{
				/* Can't support VK_FORMAT_R8G8B8A8_UNORM !? */
				assert(!"No support for R8G8B8A8_UNORM as texture image format");
			}


			//-----------------------------------------------------
			// サンプラーの作成
			//-----------------------------------------------------
			{
				/* create sampler */
				const VkSamplerCreateInfo sci = 
				{
					.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
					.pNext = NULL,
					.magFilter = VK_FILTER_NEAREST,
					.minFilter = VK_FILTER_NEAREST,
					.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST,
					.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
					.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
					.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
					.mipLodBias = 0.0f,
					.anisotropyEnable = VK_FALSE,
					.maxAnisotropy = 1,
					.compareOp = VK_COMPARE_OP_NEVER,
					.minLod = 0.0f,
					.maxLod = 0.0f,
					.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE,
					.unnormalizedCoordinates = VK_FALSE,
				};
				VkResult  err;
				err = vkCreateSampler(vk.device, &sci, NULL, &vk.textures[i].sampler);	//create15s
				assert(!err);
			}

			//-----------------------------------------------------
			// イメージビューの作成
			//-----------------------------------------------------
			{
				/* create image imgview */
				VkImageViewCreateInfo ivc = 
				{
					.sType 		= VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
					.pNext 		= NULL,
					.image 		= VK_NULL_HANDLE,
					.viewType 	= VK_IMAGE_VIEW_TYPE_2D,
					.format 	= tex_format,
					.components =
						{
							VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G,
							VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A,
						},
					.subresourceRange = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1},
					.flags = 0,
				};
				ivc.image = vk.textures[i].image;
				VkResult  err;
				err = vkCreateImageView(vk.device, &ivc, NULL, &vk.textures[i].imgview);	//create12s
				assert(!err);
			}
			
		}
	}

}


//-----------------------------------------------------------------------------
void	vk_release( VulkanInf& vk )
//-----------------------------------------------------------------------------
{
// setup
	// Wait for fences from present operations
	for (int i = 0; i < FRAME_LAG; i++) 
	{
		vkWaitForFences(vk.device, 1, &vk.fences[i], VK_TRUE, UINT64_MAX);
		vkDestroyFence(vk.device, vk.fences[i], NULL);										//create1	*	setup
		vkDestroySemaphore(vk.device, vk.image_acquired_semaphores[i], NULL)	;			//create2	*	setup
		vkDestroySemaphore(vk.device, vk.draw_complete_semaphores[i], NULL);				//create3	*	setup
/*
		if (vk.flg_separate_present_queue) 
		{
			vkDestroySemaphore(vk.device, vk.image_ownership_semaphores[i], NULL);			//create4	*	setup
		}
*/
	}
	for (int i = 0; i < DEMO_TEXTURE_COUNT; i++) 
	{
		vkDestroyImageView(vk.device, vk.textures[i].imgview, NULL);	//create12	*	setup
		vkDestroyImage(vk.device, vk.textures[i].image, NULL);			//create13	*	setup
		vkFreeMemory(vk.device, vk.textures[i].devmem, NULL);			//create14	*	setup
		vkDestroySampler(vk.device, vk.textures[i].sampler, NULL);		//create15	*	setup
	}
	vkDestroySwapchainKHR(vk.device, vk.swapchain, NULL);			//create16	*	setup

	vkDestroyImageView(vk.device, vk.depth_inf.imgview, NULL);			//create17	*	setup
	vkDestroyImage(vk.device, vk.depth_inf.image, NULL);				//create18	*	setup
	vkFreeMemory(vk.device, vk.depth_inf.devmem, NULL);					//create19	*	setup

	for (int i = 0; i < vk.swapchainImageCount; i++) 
	{
		vkDestroyImageView(vk.device, vk.sir_imgview[i], NULL);				//create20	*	setup
	}

//	free(vk.swapchain_image_resources);									//create24	*	setup
	free(vk.sir_image);
	free(vk.sir_cmdbuf);
	free(vk.sir_graphics_to_present_cmdbuf);
	free(vk.sir_imgview);
//	free(vk.sir_uniform_buffer);
//	free(vk.sir_uniform_memory);
	free(vk.sir_framebuffer);
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


//=================


//-----------------------------------------------------------------------------
void vk_create( VulkanInf& vk, HINSTANCE hInstance, HWND hWin, int _width, int _height )
//-----------------------------------------------------------------------------
{
	vk_init( vk );

	vk_setup( vk, hInstance, hWin, _width, _height );

	printf("vk create \n");

}

//-----------------------------------------------------------------------------
VkInf::VkInf( HINSTANCE hInstance, HWND hWin, int _width, int _height )
//-----------------------------------------------------------------------------
{
	vk_create( vk, hInstance, hWin, _width, _height );

	this->flgSetModel = false;

}

//-----------------------------------------------------------------------------
VkInf::~VkInf()
//-----------------------------------------------------------------------------
{
	vk_release( vk );
}



