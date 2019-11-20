#pragma once

#include "opengl.h"
#include <memory>

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

class Texture
{
public:
	Texture(const std::string& filename) : textureId_(new GLuint(), [](auto id) { glDeleteTextures(1, id); })
	{
		glGenTextures(1, textureId_.get());
		Bind();
		int numComponents;
		stbi_set_flip_vertically_on_load(1);
		const auto pixels = stbi_load(filename.c_str(), &width_, &height_, &numComponents, 4);
		if (!pixels)
		{
			std::cerr << "stbi_load(" << filename << ") failed with: " << stbi_failure_reason() << std::endl;
		}
		stbi_set_flip_vertically_on_load(0);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8_ALPHA8, width_, height_, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAnisotropy);
		glGenerateMipmap(GL_TEXTURE_2D);

		stbi_image_free(pixels);
	}

	void Bind() const
	{
		glBindTexture(GL_TEXTURE_2D, *textureId_);
	}
	
private:
	std::shared_ptr<GLuint> textureId_;
	GLsizei width_;
	GLsizei height_;
	
};
