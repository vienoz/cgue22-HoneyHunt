/*
* Copyright 2020 Vienna University of Technology.
* Institute of Computer Graphics and Algorithms.
* This file is part of the ECG Lab Framework and must not be redistributed.
*/
// Codename HoneyHero Polygonal Engine Copyright 2022 Julia Hofmann :)


#include "Utils.h"
#include <sstream>
#include "Light.h"
#include "Camera.h"
#include "ShaderNew.h"
#include "Material.h"
#include "Texture.h"
#include "GamePhysx.h"
#include "LODModel.h"
#include "Model.h"
#include "Entity.h"
#include <math.h> 
#include "Asset.h"
#include "Octtree.h"
#include <thread>         // std::this_thread::sleep_for
#include <chrono>         // std::chrono::seconds
#include "TextHandler.h"
//#include <filesystem>


/* --------------------------------------------- */
// Prototypes
/* --------------------------------------------- */

static void APIENTRY DebugCallbackDefault(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const GLvoid* userParam);
static std::string FormatDebugOutput(GLenum source, GLenum type, GLuint id, GLenum severity, const char* msg);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void setPerFrameUniforms(ShaderNew* shader, Camera& camera, DirectionalLight& dirL);
float getPlayerDistance(glm::vec3 position);
std::shared_ptr<PhysxStaticEntity> getLODModel(std::vector<std::shared_ptr<PhysxStaticEntity> >, float cameraDistance);
glm::vec3 updateMovement();

LODModel InitLodModel(std::vector<string> modelPaths, std::shared_ptr<ShaderNew> shader,
	glm::mat4 rotation, glm::vec3 position, std::vector<physx::PxGeometry> geoms, GamePhysx physx);
std::shared_ptr<PhysxStaticEntity> InitStaticEntity(string modelPath, std::shared_ptr<ShaderNew> shader,
	glm::mat4 rotation, glm::vec3 position, std::vector<physx::PxGeometry> geoms, GamePhysx physx);
std::shared_ptr<PhysxDynamicEntity> InitDynamicEntity(string modelPath, std::shared_ptr<ShaderNew> shader,
	glm::mat4 rotation, glm::vec3 position, std::vector<physx::PxGeometry> geoms, GamePhysx physx);

/* --------------------------------------------- */
// Global variables
/* --------------------------------------------- */

static bool _wireframe = false;
static bool _culling = true;
static bool _dragging = false;
static bool _strafing = false;
static float _zoom = 15.0f;
bool keys[512];

Camera camera;
Octtree _octtree;
std::shared_ptr<PhysxDynamicEntity> playerEntity;
std::vector<std::shared_ptr<PhysxStaticEntity> > collisionStatics;
std::vector<std::shared_ptr<PhysxStaticEntity> > normalStatics;

std::shared_ptr<PhysxStaticEntity> _fallbackEntity;

/* --------------------------------------------- */
// Main
/* --------------------------------------------- */

#if 0
int protectedMain(int argc, char** argv);

int main(int argc, char** argv)
{
	try
	{
		protectedMain(argc, argv);
		return 0;
	}
	catch (const std::exception& e)
	{
		std::cerr << "Exception unhandled: " << e.what() << std::endl;
		return 1;
	}
	catch (...)
	{
		std::cerr << "Exception of unknown type unhandled" << std::endl;
		return 1;
	}
}
#endif

