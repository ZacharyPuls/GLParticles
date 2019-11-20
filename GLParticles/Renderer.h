#pragma once

#include "opengl.h"
#include "Scene.h"
#include "ShaderSet.h"

class Renderer
{
public:

	explicit Renderer(std::shared_ptr<Scene> scene)
		: scene_(scene)
	{
		shaders_.SetVersion("460");
		shaders_.SetPreambleFile("preamble.glsl");
		shaderProgramID_ = shaders_.AddProgramFromExts({ "shader.vert", "shader.frag" });
	}

	void RenderFrame()
	{
		if (lastFrameTime_ == 0.0f)
		{
			lastFrameTime_ = glfwGetTime();
		}
		currentFrameTime_ = glfwGetTime();
		auto deltaTime = currentFrameTime_ - lastFrameTime_;
		lastFrameTime_ = currentFrameTime_;

		glViewport(0, 0, viewportWidth_, viewportHeight_);
		glClearColor(100.0f / 255.0f, 149.0f / 255.0f, 237.0f / 255.0f, 1.0f);
		// glClearDepth(0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
		// glFrontFace(GL_CCW);
		// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		
		auto& mainCamera = scene_->MainCamera();

		const auto& V = mainCamera.View();
		const auto& P = mainCamera.Projection();

		const glm::mat4 VP = P * V;
		
		shaders_.UpdatePrograms();
		glUseProgram(*shaderProgramID_);

		const auto& instances = scene_->Instances();
		
		for (uint32_t instanceId : instances)
		{
			const auto instance = scene_->Instance(instanceId);
			auto mesh = scene_->Mesh(instance.MeshID);
			const auto transform = scene_->Transform(instance.TransformID);

			// std::cout << "Rendering instance " << instanceId << " of mesh with " << mesh.NumIndices() << " indices." << std::endl;
			glm::mat4 MW = glm::mat4(1.0f);
			MW = glm::translate(-transform.RotationOrigin()) * MW;
			MW = glm::mat4_cast(transform.Rotation()) * MW;
			MW = glm::translate(transform.RotationOrigin()) * MW;
			MW = glm::scale(transform.Scale()) * MW;
			MW = glm::translate(transform.Translation()) * MW;

			glm::mat3 N_MW = glm::mat4(1.0f);
			N_MW = glm::mat3_cast(transform.Rotation()) * N_MW;
			N_MW = glm::mat3(glm::scale(1.0f / transform.Scale())) * N_MW;

			glm::mat4 MVP = VP * MW;
			
			glUniformMatrix4fv(SCENE_MW_UNIFORM_LOCATION, 1, GL_FALSE, glm::value_ptr(MW));
			glUniformMatrix3fv(SCENE_N_MW_UNIFORM_LOCATION, 1, GL_FALSE, glm::value_ptr(N_MW));
			glUniformMatrix4fv(SCENE_MVP_UNIFORM_LOCATION, 1, GL_FALSE, glm::value_ptr(MVP));
			glUniform3fv(SCENE_CAMERAPOS_UNIFORM_LOCATION, 1, glm::value_ptr(mainCamera.Eye()));
			glUniform3f(SCENE_LIGHTPOS_UNIFORM_LOCATION, 0.25f, 1.0f, 0.25f);

			glBindVertexArray(*mesh.Vao());
			const auto& drawCommands = mesh.DrawCommands();
			const auto& materialIDs = mesh.MaterialIDs();
			for (size_t drawCommandIndex = 0; drawCommandIndex < drawCommands.size(); ++drawCommandIndex)
			{
				const auto& drawCommand = mesh.DrawCommand(drawCommandIndex);
				const auto& material = scene_->Material(materialIDs[drawCommandIndex]);
			
				glActiveTexture(GL_TEXTURE0 + SCENE_DIFFUSE_MAP_TEXTURE_BINDING);
				if (material.DiffuseTexture() == -1)
				{
					glBindTexture(GL_TEXTURE_2D, 0);
					glUniform1i(SCENE_HAS_DIFFUSE_MAP_UNIFORM_LOCATION, 0);
				}
				else
				{
					const auto& diffuseTexture = scene_->Texture(material.DiffuseTexture());
					diffuseTexture.Bind();
					glUniform1i(SCENE_HAS_DIFFUSE_MAP_UNIFORM_LOCATION, 1);
				}

				glActiveTexture(GL_TEXTURE0 + SCENE_NORMAL_MAP_TEXTURE_BINDING);
				if (material.NormalTexture() == -1)
				{
					glBindTexture(GL_TEXTURE_2D, 0);
					glUniform1i(SCENE_HAS_NORMAL_MAP_UNIFORM_LOCATION, 0);
				}
				else
				{
					const auto normalTexture = scene_->Texture(material.NormalTexture());
					normalTexture.Bind();
					glUniform1i(SCENE_HAS_NORMAL_MAP_UNIFORM_LOCATION, 1);
				}
			
				glUniform3fv(SCENE_AMBIENT_UNIFORM_LOCATION, 1, &material.Ambient()[0]);
				glUniform3fv(SCENE_DIFFUSE_UNIFORM_LOCATION, 1, &material.Diffuse()[0]);
				glUniform3fv(SCENE_SPECULAR_UNIFORM_LOCATION, 1, &material.Specular()[0]);
				glUniform1f(SCENE_SHININESS_UNIFORM_LOCATION, material.Shininess());
				
				glDrawElementsInstancedBaseVertexBaseInstance(
					GL_TRIANGLES,
					drawCommand.count,
					GL_UNSIGNED_INT, reinterpret_cast<GLvoid*>(sizeof(uint32_t) * drawCommand.firstIndex),
					drawCommand.primCount,
					drawCommand.baseVertex,
					drawCommand.baseInstance);
			}

			// glFinish();
			
			glBindVertexArray(0);
		}
		
	}

	void SetViewport(const int width, const int height)
	{
		viewportWidth_ = width;
		viewportHeight_ = height;
	}
	
private:
	std::shared_ptr<Scene> scene_;
	bool isFirstFrame_;
	ShaderSet shaders_;
	GLuint* shaderProgramID_;

	double lastFrameTime_ = 0.0f;
	double currentFrameTime_ = 0.0f;

	int viewportWidth_ = 0;
	int viewportHeight_ = 0;
};
