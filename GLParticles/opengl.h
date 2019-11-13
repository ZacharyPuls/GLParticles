#pragma once

#include "glad/glad.h"
#include "GLFW/glfw3.h"

typedef struct {
    uint32_t count;
    uint32_t instanceCount;
    uint32_t firstIndex;
    int baseVertex;
    uint32_t baseInstance;
} DrawElementsIndirectCommand;
