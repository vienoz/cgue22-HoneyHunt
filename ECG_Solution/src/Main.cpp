/*
* Copyright 2020 Vienna University of Technology.
* Institute of Computer Graphics and Algorithms.
* This file is part of the ECG Lab Framework and must not be redistributed.
*/
// Codename HoneyHero Polygonal Engine Copyright 2022 Julia Hofmann :)

#include <sstream>
#include <math.h> 
#include "Utils.h"
#include <thread>         // std::this_thread::sleep_for
#include <chrono>         // std::chrono::seconds
#include <random>
#include "Asset.h"
#include "GamePhysx.h"
#include "rendering/Light.h"
#include "rendering/Camera.h"
#include "rendering/ShaderNew.h"
#include "rendering/Material.h"
#include "rendering/TextHandler.h"
#include "rendering/Texture.h"
#include "models/LODModel.h"
#include "models/Model.h"
#include "models/Entity.h"
#include "models/Octtree.h"


//#include <filesystem>


/* --------------------------------------------- */
// Prototypes
/* --------------------------------------------- */

static void APIENTRY DebugCallbackDefault(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const GLvoid* userParam);
static std::string FormatDebugOutput(GLenum source, GLenum type, GLuint id, GLenum severity, const char* msg);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
glm::vec3 updateMovement();

LODModel InitLodModel(std::vector<string> modelPaths, std::shared_ptr<BaseMaterial> material,
	glm::mat4 rotation, glm::vec3 position, std::vector<physx::PxGeometry> geoms, GamePhysx physx, bool flower, objType type);

std::shared_ptr<PhysxStaticEntity> InitStaticEntity(string modelPath, std::shared_ptr<BaseMaterial> material,
	glm::mat4 rotation, glm::vec3 position, std::vector<physx::PxGeometry> geoms, GamePhysx physx, bool flower,objType type);

std::shared_ptr<PhysxDynamicEntity> InitDynamicEntity(string modelPath, std::shared_ptr<BaseMaterial> material,
	glm::mat4 rotation, glm::vec3 position, GamePhysx physx);

void GenerateTrees(uint32_t count, glm::vec2 min, glm::vec2 max, float randomMultiplier, std
	::shared_ptr<BaseMaterial> material, GamePhysx physx);

void generateFlowers(uint32_t count, glm::vec2 min, glm::vec2 max, float randomMultiplier,
	std::shared_ptr<BaseMaterial> material, GamePhysx physx);

void createFramebuffer(int width, int height, uint32_t& framebufferID, uint32_t& colorAttachmentID, uint32_t& depthAttachmentID);

/* --------------------------------------------- */
// Global variables
/* --------------------------------------------- */

static bool _wireframe = false;
static bool _culling = true;
static bool _dragging = false;
static bool _strafing = false;
static float _zoom = 15.0f;
bool keys[512];

float rectangleVertices[] = {
	1.0f, -1.0f, 1.0f, 0.0f,
   -1.0f, -1.0f, 0.0f, 0.0f,
   -1.0f,  1.0f, 0.0f, 1.0f,
   
    1.0f,  1.0f, 1.0f, 1.0f,
    1.0f, -1.0f, 1.0f, 0.0f,
   -1.0f,  1.0f, 0.0f, 1.0f,
};

Camera camera;
Octtree _octtree;
std::shared_ptr<PhysxDynamicEntity> playerEntity;
std::vector<std::shared_ptr<PhysxStaticEntity> > collisionStatics;
std::vector<std::shared_ptr<PhysxStaticEntity> > normalStatics;

