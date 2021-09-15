#pragma once
#include <string> 
#include <glm.hpp>
class ComputeShader
{
private:
	unsigned int id;
	int GetUniformLocation(const std::string& name);
	void CheckCompileErrors(unsigned int shader, std::string type);
public:
	ComputeShader(const std::string& filepath);
	~ComputeShader();

	void Bind() const;
	void Unbind() const;

	void SetUniform1i(const std::string& name, int value);
	void SetUniform1f(const std::string& name, float value);
	void SetUniform1ui(const std::string& name, unsigned int value);
	void SetUniformVec3(const std::string& name, glm::vec3 vec);
};