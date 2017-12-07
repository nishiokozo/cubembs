class	Unit12
{
	public:
	struct vk_vert12 
	{
		// Must start with MVP
		float mvp[4][4];
		float position[12 * 3][4];
		float attr[12 * 3][4];
	};

	VkInf::Vkunit	vkunit;
//	VkBuffer* 			sc_uniform_buffer	;
//	VkDeviceMemory* 	sc_uniform_memory	;
//	VkDescriptorSet* 	sc_descriptor_set	;

	vect44				mvp					;
	vect44 				mat_model			;

	void loadModel(
		 vk_vert12* pDataVert12

		, const char* fn_vert
		, const char* fn_frag
		, const char** 	tex_files
		, const int		tex_cnt
	);
	void unloadModel();
	void drawModel();
};