int main(int argc, char** argv)
{
	INIReader reader("assets/settings.ini");

	std::string window_title = reader.Get("window", "title", "HoneyHero");
	int window_width =	reader.GetInteger("window", "width", 800);
	int window_height = reader.GetInteger("window", "height", 800);
	int refresh_rate =	reader.GetInteger("window", "refresh_rate", 60);
	bool fullscreen =	reader.GetBoolean("window", "fullscreen", false);
	float fov =	  float(reader.GetReal("camera", "fov", 60.0f));
	float nearZ = float(reader.GetReal("camera", "near", 0.1f));
	float farZ =  float(reader.GetReal("camera", "far", 1000.0f));


	// Create context
	AssetManager::init();
	glfwSetErrorCallback([](int error, const char* description) { std::cout << "GLFW error " << error << ": " << description << std::endl; });
	if (!glfwInit()) EXIT_WITH_ERROR("Failed to init GLFW");
	
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
	if (fullscreen)	monitor = glfwGetPrimaryMonitor();

	GLFWwindow* window = glfwCreateWindow(window_width, window_height, window_title.c_str(), monitor, nullptr);
	if (!window) EXIT_WITH_ERROR("Failed to create window");

	// This function makes the context of the specified window current on the calling thread. 
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true;
	GLenum err = glewInit();
	if (err != GLEW_OK)	EXIT_WITH_ERROR("Failed to init GLEW: " << glewGetErrorString(err));

	// Register debug callback
	if (glDebugMessageCallback != NULL) {
		glDebugMessageCallback(DebugCallbackDefault, NULL);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS); //get callback direct after error
	}


	if (!initFramework()) EXIT_WITH_ERROR("Failed to init framework");

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

	uint64_t ticks = 0;
	
	GamePhysx physx;
	TextHandler text;

	// Initialize scene and render loop
	{
		int counter = 0;

		// Load shader(s)
		auto celShader =		 AssetManager::getInstance()->getShader("assets/shader/texture_cel");
		auto woodShader =		 AssetManager::getInstance()->getShader("assets/shader/wood");
		auto framebufferShader = AssetManager::getInstance()->getShader("assets/shader/framebuffer");
		auto finalShader =       AssetManager::getInstance()->getShader("assets/shader/final");
		text.setUpShader(		 AssetManager::getInstance()->getShader("assets/shader/textShader"));
		auto defaultMaterial =	 AssetManager::getInstance()->defaultMaterial = std::make_shared<BaseMaterial>(celShader);

		auto defaultMaterial = AssetManager::getInstance()->defaultMaterial;
		std::shared_ptr<BaseMaterial> playerMaterial = std::make_shared<CelShadedMaterial>(celShader, AssetManager::getInstance()->getTexture("assets/textures/bee.dds"), glm::vec3(0.1f, 0.7f, 0.3f), 1.0f);
		std::shared_ptr<BaseMaterial> woodMaterial = std::make_shared<BaseMaterial>(woodShader);
		std::shared_ptr<BaseMaterial> groundMaterial = std::make_shared<CelShadedMaterial>(celShader, AssetManager::getInstance()->getTexture("assets/textures/ground_texture.dds"), glm::vec3(0.1f, 0.7f, 0.3f), 1.0f);
		std::shared_ptr<BaseMaterial> flowerMaterial = std::make_shared<CelShadedMaterial>(celShader, AssetManager::getInstance()->getTexture("assets/textures/flower_texture.dds"), glm::vec3(1.0f, 1.0f, 1.0f), 1.0f);
		std::shared_ptr<BaseMaterial> treeMaterial = std::make_shared<CelShadedMaterial>(celShader, AssetManager::getInstance()->getTexture("assets/textures/tree_texture.dds"), glm::vec3(1.0f, 1.0f, 1.0f), 1.0f);
		std::shared_ptr<OutlineShadedMaterial> outlineMaterial= std::make_shared<OutlineShadedMaterial>(framebufferProgram);
		
		//TODO: make outline testing clean
		//framebufferProgram->use();
		//glUniform1i(glGetUniformLocation(frameBufferProgram->ID, AssetManager::getInstance()->getTexture("assets/textures/white.dds")), 0);

		std::vector<physx::PxGeometry> geoms;
		std::shared_ptr<Model> fallbackModel = std::make_shared<Model>("assets/sphere.obj", defaultMaterial);
		_fallbackEntity = std::make_shared<PhysxStaticEntity>(physx, fallbackModel, geoms, false, objType::Default);

		// ----------------------------init static models--------------------
		playerEntity = InitDynamicEntity("assets/biene.obj", playerMaterial, glm::mat4(1), glm::vec3(15, 10, 0), geoms, physx);
		std::shared_ptr<PhysxStaticEntity> groundEntity = InitStaticEntity("assets/ground.obj", groundMaterial, glm::mat4(1), glm::vec3(15, 10, 0), geoms, physx, false, objType::Ground);
		std::shared_ptr<PhysxStaticEntity> stumpEntity = InitStaticEntity("assets/treeStump.obj", woodMaterial, glm::mat4(1), glm::vec3(30, 0, 0), geoms, physx, false, objType::Stump);
		
		// ----------------------------init dynamic(LOD) models--------------
		_octtree = Octtree(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1000.0f, 100.0f, 1000.0f), 4);		
		GenerateTrees(18, glm::vec2(0.0f, 0.0f), glm::vec2(100.0f,100.0f), 1.0f, treeMaterial, geoms, physx);
		generateFlowers(37, glm::vec2(0.0f, 0.0f), glm::vec2(100.0f, 100.0f), 1.0f, flowerMaterial, geoms, physx);

		std::vector<string> plantModelPaths = { "assets/potted_plant_obj.obj", "assets/potted_plant_obj_02.obj", "assets/sphere.obj" };
		_octtree.insert(OcttreeNode(InitLodModel(plantModelPaths, defaultMaterial, glm::mat4(1), glm::vec3(0, 0, 0), geoms, physx, false, objType::Default)));

		//_octtree.print();

		std::vector<string> plantModelPaths = { "assets/models/potted_plant_obj.obj", "assets/models/potted_plant_obj_02.obj", "assets/models/sphere.obj" };
		_octtree.insert(OcttreeNode(InitLodModel(plantModelPaths, defaultMaterial, glm::mat4(1), glm::vec3(0, 0, 0), physx, false, "pottedPlant")));
		
		// ----------------------------init scene----------------------------
		camera = Camera(fov, float(window_width) / float(window_height), nearZ, farZ, glm::vec3(0.0, 0.0, 7.0), glm::vec3(0.0, 1.0, 0.0));
		DirectionalLight dirL(glm::vec3(0.8f), glm::vec3(0.0f, -1.0f, -1.0f));

		//--------------------frame buffers (post processing)------------------------
		
		// Prepare framebuffer rectangle VBO and VAO
		unsigned int rectVAO, rectVBO;
		glGenVertexArrays(1, &rectVAO);
		glGenBuffers(1, &rectVBO);
		glBindVertexArray(rectVAO);
		glBindBuffer(GL_ARRAY_BUFFER, rectVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(rectangleVertices), &rectangleVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
		
		uint32_t fbo, colorAttachmentID, depthAttachmentID;
		uint32_t fbo2, colorAttachmentID2, depthAttachmentID2;
		createFramebuffer(window_width, window_height, fbo, colorAttachmentID, depthAttachmentID);
		createFramebuffer(window_width, window_height, fbo2, colorAttachmentID2, depthAttachmentID2);

		//-----------------------------------Render loop------------------------------------
		float t = float(glfwGetTime());
		float dt = 0.0f;
		float t_sum = 0.0f;
		double mouse_x, mouse_y;
		//SET TIMER
		std::clock_t time = 60;
		while (!glfwWindowShouldClose(window)) {

			// Compute frame time
			dt = t;
			t = float(glfwGetTime());
			dt = t - dt;
			t_sum += dt;
			++ticks;

			physx.getScene()->simulate(dt); //elapsed time
			physx.getScene()->fetchResults(true);

			//render buffer setup 
			glBindFramebuffer(GL_FRAMEBUFFER, fbo);
			glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
			glEnable(GL_DEPTH_TEST);
			

			// Update camera
			glfwGetCursorPos(window, &mouse_x, &mouse_y);
			camera.update(int(mouse_x), int(mouse_y), _zoom, _dragging, updateMovement());

			// draw
			playerEntity->draw(camera, dirL);
	
			if (time-t_sum > 0) {
				groundEntity->draw(camera, dirL);
				stumpEntity->draw(camera, dirL);
				_octtree.setLodIDs(playerEntity->getPosition());
				_octtree.draw(camera, dirL);
				text.drawText("current progess: " + std::to_string(counter), 25.0f, 25.0f, 1.0f, glm::vec3(0.5, 0.8f, 0.2f));
				text.drawText("countdown: " + std::to_string((int)(time - t_sum)), 230.0f, 550.0f, 1.0f, glm::vec3(0.5, 0.8f, 0.2f));
			}else {
				text.drawText("you scored: " + std::to_string(counter), 200.0f, 200.0f, 2.0f, glm::vec3(0.5, 0.8f, 0.2f));
			}

			//collision handling
			if (physx.callback.collisionObj != NULL) {
				int n = 0;
				//iterate over static objects in scene
				for (auto const& value : collisionStatics) {
					//if callback actor is same as current actor
					if (value->flowerToBeVisited == true && value->getPhysxActor() == physx.callback.collisionObj) {
						physx::PxShape* temp;
						//compare if collision is between shapes at indeces two
						value->getPhysxActor()->getShapes(&temp, 1, 2);
						if (temp == physx.callback.collisionShapes) {
							counter++;
							value->flowerToBeVisited = false;
						}
					}
					n++;
				}
			}
			physx.callback.collisionObj = NULL;
			physx.callback.collisionShapes = NULL;
			
			
			// Bind the default framebuffer
			glBindFramebuffer(GL_FRAMEBUFFER, fbo2);
			framebufferShader->use();

			glBindVertexArray(rectVAO);
			glDisable(GL_DEPTH_TEST); // prevents framebuffer rectangle from being discarded
			glDisable(GL_CULL_FACE);
			glBindTexture(GL_TEXTURE_2D, colorAttachmentID);
			glDrawArrays(GL_TRIANGLES, 0, 6);

			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			finalShader->use();
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, colorAttachmentID);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, colorAttachmentID2);
			glDrawArrays(GL_TRIANGLES, 0, 6);

 			glfwSwapBuffers(window);
			glfwPollEvents();

		
			glEnable(GL_CULL_FACE);
		}
		
		// Clean-up
		glDeleteFramebuffers(1, &fbo);
		destroyFramework();
		glfwDestroyWindow(window);
		glfwTerminate();
		AssetManager::destroy();
	}
	return EXIT_SUCCESS;
}

