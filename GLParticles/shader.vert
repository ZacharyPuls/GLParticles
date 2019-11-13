#version 460
#extension GL_ARB_separate_shader_objects : enable

layout(location = SCENE_POSITION_ATTRIB_LOCATION) in vec3 inPosition;
layout(location = SCENE_TEXCOORD_ATTRIB_LOCATION) in vec2 inTexCoord;
layout(location = SCENE_NORMAL_ATTRIB_LOCATION) in vec3 inNormal;

out vec2 fragTexCoord;
out vec3 fragNormal;

layout(location = SCENE_MVP_UNIFORM_LOCATION) uniform mat4x4 modelViewProjection;
layout(location = SCENE_CAMERAPOS_UNIFORM_LOCATION) uniform vec3 cameraPos;

void main() {
    gl_Position = modelViewProjection * vec4(inPosition, 1.0);
	fragTexCoord = inTexCoord;
	fragNormal = inNormal;
}

