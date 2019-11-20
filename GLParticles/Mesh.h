#pragma once

#include "opengl.h"

#include <memory>
#include <vector>
#include <iostream>
#include <map>

#include "preamble.glsl"

class Mesh
{
public:
	Mesh()
		: vao_(new GLuint(), [](auto id) { glDeleteVertexArrays(1, id); }),
		  attributeVBO_(new GLuint(), [](auto id) { glDeleteBuffers(1, id); }),
		  indexVBO_(new GLuint(), [](auto id) { glDeleteBuffers(1, id); }),
		  numIndices_(0),
		  numVertices_(0)
	{
		glGenVertexArrays(1, vao_.get());
		glGenBuffers(1, attributeVBO_.get());
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

	std::shared_ptr<GLuint> AttributeVbo() const
	{
		return attributeVBO_;
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


	std::vector<DrawElementsIndirectCommand>& DrawCommands()
	{
		return drawCommands_;
	}

	const DrawElementsIndirectCommand& DrawCommand(const uint32_t index) const
	{
		return drawCommands_[index];
	}
	
	std::vector<uint32_t>& MaterialIDs()
	{
		return materialIDs_;
	}

	void SetMaterialIDs(std::vector<uint32_t> materialIDs)
	{
		materialIDs_ = materialIDs;
	}

private:
	std::shared_ptr<GLuint> vao_;
	std::shared_ptr<GLuint> attributeVBO_;
	std::shared_ptr<GLuint> indexVBO_;

	GLuint numIndices_;
	GLuint numVertices_;

	std::vector<DrawElementsIndirectCommand> drawCommands_;
	std::vector<uint32_t> materialIDs_;
};