int main(int argc, char** argv)
{
	/* --------------------------------------------- */
	// Load settings.ini
	/* --------------------------------------------- */

	//std::cout<<std::filesystem::current_path()<<std::endl;

	INIReader reader("assets/settings.ini");

	int window_width = reader.GetInteger("window", "width", 800);
	int window_height = reader.GetInteger("window", "height", 800);
	int refresh_rate = reader.GetInteger("window", "refresh_rate", 60);
	bool fullscreen = reader.GetBoolean("window", "fullscreen", false);
	std::string window_title = reader.Get("window", "title", "HoneyHero");
	float fov = float(reader.GetReal("camera", "fov", 60.0f));
	float nearZ = float(reader.GetReal("camera", "near", 0.1f));
	float farZ = float(reader.GetReal("camera", "far", 100.0f));


	/* --------------------------------------------- */
	// Create context
	/* --------------------------------------------- */

	AssetManager::init();

	glfwSetErrorCallback([](int error, const char* description) { std::cout << "GLFW error " << error << ": " << description << std::endl; });

	if (!glfwInit()) {
		EXIT_WITH_ERROR("Failed to init GLFW");
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4); // Request OpenGL version 4.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // Request core profile
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);  // Create an OpenGL debug context 
	glfwWindowHint(GLFW_REFRESH_RATE, refresh_rate); // Set refresh rate
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	// Enable antialiasing (4xMSAA)
	glfwWindowHint(GLFW_SAMPLES, 4);

	// Open window
	GLFWmonitor* monitor = nullptr;

	if (fullscreen)
		monitor = glfwGetPrimaryMonitor();

	GLFWwindow* window = glfwCreateWindow(window_width, window_height, window_title.c_str(), monitor, nullptr);

	if (!window) EXIT_WITH_ERROR("Failed to create window");


	// This function makes the context of the specified window current on the calling thread. 
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true;
	GLenum err = glewInit();

	// If GLEW wasn't initialized
	if (err != GLEW_OK) {
		EXIT_WITH_ERROR("Failed to init GLEW: " << glewGetErrorString(err));
	}

	// Debug callback
	if (glDebugMessageCallback != NULL) {
		// Register your callback function.

		glDebugMessageCallback(DebugCallbackDefault, NULL);
		// Enable synchronous callback. This ensures that your callback function is called
		// right after an error has occurred. This capability is not defined in the AMD
		// version.
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	}


	/* --------------------------------------------- */
	// Init framework
	/* --------------------------------------------- */

	if (!initFramework()) {
		EXIT_WITH_ERROR("Failed to init framework");
	}

	// set callbacks
	glfwSetKeyCallback(window, key_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// set GL defaults
	glClearColor(1, 1, 1, 1);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	//Freetype
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	long int unsigned long ticks = 0;

	GamePhysx physx;

	TextHandler text;
	text.textLib;

	/* --------------------------------------------- */
	// Initialize scene and render loop
	/* --------------------------------------------- */
	{
		// Load shader(s)
		std::shared_ptr<ShaderNew> textureShader = std::make_shared<ShaderNew>("assets/texture_cel.vert.glsl", "assets/texture_cel.frag.glsl");
		text.setUpShader("assets/textShader.vert.glsl", "assets/textShader.frag.glsl");
		std::shared_ptr<TextureMaterial> playerShader = std::make_shared<TextureMaterial>(textureShader, glm::vec3(0.1f, 0.7f, 0.3f), 8.0f, AssetManager::getInstance()->getTexture("assets/textures/bee.dds"));
		auto woodShader = std::make_shared<ShaderNew>("assets/wood.vert.glsl", "assets/wood.frag.glsl");
		//auto woodMaterial = std::make_shared<Material>(woodShader, glm::vec3(0), 1.0f);
		AssetManager::getInstance()->defaultMaterial = std::make_shared<TextureMaterial>(textureShader, glm::vec3(0.1f, 0.7f, 0.3f), 8.0f, AssetManager::getInstance()->getTexture("assets/textures/bee.dds"));

		std::vector<physx::PxGeometry> geoms;
		std::shared_ptr<Model> fallbackModel = std::make_shared<Model>("assets/sphere.obj", textureShader);
		_fallbackEntity = std::make_shared<PhysxStaticEntity>(physx, fallbackModel, geoms, false);

		// ----------------------------init static models--------------------
		playerEntity = InitDynamicEntity("assets/biene.obj", textureShader, glm::mat4(1), glm::vec3(15, 10, 0), geoms, physx);

		// ----------------------------init dynamic(LOD) models--------------
		_octtree = Octtree(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(100.0f, 100.0f, 100.0f), 8);		

		std::vector<string> treeModelPaths = { "assets/Lowpoly_tree_sample.obj", "assets/Lowpoly_tree_sample2.obj"};
		_octtree.insert(OcttreeNode(InitLodModel(treeModelPaths, textureShader, glm::mat4(1), glm::vec3(-5, 2, 0), geoms, physx)));

		std::vector<string> plantModelPaths = { "assets/potted_plant_obj.obj", "assets/potted_plant_obj_02.obj" };
		_octtree.insert(OcttreeNode(InitLodModel(plantModelPaths, textureShader, glm::mat4(1), glm::vec3(0, 0, 0), geoms, physx)));

		_octtree.print();

		// ----------------------------init scene----------------------------
		camera = Camera(fov, float(window_width) / float(window_height), nearZ, farZ, glm::vec3(0.0, 0.0, 7.0), glm::vec3(0.0, 1.0, 0.0));
		DirectionalLight dirL(glm::vec3(0.8f), glm::vec3(0.0f, -1.0f, -1.0f));

		// Render loop
		float t = float(glfwGetTime());
		float dt = 0.0f;
		float t_sum = 0.0f;
		double mouse_x, mouse_y;
		glm::mat4 modelMatrix4phys;
		while (!glfwWindowShouldClose(window)) {

			// Compute frame time
			dt = t;
			t = float(glfwGetTime());
			dt = t - dt;
			t_sum += dt;
			++ticks;

			physx.getScene()->simulate(dt); //elapsed time
			physx.getScene()->fetchResults(true);

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glfwPollEvents();

			// Update camera
			glfwGetCursorPos(window, &mouse_x, &mouse_y);
			camera.update(int(mouse_x), int(mouse_y), _zoom, _dragging, updateMovement());
			setPerFrameUniforms(textureShader.get(), camera, dirL);

			// draw
			playerEntity->draw(camera);

			//maybe it is magic now TODO
			_octtree.setLodIDs(playerEntity->getPosition());
			_octtree.draw(camera);

			//plantEntity->draw(camera);

			text.drawText("doing text", 25.0f, 25.0f, 1.0f, glm::vec3(0.5, 0.8f, 0.2f));

			//collision handling
			if (physx.callback.collisionObj != NULL) {
				int n = 0;
				for (auto const& value : collisionStatics) {
					if (value->getPhysxActor() == physx.callback.collisionObj) {
						physx::PxShape* temp;
						value->getPhysxActor()->getShapes(&temp, 1, 1);
						if (temp == physx.callback.collisionShapes) {
							std::cout<<" collision with Blüte oder so" << "\n";
						}
					}
					n++;
				}
			}
			physx.callback.collisionObj = NULL;
			physx.callback.collisionShapes = NULL;

 			glfwSwapBuffers(window);
		}
	}


	/* --------------------------------------------- */
	// Destroy framework
	/* --------------------------------------------- */

	destroyFramework();


	/* --------------------------------------------- */
	// Destroy context and exit
	/* --------------------------------------------- */

	glfwTerminate();

	AssetManager::destroy();

	return EXIT_SUCCESS;
}

