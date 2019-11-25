#pragma once

#include <glad/glad.h>

#include <Core/math/Vector.h>
#include <Core/math/Matrix.h>

#include <map>

struct charcmp
{
	bool operator()(char const *a, char const *b) const
	{
		return std::strcmp(a, b) < 0;
	}
};


class Shader {
public:
	unsigned int ID;

	Shader(const char* vertexPath, const char* fragmentPath);

	void Use();

	void SetBool(const char* name, bool value);
	void SetInt(const char* name, int value);
	void SetFloat(const char* name, float value);
	void SetVec3(const char* name, const math::Vec3f& value);
	void SetVec4(const char* name, const math::Vec4f& value);
	void SetMat4(const char* name, const math::Mat4f& value);
private:
	const GLint Shader::GetUniformLocation(const char* name);
	std::map<const char*, GLint, charcmp> m_uniformMap;
};
