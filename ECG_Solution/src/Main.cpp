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
#include "Model.h"
#include "Entity.h"
#include <math.h> 
#include "Asset.h"
#include <thread>         // std::this_thread::sleep_for
#include <chrono>         // std::chrono::seconds
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
glm::vec3 updateMovement();
Camera camera;


/* --------------------------------------------- */
// Global variables
/* --------------------------------------------- */

static bool _wireframe = false;
static bool _culling = true;
static bool _dragging = false;
static bool _strafing = false;
static float _zoom = 15.0f;
bool keys[512];

//std::vector<Geometry*> gameObjects;
std::shared_ptr<PhysxDynamicEntity> playerEntity;
std::vector<std::shared_ptr<PhysxDynamicEntity>> gameObjects;


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

	long int unsigned long ticks = 0;

	GamePhysx physx;


	/* --------------------------------------------- */
	// Initialize scene and render loop
	/* --------------------------------------------- */
	{
		// Load shader(s)
		std::shared_ptr<ShaderNew> textureShader = std::make_shared<ShaderNew>("assets/texture_cel.vert.glsl", "assets/texture_cel.frag.glsl");
		//std::shared_ptr<ShaderNew> textureShader = std::make_shared<ShaderNew>("assets/identity.vert.glsl", "assets/identity.frag.glsl");

		// Create textures
		//std::shared_ptr<Texture> woodTexture = AssetManager::getInstance()->getTexture("assets/textures/wood_texture.dds");
		//std::shared_ptr<Texture> tileTexture = AssetManager::getInstance()->getTexture("assets/textures/bee.dds");

		// Create materials
		//std::shared_ptr<Material> woodTextureMaterial = std::make_shared<TextureMaterial>(textureShader, glm::vec3(0.1f, 0.7f, 0.1f), 2.0f, woodTexture);
		//std::shared_ptr<Material> tileTextureMaterial = std::make_shared<TextureMaterial>(textureShader, glm::vec3(0.1f, 0.7f, 0.3f), 8.0f, tileTexture);

		AssetManager::getInstance()->defaultMaterial = std::make_shared<TextureMaterial>(textureShader, glm::vec3(0.1f, 0.7f, 0.3f), 8.0f, AssetManager::getInstance()->getTexture("assets/textures/bee.dds"));



		// Create geometry

		//Geometry cube = Geometry(glm::translate(glm::mat4(1.0f), glm::vec3(1.5f, 5.0f, 0.0f)), Geometry::createCubeGeometry(1.5f, 1.5f, 1.5f, glm::vec3(1.5f, 5.0f, 0.0f), 1.0f, phys.getMaterial(), phys.getPhysics()), woodTextureMaterial);
		//Geometry cube2 = Geometry(glm::translate(glm::mat4(1.0f), glm::vec3(1.5f, 1.0f, 0.0f)), Geometry::createCubeGeometry(1.5f, 1.5f, 1.5f, glm::vec3(1.5f, 1.0f, 0.0f), 0.0f, phys.getMaterial(), phys.getPhysics()), tileTextureMaterial);
		//Geometry player = Geometry(glm::translate(glm::mat4(1.0f), glm::vec3(-1.5f, 3.0f, 0.0f)), Geometry::createSphereGeometry(64, 32, 1.0f, glm::vec3(-1.5f, 3.0f, 0.0f), phys.getMaterial(), phys.getPhysics()), tileTextureMaterial);

		//gameObjects.push_back(&player);
		//gameObjects.push_back(&cube);
		//gameObjects.push_back(&cube2);

		//phys.getScene()->addActor(*cube.physObj);
		//phys.getScene()->addActor(*cube2.physObj);s
		//phys.getScene()->addActor(*player.physObj);
		
		std::vector<physx::PxGeometry> geoms;
		//geoms.push_back(physx::PxBoxGeometry(4.f, 4.f, 4.f));

		std::shared_ptr<Model> tree = std::make_shared<Model>("assets/Lowpoly_tree_sample.obj", textureShader);
		//std::shared_ptr<Model> teapot = std::make_shared<Model>("assets/dragön.fbx", textureShader);
		std::shared_ptr<Model> butterfliege = std::make_shared<Model>("assets/potted_plant_obj.obj", textureShader);
		std::shared_ptr<Model> player = std::make_shared<Model>("assets/biene.obj", textureShader);

		std::shared_ptr<PhysxDynamicEntity> treeEntity = std::make_shared<PhysxDynamicEntity>(physx, tree, geoms, false);
		//std::shared_ptr<PhysxDynamicEntity> teapotEntity = std::make_shared<PhysxDynamicEntity>(physx, teapot, geoms, false);
		std::shared_ptr<PhysxDynamicEntity> butterfliegeEntity = std::make_shared<PhysxDynamicEntity>(physx, butterfliege, geoms, false);
		playerEntity = std::make_shared<PhysxDynamicEntity>(physx, player, geoms, false);

		treeEntity->setGlobalPose(glm::translate(glm::mat4(1), glm::vec3(-5, 0, 0)));
		playerEntity->setGlobalPose(glm::translate(glm::mat4(1), glm::vec3(15, 10, 0)));
		//teapotEntity->setGlobalPose(glm::translate(glm::mat4(1), glm::vec3(5, 0, 0)));
		butterfliegeEntity->setGlobalPose(glm::translate(glm::mat4(1), glm::vec3(0, 0, 0)));

		gameObjects.push_back(playerEntity);
		gameObjects.push_back(treeEntity);
		gameObjects.push_back(butterfliegeEntity);

		//treeEntity->setPosition(glm::vec3(0, 0, 0));
		//treeEntity->setRotation(glm::vec3(0, 0, 0));
		//treeEntity->setScale(glm::vec3(1, 1, 1));

		// Initialize camera
		camera = Camera(fov, float(window_width) / float(window_height), nearZ, farZ, glm::vec3(0.0, 0.0, 7.0), glm::vec3(0.0, 1.0, 0.0));

		// Initialize lights
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
			//physx.getScene()->collide(dt);
			//physx.getScene()->fetchCollision(true);
			//physx.getScene()->advance();
			
			//gScene->fetchCollision(true);

			//physx.getScene()->collide(dt);
			//physx.getScene()->fetchCollision(true);
			//physx.getScene()->advance(); // Can this be skipped
			//physx.getScene()->fetchResults(true);


			// Clear backbuffer
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			// Poll events
			glfwPollEvents();

			// Update camera
			glfwGetCursorPos(window, &mouse_x, &mouse_y);
			camera.update(int(mouse_x), int(mouse_y), _zoom, _dragging, updateMovement());

			// Set per-frame uniforms
			setPerFrameUniforms(textureShader.get(), camera, dirL);

			// Render
			//gameObjects[0]->draw();
			//gameObjects[1]->draw();
			//gameObjects[2]->draw();
			
			//teapotEntity->draw(camera);
			playerEntity->draw(camera);
			treeEntity->draw(camera);
			butterfliegeEntity->draw(camera);

			if (physx.callback.collisionObj != NULL) {
				int n = 0;
				for (auto const& value : gameObjects) {
					if (value->getPhysxActor() == physx.callback.collisionObj) {
						std::cout << "collision with obj: " << n << "\n";
						//std::cout << gameObjects[n]->physObj->getName() << "\n";
						physx.callback.collisionObj = NULL;
					}
					n++;
				}
			}

			//std::cout << status << std::endl;


			// Swap buffers
 			glfwSwapBuffers(window);

			//std::this_thread::sleep_for(std::chrono::milliseconds(1000 / 60));
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


void setPerFrameUniforms(ShaderNew* shader, Camera& camera, DirectionalLight& dirL)
{
	shader->use();
	shader->setUniform(1, camera.getViewMatrix());
	shader->setUniform(2, camera.getProjMatrix());
	shader->setUniform(3, camera.getPosition());
	shader->setUniform(4, dirL.color);
	shader->setUniform(5, dirL.direction);
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


//physx::PxDefaultAllocator		gAllocator;
//physx::PxDefaultErrorCallback	gErrorCallback;

//physx::PxFoundation* gFoundation = nullptr;

//physx::PxDefaultCpuDispatcher* gDispatcher = nullptr;

//physx::PxPvd* gPvd = nullptr;


//void initPhysics()
//{
//	gFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gAllocator, gErrorCallback);
//if (!gFoundation) throw ("failed to create PxCreateFoundation");

	// for visual debugger?
//	gPvd = PxCreatePvd(*gFoundation);
//	physx::PxPvdTransport* transport = physx::PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10); //what on earth...
//	gPvd->connect(*transport, physx::PxPvdInstrumentationFlag::eALL);

	//create physics
//	gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation, physx::PxTolerancesScale(), true, gPvd);
//	if (!gPhysics) throw ("failed to create PxCreatePhysics");

	//setup scene
//	physx::PxSceneDesc sceneDesc(gPhysics->getTolerancesScale());
	//sceneDesc.gravity = physx::PxVec3(0.0f, -9.81f, 0.0f);
//	sceneDesc.gravity = physx::PxVec3(0.0f, .0f, 0.0f);
//	gDispatcher = physx::PxDefaultCpuDispatcherCreate(2);
//	sceneDesc.cpuDispatcher = gDispatcher;
//	sceneDesc.filterShader = physx::PxDefaultSimulationFilterShader;
//	gScene = gPhysics->createScene(sceneDesc);


	//scene client, aslo for debugger
//	physx::PxPvdSceneClient* pvdClient = gScene->getScenePvdClient();
//	if (pvdClient)
//	{
//		pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
//		pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
//		pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
//	}

	//create body 
//	gMaterial = gPhysics->createMaterial(0.5f, 0.5f, 0.6f);

//	physx::PxRigidStatic* groundPlane = PxCreatePlane(*gPhysics, physx::PxPlane(0, 1, 0, 0), *gMaterial);
//	gScene->addActor(*groundPlane);



	//physx::PxRigidDynamic* aBoxActor = gPhysics->createRigidDynamic(physx::PxTransform(physx::PxVec3(5.0, 15.0, 5.0)));
	//physx::PxShape* aBoxShape = physx::PxRigidActorExt::createExclusiveShape(*aBoxActor,physx::PxBoxGeometry(2 / 2, 2 / 2, 2 / 2), *gMaterial);
	///gScene->addActor(*aBoxActor);


//	std::cout << "doing physx" << std::endl;
	//run sim
	
//}
//glm::vec3 oldDirection = glm::normalize(glm::vec3(0.0, 0.0, 1.0));
glm::vec3 updateMovement() {
	//glm::vec3 newDirection = glm::normalize(camera.getCameraFoward() + camera.getCameraRight());
	glm::vec3 newDirection = glm::normalize(camera.getCameraFoward());
	
	//glm::vec3 faceDir = newDirection - oldDirection;

	//double yaw = std::atan2(faceDir.x, faceDir.z);
	//double pitch = std::atan2(std::sqrt(stuff.z * stuff.z + stuff.x * stuff.x), stuff.y) + glm::pi<float>();
	/*physx::PxTransform a = playerEntity->getPhysxActor()->getGlobalPose();
	playerEntity->getPhysxActor()->*/
	//glm::vec3 rotationAxis = glm::normalize(glm::cross(oldDirection, newDirection));

	//float rotationOnY = (glm::dot(glm::vec2(oldDirection.x, oldDirection.z), glm::vec2(newDirection.x, newDirection.z)));
	
	//rotationOnY = atan2(newDirection.y, newDirection.x) - atan2(oldDirection.y, oldDirection.x);
	
	//std::cout << newDirection.x <<" " << newDirection.z << "\n";


	//rotationOnY = acos(rotationOnY);



	//float fml = glm::cross(glm::vec2(oldDirection.x, oldDirection.z), glm::vec2(newDirection.x, newDirection.z)));

	float oida = atan(newDirection.x / newDirection.z);

	//oida = glm::radians((oida));
	std::cout << oida << "\n";

	if (keys[GLFW_KEY_W]) {
		playerEntity->getPhysxActor()->setLinearVelocity(physx::PxVec3(2*-camera.getCameraFoward().x, 2*-camera.getCameraFoward().y,2* -camera.getCameraFoward().z), true);
	}
	if (keys[GLFW_KEY_A]) {
		playerEntity->getPhysxActor()->setLinearVelocity(physx::PxVec3(-camera.getCameraRight().x, -camera.getCameraRight().y, -camera.getCameraRight().z), true);
	}
	if (keys[GLFW_KEY_S]) {
		playerEntity->getPhysxActor()->setLinearVelocity(physx::PxVec3(camera.getCameraFoward().x, camera.getCameraFoward().y, camera.getCameraFoward().z), true);
	}
	if (keys[GLFW_KEY_D]) {
		playerEntity->getPhysxActor()->setLinearVelocity(physx::PxVec3(camera.getCameraRight().x, camera.getCameraRight().y, camera.getCameraRight().z), true);
	}
	if (keys[GLFW_KEY_SPACE]) {
		playerEntity->getPhysxActor()->setLinearVelocity(physx::PxVec3(.0, 1.0, .0), true);
	}
	if (keys[GLFW_KEY_LEFT_SHIFT]) {
		playerEntity->getPhysxActor()->setLinearVelocity(physx::PxVec3(.0, -1.0, .0), true);
	}



	physx::PxVec3 position = playerEntity->getPhysxActor()->getGlobalPose().p;

	physx::PxTransform a;
	a.p = position;
	//winkel:lookat
	//a.q= physx::PxQuat(cos(glm::radians(90.0f / 2)), 0, sin(glm::radians(90.0f / 2)) * 1, 0);   bullshit
	//a.q = physx::PxQuat(0.5, physx::PxVec3(camera.getCameraFoward().x, camera.getCameraFoward().y, camera.getCameraFoward().z));
	a.q = physx::PxQuat(oida, physx::PxVec3(0.0, 1.0, 0.0));
	playerEntity->getPhysxActor()->setGlobalPose(a);

	glm::rotate(glm::mat4(1.0), 90.0f, glm::vec3(0.0, 1.0, 0.0));


	//glm::quat rot = glm::angleAxis(glm::radians(12.5), glm::vec3(.01, 1.0,0.0));

	//newDirection = oldDirection;

	return glm::vec3(position.x, position.y, position.z);
	

	//return glm::vec3(15, 10, 0);
}
