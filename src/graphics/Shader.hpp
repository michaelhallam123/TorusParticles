#pragma once

/**
 * A simple wrapper for OpenGL shader programs
 */

#include <string>
#include <unordered_map>
#include <array>
#include <iostream>

class Shader
{
private:
	std::unordered_map<std::string, int> m_uniformLocationCache;
	std::string                          m_vertexShaderFilePath;
	std::string                          m_fragmentShaderFilePath;
	unsigned int                         m_shaderID;
public:
	Shader(const std::string& vertexShaderFilePath, const std::string& fragmentShaderFilePath);
	~Shader();

	void bind() const;
	void unbind() const;

	//Set uniforms
	void setUniform1i(const std::string& name, int value);
	void setUniform1f(const std::string& name, float value);
	void setUniform4f(const std::string& name, float v0, float v1, float v2, float v3);
	void setUniform4f(const std::string& name, std::array<float, 4> v);

private:
	int          getUniformLocation(const std::string& name);
	std::string  parseShader(const std::string& path);
	unsigned int compileShader(unsigned int type, const std::string& source);
	unsigned int createShader(const std::string& vertexShader, const std::string& fragmentShader);
};