/*
* Copyright 2020 Vienna University of Technology.
* Institute of Computer Graphics and Algorithms.
* This file is part of the ECG Lab Framework and must not be redistributed.
*/


#include "Utils.h"
#include <sstream>
#define _USE_MATH_DEFINES
#include <fstream>
#include <vector>
#include <glm/gtc/type_ptr.hpp>
#include <math.h> 


/* --------------------------------------------- */
// Prototypes
/* --------------------------------------------- */

void APIENTRY DebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const GLvoid* userParam);

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

static std::string FormatDebugOutput(GLenum source, GLenum type, GLuint id, GLenum severity, const char* msg);

static std::string readShader(const std::string& filepath); //loads shaders from file

unsigned int prepareShader(const std::string& vertPath, const std::string& fragPath); //initializes an compiles shader program

DDSImage prepareTexture(const char* path);

static glm::mat4 lookAt(glm::vec3 eye, glm::vec3 target, glm::vec3 up); // calculates view matrix

void mouseMove(GLFWwindow* window, double xpos, double ypos);

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset); //mousewheel zoom

void mouseClick(GLFWwindow* window, int button, int action, int mods);

void generateCube(float height, float depth, float width);

void generateCylinder(int segCount, float heigh, float radius);

void generateSphere(float radius, int sectors, int stacks);

void drawSphere();

void drawSphere_g();

void drawCylinder();

void drawCube();




/* --------------------------------------------- */
// Global variables
/* --------------------------------------------- */
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 6.0f); // camera position
glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);  // camera focus point
glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f); //positive y
float cameraDistance = (glm::length(cameraPos - cameraTarget)); //distance between camera and focus point
float yaw; //camera side movement
float pitch; //camera up/down movement
float lastX = 400, lastY = 400; //set 0 0 to break everything for ever...

//task3 variables
unsigned int VBOcube;
unsigned int cubeNormals;
unsigned int VAOcube;
unsigned int EBOcube;

unsigned int VBOcyl;
unsigned int cylNormals;
unsigned int VAOcyl;
unsigned int EBOcyl;

unsigned int VBOsph;
unsigned int sphNormals;
unsigned int VAOsph;
unsigned int EBOsph;

unsigned int VBOsph_g;
unsigned int sphNormals_g;
unsigned int VAOsph_g;
unsigned int EBOsph_g;

float twoPi = 2 * M_PI;
boolean backF = true;
boolean wireF = false;
std::vector <int> cyIndex;
std::vector <int> cubeIndex;
std::vector <int> sphIndex;

//task4 variables
unsigned int VAOLightP;
unsigned int VBOLightP;

unsigned int lightShader;
unsigned int shaderProgram;

glm::vec3 lightPos(0.0f, 0.0f, 0.0f);
int displaySpot = 0;

//task5 variables
int texUnit = 0;

/* --------------------------------------------- */
// Main
/* --------------------------------------------- */


int main(int argc, char** argv)
{
	/* --------------------------------------------- */
	// Load settings.ini
	/* --------------------------------------------- */
	//init reader for ini files
	INIReader reader("assets/settings.ini");

	// load window size
	int width = reader.GetInteger("window", "width", 800);
	int height = reader.GetInteger("window", "height", 800);

	// load camera informatrion for projection matrix
	float fov = reader.GetInteger("camera", "fov", 800);
	float nearPlane = reader.GetReal("camera", "near", 800);
	float farPlane = reader.GetReal("camera", "far", 800);
	// load window name
	std::string window_title = reader.Get("window", "title", "ECG 2020");

	/* --------------------------------------------- */
	// Init framework
	/* --------------------------------------------- */
	if (!glfwInit())
		EXIT_WITH_ERROR("fehler bei glfwInit()");


#ifdef _DEBUG
	//Create a debug OpenGL context or tell your OpenGL library to do so.
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#endif
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);




	GLFWwindow* window = glfwCreateWindow(width, height, window_title.c_str(), nullptr, nullptr);
	if (!window) {
		std::cout << "failed to open window" << std::endl;
		glfwTerminate();
	}
	glfwMakeContextCurrent(window);
	glewExperimental = true;
	GLenum err = glewInit();
	if (err != GLEW_OK) {
		EXIT_WITH_ERROR("glewinit() failed")
	}



