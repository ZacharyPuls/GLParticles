#pragma once

#include "opengl.h"

#include "packed_freelist.h"
#include "Material.h"
#include "Mesh.h"
#include "Transform.h"
#include "Camera.h"

class Scene
{
public:
	Scene() : materials_(256), meshes_(256), transforms_(256), instances_(256), cameras_(256)
	{
	}

	packed_freelist<Material::DiffuseMap> DiffuseMaps() const
	{
		return diffuseMaps_;
	}

	Material::DiffuseMap& DiffuseMap(const uint32_t id) const
	{
		return diffuseMaps_[id];
	}

	uint32_t AddDiffuseMap(const Material::DiffuseMap& diffuseMap)
	{
		return diffuseMaps_.insert(diffuseMap);
	}
	
	packed_freelist<Material> Materials() const
	{
		return materials_;
	}

	::Material& Material(const uint32_t id) const
	{
		return materials_[id];
	}

	packed_freelist<Mesh> Meshes() const
	{
		return meshes_;
	}

	Mesh& Mesh(const uint32_t id) const
	{
		return meshes_[id];
	}

	packed_freelist<Transform> Transforms() const
	{
		return transforms_;
	}

	Transform& Transform(const uint32_t id) const
	{
		return transforms_[id];
	}
	
	packed_freelist<Mesh::Instance> Instances() const
	{
		return instances_;
	}

	Mesh::Instance& Instance(const uint32_t id) const
	{
		return instances_[id];
	}

	packed_freelist<Camera> Cameras() const
	{
		return cameras_;
	}

	::Camera& Camera(const uint32_t id) const
	{
		return cameras_[id];
	}

	::Camera& MainCamera() const
	{
		return Camera(MainCameraId());
	}

	uint32_t MainCameraId() const
	{
		return mainCameraId_;
	}

