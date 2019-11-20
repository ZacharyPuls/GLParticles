#pragma once

#include <string>
#include <array>
#include "Texture.h"

class Material
{
public:
	Material() = default;

	Material(const std::string& name, const glm::vec3& ambient, const glm::vec3& diffuse,
	         const glm::vec3& specular, const float shininess)
		: name_(name),
		  ambient_(ambient),
		  diffuse_(diffuse),
		  specular_(specular),
		  shininess_(shininess),
		  diffuseTexture_(-1),
		  normalTexture_(-1)
	{
	}

	std::string Name() const
	{
		return name_;
	}

	void SetName(const std::string& name)
	{
		name_ = name;
	}

	glm::vec3 Ambient() const
	{
		return ambient_;
	}

	void SetAmbient(const glm::vec3& ambient)
	{
		ambient_ = ambient;
	}

	glm::vec3 Diffuse() const
	{
		return diffuse_;
	}

	void SetDiffuse(const glm::vec3& diffuse)
	{
		diffuse_ = diffuse;
	}

	glm::vec3 Specular() const
	{
		return specular_;
	}

	void SetSpecular(const glm::vec3& specular)
	{
		specular_ = specular;
	}

	float Shininess() const
	{
		return shininess_;
	}

	void SetShininess(float shininess)
	{
		shininess_ = shininess;
	}
	
	uint32_t DiffuseTexture() const
	{
		return diffuseTexture_;
	}

	void SetDiffuseTexture(uint32_t diffuseTexture)
	{
		diffuseTexture_ = diffuseTexture;
	}

	uint32_t NormalTexture() const
	{
		return normalTexture_;
	}

	void SetNormalTexture(uint32_t normalTexture)
	{
		normalTexture_ = normalTexture;
	}
private:
	std::string name_;
	glm::vec3 ambient_;
	glm::vec3 diffuse_;
	glm::vec3 specular_;
	float shininess_;

	uint32_t diffuseTexture_;
	uint32_t normalTexture_;
};
