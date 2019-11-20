#pragma once

#include "opengl.h"

#include <array>
#include <map>

#include "packed_freelist.h"
#include "Material.h"
#include "Mesh.h"
#include "Transform.h"
#include "Camera.h"
#include "Texture.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#include "Vertex.h"
#include <unordered_map>

class Scene
{
public:
	Scene() : textures_(256), materials_(256), meshes_(256), transforms_(256), instances_(256), cameras_(256)
	{
	}
	
	uint32_t AddTexture(const Texture& texture)
	{
		return textures_.insert(texture);
	}

	Texture& Texture(const uint32_t id)
	{
		return textures_[id];
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

	void SetMainCameraId(uint32_t mainCameraId)
	{
		mainCameraId_ = mainCameraId;
	}

	uint32_t AddMesh(const std::string& filename, const std::string& materialDir, const std::string& textureDir)
	{
		::Mesh mesh;

		tinyobj::ObjReader objectReader;

		tinyobj::ObjReaderConfig config;
		config.mtl_search_path = materialDir;
		config.triangulate = true;
		if (!objectReader.ParseFromFile(filename, config))
		{
			std::cerr << "Failed to load model file [" << filename << "]." << std::endl << "\tWarning: " << objectReader
				.Warning() << std::endl << "\tError: " << objectReader.Error() << std::endl;
			return 0xFFFF;
		}

		const auto& attrib = objectReader.GetAttrib();
		const auto hasTexcoords = !attrib.texcoords.empty();
		const auto hasNormals = !attrib.normals.empty();
		
		const auto& materials = objectReader.GetMaterials();

		std::map<std::string, uint32_t> diffuseMapCache;
		std::map<std::string, uint32_t> normalMapCache;
		std::vector<uint32_t> newMaterialIDs;
		for (auto& material : materials)
		{
			::Material newMaterial(material.name, glm::make_vec3(material.ambient),
			                       glm::make_vec3(material.diffuse),
			                       glm::make_vec3(material.specular), material.shininess);

			if (!material.diffuse_texname.empty())
			{
				auto cachedTexture = diffuseMapCache.find(material.diffuse_texname);

				if (cachedTexture != std::end(diffuseMapCache))
				{
					newMaterial.SetDiffuseTexture(cachedTexture->second);
				}
				else
				{
					auto diffuseTextureFilename = textureDir + material.diffuse_texname;
					::Texture diffuseTexture(diffuseTextureFilename);
					auto textureId = AddTexture(diffuseTexture);
					diffuseMapCache.emplace(material.diffuse_texname, textureId);
					newMaterial.SetDiffuseTexture(textureId);
				}
			}

			if (!material.normal_texname.empty())
			{
				auto cachedTexture = normalMapCache.find(material.normal_texname);

				if (cachedTexture != std::end(normalMapCache))
				{
					newMaterial.SetNormalTexture(cachedTexture->second);
				}
				else
				{
					auto normalTextureFilename = textureDir + material.normal_texname;
					::Texture normalTexture(normalTextureFilename);
					auto textureId = AddTexture(normalTexture);
					normalMapCache.emplace(material.normal_texname, textureId);
					newMaterial.SetNormalTexture(textureId);
				}
			}

			newMaterialIDs.push_back(materials_.insert(newMaterial));
		}

		for (const auto& shape : objectReader.GetShapes())
		{
			std::vector<tinyobj::real_t> attributes = {};
			std::vector<uint32_t> indices = {};

			for (auto i = 0; i < shape.mesh.indices.size(); i+=3)
			{
				auto index1 = shape.mesh.indices[i];
				auto index2 = shape.mesh.indices[i + 1];
				auto index3 = shape.mesh.indices[i + 2];

				auto vertex_idx1 = 3 * index1.vertex_index;
				auto vertex_idx2 = 3 * index2.vertex_index;
				auto vertex_idx3 = 3 * index3.vertex_index;
				
				const std::array<glm::vec3, 3> vertices = {
					glm::vec3 {attrib.vertices[vertex_idx1], attrib.vertices[vertex_idx1 + 1], attrib.vertices[vertex_idx1 + 2]},
					{attrib.vertices[vertex_idx2], attrib.vertices[vertex_idx2 + 1], attrib.vertices[vertex_idx2 + 2]},
					{attrib.vertices[vertex_idx3], attrib.vertices[vertex_idx3 + 1], attrib.vertices[vertex_idx3 + 2]}
				};

				std::array<glm::vec2, 3> texcoords;

				if (hasTexcoords)
				{
					auto texcoord_idx1 = 2 * index1.texcoord_index;
					auto texcoord_idx2 = 2 * index2.texcoord_index;
					auto texcoord_idx3 = 2 * index3.texcoord_index;
					texcoords = {
						glm::vec2{attrib.texcoords[texcoord_idx1], attrib.texcoords[texcoord_idx1 + 1]},
						{attrib.texcoords[texcoord_idx2], attrib.texcoords[texcoord_idx2 + 1]},
						{attrib.texcoords[texcoord_idx3], attrib.texcoords[texcoord_idx3 + 1]}
					};
				}

				std::array<glm::vec3, 3> normals;

				if (hasNormals)
				{
					auto normal_idx1 = 3 * index1.normal_index;
					auto normal_idx2 = 3 * index2.normal_index;
					auto normal_idx3 = 3 * index3.normal_index;
					normals = {
						glm::vec3{attrib.normals[normal_idx1], attrib.normals[normal_idx1 + 1], attrib.normals[normal_idx1 + 2]},
						{attrib.normals[normal_idx2], attrib.normals[normal_idx2 + 1], attrib.normals[normal_idx2 + 2]},
						{attrib.normals[normal_idx3], attrib.normals[normal_idx3 + 1], attrib.normals[normal_idx3 + 2]}
					};
				}

				glm::vec3 tangent;
				if (hasTexcoords && hasNormals)
				{
					const glm::vec3 edge1 = vertices[1] - vertices[0];
					const glm::vec3 edge2 = vertices[2] - vertices[0];
					const glm::vec2 deltaUV1 = texcoords[1] - texcoords[0];
					const glm::vec2 deltaUV2 = texcoords[2] - texcoords[0];
					auto f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);
					tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
					tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
					tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
					tangent = glm::normalize(tangent);

				}
				
				for (size_t v = 0; v < vertices.size(); ++v) {
					attributes.insert(attributes.end(), {vertices[v].x, vertices[v].y, vertices[v].z});

					if (hasTexcoords)
					{
						attributes.insert(attributes.end(), { texcoords[v].x, texcoords[v].y });
					}

					if (hasNormals)
					{
						attributes.insert(attributes.end(), { normals[v].x, normals[v].y, normals[v].z });
					}

					if (hasTexcoords && hasNormals)
					{
						attributes.insert(attributes.end(), { tangent.x, tangent.y, tangent.z });
					}
					indices.push_back(indices.size());
				}
			}

			assert(!attributes.empty());
			assert(!indices.empty());

			std::cout << "Attributes: " << attributes.size() << std::endl;;
			std::cout << "Indices: " << indices.size() << std::endl;

			const GLenum valueType = sizeof(tinyobj::real_t) == sizeof(double) ? GL_DOUBLE : GL_FLOAT;

			glBindVertexArray(*mesh.Vao());
			
			glBindBuffer(GL_ARRAY_BUFFER, *mesh.AttributeVbo());
			glBufferData(GL_ARRAY_BUFFER,
				attributes.size() * sizeof(float),
				attributes.data(), GL_STATIC_DRAW);
			mesh.SetNumVertices(indices.size());
			
			glVertexAttribPointer(SCENE_POSITION_ATTRIB_LOCATION, 3, valueType, GL_FALSE,
				sizeof(tinyobj::real_t) * 11, nullptr);
			glEnableVertexAttribArray(SCENE_POSITION_ATTRIB_LOCATION);
			
			glVertexAttribPointer(SCENE_TEXCOORD_ATTRIB_LOCATION, 2, valueType, GL_FALSE,
				sizeof(tinyobj::real_t) * 11, reinterpret_cast<void*>(sizeof(tinyobj::real_t) * 3));
			glEnableVertexAttribArray(SCENE_TEXCOORD_ATTRIB_LOCATION);
			
			glVertexAttribPointer(SCENE_NORMAL_ATTRIB_LOCATION, 3, valueType, GL_FALSE,
				sizeof(tinyobj::real_t) * 11, reinterpret_cast<void*>(sizeof(tinyobj::real_t) * 5));
			glEnableVertexAttribArray(SCENE_NORMAL_ATTRIB_LOCATION);

			glVertexAttribPointer(SCENE_TANGENT_ATTRIB_LOCATION, 3, valueType, GL_FALSE, sizeof(tinyobj::real_t) * 11,
			                      reinterpret_cast<void*>(sizeof(tinyobj::real_t) * 8));
			glEnableVertexAttribArray(SCENE_TANGENT_ATTRIB_LOCATION);
			
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *mesh.IndexVbo());
			glBufferData(GL_ELEMENT_ARRAY_BUFFER,
				indices.size() * sizeof(uint32_t),
				indices.data(), GL_STATIC_DRAW);
			mesh.SetNumIndices(indices.size());

			glBindVertexArray(0);

			auto numFaces = static_cast<int>(mesh.NumIndices()) / 3;
			auto currMaterialFirstFaceIndex = 0;
			auto& drawCommands = mesh.DrawCommands();
			auto& materialIDs = mesh.MaterialIDs();
			for (size_t faceIdx = 0; faceIdx < numFaces; faceIdx++)
			{
				auto isLastFace = faceIdx + 1 == numFaces;
				auto isNextFaceDifferent = isLastFace || shape.mesh.material_ids[faceIdx + 1] != shape.mesh.material_ids[faceIdx];
				if (isNextFaceDifferent)
				{
					DrawElementsIndirectCommand currDrawCommand;
					currDrawCommand.count = ((faceIdx + 1) - currMaterialFirstFaceIndex) * 3;
					currDrawCommand.primCount = 1;
					currDrawCommand.firstIndex = currMaterialFirstFaceIndex * 3;
					currDrawCommand.baseVertex = 0;
					currDrawCommand.baseInstance = 0;

					auto currMaterialID = newMaterialIDs[shape.mesh.material_ids[faceIdx]];

					drawCommands.push_back(currDrawCommand);
					materialIDs.push_back(currMaterialID);

					currMaterialFirstFaceIndex = faceIdx + 1;
				}
			}
		}

		return meshes_.insert(mesh);
	}

	uint32_t AddTransform(const ::Transform transform)
	{
		return transforms_.insert(transform);
	}

	uint32_t AddInstance(const Mesh::Instance instance)
	{
		return instances_.insert(instance);
	}

	uint32_t AddCamera(const ::Camera camera)
	{
		return cameras_.insert(camera);
	}

private:
	packed_freelist<::Texture> textures_;
	packed_freelist<::Material> materials_;
	packed_freelist<::Mesh> meshes_;
	packed_freelist<::Transform> transforms_;
	packed_freelist<Mesh::Instance> instances_;
	packed_freelist<::Camera> cameras_;

	uint32_t mainCameraId_;
};