#if _DEBUG
	// Register your callback function.
	glDebugMessageCallback(DebugCallback, NULL);
	// Enable synchronous callback. This ensures that your callback function is called
	// right after an error has occurred. 
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
#endif

	if (!initFramework()) {
		EXIT_WITH_ERROR("Failed to init framework");
	}

	//Initialize shaders
	shaderProgram = prepareShader("assets/task_1_VertShader.txt", "assets/task_1_FragShader.txt");
	lightShader = prepareShader("assets/gouraud_vertShader.txt", "assets/gouraud_fragShader.txt");
	glUseProgram(shaderProgram);
	//glUniformMatrix4fv(2, 1, GL_FALSE, glm::value_ptr(projection));

	//glUniform4f(4, 0.2f, 0.1f, 0.2f, 1.0f); //light color

	prepareTexture("assets/textures/tiles_diffuse.dds"); //0
	prepareTexture("assets/textures/wood_texture.dds");  //1
	prepareTexture("assets/textures/wood_texture_specular.dds"); //2
	prepareTexture("assets/textures/tiles_specular.dds"); //3
	glUniform1i(9, 3); //specMap

	//declares and initialized model matrices
	glm::mat4 modelCube = glm::mat4(1.0f);
	glm::mat4 modelCylinder = glm::mat4(1.0f);
	glm::mat4 modelSphere1 = glm::lowp_mat4(1.0f);
	glm::mat4 modelSphere2 = glm::lowp_mat4(1.0f);

	modelCube = glm::translate(modelCube, glm::vec3(0.0f, -1.5f, 0.0f));

	modelCylinder = glm::translate(modelCylinder, glm::vec3(1.5f, 1.0f, 0.0f));

	modelSphere1 = glm::translate(modelSphere1, glm::vec3(-1.5f, 1.0f, 0.0f));

	modelSphere2 = glm::translate(modelSphere2, glm::vec3(1.2f, 1.0f, 0.0f));

	//viewmatrix
	glm::mat4 view;

	//projectiomatrix
	glm::mat4 projection;
	projection = glm::perspective(glm::radians(fov), float(width) / float(height), nearPlane, farPlane);
	glUniformMatrix4fv(2, 1, GL_FALSE, glm::value_ptr(projection));

	glUseProgram(lightShader);
	glUniformMatrix4fv(2, 1, GL_FALSE, glm::value_ptr(projection));
	glUseProgram(shaderProgram);

	//declare VAO, EBO, VBO for each geometry;
	glGenVertexArrays(1, &VAOcube);
	glGenBuffers(1, &VBOcube);
	glGenBuffers(1, &EBOcube);

	glGenVertexArrays(1, &VAOcyl);
	glGenBuffers(1, &VBOcyl);
	glGenBuffers(1, &EBOcyl);

	glGenVertexArrays(1, &VAOsph);
	glGenBuffers(1, &VBOsph);
	glGenBuffers(1, &EBOsph);

	glUseProgram(lightShader);
	glGenVertexArrays(1, &VAOsph_g);
	glGenBuffers(1, &VBOsph_g);
	glGenBuffers(1, &EBOsph_g);
	glUseProgram(shaderProgram);

	/* --------------------------------------------- */
	// Initialize scene and render loop
	/* --------------------------------------------- */



	glEnable(GL_DEPTH_TEST);
	glViewport(0, 0, width, height);
	glfwSetKeyCallback(window, key_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetCursorPosCallback(window, mouseMove);
	glfwSetMouseButtonCallback(window, mouseClick);

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glEnable(GL_CULL_FACE);

	generateSphere(1, 64, 32);
	generateCylinder(32, 1.3, 1.0);
	generateCube(1.5f, 1.5f, 1.5f);

	int nrAttributes;
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);
	std::cout << "Maximum nr of vertex attributes supported: " << nrAttributes << std::endl;

	float cutOff = glm::cos(glm::radians(8.0f));
	float outerCutOff = glm::cos(glm::radians(10.0f));

	glUniform3f(5, lightPos.x, lightPos.y, lightPos.z); // pointlight pos
	glUniform3f(4, 1.0f, 1.0f, 1.0f); //light color
	glUniform1f(11, 1.0f); //point constant
	glUniform1f(12, 0.4f); //point linear
	glUniform1f(13, 0.1f); //point quadratic

	glUniform3f(14, 0.8f, 0.8f, 0.8f); //direction color
	glUniform3f(15, 0.0f, -1.0f, -1.0f); //direction  direction
	glUniform1ui(20, displaySpot);

	//spotlight
	glUniform1f(16, cutOff);
	glUniform3f(17, 0.0f, 0.0f, 1.0f); //direction
	glUniform3f(18, -0.0f, -1.0f, -12.0f);
	glUniform1f(19, outerCutOff);

	glUseProgram(lightShader);
	glUniform3f(5, lightPos.x, lightPos.y, lightPos.z);
	glUniform3f(4, 1.0f, 1.0f, 1.0f); //light color
	glUniform1f(11, 1.0f); //point constant
	glUniform1f(12, 0.4f); //point linear
	glUniform1f(13, 0.1f); //point quadratic

	glUniform3f(14, 0.8f, 0.8f, 0.8f); //direction color
	glUniform3f(15, 0.0f, -1.0f, -8.0f); //direction  direction
	glUniform1ui(20, displaySpot);

	//spotlight
	glUniform1f(16, cutOff);
	glUniform3f(17, 0.0f, 0.0f, 1.0f);
	glUniform3f(18, -0.0f, -1.0f, -12.0f);
	glUniform1f(19, outerCutOff);
	glUseProgram(shaderProgram);


	while (!glfwWindowShouldClose(window)) {

		// clear the colorbuffer
		glClearColor(1, 1, 1, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//provokes error for debug testing
		//glTranslatef(0.0f, 0.0f, 0.0f); 

		//calculates view matrix and passes it to vertex shader
		view = lookAt(cameraPos, cameraTarget, up);
		glUniformMatrix4fv(1, 1, GL_FALSE, glm::value_ptr(view));
		glm::vec3 ding = cameraPos - cameraTarget;
		glUniform3f(6, ding.x, ding.y, ding.z);

		glUseProgram(lightShader);
		glUniformMatrix4fv(1, 1, GL_FALSE, glm::value_ptr(view));
		glUniform3f(6, ding.x, ding.y, ding.z);
		glUseProgram(shaderProgram);

		//draw Cylinder
		glUniformMatrix4fv(0, 1, GL_FALSE, glm::value_ptr(modelCylinder));
		glUniform1i(3, 0); //texture
		drawCylinder();

		//draw Cube
		glUniform1i(9, 2); //specMap
		glUniformMatrix4fv(0, 1, GL_FALSE, glm::value_ptr(modelCube));
		glUniform1i(3, 1); //texture
		drawCube();

		//draw Sphere_left
		glUniform1i(9, 3); //specMap
		glUniformMatrix4fv(0, 1, GL_FALSE, glm::value_ptr(modelSphere1));
		glUniform1i(3, 0); //texture
		drawSphere();

		glUniform1ui(20, displaySpot);
		//glUseProgram(lightShader);
		//glUniform1ui(20, displaySpot);
		//draw Sphere_right
		//glUniformMatrix4fv(0, 1, GL_FALSE, glm::value_ptr(modelSphere2));
		//glUniform3f(3, 1.0f, 0.0f, 0.0f); //RGBA
		//drawSphere_g();
		//glUseProgram(shaderProgram);


		// swap buffers and poll IO events
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	/* --------------------------------------------- */
	// Destroy framework
	/* --------------------------------------------- */

	destroyFramework();


	/* --------------------------------------------- */
	// Destroy context and exit
	/* --------------------------------------------- */
	glfwDestroyWindow(window);




	return EXIT_SUCCESS;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}

	if (key == GLFW_KEY_F3 && action == GLFW_PRESS) {
		if (displaySpot == 0) {
			displaySpot = 1;
			return;
		}
		if (displaySpot == 1) {
			displaySpot = 0;
		}
	}

	if (key == GLFW_KEY_F1 && action == GLFW_PRESS) {
		if (wireF == false) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}
		if (wireF == true) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
		wireF = !wireF;
	}

	if (key == GLFW_KEY_F2 && action == GLFW_PRESS) {
		if (backF == false) {
			glEnable(GL_CULL_FACE);;
		}
		if (backF == true) {
			glDisable(GL_CULL_FACE);;
		}
		backF = !backF;
	}
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {

	const float cameraSpeed = 0.05f * yoffset;
	cameraPos.x = cameraPos.x * (1 - cameraSpeed);
	cameraPos.y = cameraPos.y * (1 - cameraSpeed);
	cameraPos.z = cameraPos.z * (1 - cameraSpeed);

	cameraDistance = (glm::length(cameraPos - cameraTarget));
}

