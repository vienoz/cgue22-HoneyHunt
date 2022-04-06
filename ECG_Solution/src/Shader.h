#pragma once

#include <GL\glew.h>
#include <sstream>
#include <fstream>
#include <iostream>
#include <glm\glm.hpp>
#include <glm\gtc\type_ptr.hpp>




/*!
 * Shader class that encapsulates all shader access
 */
class Shader
{
protected:
	/*!
	 * The shader program handle
	 */
	unsigned int _handle;

	/*!
	 * Path to vertex shader and fragment shader
	 */
	std::string _vs, _fs;

	/*!
	 * Whether an internal shader is being used
	 */
	bool _useFileAsSource;


	unsigned int prepareShader(std::string vertPath, std::string fragPath);

	std::string readFromFile(const std::string& filepath);


	/*!
	 * @param uniform: uniform string in shader
	 * @return the location ID of the uniform
	 */
	GLint getUniformLocation(std::string uniform);

public:

	/*!
	 * Default constructor of a simple color shader
	 */
	Shader();

	/*!
	 * Shader constructor with specified vertex and fragment shader
	 * Loads and compiles the shader
	 * @param vs: path to the vertex shader
	 * @param fs: path to the fragment shader
	 */
	Shader(std::string vs, std::string fs);

	~Shader();

	/*!
	 * Uses the shader with glUseProgram
	 */
	void use();

	/*!
	 * Un-uses the shader
	 */
	void unuse();

	/*!
	 * Sets an integer uniform in the shader
	 * @param uniform: the name of the uniform
	 * @param i: the value to be set
	 */
	void setUniform(std::string uniform, const int i);
	/*!
	 * Sets an integer uniform in the shader
	 * @param location: location ID of the uniform
	 * @param i: the value to be set
	 */
	void setUniform(GLint location, const int i);				//direct
	/*!
	 * Sets an unsigned integer uniform in the shader
	 * @param uniform: the name of the uniform
	 * @param i: the value to be set
	 */
	void setUniform(std::string uniform, const unsigned int i);
	/*!
	 * Sets an unsigned integer uniform in the shader
	 * @param location: location ID of the uniform
	 * @param i: the value to be set
	 */
	void setUniform(GLint location, const unsigned int i);			//direct
	/*!
	 * Sets a float uniform in the shader
	 * @param uniform: the name of the uniform
	 * @param f: the value to be set
	 */
	void setUniform(std::string uniform, const float f);
	/*!
	 * Sets a float uniform in the shader
	 * @param location: location ID of the uniform
	 * @param f: the value to be set
	 */
	void setUniform(GLint location, const float f);					//direct
	/*!
	 * Sets a 4x4 matrix uniform in the shader
	 * @param uniform: the name of the uniform
	 * @param mat: the value to be set
	 */
	void setUniform(std::string uniform, const glm::mat4& mat);
	/*!
	 * Sets a 4x4 matrix uniform in the shader
	 * @param location: location ID of the uniform
	 * @param mat: the value to be set
	 */
	void setUniform(GLint location, const glm::mat4& mat);					//direct
	/*!
	 * Sets a 3x3 matrix uniform in the shader
	 * @param uniform: the name of the uniform
	 * @param mat: the value to be set
	 */
	void setUniform(std::string uniform, const glm::mat3& mat);
	/*!
	 * Sets a 3x3 matrix uniform in the shader
	 * @param location: location ID of the uniform
	 * @param mat: the value to be set
	 */
	void setUniform(GLint location, const glm::mat3& mat);				//direct
	/*!
	 * Sets a 2D vector uniform in the shader
	 * @param uniform: the name of the uniform
	 * @param vec: the value to be set
	 */
	void setUniform(std::string uniform, const glm::vec2& vec);
	/*!
	 * Sets a 2D vector uniform in the shader
	 * @param location: location ID of the uniform
	 * @param vec: the value to be set
	 */
	void setUniform(GLint location, const glm::vec2& vec);					//direct
	/*!
	 * Sets a 3D vector uniform in the shader
	 * @param uniform: the name of the uniform
	 * @param vec: the value to be set
	 */
	void setUniform(std::string uniform, const glm::vec3& vec);
	/*!
	 * Sets a 3D vector uniform in the shader
	 * @param location: location ID of the uniform
	 * @param vec: the value to be set
	 */
	void setUniform(GLint location, const glm::vec3& vec);						//direct
	/*!
	 * Sets a 4D vector uniform in the shader
	 * @param uniform: the name of the uniform
	 * @param vec: the value to be set
	 */
	void setUniform(std::string uniform, const glm::vec4& vec);
	/*!
	 * Sets a 4D vector uniform in the shader
	 * @param location: location ID of the uniform
	 * @param vec: the value to be set
	 */
	void setUniform(GLint location, const glm::vec4& vec);					//direct
	/*!
	 * Sets a uniform array property
	 * @param arr: name of the uniform array
	 * @param i: index of the value to be set
	 * @param prop: property name
	 * @param vec: the value to be set
	 */
	 //void setUniformArr(std::string arr, unsigned int i, std::string prop, const glm::vec3& vec);
	 /*!
	  * Sets a uniform array property
	  * @param arr: name of the uniform array
	  * @param i: index of the value to be set
	  * @param prop: property name
	  * @param f: the value to be set
	  */
	  //void setUniformArr(std::string arr, unsigned int i, std::string prop, const float f);
};
