#define DEMO_TEXTURE_COUNT 1
#define APP_SHORT_NAME "cube"
#define FRAME_LAG 2// Allow a maximum of two outstanding presentation operations.

/*
typedef struct 
{
	VkImage 			sir_image;
	VkCommandBuffer 	sir_cmdbuf;
	VkCommandBuffer 	sir_graphics_to_present_cmdbuf;
	VkImageView 		sir_imgview;
	VkBuffer 			sir_uniform_buffer;
	VkDeviceMemory 		sir_uniform_memory;
	VkFramebuffer 		sir_framebuffer;
	VkDescriptorSet 	sir_descriptor_set;
} SwapchainImageResources;
*/

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
//	bool 			flg_separate_present_queue;


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
//	char*		enabled_layers[64];

	VkFormat 		format;
	VkColorSpaceKHR color_space;

/*
	PFN_vkGetPhysicalDeviceSurfaceSupportKHR 		fpGetPhysicalDeviceSurfaceSupportKHR;
	PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR	fpGetPhysicalDeviceSurfaceCapabilitiesKHR;
	PFN_vkGetPhysicalDeviceSurfaceFormatsKHR		fpGetPhysicalDeviceSurfaceFormatsKHR;
	PFN_vkGetPhysicalDeviceSurfacePresentModesKHR	fpGetPhysicalDeviceSurfacePresentModesKHR;
	PFN_vkCreateSwapchainKHR						fpCreateSwapchainKHR;
	PFN_vkDestroySwapchainKHR						fpDestroySwapchainKHR;
	PFN_vkGetSwapchainImagesKHR						fpGetSwapchainImagesKHR;
	PFN_vkAcquireNextImageKHR						fpAcquireNextImageKHR;
	PFN_vkQueuePresentKHR							fpQueuePresentKHR;
*/

	uint32_t 					swapchainImageCount;
	VkSwapchainKHR				swapchain;
//	SwapchainImageResources*	swapchain_image_resources;
		VkImage* 			sir_image;
		VkCommandBuffer* 	sir_cmdbuf;
		VkCommandBuffer* 	sir_graphics_to_present_cmdbuf;
		VkImageView* 		sir_imgview;
//		VkBuffer* 			sir_uniform_buffer;
//		VkDeviceMemory* 	sir_uniform_memory;
		VkFramebuffer* 		sir_framebuffer;
//		VkDescriptorSet* 	sir_descriptor_set;

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
//	struct texture_object staging_texture;

	VkCommandBuffer 		nor_cmdbuf;  // Buffer for initialization commands
	VkPipelineLayout		pipeline_layout;
	VkDescriptorSetLayout 	desc_layout;
	VkPipelineCache 		pipelineCache;
	VkRenderPass 			render_pass;
	VkPipeline 				pipeline;

	VkDescriptorPool desc_pool;

	uint32_t current_buffer;
	uint32_t queue_family_count;
};


//void vk2_create( VulkanInf& vk, int _width, int _height
//	, int unit_cnt
//	, const int		tex_cnt
//);

void	vk2_updateBegin( VulkanInf& vk
	, const int 		_width
	, const int 		_height
);
void	vk2_updateEnd( VulkanInf& vk);
void	vk2_drawPolygon( VulkanInf& vk
,const void* pMVP
,int matrixSize
	, VkDeviceMemory* 			&	sir_uniform_memory
	,int _vertexCount		//	= 12*3;
	,int _instanceCount		//	= 1;
	,int _firstVertex		//	= 0;
	,int _firstInstance		// = 0;
	, VkDescriptorSet* 			&	sir_descriptor_set
);
//void	vk2_release( VulkanInf& vk );

void	vk2_removeModel( VulkanInf& vk
	, VkBuffer* 				&	sir_uniform_buffer
	, VkDeviceMemory* 			&	sir_uniform_memory
	, VkDescriptorSet* 			&	sir_descriptor_set

 );
 
//-----------------------------------------------------------------------------
void vk2_loadTexture( VulkanInf& vk
//-----------------------------------------------------------------------------
	, const char** tex_files
);

//-----------------------------------------------------------------------------
void vk2_loadModel( VulkanInf& vk
//-----------------------------------------------------------------------------
	,void* pDataVert
	,int sizeofStructDataVert

	, VkBuffer* 				&	sc_uniform_buffer
	, VkDeviceMemory* 			&	sc_uniform_memory
	, VkDescriptorSet* 			&	sc_descriptor_set
	, const char* fn_vert
	, const char* fn_frag
	, const char** 	tex_files
	, const int		tex_cnt
);
//-----------------------------------------------------------------------------
void vk2_cmd2( VulkanInf& vk
//-----------------------------------------------------------------------------
		,int _vertexCount		//	= 12*3;
		,int _instanceCount		//	= 1;
		,int _firstVertex		//	= 0;
		,int _firstInstance		// = 0;
	, VkDescriptorSet* 			&	sir_descriptor_set
);


class VkInf
{
public:
	struct Vkunit
	{
		VkBuffer* 			uniform_buffer;
		VkDeviceMemory* 	uniform_memory;
		VkDescriptorSet* 	descriptor_set;
		int					tex_cnt;
	};

	VulkanInf vk;
//	bool	flgSetModel;

	bool isNull()
	{
		return ((long long)this==0);
	}
	
	void release()
	{
	
	printf("---relese\n");
		delete this;
	}
	VkInf( HINSTANCE hInstance, HWND hWin, int _width, int _height, int unit_max, int tex_max );
	~VkInf();

	void loadModel(
		 void* pDataVert
		,int sizeofStructDataVert

//		, VkBuffer* 				&	sc_uniform_buffer
//		, VkDeviceMemory* 			&	sc_uniform_memory
//		, VkDescriptorSet* 			&	sc_descriptor_set
		, const char* fn_vert
		, const char* fn_frag
		, const char** 	tex_files
		, const int		tex_cnt
		
		, Vkunit	&vkunit
	);
	void unloadModel(
		 Vkunit	&vkunit
//		  VkBuffer* 				&	sir_uniform_buffer
//		, VkDeviceMemory* 			&	sir_uniform_memory
//		, VkDescriptorSet* 			&	sir_descriptor_set
	);
	void drawModel(
		 const void* pMVP
		,int matrixSize
//		, VkDeviceMemory* 			&	sir_uniform_memory
		,int _vertexCount
		,int _instanceCount
		,int _firstVertex
		,int _firstInstance
//		, VkDescriptorSet* 			&	sir_descriptor_set
		, Vkunit	&vkunit
	);

	void drawBegin( int _width, int _height );
	void drawEnd();

};