void APIENTRY DebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const GLvoid* userParam) {
	// if (id == 131185 || id == 131218) return; // get rid of unesseccary warnings
	std::string error = FormatDebugOutput(source, type, id, severity, message);
	std::cout << error << std::endl; //set breakpoint here to debug
}

static std::string FormatDebugOutput(GLenum source, GLenum type, GLuint id, GLenum severity, const char* msg) {
	std::stringstream stringStream;
	std::string sourceString;
	std::string typeString;
	std::string severityString;

	switch (source) {
	case GL_DEBUG_SOURCE_API: {
		sourceString = "API";
		break;
	}
	case GL_DEBUG_SOURCE_APPLICATION: {
		sourceString = "Application";
		break;
	}
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM: {
		sourceString = "Window System";
		break;
	}
	case GL_DEBUG_SOURCE_SHADER_COMPILER: {
		sourceString = "Shader Compiler";
		break;
	}
	case GL_DEBUG_SOURCE_THIRD_PARTY: {
		sourceString = "Third Party";
		break;
	}
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
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: {
		typeString = "Deprecated Behavior";
		break;
	}
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: {
		typeString = "Undefined Behavior";
		break;
	}
	case GL_DEBUG_TYPE_PORTABILITY_ARB: {
		typeString = "Portability";
		break;
	}
	case GL_DEBUG_TYPE_PERFORMANCE: {
		typeString = "Performance";
		break;
	}
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

static std::string readShader(const std::string& filepath) {

	std::stringstream result;
	std::ifstream file(filepath);

	if (file.is_open())
	{
		//std::cout << "shaderfile successfully opened" << std::endl;
		std::string line;
		while (getline(file, line))
		{
			result << line << '\n';

		}
		file.close();
	}
	else
		std::cout << "shaderfile not found" << std::endl;
	return result.str();


}

unsigned int prepareShader(const std::string& vertPath, const std::string& fragPath) {
	//creates empty shader and returns handle
	unsigned int vertShader = glCreateShader(GL_VERTEX_SHADER);
	unsigned int fragShader = glCreateShader(GL_FRAGMENT_SHADER);

	//read from file 
	std::string vertexShaderSource = readShader(vertPath);
	std::string fragmentShaderSource = readShader(fragPath);
	const char* vertShaderSrc = vertexShaderSource.c_str();
	const char* fragShaderSrc = fragmentShaderSource.c_str();
	int length;

	// shader compilation and checking
	int success;
	GLint logSize;

	//-> Vertex shader
	glShaderSource(vertShader, 1, &vertShaderSrc, NULL);
	glCompileShader(vertShader);

	glGetShaderiv(vertShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderiv(vertShader, GL_INFO_LOG_LENGTH, &logSize);
		GLchar* errLog = new char[logSize];

		glGetShaderInfoLog(vertShader, logSize, NULL, errLog);
		std::cout << "ERROR: VERTEX SHADER COMPILATION FAILED: " << errLog << std::endl;
		delete[] errLog;
	}

	//-> Fragment shader
	glShaderSource(fragShader, 1, &fragShaderSrc, NULL);
	glCompileShader(fragShader);

	glGetShaderiv(fragShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderiv(fragShader, GL_INFO_LOG_LENGTH, &logSize);
		GLchar* errLog = new char[logSize];

		glGetShaderInfoLog(fragShader, logSize, NULL, errLog);  // why is &errLog invalid?
		std::cout << "ERROR: FRAGMENT SHADER COMPILATION FAILED: " << errLog << std::endl;
		delete[] errLog;
	}
	//linking shader programs and cheching success
	unsigned int shaderProg = glCreateProgram();
	glAttachShader(shaderProg, vertShader);
	glAttachShader(shaderProg, fragShader);
	glLinkProgram(shaderProg);

	glGetProgramiv(shaderProg, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramiv(shaderProg, GL_INFO_LOG_LENGTH, &logSize);
		GLchar* errLog = new char[logSize];

		glGetProgramInfoLog(shaderProg, logSize, NULL, errLog);
		std::cout << "ERROR: SHADER LINKING FAILED: " << errLog << std::endl;
		delete[] errLog;
	}

	glDeleteShader(vertShader);
	glDeleteShader(fragShader);
	return shaderProg;
}

DDSImage prepareTexture(const char* path)
{
	unsigned int texHandle;
	DDSImage img = loadDDS(path);

	glGenTextures(1, &texHandle); // generate texture
	glActiveTexture(GL_TEXTURE0 + texUnit); //activate before binding; GL_TEXTURE0 is activated by default
	glBindTexture(GL_TEXTURE_2D, texHandle); //binding texture
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); //GL_NEAREST //magnification filter
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // minification filter:  mipmap linear interpolation
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);  // behaviour if tex coordinates >1 or <0
	glCompressedTexImage2D(GL_TEXTURE_2D, 0, GL_COMPRESSED_RGBA_S3TC_DXT1_EXT, img.width, img.height, 0, img.size, img.data); 
	glGenerateMipmap(GL_TEXTURE_2D);
	texUnit++;



	return DDSImage();
}

