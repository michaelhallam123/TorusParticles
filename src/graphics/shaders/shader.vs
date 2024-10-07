#version 330 core

layout(location = 0) in vec2 a_texCoord; // One of (-1,-1), (-1,1), (1,-1), (1,1)
layout(location = 1) in vec2 a_offset;   // Translate of particle copy in world space
layout(location = 2) in vec2 a_center;   // Location of particle's centre in world space

uniform float u_radius;                  // Radius of particle
uniform vec4  u_worldToScreenTransform;

out vec2 v_texCoord;                     // For passing a_texCoord to the fragment shader

void main()
{
	gl_Position = u_worldToScreenTransform * vec4(a_offset + a_center + u_radius * a_texCoord, 0.0, 1.0);
	v_texCoord = a_texCoord;
}