std::shared_ptr<PhysxStaticEntity> InitStaticEntity(string modelPath, std::shared_ptr<BaseMaterial> material, 
	glm::mat4 rotation, glm::vec3 position, std::vector<physx::PxGeometry> geoms, GamePhysx physx, bool flower, objType type)
{
	std::shared_ptr<Model> model = std::make_shared<Model>(modelPath, material);
	std::shared_ptr<PhysxStaticEntity> entity = std::make_shared<PhysxStaticEntity>(physx, model, geoms, flower, type);
	entity->setGlobalPose(glm::translate(rotation, position));
	collisionStatics.push_back(entity);
	return entity;
}

std::shared_ptr<PhysxDynamicEntity> InitDynamicEntity(string modelPath, std::shared_ptr<BaseMaterial> material,
	glm::mat4 rotation, glm::vec3 position, GamePhysx physx)
{
	auto model =  std::make_shared<Model>(modelPath, material);
	auto entity = std::make_shared<PhysxDynamicEntity>(physx, model, false);
	entity->setGlobalPose(glm::translate(rotation, position));
	return entity;
}

LODModel InitLodModel(std::vector<string> modelPaths, std::shared_ptr<BaseMaterial> material,
	glm::mat4 rotation, glm::vec3 position, std::vector<physx::PxGeometry> geoms, GamePhysx physx, bool flower, objType type)
{
	LODModel models;
	int n = 0;
	for (auto const& path : modelPaths) {
		if (n == 0) {
			models.addModel(InitStaticEntity(path, material, rotation, position, geoms, physx, flower, type));
		}
		else {
			models.addModel(InitStaticEntity(path, material, rotation, position, geoms, physx, false, objType::Default));
		}
		n++;
	}

	return models;
}