static glm::mat4 lookAt(glm::vec3 eye, glm::vec3 target, glm::vec3 up) { //eye: cameraPosition, target: cameraTarget, ip: up

	glm::vec3 zaxis = glm::normalize(eye - target);                 //camerDirection
	glm::vec3 xaxis = glm::normalize(glm::cross(up, zaxis));        // normal of global y and straight foward is cameraRight/ cameraX
	glm::vec3 yaxis = glm::normalize(glm::cross(zaxis, xaxis));     // normal of right and straight foward is cameraUp/cameraY


	glm::mat4 orientation = {
		glm::vec4(xaxis.x, yaxis.x, zaxis.x, 0),
		glm::vec4(xaxis.y, yaxis.y, zaxis.y, 0),
		glm::vec4(xaxis.z, yaxis.z, zaxis.z, 0),
		glm::vec4(0,       0,       0,     1)
	};

	glm::mat4 translation = {
		glm::vec4(1,0,0,0),
		glm::vec4(0,1,0,0),
		glm::vec4(0,0,1,0),
		glm::vec4(-eye.x, -eye.y, -eye.z, 1)
	};

	return (orientation * translation);
}

void mouseMove(GLFWwindow* window, double xpos, double ypos) {
	int stateL = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
	int stateR = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT);

	if (stateR == 1) {
		float xoffset = xpos - lastX;
		lastX = xpos;
		//move along camera right
		cameraPos += glm::normalize(glm::cross((cameraPos - cameraTarget), up)) * (xoffset * 0.01f);
		cameraTarget += glm::normalize(glm::cross((cameraPos - cameraTarget), up)) * (xoffset * 0.01f);
	}


	if (stateL == 1) {

		float xoffset = xpos - lastX;
		float yoffset = lastY - ypos;

		lastX = xpos;
		lastY = ypos;

		const float sensitivity = 0.5f;
		xoffset *= sensitivity;
		yoffset *= sensitivity;

		yaw += xoffset;
		pitch += yoffset;

		//prevent inverted camera system
		if (pitch > 89.0f)
			pitch = 89.0f;
		if (pitch < -89.0f)
			pitch = -89.0f;

		cameraPos.x = cameraTarget.x + cameraDistance * -sin(glm::radians(yaw)) * cos(glm::radians(pitch));
		cameraPos.y = cameraTarget.y + cameraDistance * -sin(glm::radians(pitch)); //negative to achieve expected y movement
		cameraPos.z = cameraTarget.z + cameraDistance * cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	}
}