float getPlayerDistance(glm::vec3 position){
	float distance = glm::distance(position, playerEntity->getPosition());
	return distance;
}

void setPerFrameUniforms(ShaderNew* shader, Camera& camera, DirectionalLight& dirL)
{
	shader->use();
	shader->setUniform(1, camera.getViewMatrix());
	shader->setUniform(2, camera.getProjMatrix());
	shader->setUniform(3, camera.getPosition());
	shader->setUniform(4, dirL.color);
	shader->setUniform(5, dirL.direction);
}


std::shared_ptr<PhysxStaticEntity> InitStaticEntity(string modelPath, std::shared_ptr<ShaderNew> shader, 
	glm::mat4 rotation, glm::vec3 position, std::vector<physx::PxGeometry> geoms, GamePhysx physx)
{
	std::shared_ptr<Model> temp = std::make_shared<Model>(modelPath, shader);
	std::shared_ptr<PhysxStaticEntity> entity = std::make_shared<PhysxStaticEntity>(physx, temp, geoms, false);
	entity->setGlobalPose(glm::translate(rotation, position));
	collisionStatics.push_back(entity);
	return entity;
}

std::shared_ptr<PhysxDynamicEntity> InitDynamicEntity(string modelPath, std::shared_ptr<ShaderNew> shader,
	glm::mat4 rotation, glm::vec3 position, std::vector<physx::PxGeometry> geoms, GamePhysx physx)
{
	std::shared_ptr<Model> temp = std::make_shared<Model>(modelPath, shader);
	std::shared_ptr<PhysxDynamicEntity> entity = std::make_shared<PhysxDynamicEntity>(physx, temp, geoms, false);
	entity->setGlobalPose(glm::translate(rotation, position));
	return entity;
}

