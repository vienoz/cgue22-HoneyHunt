#pragma once

#include <string>
#include <GL\glew.h>
#include "../Utils.h"

class Texture
{
protected:
	GLuint _texHandle;
	static int texUnit;

public:
	/*!
	 * Creates a texture from a file
	 * @param file: path to the texture file (a DSS image)
	 */
	Texture(std::string file);
	~Texture();

	/*!
	 * Activates the texture unit and binds this texture
	 * @param unit: the texture unit
	 */
	void bind(unsigned int unit);
};