#pragma once

/**
 * A simple struct for storing OpenGL vertex
 * information.
 * 
 * Note colors are implemented via uniforms,
 * due to many balls being given identical
 * colors.
 */

struct GLVertex
{
	float position[2];
	float textureCoords[2];
};