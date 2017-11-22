void vk2_create( VulkanInf& vk, int _width, int _height
	,void* pDataVert
	,int sizeofStructDataVert
 );

void	vk2_draw( VulkanInf& vk
,const void* pMVP
,int& matrixSize
);
void	vk2_release( VulkanInf& vk );
