#include "opengl.h"

#define IMGUI_DISABLE_WIN32_FUNCTIONS
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>
#include <array>
#include "Scene.h"
#include "Renderer.h"

#pragma comment(lib, "glfw3dll.lib")
// #pragma comment(lib, "legacy_stdio_definitions")

/*
GLuint createShaderProgram(const char* vertexShaderSource, const char* fragmentShaderSource)
{
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, 0);

	glCompileShader(vertexShader);

	GLint isCompiled = 0;
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &isCompiled);
	if (isCompiled == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &maxLength);

		char* infoLog = new char[maxLength];
		glGetShaderInfoLog(vertexShader, maxLength, &maxLength, infoLog);

		glDeleteShader(vertexShader);

		std::cerr << "Failed to compile vertex shader: " << infoLog << std::endl;
		delete[] infoLog;
		return -1;
	}

	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(fragmentShader, 1, &fragmentShaderSource, 0);

	glCompileShader(fragmentShader);

	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &isCompiled);
	if (isCompiled == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &maxLength);

		char* infoLog = new char[maxLength];
		glGetShaderInfoLog(fragmentShader, maxLength, &maxLength, infoLog);

		glDeleteShader(fragmentShader);
		glDeleteShader(vertexShader);

		std::cerr << "Failed to compile fragment shader: " << infoLog << std::endl;
		delete[] infoLog;
		return -1;
	}

	GLuint program = glCreateProgram();

	glAttachShader(program, vertexShader);
	glAttachShader(program, fragmentShader);

	glLinkProgram(program);

	GLint isLinked = 0;
	glGetProgramiv(program, GL_LINK_STATUS, (int*)&isLinked);
	if (isLinked == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

		char* infoLog = new char[maxLength];
		glGetProgramInfoLog(program, maxLength, &maxLength, infoLog);

		glDeleteProgram(program);
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);

		std::cerr << "Failed to link shader program: " << infoLog << std::endl;
		delete[] infoLog;
		return -1;
	}

	glDetachShader(program, vertexShader);
	glDetachShader(program, fragmentShader);

	return program;
}

std::stringstream readFile(const std::string& filename)
{
	std::ifstream file(filename);

	if (!file.is_open())
	{
		throw std::runtime_error("Failed to open file with filename [" + filename + "].");
	}

	std::stringstream buffer;
	buffer << file.rdbuf();

	file.close();

	return buffer;
}

struct _particle
{
	glm::vec3 position;
	glm::vec3 color;
	glm::vec3 velocity;
	float size;
	float life = 1.0f;
	float decay;
	const float DAMPENING = 2000.0f;
	const glm::vec3 GRAVITY = { 0.0f, -0.8f, 0.0f };


	_particle() = default;

	_particle(const glm::vec3& position, const glm::vec3& color, const glm::vec3& velocity, float size, float decay)
		: position(position),
		  color(color),
		  velocity(velocity),
		  size(size),
		  decay(decay)
	{
	}


	_particle(const _particle& other)
		: position(other.position),
		  color(other.color),
		  velocity(other.velocity),
		  size(other.size),
		  life(other.life),
		  DAMPENING(other.DAMPENING)
	{
	}

	_particle(_particle&& other) noexcept
		: position(std::move(other.position)),
		  color(std::move(other.color)),
		  velocity(std::move(other.velocity)),
		  size(other.size),
		  life(other.life),
		  DAMPENING(other.DAMPENING)
	{
	}

	_particle& operator=(const _particle& other)
	{
		if (this == &other)
			return *this;
		position = other.position;
		color = other.color;
		velocity = other.velocity;
		size = other.size;
		life = other.life;
		return *this;
	}

	_particle& operator=(_particle&& other) noexcept
	{
		if (this == &other)
			return *this;
		position = std::move(other.position);
		color = std::move(other.color);
		velocity = std::move(other.velocity);
		size = other.size;
		life = other.life;
		return *this;
	}

	void Update(const float deltaTime)
	{
		position += (velocity * deltaTime) / DAMPENING;
		velocity += (GRAVITY * deltaTime) / DAMPENING;
		life -= decay * deltaTime;
		color = glm::vec3(1.0f, glm::mix(0.0f, 1.0f, life * 0.5f), 0.0f);
		if (life <= 0.0f)
		{
			Reset();
		}
	}

	void Reset()
	{
		position = glm::vec3(0.0f, 0.0f, 0.0f);
		color = glm::vec3(1.0f, 1.0f, 0.0f);
		// velocity = glm::vec3(float((rand() % 60) - 32.0f), float((rand() % 60) - 30.0f), float((rand() % 60) - 30.0f));
		// velocity = glm::vec3(0.0f);
		// velocity = glm::vec3(float((rand() % 50) - 26.0f) * 10.0f, float((rand() % 50) - 25.0f) * 10.0f, float((rand() % 50) - 25.0f) * 10.0f);
		velocity = glm::ballRand(5.0f);
		// velocity = glm::normalize(glm::vec3(((rand() % 20) - 10) / 50.0f, 1.0f, ((rand() % 20) - 10) / 50.0f));
		life = 1.0f;
		decay = (float(rand() % 100) / 1000.0f + 0.003f) * 0.5f;
		size = 0.02f;
	}
	
	std::array<glm::vec4, 6> GetColor() const
	{
		return {
			glm::vec4(color, life),
			glm::vec4(color, life),
			glm::vec4(color, life),
			glm::vec4(color, life),
			glm::vec4(color, life),
			glm::vec4(color, life)
		};
	}

	std::array<glm::vec3, 6> GetVertices()
	{
		auto halfSize = size / 2.0f;
		auto x0 = glm::vec3{ position.x - halfSize, position.y - halfSize, position.z };
		auto x1 = glm::vec3{ position.x + halfSize, position.y - halfSize, position.z };
		auto x2 = glm::vec3{ position.x + halfSize, position.y + halfSize, position.z };
		auto x3 = glm::vec3{ position.x - halfSize, position.y + halfSize, position.z };
		
		return {
			x0, x1, x3,
			x1, x2, x3
		};
	}

	std::array<glm::vec2, 6> GetTexCoords()
	{
		return {
			glm::vec2{0.0f, 0.0f},
			glm::vec2{1.0f, 0.0f},
			glm::vec2{0.0f, 1.0f},
			glm::vec2{1.0f, 0.0f},
			glm::vec2{1.0f, 1.0f},
			glm::vec2{0.0f, 1.0f}
		};
	}
};

struct _particleEffect
{
	glm::vec3 position;
	glm::vec3 initialColor;
	glm::vec3 endColor;
	float colorFalloff;
	float particleSize;
	int numParticles;
	float (*decayFunc)();
	glm::vec3(*velocityFunc)();

	std::vector<_particle> particles;
	std::array<GLuint, 3> buffers;

	_particleEffect(const glm::vec3& position, const glm::vec3& initialColor, const glm::vec3& endColor,
		float colorFalloff, float particleSize, int numParticles, float(* decayFunc)(), glm::vec3(* velocityFunc)())
		: position(position),
		  initialColor(initialColor),
		  endColor(endColor),
		  colorFalloff(colorFalloff),
		  particleSize(particleSize),
		  numParticles(numParticles),
		  decayFunc(decayFunc),
		  velocityFunc(velocityFunc)
	{
		particles.reserve(numParticles);
		glGenBuffers(3, &buffers[0]);
	}

	void Update(float deltaTime)
	{
		std::sort(particles.begin(), particles.end(), [](_particle a, _particle b)
			{
				return a.GetVertices()[0].z < b.GetVertices()[0].z;
			});
		std::vector<glm::vec3> vertices;
		vertices.reserve(6 * numParticles);
		std::vector<glm::vec4> colors;
		colors.reserve(6 * numParticles);
		std::vector<glm::vec2> texCoords;
		texCoords.reserve(6 * numParticles);
		for (auto& particle : particles)
		{
			particle.Update(deltaTime);
			auto vert = particle.GetVertices();
			auto col = particle.GetColor();
			auto coords = particle.GetTexCoords();
			vertices.insert(vertices.end(), vert.begin(), vert.end());
			colors.insert(colors.end(), col.begin(), col.end());
			texCoords.insert(texCoords.end(), coords.begin(), coords.end());
		}
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0]) * vertices.size(), &vertices[0], GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, cbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(colors[0]) * colors.size(), &colors[0], GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, tbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(texCoords[0]) * texCoords.size(), &texCoords[0], GL_DYNAMIC_DRAW);
	}
};

const int numParticles = 100;
std::array<_particle, numParticles> particles;

void resetParticles()
{
	srand(time(NULL));
	for (auto& particle : particles)
	{
		particle.Reset();
	}
}

void updateParticles(const float deltaTime, const GLuint vbo, const GLuint cbo, const GLuint tbo)
{
	std::sort(particles.begin(), particles.end(), [](_particle a, _particle b)
	{
			return a.GetVertices()[0].z < b.GetVertices()[0].z;
	});
	std::vector<glm::vec3> vertices;
	vertices.reserve(6 * numParticles);
	std::vector<glm::vec4> colors;
	colors.reserve(6 * numParticles);
	std::vector<glm::vec2> texCoords;
	texCoords.reserve(6 * numParticles);
	for (auto& particle : particles)
	{
		particle.Update(deltaTime);
		auto vert = particle.GetVertices();
		auto col = particle.GetColor();
		auto coords = particle.GetTexCoords();

		// std::cout << "vert: [" << vert[0].x << "," << vert[0].y << "," << vert[0].z << ";" << vert[1].x << "," << vert[1].y << "," << vert[1].z << ";" << vert[2].x << "," << vert[2].y << "," << vert[2].z << ";"
			// << vert[3].x << "," << vert[3].y << "," << vert[3].z << ";" << vert[4].x << "," << vert[4].y << "," << vert[4].z << ";" << vert[5].x << "," << vert[5].y << "," << vert[5].z << "]" << std::endl;
		// std::cout << "col: [" << col[0].x << "," << col[0].y << "," << col[0].z << ";" << col[1].x << "," << col[1].y << "," << col[1].z << ";" << col[2].x << "," << col[2].y << "," << col[2].z << ";"
			// << col[3].x << "," << col[3].y << "," << col[3].z << ";" << col[4].x << "," << col[4].y << "," << col[4].z << ";" << col[5].x << "," << col[5].y << "," << col[5].z << "]" << std::endl;
		// std::cout << "tex: [" << coords[0].x << "," << coords[0].y << "," << ";" << coords[1].x << "," << coords[1].y << "," << ";" << coords[2].x << "," << coords[2].y << "," << ";"
			// << coords[3].x << "," << coords[3].y << "," << ";" << coords[4].x << "," << coords[4].y << "," << ";" << coords[5].x << "," << coords[5].y << "," << "]" << std::endl;
		
		vertices.insert(vertices.end(), vert.begin(), vert.end());
		colors.insert(colors.end(), col.begin(), col.end());
		texCoords.insert(texCoords.end(), coords.begin(), coords.end());
	}                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0]) * vertices.size(), &vertices[0], GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, cbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(colors[0]) * colors.size(), &colors[0], GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, tbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(texCoords[0]) * texCoords.size(), &texCoords[0], GL_DYNAMIC_DRAW);
}
*/

