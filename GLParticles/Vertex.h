#pragma once

#include "opengl.h"
#include "glm/gtx/hash.hpp"
#include <vector>

class Vertex
{
public:
	Vertex() = default;

	explicit Vertex(const glm::vec3& position)
		: position(position),
		  hasTexcoord_(false),
		  hasNormal_(false),
		  hasTangent_(false)
	{
	}

	Vertex(const glm::vec3& position, const glm::vec2& texcoord)
		: position(position),
		  texcoord(texcoord),
		  hasTexcoord_(true),
		  hasNormal_(false),
		  hasTangent_(false)
	{
	}

	Vertex(const glm::vec3& position, const glm::vec2& texcoord, const glm::vec3& normal, const glm::vec3& tangent)
		: position(position),
		  texcoord(texcoord),
		  normal(normal),
		  tangent(tangent),
		  hasTexcoord_(true),
		  hasNormal_(true),
		  hasTangent_(true)
	{
	}

	Vertex(const Vertex& other)
		: position(other.position),
		  texcoord(other.texcoord),
		  normal(other.normal),
		  tangent(other.tangent)
	{
	}

	Vertex(Vertex&& other) noexcept
		: position(std::move(other.position)),
		  texcoord(std::move(other.texcoord)),
		  normal(std::move(other.normal)),
		  tangent(std::move(other.tangent))
	{
	}

	Vertex& operator=(const Vertex& other)
	{
		if (this == &other)
			return *this;
		position = other.position;
		texcoord = other.texcoord;
		normal = other.normal;
		tangent = other.tangent;
		return *this;
	}

	Vertex& operator=(Vertex&& other) noexcept
	{
		if (this == &other)
			return *this;
		position = std::move(other.position);
		texcoord = std::move(other.texcoord);
		normal = std::move(other.normal);
		tangent = std::move(other.tangent);
		return *this;
	}

	glm::vec3 Position() const
	{
		return position;
	}

	void SetPosition(const glm::vec3& position)
	{
		this->position = position;
	}

	glm::vec2 Texcoord() const
	{
		return texcoord;
	}

	void SetTexcoord(const glm::vec2& texcoord)
	{
		this->texcoord = texcoord;
	}

	glm::vec3 Normal() const
	{
		return normal;
	}

	void SetNormal(const glm::vec3& normal)
	{
		this->normal = normal;
	}

	glm::vec3 Tangent() const
	{
		return tangent;
	}

	void SetTangent(const glm::vec3& tangent)
	{
		this->tangent = tangent;
	}

	friend bool operator==(const Vertex& lhs, const Vertex& rhs)
	{
		return lhs.position == rhs.position
			&& lhs.texcoord == rhs.texcoord
			&& lhs.normal == rhs.normal
			&& lhs.tangent == rhs.tangent;
	}

	friend bool operator!=(const Vertex& lhs, const Vertex& rhs)
	{
		return !(lhs == rhs);
	}

	friend std::size_t hash_value(const Vertex& obj)
	{
		std::size_t seed = 0x0F46A72E;
		seed ^= (seed << 6) + (seed >> 2) + 0x3F65E3B1 + std::hash<glm::vec3>{}(obj.position);
		seed ^= (seed << 6) + (seed >> 2) + 0x15615B31 + std::hash<glm::vec2>{}(obj.texcoord);
		seed ^= (seed << 6) + (seed >> 2) + 0x58E70045 + std::hash<glm::vec3>{}(obj.normal);
		seed ^= (seed << 6) + (seed >> 2) + 0x690F5B85 + std::hash<glm::vec3>{}(obj.tangent);
		return seed;
	}

	std::vector<float> Flatten() const
	{
		std::vector<float> result = {};
		auto value = glm::value_ptr(position);
		result.insert(result.end(), value, value + 3);
		
		if (hasTexcoord_)
		{
			value = glm::value_ptr(texcoord);
			result.insert(result.end(), value, value + 2);
		}
		
		if (hasNormal_)
		{
			value = glm::value_ptr(normal);
			result.insert(result.end(), value, value + 3);
		}

		if (hasTangent_)
		{
			value = glm::value_ptr(tangent);
			result.insert(result.end(), value, value + 3);
		}

		return result;
	}
private:
	glm::vec3 position;
	glm::vec2 texcoord;
	glm::vec3 normal;
	glm::vec3 tangent;

	bool hasTexcoord_;
	bool hasNormal_;
	bool hasTangent_;
};