void createFramebuffer(int width, int height, uint32_t& framebufferID, uint32_t& colorAttachmentID, uint32_t& depthAttachmentID)
{
	// Create Frame Buffer Object
	glGenFramebuffers(1, &framebufferID);
	glBindFramebuffer(GL_FRAMEBUFFER, framebufferID);
	
	// Create Framebuffer Texture
	glGenTextures(1, &colorAttachmentID);
	glGenTextures(1, &depthAttachmentID);

	glBindTexture(GL_TEXTURE_2D, colorAttachmentID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // Prevents edge bleeding
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // Prevents edge bleeding

	glBindTexture(GL_TEXTURE_2D, depthAttachmentID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, width, height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);
	
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorAttachmentID, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depthAttachmentID, 0);


	// Error checking framebuffer
	auto fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (fboStatus != GL_FRAMEBUFFER_COMPLETE)
		throw new std::exception("Framebuffer error: " + fboStatus);
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

void GenerateTrees(uint32_t count, glm::vec2 min, glm::vec2 max, float randomMultiplier,
	std::shared_ptr<BaseMaterial> material, GamePhysx physx) {
	std::vector<string> treeModelPaths = { "assets/models/Lowpoly_tree_sample.obj", "assets/models/Lowpoly_tree_sample2.obj", "assets/models/sphere.obj" };

	//create advanced random values for x and y
	std::random_device dev;
	std::mt19937 rng(dev());
	std::uniform_int_distribution<std::mt19937::result_type> distX(min.x, max.x); // distribution in range [minX, maxX]
	std::uniform_int_distribution<std::mt19937::result_type> distZ(min.y, max.y); // distribution in range [minY, maxY]
	float positions[] = {-60,10,-60,50,-25,70,-20,10,-50,-80,15,-75,45,-80,90,-75,80,-55,55,-45,30,-30,80,-30,65,0,90,15,45,10,15,25,25,50,5,-45};
	for (uint32_t i = 0; i < count; ++i) {
		std::string n = "tree: " + std::to_string(i);
		_octtree.insert(OcttreeNode(InitLodModel(treeModelPaths, material,
			glm::mat4(1), glm::vec3(positions[i * 2], 0, positions[i * 2 + 1]), geoms, physx, false, objType::Tree)));
	}
}

void generateFlowers(uint32_t count, glm::vec2 min, glm::vec2 max, float randomMultiplier,
	std::shared_ptr<BaseMaterial> material, GamePhysx physx) {
	std::vector<string> flowerModelPaths = { "assets/models/Flower_Test.obj", "assets/models/Flower_Test_LOD1.obj", "assets/models/sphere.obj" };

	//create advanced random values for x and y
	std::random_device dev;
	std::mt19937 rng(dev());
	std::uniform_int_distribution<std::mt19937::result_type> distX(min.x, max.x); // distribution in range [minX, maxX]
	std::uniform_int_distribution<std::mt19937::result_type> distZ(min.y, max.y); // distribution in range [minY, maxY]

	float positions[] = {-65,-80,-40,-80,-15,-80,-25,-60,-50,-65,-70,-65,-60,-50,-35,-45,-50,-35,-65,-30,-15,-30,-35,-5,0,-30,0,-10,30,-15,50,-55,95,-40,65,-10,0,10,-30,20,-65,30,-60,65,-30,50,20,10,45,30,20,35,10,60,25,75,30,35,40,55,80,25,85,45,90,70,60,55,70,65,50,75,75,80};
	for (uint32_t i = 0; i < count; ++i) {
		std::string n = "flower: " + std::to_string(i);
		_octtree.insert(OcttreeNode(InitLodModel(flowerModelPaths, material,
			glm::mat4(1), glm::vec3(positions[i*2], 0,positions[i*2+1]), geoms, physx, true, objType::Flower)));
	}

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
			Octtree::IsLodActive = !Octtree::IsLodActive;
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
