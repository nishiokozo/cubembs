#version 400
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable
layout (binding = 1) uniform sampler2D tex;

layout (location = 0) in vec4 N;
layout (location = 0) out vec4 uFragColor;
void main() 
{
	vec3	vlignt = normalize(vec3(0.3,-1,0.5));
	float	d = dot(-vlignt, N.xyz );
	uFragColor = vec4(d,d,d,1)+vec4(0.2,0.2,0.2,1);
}
