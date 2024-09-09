#pragma once

#include <string>
#include <unordered_map>
#include <array>
#include <iostream>

class Shader
{
private:
	std::unordered_map<std::string, int> m_UniformLocationCache;
	std::string m_VertexShaderFilePath;
	std::string m_FragmentShaderFilePath;
	unsigned int m_ShaderID;
	//caching for uniforms
public:
	Shader(const std::string& vertexShaderFilePath, const std::string& fragmentShaderFilePath);
	~Shader();

	void Bind() const;
	void Unbind() const;

	//Set uniforms
	void SetUniform1i(const std::string& name, int value);
	void SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3);
	void SetUniform4f(const std::string& name, std::array<float, 4> v);

private:
	int GetUniformLocation(const std::string& name);
	std::string ParseShader(const std::string& path);
	unsigned int CompileShader(unsigned int type, const std::string& source);
	unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader);
};