void mouseClick(GLFWwindow* window, int button, int action, int mods)
{
	// Whenever the left mouse button is pressed, the
	// mouse cursor's position is stored for the arc-
	// ball camera as a reference.
	if (button == GLFW_MOUSE_BUTTON_LEFT || GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
	{
		double x = 0, y = 0;

		glfwGetCursorPos(window, &x, &y);
		lastX = x;
		lastY = y;

	}
}

void generateCube(float height, float depth, float width) { //vertices added
	//CUBE
	float heightH = height / 2;
	float depthH = depth / 2;
	float widthH = width / 2;

	std::vector<float> CubeVertz{
		//links oben front:: -1; 1 ; 1 
		-widthH, -heightH, depthH, //0=0 norm
		-1, 0, 0,
		1, 0, //lefttex
		-widthH, -heightH, depthH,
		0, 0, 1,
		0, 0,
		-widthH, -heightH, depthH,
		0, -1, 0,
		0, 0,
	   widthH, -heightH, depthH,     //1=3 norm
	   1, 0, 0,                     //x
	   0, 0, //righttex
	   widthH, -heightH, depthH,
		  0, 0, 1,                  //z
		  1, 0,
	   widthH, -heightH, depthH,
		 0,-1,0,                   //y
		 1, 0,
	   widthH, -heightH, -depthH,   //2 =6
	   1, 0, 0,
	   1, 0, //righttex
	   widthH, -heightH, -depthH,
	   0,0,-1,
	   0, 0, //backtex
	   widthH, -heightH, -depthH,
	   0,-1,0,
	   1, 1,
		-widthH, -heightH, -depthH, //3=9
		-1, 0, 0,
		0, 0, //lefttex
		-widthH, -heightH, -depthH,
		0, 0, -1,
		1, 0, //backtex
		-widthH, -heightH, -depthH,
		0, -1, 0,
		0, 1,
		widthH, heightH, depthH, //4=12
		1, 0, 0,
		0, 1, //righttex
		widthH, heightH, depthH,
		0, 0, 1,
		1, 1,
		widthH, heightH, depthH,
		0, 1, 0,
		1, 0, //toptex
		widthH, heightH, -depthH, //5=15
		1, 0, 0,
		1, 1, //righttex
		widthH, heightH, -depthH,
		0, 0, -1,
		0, 1, //backtex
		widthH, heightH, -depthH,
		0, 1, 0,
		1, 1, //toptex
		-widthH, heightH, -depthH, //6=18
		-1,0, 0,
		0, 1, //lefttex
		-widthH, heightH, -depthH,
		0, 0, -1,
		1, 1, //backtex
		-widthH, heightH, -depthH,
		0, 1, 0,
		0, 1, //toptex
		-widthH, heightH, depthH, //7=21
		-1, 0, 0,
		1, 1, //lefttex
		-widthH, heightH, depthH,
		 0, 0, 1,
		 0, 1,
		-widthH, heightH, depthH,
		 0, 1, 0,
		 0, 0 //toptex
	};

	cubeIndex = {
	11, 8, 2, //bottom
	8, 5, 2,
	1, 4, 22, //front
	13, 22,4,
	7, 10, 16, //back
	10, 19, 16,
	0 * 3, 6 * 3, 3 * 3, //left
	0 * 3, 7 * 3, 6 * 3,
	2 * 3, 4 * 3, 1 * 3, //right
	5 * 3, 4 * 3, 2 * 3,
	14,20, 23, //top
	17, 20, 14
	};


	glBindVertexArray(VAOcube);
	glBindBuffer(GL_ARRAY_BUFFER, VBOcube);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOcube);
	//positions to location 0
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	//normals
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	//texture coordinates
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);
	//arraybuffer filled with positions, normals, texture coordinates
	glBufferData(GL_ARRAY_BUFFER, CubeVertz.size() * 4, &CubeVertz[0], GL_STATIC_DRAW); //one float => 4 byte
	//elementbuffer filled with indices
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, cubeIndex.size() * 4, &cubeIndex[0], GL_STATIC_DRAW); //woher weiß das ebo dass sich die indizes auf attribpointer 0 beziehen
	glDrawElements(GL_TRIANGLES, cubeIndex.size(), GL_UNSIGNED_INT, 0);

}

