void vk2_create( VulkanInf& vk, int _width, int _height
//	,void* pDataVert
//	,int sizeofStructDataVert
	, int unit_cnt
);

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
void	vk2_release( VulkanInf& vk );

void vk2_loadModel( VulkanInf& vk
	,void* pDataVert
	,int sizeofStructDataVert

	, VkBuffer* 				&	sc_uniform_buffer
	, VkDeviceMemory* 			&	sc_uniform_memory
	, VkDescriptorSet* 			&	sc_descriptor_set
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
