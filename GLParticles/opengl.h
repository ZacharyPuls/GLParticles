#pragma once

#define GLFW_INCLUDE_NONE
// #define GLFW_EXPOSE_NATIVE_WIN64
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include <glad/glad.h>

typedef struct
{
	uint32_t count;
	uint32_t primCount;
	uint32_t firstIndex;
	uint32_t baseVertex;
	uint32_t baseInstance;
} DrawElementsIndirectCommand;

#define GLM_ENABLE_EXPERIMENTAL
#define GLM_FORCE_RADIANS
#include "glm/glm.hpp"
#include "glm/ext.hpp"
