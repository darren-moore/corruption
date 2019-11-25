#include "ResourceEngine.h"

#include <Core/Assert.h>
#include <Core/graphics/Shader.h>
#include <Core/graphics/Texture.h>

#include <sstream>
#include <fstream>
#include <iostream>


#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

void ResourceEngine::Init(std::string assetsPath) {
	m_assetsPath = assetsPath;
}

const std::string ResourceEngine::GetAsset(const char* filePath) const {
	try {
		std::string fullPath = m_assetsPath;
		fullPath += "\\";
		fullPath += filePath;
		std::fstream file(fullPath);
		ASSERT_FATAL(file.is_open(), "file not found: " << filePath);

		std::stringstream buffer;
		buffer << file.rdbuf();
		const std::string fileContents = buffer.str();
		file.close();
		return fileContents;
	}
	catch (std::fstream::failure e) {
		ASSERT_FATAL(false, "read error for file: " << filePath);
		return "";
	}
}

void ResourceEngine::AddShader(const char* vertShaderFilePath, const char* fragShaderFilePath, const char* shaderName) {
	Shader* shader = new Shader(GetAsset(vertShaderFilePath).c_str(), GetAsset(fragShaderFilePath).c_str());
	m_shaderMap[shaderName] = shader;
}

Shader* ResourceEngine::GetShader(const char* shaderName) {
	auto shader = m_shaderMap.find(shaderName);
	ASSERT_FATAL(shader != m_shaderMap.end(), "shader not found: " << shaderName);
	return (*shader).second;
};

void ResourceEngine::AddTexture(const char* filePath, const char* textureName) {
	int width, height, nrChannels;
	std::string fullPath = GetAssetsPath() + filePath;
	unsigned char* data = stbi_load(fullPath.c_str(), &width, &height, &nrChannels, STBI_rgb_alpha);
	Texture* texture = new Texture(data, width, height, nrChannels);
	m_textureMap[textureName] = texture;
	stbi_image_free(data);
}

Texture* ResourceEngine::GetTexture(const char* textureName) {
	auto texture = m_textureMap.find(textureName);
	ASSERT_FATAL(texture != m_textureMap.end(), "texture not found: " << textureName);
	return (*texture).second;
}

const std::string& ResourceEngine::GetAssetsPath() const {
	return m_assetsPath;
}