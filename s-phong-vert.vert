#version 400
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable
layout(std140, binding = 0) uniform buf {
        mat4 MVP;
        vec4 position[12*3];
        vec4 norm[12*3];
} ubuf;

layout (location = 0) out vec4 N;

void main() 
{
   N = ubuf.norm[gl_VertexIndex];
   gl_Position = ubuf.MVP * ubuf.position[gl_VertexIndex];
}
