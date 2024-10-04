#version 330 core

layout(location = 0) in vec2 a_texCoord; // One of (-1,-1), (-1,1), (1,-1), (1,1)
layout(location = 1) in vec2 a_offset;   // Element of {-2,0,2}x{-2,0,2}
layout(location = 2) in vec2 a_center;   // Location of particle's centre in [-1,1]x[-1,1]

uniform float u_radius;                  // Radius of particle

out vec2 v_texCoord;                     // For passing a_texCoord to the fragment shader

void main()
{
	gl_Position = vec4(a_offset + a_center + u_radius * a_texCoord, 0.0, 1.0);
	v_texCoord = a_texCoord;
}