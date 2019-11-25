#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <unordered_map>
#include <string>

class Shader;
class Texture;

// Darren comment: we may want to move away from using named resources if it's a perf issue

// Provides a way to access raw file data and stores global shaders or textures.
#define RESOURCE_ENGINE_CLASS
class ResourceEngine {
public:
	void Init(std::string assetsPath);
	void Stop() {};

	void AddFile(const char* filePath, const char* fileName);
	const char* GetFile(const char* fileName);

	void AddShader(const char* vertShaderFilePath, const char* fragShaderFilePath, const char* shaderName);
	Shader* GetShader(const char* shaderName);

	void AddTexture(const char* filePath, const char* textureName);
	Texture* GetTexture(const char* textureName);

	const std::string& GetAssetsPath() const;

private:
	// must return a string here because stream kills memory
	const std::string GetAsset(const char* filePath) const;
	std::string m_assetsPath;

	std::unordered_map<std::string, Shader*> m_shaderMap;
	std::unordered_map<std::string, Texture*> m_textureMap;
	std::unordered_map<std::string, const char*> m_fileMap;
};