LODModel InitLodModel(std::vector<string> modelPaths, std::shared_ptr<ShaderNew> shader,
	glm::mat4 rotation, glm::vec3 position, std::vector<physx::PxGeometry> geoms, GamePhysx physx)
{
	LODModel models;
	for (auto const& path : modelPaths)
		models.addModel(InitStaticEntity(path, shader, rotation, position, geoms, physx));

	return models;
}


void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		_dragging = true;
	}
	else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
		_dragging = false;
	}
	else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
		_strafing = true;
	}
	else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE) {
		_strafing = false;
	}
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	_zoom -= float(yoffset) * 0.5f;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	// F1 - Wireframe
	// F2 - Culling
	// Esc - Exit

	if (action == GLFW_PRESS && (key > 0 && key < 512)) {
		keys[key] = true;
	}

	if (action == GLFW_RELEASE && (key > 0 && key < 512)) {
		keys[key] = false;
		playerEntity->getPhysxActor()->setLinearVelocity(physx::PxVec3(.0, .0, .0), true);
	}

	switch (action) {

	case GLFW_PRESS:

		switch (key) {

		case GLFW_KEY_ESCAPE:
			glfwSetWindowShouldClose(window, true);
			break;

		case GLFW_KEY_F1:
			_wireframe = !_wireframe;
			glPolygonMode(GL_FRONT_AND_BACK, _wireframe ? GL_LINE : GL_FILL);
			break;

		case GLFW_KEY_F2:
			_culling = !_culling;
			if (_culling) glEnable(GL_CULL_FACE);
			else glDisable(GL_CULL_FACE);
			break;

		case GLFW_KEY_L:
			_octtree.IsLodActive = !_octtree.IsLodActive;
			break;
		}
	}
}

static void APIENTRY DebugCallbackDefault(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const GLvoid* userParam) {
	if (id == 131185 || id == 131218) return; // ignore performance warnings from nvidia
	std::string error = FormatDebugOutput(source, type, id, severity, message);
	std::cout << error << std::endl;
}