void GLAPIENTRY
MessageCallback(GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar* message,
	const void* userParam)
{
	fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
		(type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
		type, severity, message);
}

std::shared_ptr<Scene> scene;
std::shared_ptr<Renderer> renderer;

void resize(GLFWwindow* window, int width, int height)
{
	scene->MainCamera().SetAspect(static_cast<float>(width) / static_cast<float>(height));
	renderer->SetViewport(width, height);
}

int main(int argc, char** argv)
{
	glfwInit();
	auto initialWidth = 640;
	auto initialHeight = 480;
	auto window = glfwCreateWindow(initialWidth, initialHeight, "GL Particles!", nullptr, nullptr);
	glfwMakeContextCurrent(window);
	gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress));

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 460");
	ImGui::StyleColorsDark();
	
	glfwSwapInterval(1);

	glfwSetFramebufferSizeCallback(window, resize);
	
	// glEnable(GL_DEBUG_OUTPUT);
	// glDebugMessageCallback(MessageCallback, 0);

	scene.reset(new Scene());
	
	renderer.reset(new Renderer(scene));

	const auto cubeMesh = scene->AddMesh("cube/cube.obj", "", "cube/");

	constexpr int MAX_X = 1;
	constexpr int MAX_Z = 1;
	
	for (auto x = 0; x < MAX_X; ++x)
	{
		for (auto z = 0; z < MAX_Z; ++z)
		{			
			const auto cubeTransform = scene->AddTransform({ {1.0f, 1.0f, 1.0f}, {}, {}, {-(MAX_X - 1) + (static_cast<float>(x) * 1.25f), 0.0f, -(MAX_Z - 1)+ (static_cast<float>(z) * 1.25f)} });
			const auto cubeInstance = scene->AddInstance({ cubeMesh, cubeTransform });
		}
	}
	

	const auto mainCamera = scene->AddCamera({
		{2.0f, 1.5f, 2.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, glm::radians(70.0f), {}, 0.1f,
		200.0f
	});
	scene->SetMainCameraId(mainCamera);

	resize(window, initialWidth, initialHeight);

	auto materialAmbient = glm::vec3(1.0f);
	auto materialDiffuse = glm::vec3(1.0f);
	auto materialSpecular = glm::vec3(0.2f);
	while (!glfwWindowShouldClose(window))
	{
		auto& material = scene->Material(scene->Mesh(cubeMesh).MaterialIDs()[0]);
		material.SetAmbient(materialAmbient);
		material.SetDiffuse(materialDiffuse);
		material.SetSpecular(materialSpecular);
		renderer->RenderFrame();
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		ImGui::SetNextWindowPos({ 0.0f, 0.0f });
		ImGui::Begin("Demo window", {}, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav);
		ImGui::ColorPicker3("Ambient", glm::value_ptr(materialAmbient), ImGuiColorEditFlags_Float);
		ImGui::ColorPicker3("Diffuse", glm::value_ptr(materialDiffuse), ImGuiColorEditFlags_Float);
		ImGui::ColorPicker3("Specular", glm::value_ptr(materialSpecular), ImGuiColorEditFlags_Float);
		ImGui::End();
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}