	void AddMesh(const std::string& filename, const std::string& materialDir)
	{
		::Mesh mesh;
		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;
		std::string warn;
		std::string err;
		if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err,
			filename.c_str(), materialDir.c_str(), true))
		{
			std::cerr << "Failed to load model file [" << filename << "]: " << err << std::endl;
			return;
		}

		for (const auto& shape : shapes)
		{
			
			if (!shape.mesh.num_face_vertices.empty())
			{
				const auto& numVertices = shape.mesh.num_face_vertices;
				glBindBuffer(GL_ARRAY_BUFFER, *mesh.PositionVbo());
				glBufferData(GL_ARRAY_BUFFER,
					numVertices.size() * sizeof(float) * 3,
					attrib.vertices.data(), GL_STATIC_DRAW);
				mesh.SetNumVertices(attrib.vertices.size());
				glBindBuffer(GL_ARRAY_BUFFER, 0);
			}

			if (!attrib.texcoords.empty())
			{
				glBindBuffer(GL_ARRAY_BUFFER, *mesh.TexcoordVbo());
				glBufferData(GL_ARRAY_BUFFER,
					attrib.texcoords.size() * sizeof(float),
					attrib.texcoords.data(), GL_STATIC_DRAW);
				glBindBuffer(GL_ARRAY_BUFFER, 0);
			}

			if (!attrib.normals.empty())
			{
				glBindBuffer(GL_ARRAY_BUFFER, *mesh.NormalVbo());
				glBufferData(GL_ARRAY_BUFFER,
					attrib.normals.size() * sizeof(float),
					attrib.normals.data(), GL_STATIC_DRAW);
				glBindBuffer(GL_ARRAY_BUFFER, 0);
			}

			if (!shapes[0].mesh.indices.empty())
			{
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *mesh.IndexVbo());
				glBufferData(GL_ELEMENT_ARRAY_BUFFER,
					shapes[0].mesh.indices.size() * sizeof(unsigned int),
					shapes[0].mesh.indices.data(), GL_STATIC_DRAW);
				mesh.SetNumIndices(shapes[0].mesh.indices.size());
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
			}

			glBindVertexArray(*mesh.Vao());

			glBindBuffer(GL_ARRAY_BUFFER, *mesh.PositionVbo());
			glVertexAttribPointer(SCENE_POSITION_ATTRIB_LOCATION, 3, GL_FLOAT, GL_FALSE,
				sizeof(float) * 3, nullptr);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glEnableVertexAttribArray(SCENE_POSITION_ATTRIB_LOCATION);

			glBindBuffer(GL_ARRAY_BUFFER, *mesh.TexcoordVbo());
			glVertexAttribPointer(SCENE_TEXCOORD_ATTRIB_LOCATION, 2, GL_FLOAT, GL_FALSE,
				sizeof(float) * 2, nullptr);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glEnableVertexAttribArray(SCENE_TEXCOORD_ATTRIB_LOCATION);

			glBindBuffer(GL_ARRAY_BUFFER, *mesh.NormalVbo());
			glVertexAttribPointer(SCENE_NORMAL_ATTRIB_LOCATION, 3, GL_FLOAT, GL_FALSE,
				sizeof(float) * 3, nullptr);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glEnableVertexAttribArray(SCENE_NORMAL_ATTRIB_LOCATION);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *mesh.IndexVbo());

			glBindVertexArray(0);
		}
		std::map<std::string, uint32_t> diffuseMapCache;
		std::vector<uint32_t> newMaterialIDs;
		for (auto& material : materials)
		{
			::Material newMaterial(material.name, *reinterpret_cast<std::array<float, 3>*>(&material.ambient),
				*reinterpret_cast<std::array<float, 3>*>(&material.diffuse),
				*reinterpret_cast<std::array<float, 3>*>(&material.specular), material.shininess);

			if (!material.diffuse_texname.empty())
			{
				auto cachedTexture = diffuseMapCache.find(material.diffuse_texname);

				if (cachedTexture != std::end(diffuseMapCache))
				{
					newMaterial.SetDiffuseMapId(cachedTexture->second);
				}
				else
				{
					std::string diffuse_texname_full = materialDir + material.diffuse_texname;
					int x, y, comp;
					stbi_set_flip_vertically_on_load(1);
					stbi_uc* pixels = stbi_load(diffuse_texname_full.c_str(), &x, &y, &comp, 4);
					stbi_set_flip_vertically_on_load(0);

					if (!pixels)
					{
						std::cerr << "stbi_load(" << diffuse_texname_full << ") failed with: " << stbi_failure_reason() << std::endl;
					}
					else
					{
						float maxAnisotropy;
						glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAnisotropy);

						GLuint newDiffuseMapTextureID;
						glGenTextures(1, &newDiffuseMapTextureID);
						glBindTexture(GL_TEXTURE_2D, newDiffuseMapTextureID);
						glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8_ALPHA8, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
						glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAnisotropy);
						glGenerateMipmap(GL_TEXTURE_2D);
						glBindTexture(GL_TEXTURE_2D, 0);

						Material::DiffuseMap newDiffuseMap;
						newDiffuseMap.DiffuseMapTextureID = newDiffuseMapTextureID;

						auto newDiffuseMapID = AddDiffuseMap(newDiffuseMap);

						diffuseMapCache.emplace(material.diffuse_texname, newDiffuseMapID);

						newMaterial.SetDiffuseMapId(newDiffuseMapID);

						stbi_image_free(pixels);
					}
				}
			}

			uint32_t newMaterialID = materials_.insert(newMaterial);

			newMaterialIDs.push_back(newMaterialID);
		}

	}

private:
	packed_freelist<Material::DiffuseMap> diffuseMaps_;
	packed_freelist<::Material> materials_;
	packed_freelist<::Mesh> meshes_;
	packed_freelist<::Transform> transforms_;
	packed_freelist<Mesh::Instance> instances_;
	packed_freelist<::Camera> cameras_;

	uint32_t mainCameraId_;
};
