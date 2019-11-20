layout(location = SCENE_POSITION_ATTRIB_LOCATION)
in vec3 Position;

layout(location = SCENE_TEXCOORD_ATTRIB_LOCATION)
in vec2 TexCoord;

layout(location = SCENE_NORMAL_ATTRIB_LOCATION)
in vec3 Normal;

layout(location = SCENE_TANGENT_ATTRIB_LOCATION)
in vec3 Tangent;

layout(location = SCENE_MW_UNIFORM_LOCATION)
uniform mat4 MW;

layout(location = SCENE_MVP_UNIFORM_LOCATION)
uniform mat4 MVP;

layout(location = SCENE_N_MW_UNIFORM_LOCATION)
uniform mat3 N_MW;

layout(location = SCENE_CAMERAPOS_UNIFORM_LOCATION)
uniform vec3 CameraPos;

layout(location = SCENE_LIGHTPOS_UNIFORM_LOCATION)
uniform vec3 LightPos;

out vec3 fWorldPosition;
out vec2 fTexCoord;
out vec3 fWorldNormal;
out vec3 fTangentLightPosition;
out vec3 fTangentViewPosition;
out vec3 fTangentFragPosition;

void main()
{
    gl_Position = MVP * vec4(Position, 1.0f);
    fWorldPosition = (MW * vec4(Position, 1.0f)).xyz;
    fTexCoord = TexCoord;
    
    vec3 N = normalize(N_MW * Normal);
    vec3 T = normalize(N_MW * Tangent);
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T);
    mat3 TBN = transpose(mat3(T, B, N));

    fWorldNormal = N;
    fTangentLightPosition = TBN * LightPos;
    fTangentViewPosition = TBN * CameraPos;
    fTangentFragPosition = TBN * fWorldPosition;
}