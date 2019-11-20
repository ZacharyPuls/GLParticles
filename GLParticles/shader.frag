in vec3 fWorldPosition;
in vec2 fTexCoord;
in vec3 fWorldNormal;
in vec3 fTangentLightPosition;
in vec3 fTangentViewPosition;
in vec3 fTangentFragPosition;

layout(location = SCENE_AMBIENT_UNIFORM_LOCATION)
uniform vec3 Ambient;

layout(location = SCENE_DIFFUSE_UNIFORM_LOCATION)
uniform vec3 Diffuse;

layout(location = SCENE_SPECULAR_UNIFORM_LOCATION)
uniform vec3 Specular;

layout(location = SCENE_SHININESS_UNIFORM_LOCATION)
uniform float Shininess;

layout(location = SCENE_HAS_DIFFUSE_MAP_UNIFORM_LOCATION)
uniform int HasDiffuseMap;

layout(binding = SCENE_DIFFUSE_MAP_TEXTURE_BINDING)
uniform sampler2D DiffuseMap;

layout(location = SCENE_HAS_NORMAL_MAP_UNIFORM_LOCATION)
uniform int HasNormalMap;

layout(binding = SCENE_NORMAL_MAP_TEXTURE_BINDING)
uniform sampler2D NormalMap;

out vec4 FragColor;

void main()
{
    vec3 Ia = vec3(0.1); // ambient light
    vec3 I0 = vec3(7); // light 0 intensity
    
    vec3 N;
    if (HasNormalMap != 0)
    {
        N = normalize(texture(NormalMap, fTexCoord).rgb * 2.0f - 1.0f);
    }
    else
    {
        N = normalize(fWorldNormal);
    }

    vec3 V = normalize(fTangentViewPosition - fTangentFragPosition);
    vec3 H = normalize(fTangentLightPosition + V);
    float G = max(0, dot(fTangentLightPosition, N));
    float PH = pow(max(0, dot(N, H)), Shininess);

    vec3 ambient = Ia * Ambient;

    vec3 diffuseMap;
    if (HasDiffuseMap != 0)
    {
        diffuseMap = texture(DiffuseMap, fTexCoord).rgb;
    }
    else
    {
        diffuseMap = vec3(1.0);
    }

    vec3 diffuse = I0 * diffuseMap * Diffuse * G;

    vec3 specular = I0 * Specular * PH;

    vec3 radiance = ambient + diffuse + specular;
    // FragColor = vec4(diffuseMap, 1.0f);
    FragColor = vec4(radiance, 1);
    // FragColor = vec4(1.0f);
}