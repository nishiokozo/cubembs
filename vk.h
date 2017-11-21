#define DEMO_TEXTURE_COUNT 1
#define APP_SHORT_NAME "cube"
#define FRAME_LAG 2// Allow a maximum of two outstanding presentation operations.

typedef struct 
{
	VkImage image;
	VkCommandBuffer 	cmdbuf;
	VkCommandBuffer 	graphics_to_present_cmdbuf;
	VkImageView 		imgview;
	VkBuffer 			uniform_buffer;
	VkDeviceMemory 		uniform_memory;
	VkFramebuffer 		framebuffer;
	VkDescriptorSet 	descriptor_set;
} SwapchainImageResources;

struct texture_object 
{
	VkSampler sampler;
	VkImage image;
	VkImageLayout imageLayout;
	VkMemoryAllocateInfo	mem_alloc;
	VkDeviceMemory 			devmem;
	VkImageView				imgview;
	int32_t 				tex_width;
	int32_t 				tex_height;
};

struct VulkanInf 
{
	VkSurfaceKHR 	surface;
//	bool 			flgPrepared;
	bool 			flg_separate_present_queue;


	VkInstance 			inst;
	VkPhysicalDevice 	gpu;
	VkDevice 			device;
	VkQueue 			graphics_queue;
	VkQueue 			present_queue;
	uint32_t 			q_graphics_queue_family_index;
	uint32_t 			q_present_queue_family_index;
	VkSemaphore 		image_acquired_semaphores[FRAME_LAG];
	VkSemaphore 		draw_complete_semaphores[FRAME_LAG];
	VkSemaphore 		image_ownership_semaphores[FRAME_LAG];
	VkPhysicalDeviceProperties 		gpu_props;
	VkQueueFamilyProperties*			queue_props;
	VkPhysicalDeviceMemoryProperties 	memory_properties;

	uint32_t 	enabled_extension_count;
	uint32_t 	enabled_layer_count;
	const char*	extension_names[64];
	char*		enabled_layers[64];

	VkFormat 		format;
	VkColorSpaceKHR color_space;

	PFN_vkGetPhysicalDeviceSurfaceSupportKHR 		fpGetPhysicalDeviceSurfaceSupportKHR;
	PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR	fpGetPhysicalDeviceSurfaceCapabilitiesKHR;
	PFN_vkGetPhysicalDeviceSurfaceFormatsKHR		fpGetPhysicalDeviceSurfaceFormatsKHR;
	PFN_vkGetPhysicalDeviceSurfacePresentModesKHR	fpGetPhysicalDeviceSurfacePresentModesKHR;
	PFN_vkCreateSwapchainKHR						fpCreateSwapchainKHR;
	PFN_vkDestroySwapchainKHR						fpDestroySwapchainKHR;
	PFN_vkGetSwapchainImagesKHR						fpGetSwapchainImagesKHR;
	PFN_vkAcquireNextImageKHR						fpAcquireNextImageKHR;
	PFN_vkQueuePresentKHR							fpQueuePresentKHR;

	uint32_t 					swapchainImageCount;
	VkSwapchainKHR				swapchain;
	SwapchainImageResources*	swapchain_image_resources;
	VkPresentModeKHR 			presentMode;
	VkFence 					fences[FRAME_LAG];
	int 						frame_index;

	VkCommandPool cmd_pool;
	VkCommandPool present_cmd_pool;

	struct 
	{
		VkFormat 				format;
		VkImage					image;
		VkMemoryAllocateInfo	mem_alloc;
		VkDeviceMemory 			devmem;
		VkImageView 			imgview;
	} depth_inf;

	struct texture_object textures[DEMO_TEXTURE_COUNT];
	struct texture_object staging_texture;

	VkCommandBuffer 		cmdbuf;  // Buffer for initialization commands
	VkPipelineLayout		pipeline_layout;
	VkDescriptorSetLayout 	desc_layout;
	VkPipelineCache 		pipelineCache;
	VkRenderPass 			render_pass;
	VkPipeline 				pipeline;

	VkDescriptorPool desc_pool;

	uint32_t current_buffer;
	uint32_t queue_family_count;
};

class VkInf
{
	VulkanInf vk;
public:

	bool isNull()
	{
		return ((long long)this==0);
	}

	void setmodel( int _width, int _height
		,void* pDataVert
		,int sizeofStructDataVert
	);
	void v_draw(
		 void*	pMVP
		,int matrixSize
	);
	VkInf( HINSTANCE hInstance, HWND hWin, int _width, int _height );
	~VkInf();

};