void generateCylinder(int segCount, float heightC, float r) {
	// CYLINDER
	float heightActual = heightC / 2;

	std::vector <float> cylVerts;

	//top middle
	cylVerts.push_back(0); cylVerts.push_back(heightActual); cylVerts.push_back(0);
	//top normal
	cylVerts.push_back(0); cylVerts.push_back(1); cylVerts.push_back(0);
	//toptex
	cylVerts.push_back(0.5); cylVerts.push_back(0.5);

	//bottom middle
	cylVerts.push_back(0); cylVerts.push_back(-heightActual); cylVerts.push_back(0);
	//bottom normal
	cylVerts.push_back(0); cylVerts.push_back(-1); cylVerts.push_back(0);
	//bottomtex
	cylVerts.push_back(0.5); cylVerts.push_back(0.5);

	//both cricles
	for (int i = 0; i < segCount; i++) {
		//bottom verts-twice; normals bottom je eine nach oben, eine nach draußen; vert-normal(out)-uv(out)-vert-normal(down)-uv(down)
		cylVerts.push_back(r * cos((twoPi * i) / segCount)); cylVerts.push_back(-heightActual);  cylVerts.push_back(r * sin((twoPi * i) / segCount));
		cylVerts.push_back(cos((twoPi * i) / segCount)); cylVerts.push_back(0); cylVerts.push_back(sin((twoPi * i) / segCount));
		cylVerts.push_back((float)i / segCount); cylVerts.push_back(0);//tex nach außen
		cylVerts.push_back(r * cos((twoPi * i) / segCount));  cylVerts.push_back(-heightActual); cylVerts.push_back(r * sin((twoPi * i) / segCount));
		cylVerts.push_back(0); cylVerts.push_back(-1);  cylVerts.push_back(0);
		cylVerts.push_back(-(r * cos((twoPi * i) / segCount)) * 0.5f + 0.5f); cylVerts.push_back(-(r * sin((twoPi * i) / segCount)) * 0.5f + 0.5f);

		//top verts-twice; normals top je eine nach oben, eine nach draußen
		cylVerts.push_back(r * cos((twoPi * i) / segCount));  cylVerts.push_back(heightActual); cylVerts.push_back(r * sin((twoPi * i) / segCount));
		cylVerts.push_back(cos((twoPi * i) / segCount)); cylVerts.push_back(0); cylVerts.push_back(sin((twoPi * i) / segCount));
		cylVerts.push_back((float)i / segCount); cylVerts.push_back(1);//tex nach außen
		cylVerts.push_back(r * cos((twoPi * i) / segCount));  cylVerts.push_back(heightActual); cylVerts.push_back(r * sin((twoPi * i) / segCount));
		cylVerts.push_back(0); cylVerts.push_back(1); cylVerts.push_back(0);
		cylVerts.push_back(-(r * cos((twoPi * i) / segCount)) * 0.5f + 0.5f); cylVerts.push_back(-(r * sin((twoPi * i) / segCount)) * 0.5f + 0.5f);
	}

	//indices
	//blue tris
	for (int i = 2; i < ((cylVerts.size() / 8)) - 6; i += 4) {
		cyIndex.push_back(i);
		cyIndex.push_back(i + 2);
		cyIndex.push_back(i + 4);

	}
	std::cout << cylVerts.size() << std::endl;
	//last blue tri 
	cyIndex.push_back((cylVerts.size() / 8) - 4);
	cyIndex.push_back((cylVerts.size() / 8) - 2);
	cyIndex.push_back(2);
	//white tris 
	for (uint32_t i = 8; i < ((cylVerts.size() / 8)); i += 4) {
		cyIndex.push_back(i);
		cyIndex.push_back(i - 2);
		cyIndex.push_back(i - 4);

	}
	//last white tri 
	cyIndex.push_back(4);
	cyIndex.push_back(2);
	cyIndex.push_back((cylVerts.size() / 8) - 2);

	//top, bottom cirlces  
	for (int i = 3; i < (cylVerts.size() / 8) - 6; i += 4) {
		cyIndex.push_back(i + 4);
		cyIndex.push_back(1);
		cyIndex.push_back(i);
		//top -done
		cyIndex.push_back(i + 2);
		cyIndex.push_back(0);
		cyIndex.push_back(i + 6);
	}
	//bottom last  
	cyIndex.push_back(3);
	cyIndex.push_back(1);
	cyIndex.push_back((cylVerts.size() / 8) - 3);
	//top last  
	cyIndex.push_back((cylVerts.size() / 8) - 1);
	cyIndex.push_back(0);
	cyIndex.push_back(5);

	glBindVertexArray(VAOcyl);
	glBindBuffer(GL_ARRAY_BUFFER, VBOcyl);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOcyl);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glBufferData(GL_ARRAY_BUFFER, cylVerts.size() * 4, &cylVerts[0], GL_STATIC_DRAW); //one float => 4 byte

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, cyIndex.size() * 4, &cyIndex[0], GL_STATIC_DRAW);

	glDrawElements(GL_TRIANGLES, cyIndex.size(), GL_UNSIGNED_INT, 0);
}

