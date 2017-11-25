void vk2_create( VulkanInf& vk, int _width, int _height
	,void* pDataVert
	,int sizeofStructDataVert
 );

void	vk2_updateBegin( VulkanInf& vk);
void	vk2_updateEnd( VulkanInf& vk);
void	vk2_drawPolygon( VulkanInf& vk
,const void* pMVP
,int matrixSize
);
void	vk2_release( VulkanInf& vk );

void vk2_loadModel( VulkanInf& vk
	,void* pDataVert
	,int sizeofStructDataVert
 );
//-----------------------------------------------------------------------------
void vk2_cmd3( VulkanInf& vk );
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void vk2_cmd2( VulkanInf& vk
//-----------------------------------------------------------------------------
		,int _vertexCount		//	= 12*3;
		,int _instanceCount		//	= 1;
		,int _firstVertex		//	= 0;
		,int _firstInstance		// = 0;
);
//-----------------------------------------------------------------------------
void vk2_cmd1( VulkanInf& vk
//-----------------------------------------------------------------------------
	, const int 		_width
	, const int 		_height
);