static std::string FormatDebugOutput(GLenum source, GLenum type, GLuint id, GLenum severity, const char* msg) {
	std::stringstream stringStream;
	std::string sourceString;
	std::string typeString;
	std::string severityString;

	// The AMD variant of this extension provides a less detailed classification of the error,
	// which is why some arguments might be "Unknown".
	switch (source) {
	case GL_DEBUG_CATEGORY_API_ERROR_AMD:
	case GL_DEBUG_SOURCE_API: {
		sourceString = "API";
		break;
	}
	case GL_DEBUG_CATEGORY_APPLICATION_AMD:
	case GL_DEBUG_SOURCE_APPLICATION: {
		sourceString = "Application";
		break;
	}
	case GL_DEBUG_CATEGORY_WINDOW_SYSTEM_AMD:
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM: {
		sourceString = "Window System";
		break;
	}
	case GL_DEBUG_CATEGORY_SHADER_COMPILER_AMD:
	case GL_DEBUG_SOURCE_SHADER_COMPILER: {
		sourceString = "Shader Compiler";
		break;
	}
	case GL_DEBUG_SOURCE_THIRD_PARTY: {
		sourceString = "Third Party";
		break;
	}
	case GL_DEBUG_CATEGORY_OTHER_AMD:
	case GL_DEBUG_SOURCE_OTHER: {
		sourceString = "Other";
		break;
	}
	default: {
		sourceString = "Unknown";
		break;
	}
	}

	switch (type) {
	case GL_DEBUG_TYPE_ERROR: {
		typeString = "Error";
		break;
	}
	case GL_DEBUG_CATEGORY_DEPRECATION_AMD:
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: {
		typeString = "Deprecated Behavior";
		break;
	}
	case GL_DEBUG_CATEGORY_UNDEFINED_BEHAVIOR_AMD:
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: {
		typeString = "Undefined Behavior";
		break;
	}
	case GL_DEBUG_TYPE_PORTABILITY_ARB: {
		typeString = "Portability";
		break;
	}
	case GL_DEBUG_CATEGORY_PERFORMANCE_AMD:
	case GL_DEBUG_TYPE_PERFORMANCE: {
		typeString = "Performance";
		break;
	}
	case GL_DEBUG_CATEGORY_OTHER_AMD:
	case GL_DEBUG_TYPE_OTHER: {
		typeString = "Other";
		break;
	}
	default: {
		typeString = "Unknown";
		break;
	}
	}

	switch (severity) {
	case GL_DEBUG_SEVERITY_HIGH: {
		severityString = "High";
		break;
	}
	case GL_DEBUG_SEVERITY_MEDIUM: {
		severityString = "Medium";
		break;
	}
	case GL_DEBUG_SEVERITY_LOW: {
		severityString = "Low";
		break;
	}
	default: {
		severityString = "Unknown";
		break;
	}
	}

	stringStream << "OpenGL Error: " << msg;
	stringStream << " [Source = " << sourceString;
	stringStream << ", Type = " << typeString;
	stringStream << ", Severity = " << severityString;
	stringStream << ", ID = " << id << "]";

	return stringStream.str();
}

//TODO: add slight pitch when moving forward, by adding quaternions?
glm::vec3 updateMovement() {
	glm::vec3 newDirection = glm::normalize(camera.getCameraFoward());
	
	float playerDirection = atan2(newDirection.x , newDirection.z);

	if (keys[GLFW_KEY_W]) {
		playerEntity->getPhysxActor()->setLinearVelocity(physx::PxVec3(8*-camera.getCameraFoward().x, 8*-camera.getCameraFoward().y,8* -camera.getCameraFoward().z), true);
	}
	if (keys[GLFW_KEY_A]) {
		playerEntity->getPhysxActor()->setLinearVelocity(physx::PxVec3(6 * -camera.getCameraRight().x, 6 * -camera.getCameraRight().y, 6 * -camera.getCameraRight().z), true);
	}
	if (keys[GLFW_KEY_S]) {
		playerEntity->getPhysxActor()->setLinearVelocity(physx::PxVec3(6 * camera.getCameraFoward().x, 6 * camera.getCameraFoward().y, 6 * camera.getCameraFoward().z), true);
	}
	if (keys[GLFW_KEY_D]) {
		playerEntity->getPhysxActor()->setLinearVelocity(physx::PxVec3(6 * camera.getCameraRight().x, 6 * camera.getCameraRight().y, 6 * camera.getCameraRight().z), true);
	}
	if (keys[GLFW_KEY_SPACE]) {
		playerEntity->getPhysxActor()->setLinearVelocity(physx::PxVec3(.0, 4.0, .0), true);
	}
	if (keys[GLFW_KEY_LEFT_SHIFT]) {
		playerEntity->getPhysxActor()->setLinearVelocity(physx::PxVec3(.0, -4.0, .0), true);
	}

	physx::PxVec3 position = playerEntity->getPhysxActor()->getGlobalPose().p;


	//physx::PxTransform relativePose(physx::PxQuat(physx::PxHalfPi/3, physx::PxVec3(0, 0, 1)));

	physx::PxTransform a;
	a.p = position;
	a.q = physx::PxQuat(playerDirection, physx::PxVec3(0.0, 1.0, 0.0));
	playerEntity->getPhysxActor()->setGlobalPose(a);

	return glm::vec3(position.x, position.y, position.z);
	
}