void generateSphere(float radius, int secs, int stacks) {

	float x, y, z, xz;
	float nx, ny, nz, radInv = 1.0f / radius;
	float secSteps = twoPi / secs;
	float stacksStep = M_PI / stacks;
	std::vector <float> sphVerts;
	std::vector <float> sphNormals;
	float sectorAngle, stackAngle;

	for (int i = 0; i <= stacks; i++) {             //start at top, generate points in cirlce, go one stack below, repeat 
		stackAngle = M_PI / 2 - i * stacksStep;
		xz = radius * cos(stackAngle);             //"side" movement around circle
		y = radius * sin(stackAngle);              //vertical movement
		for (int j = 0; j <= secs; j++) {
			sectorAngle = j * secSteps;
			x = xz * cos(sectorAngle);            // r * cos(stackAngle) * cos(sectorAngle)
			z = xz * sin(sectorAngle);
			sphVerts.push_back(x);
			sphVerts.push_back(y);
			sphVerts.push_back(z);

			nx = x * radInv; //"divide" by radius
			ny = y * radInv;
			nz = z * radInv;
			sphVerts.push_back(nx);
			sphVerts.push_back(ny);
			sphVerts.push_back(nz);
			// tex coordinates
			sphVerts.push_back((float)j / secs);
			sphVerts.push_back((float)i / stacks);
		}
	}

	int count1, count2;
	for (int n = 0; n < stacks; n++) {
		count1 = n * (secs + 1);                        //first index of this stack
		count2 = count1 + secs + 1;                     //first index of next stack
		for (int i = 0; i < secs; i++, count1++, count2++) {
			if (n != 0) {
				sphIndex.push_back(count1 + 1);
				sphIndex.push_back(count2);
				sphIndex.push_back(count1);
			}
			if (n != (stacks - 1)) {
				sphIndex.push_back(count1 + 1);
				sphIndex.push_back(count2 + 1);
				sphIndex.push_back(count2);
			}
		}
	}

	glBindVertexArray(VAOsph);
	glBindBuffer(GL_ARRAY_BUFFER, VBOsph);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOsph);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glBufferData(GL_ARRAY_BUFFER, sphVerts.size() * 4, &sphVerts[0], GL_STATIC_DRAW); //one float => 4 byte


	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sphIndex.size() * 4, &sphIndex[0], GL_STATIC_DRAW);
	//gouraud shader
	glUseProgram(lightShader);
	glBindVertexArray(VAOsph_g);
	glBindBuffer(GL_ARRAY_BUFFER, VBOsph_g);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOsph_g);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glBufferData(GL_ARRAY_BUFFER, sphVerts.size() * 4, &sphVerts[0], GL_STATIC_DRAW); //one float => 4 byte
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sphIndex.size() * 4, &sphIndex[0], GL_STATIC_DRAW);
	glUseProgram(shaderProgram);
}

