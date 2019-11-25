
#include "Shader.h"

#include <Core/Assert.h>
#include <iostream>

using namespace std;

Shader::Shader(const char* vertString, const char* fragString) {
	int  shaderSuccess;
	char shaderInfoLog[512];

	const char* vertexShaderSource = vertString;
	const char* fragmentShaderSource = fragString;

	// Vertex shader compilation
	unsigned int vertexShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);

	// Check for shader compilation errors
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &shaderSuccess);
	if (!shaderSuccess) {
		glGetShaderInfoLog(vertexShader, 512, NULL, shaderInfoLog);
		ASSERT_BREAK_FATAL("vertex shader compilation failed: " << shaderInfoLog);
	}


	// Fragment shader compilation
	unsigned int fragmentShader;
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);

	// Check for shader compilation errors
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &shaderSuccess);
	if (!shaderSuccess) {
		glGetShaderInfoLog(fragmentShader, 512, NULL, shaderInfoLog);
		ASSERT_BREAK_FATAL("fragment shader compilation failed: " << shaderInfoLog);
	}

	// Attach shaders to shader program
	Shader::ID = glCreateProgram();
	glAttachShader(ID, vertexShader);
	glAttachShader(ID, fragmentShader);
	glLinkProgram(ID);

	// Check for shader linking errors
	glGetShaderiv(ID, GL_LINK_STATUS, &shaderSuccess);
	if (!shaderSuccess) {
		glGetProgramInfoLog(ID, 512, NULL, shaderInfoLog);
		ASSERT_BREAK_FATAL("shader linking failed: " << shaderInfoLog);
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}

void Shader::Use() {
	glUseProgram(ID);
}

const GLint Shader::GetUniformLocation(const char* name) {
	auto& it = m_uniformMap.find(name);
	if (it == m_uniformMap.end()) {
		const GLint uniformLocation = glGetUniformLocation(ID, name);
		m_uniformMap.insert(std::pair<const char*, GLint>(name, uniformLocation));
		return uniformLocation;
	}
	return (*it).second;
}

void Shader::SetBool(const char* name, bool value) {
	glUniform1i(GetUniformLocation(name), (int)value);
}

void Shader::SetInt(const char* name, int value) {
	glUniform1i(GetUniformLocation(name), value);
}

void Shader::SetFloat(const char* name, float value) {
	glUniform1f(GetUniformLocation(name), value);
}

void Shader::SetVec3(const char* name, const math::Vec3f& value) {
	glUniform3f(GetUniformLocation(name), value[0], value[1], value[2]);
}

void Shader::SetVec4(const char* name, const math::Vec4f& value) {
	glUniform4f(GetUniformLocation(name), value[0], value[1], value[2], value[3]);
}

void Shader::SetMat4(const char* name, const math::Mat4f& value) {
	glUniformMatrix4fv(GetUniformLocation(name), 1, GL_TRUE, value.Data());
}
