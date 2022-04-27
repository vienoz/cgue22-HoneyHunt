#include "ShaderNew.h"

unsigned int ShaderNew::prepareShader(std::string vertPath, std::string fragPath) {
	//creates empty shader and returns handle
	unsigned int vertShader = glCreateShader(GL_VERTEX_SHADER);
	unsigned int fragShader = glCreateShader(GL_FRAGMENT_SHADER);

	//red from file
	std::string vertShaderContent = readFromFile(vertPath);
	std::string fragShaderContent = readFromFile(fragPath);
	const char* vertShaderTxt = vertShaderContent.c_str();
	const char* fragShaderTxt = fragShaderContent.c_str();
	int length;

	// shader compilation and checking
	int success;
	GLint logSize;

	//-> Vertex shader
	glShaderSource(vertShader, 1, &vertShaderTxt, NULL);
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
	glShaderSource(fragShader, 1, &fragShaderTxt, NULL);
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
std::string ShaderNew::readFromFile(const std::string& filepath) {
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
		std::cout << "shader at location: " << filepath << " not found" << std::endl;
	return result.str();
}


GLint ShaderNew::getUniformLocation(std::string uniform) {
	return 1;
}

ShaderNew::ShaderNew() {
}

ShaderNew::ShaderNew(std::string vs, std::string fs) {
	_handle = prepareShader(vs, fs);
}

ShaderNew::~ShaderNew() {
}

void ShaderNew::use() {
	glUseProgram(_handle);
}

void ShaderNew::unuse() {
}

void ShaderNew::setUniform(GLint location, const int i) {
	glUniform1i(location, i);
}

void ShaderNew::setUniform(GLint location, const unsigned int i) {
	glUniform1ui(location, i);
}

void ShaderNew::setUniform(GLint location, const float i) {
	glUniform1f(location, i);
}

void ShaderNew::setUniform(GLint location, const glm::mat4& i) {
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(i));
}

void ShaderNew::setUniform(GLint location, const physx::PxMat44& i) {
	glUniformMatrix4fv(location, 1, GL_FALSE, &(i[0].x));
}

void ShaderNew::setUniform(GLint location, const glm::mat3& i) {
	glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(i));
}

void ShaderNew::setUniform(GLint location, const glm::vec2& i) {
	glUniform2f(location, i.x, i.y);
}

void ShaderNew::setUniform(GLint location, const glm::vec3& i) {
	glUniform3f(location, i.x, i.y, i.z);
}

void ShaderNew::setUniform(GLint location, const glm::vec4& i) {
	glUniform4f(location, i.w, i.x, i.y, i.z);
}