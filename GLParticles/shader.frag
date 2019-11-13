#version 460
#extension GL_ARB_separate_shader_objects : enable

in vec2 fragTexCoord;
in vec3 fragNormal;

out vec4 outColor;

uniform sampler2D textureSampler;

void main() {
	outColor = texture(textureSampler, fragTexCoord);
}

