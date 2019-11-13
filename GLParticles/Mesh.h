#pragma once

#include "opengl.h"
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#include <memory>
#include <vector>
#include <iostream>
#include <map>

#include "preamble.glsl"

#define STBI_IMPLEMENTATION
#include "stb/stb_image.h"

class Mesh
{
public:
	Mesh()
		: vao_(new GLuint(), [](auto id) { glDeleteVertexArrays(1, id); }),
		  positionVBO_(new GLuint(), [](auto id) { glDeleteBuffers(1, id); }),
		  texcoordVBO_(new GLuint(), [](auto id) { glDeleteBuffers(1, id); }),
		  normalVBO_(new GLuint(), [](auto id) { glDeleteBuffers(1, id); }),
		  indexVBO_(new GLuint(), [](auto id) { glDeleteBuffers(1, id); }),
		  numIndices_(0),
		  numVertices_(0)
	{
		glGenVertexArrays(1, vao_.get());
		glGenBuffers(1, positionVBO_.get());
		glGenBuffers(1, texcoordVBO_.get());
		glGenBuffers(1, normalVBO_.get());
		glGenBuffers(1, indexVBO_.get());
	}

	struct Instance
	{
		uint32_t MeshID;
		uint32_t TransformID;
	};

	std::shared_ptr<GLuint> Vao() const
	{
		return vao_;
	}

	std::shared_ptr<GLuint> PositionVbo() const
	{
		return positionVBO_;
	}

	std::shared_ptr<GLuint> TexcoordVbo() const
	{
		return texcoordVBO_;
	}

	std::shared_ptr<GLuint> NormalVbo() const
	{
		return normalVBO_;
	}

	std::shared_ptr<GLuint> IndexVbo() const
	{
		return indexVBO_;
	}

	GLuint NumIndices() const
	{
		return numIndices_;
	}

	void SetNumIndices(GLuint numIndices)
	{
		numIndices_ = numIndices;
	}

	GLuint NumVertices() const
	{
		return numVertices_;
	}

	void SetNumVertices(GLuint numVertices)
	{
		numVertices_ = numVertices;
	}
	
private:
	std::shared_ptr<GLuint> vao_;
	std::shared_ptr<GLuint> positionVBO_;
	std::shared_ptr<GLuint> texcoordVBO_;
	std::shared_ptr<GLuint> normalVBO_;
	std::shared_ptr<GLuint> indexVBO_;

	GLuint numIndices_;
	GLuint numVertices_;

	std::vector<GLDrawElementsIndirectCommand> drawCommands_;
	std::vector<uint32_t> materialIDs_;
};
