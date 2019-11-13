#pragma once

#include <string>
#include <array>

class Material
{
public:
	Material() = default;

	Material(const std::string& name, const std::array<float, 3>& ambient, const std::array<float, 3>& diffuse,
	         const std::array<float, 3>& specular, const float shininess)
		: name_(name),
		  ambient_(ambient),
		  diffuse_(diffuse),
		  specular_(specular),
		  shininess_(shininess),
		  diffuseMapId_(-1)
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

	std::array<float, 3> Ambient() const
	{
		return ambient_;
	}

	void SetAmbient(const std::array<float, 3>& ambient)
	{
		ambient_ = ambient;
	}

	std::array<float, 3> Diffuse() const
	{
		return diffuse_;
	}

	void SetDiffuse(const std::array<float, 3>& diffuse)
	{
		diffuse_ = diffuse;
	}

	std::array<float, 3> Specular() const
	{
		return specular_;
	}

	void SetSpecular(const std::array<float, 3>& specular)
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

	uint32_t DiffuseMapId() const
	{
		return diffuseMapId_;
	}

	void SetDiffuseMapId(uint32_t diffuseMapId)
	{
		diffuseMapId_ = diffuseMapId;
	}

	struct DiffuseMap
	{
		GLuint DiffuseMapTextureID;
	};
	
private:
	std::string name_;
	std::array<float, 3> ambient_;
	std::array<float, 3> diffuse_;
	std::array<float, 3> specular_;
	float shininess_;

	uint32_t diffuseMapId_;
};