void drawCube() {

	//dieser block kann wieder raus wenn drawCylinder aktiv ist
	glUniform3f(7, 0.1f, 0.1f, 0.1f); //ambient
	glUniform3f(8, 0.7f, 0.7f, 0.7f); //diffuse
	glUniform3f(9, 0.1f, 0.1f, 0.1f); //specular
	glUniform1f(10, 2.0f); //shininess/alpha

	//vbo und ebo binden sollt auch weg können, weils vao eh da drauf zeigt
	glBindVertexArray(VAOcube);
	glBindBuffer(GL_ARRAY_BUFFER, VBOcube);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOcube);
	glDrawElements(GL_TRIANGLES, cubeIndex.size(), GL_UNSIGNED_INT, 0);

}

void drawSphere() {

	//set values for spheres
	glUniform3f(7, 0.1f, 0.1f, 0.1f); //ambient
	glUniform3f(8, 0.7f, 0.7f, 0.7f); //diffuse
	glUniform3f(9, 0.3f, 0.3f, 0.3f); //specular
	glUniform1f(10, 8.0f); //shininess/alpha

	glBindVertexArray(VAOsph);
	glBindBuffer(GL_ARRAY_BUFFER, VBOsph);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOsph);
	glDrawElements(GL_TRIANGLES, sphIndex.size(), GL_UNSIGNED_INT, 0);
}

void drawSphere_g() {
	glBindVertexArray(VAOsph);
	glBindBuffer(GL_ARRAY_BUFFER, VBOsph);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOsph);

	glUniform3f(7, 0.1f, 0.1f, 0.1f); //ambient
	glUniform3f(8, 0.9f, 0.9f, 0.9f); //diffuse
	glUniform3f(9, 0.3f, 0.3f, 0.3f); //specular
	glUniform1f(10, 10.0f); //shininess/alpha
	glDrawElements(GL_TRIANGLES, sphIndex.size(), GL_UNSIGNED_INT, 0);
}

void drawCylinder() {

	//set values for cube/cylinder
	glUniform3f(7, 0.1f, 0.1f, 0.1f); //ambient
	glUniform3f(8, 0.7f, 0.7f, 0.7f); //diffuse
	glUniform3f(9, 0.3f, 0.3f, 0.3f); //specular
	glUniform1f(10, 8.0f); //shininess/alpha

	glBindVertexArray(VAOcyl);
	glBindBuffer(GL_ARRAY_BUFFER, VBOcyl);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOcyl);
	glDrawElements(GL_TRIANGLES, cyIndex.size(), GL_UNSIGNED_INT, 0);

}
