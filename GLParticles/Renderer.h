#pragma once

#include "opengl.h"
#include "Scene.h"
#include "ShaderSet.h"
#include "glm/gtx/transform.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtc/type_ptr.hpp"

class Renderer
{
public:

	explicit Renderer(Scene* const scene)
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
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		auto& mainCamera = scene_->MainCamera();

		const auto& V = mainCamera.View();
		const auto& P = mainCamera.Projection();
	
		const auto VP = P * V;

		glUseProgram(*shaderProgramID_);
		
		for (auto instanceId : scene_->Instances())
		{
			const auto instance = scene_->Instance(instanceId);
			const auto mesh = scene_->Mesh(instance.MeshID);
			const auto transform = scene_->Transform(instance.TransformID);
			
			glm::mat4 MW;
			MW = glm::translate(-transform.RotationOrigin()) * MW;
			MW = glm::mat4_cast(transform.Rotation()) * MW;
			MW = glm::translate(transform.RotationOrigin()) * MW;
			MW = glm::scale(transform.Scale()) * MW;
			MW = glm::translate(transform.Translation()) * MW;

			glm::mat3 N_MW;
			N_MW = glm::mat3_cast(transform.Rotation()) * N_MW;
			N_MW = glm::mat3(glm::scale(1.0f / transform.Scale())) * N_MW;

			glm::mat4 MVP = VP * MW;

			glUniformMatrix4fv(SCENE_MW_UNIFORM_LOCATION, 1, GL_FALSE, glm::value_ptr(MW));
			glUniformMatrix3fv(SCENE_N_MW_UNIFORM_LOCATION, 1, GL_FALSE, glm::value_ptr(N_MW));
			glUniformMatrix4fv(SCENE_MVP_UNIFORM_LOCATION, 1, GL_FALSE, glm::value_ptr(MVP));

			glBindVertexArray(*mesh.Vao());
			const auto material = scene_.Material(mesh.);
			glDrawElements(GL_TRIANGLES, mesh.NumIndices(), GL_UNSIGNED_INT, nullptr);
			glBindVertexArray(0);
		}
		
	}

	void SetViewport(const int width, const int height)
	{
		viewportWidth_ = width;
		viewportHeight_ = height;
	}
	
private:
	Scene* scene_;
	bool isFirstFrame_;
	ShaderSet shaders_;
	GLuint* shaderProgramID_;

	double lastFrameTime_ = 0.0f;
	double currentFrameTime_ = 0.0f;

	int viewportWidth_ = 0;
	int viewportHeight_ = 0;
};
