#pragma once
#include <ft2build.h>
#include FT_FREETYPE_H
#include "Utils.h"
#include <map>
#include "ShaderNew.h"


struct Character {
	unsigned int textureID;  // ID handle of the glyph texture
	glm::ivec2   size;       // Size of glyph
	glm::ivec2   bearing;    // Offset from baseline to left/top of glyph
	unsigned int advance;    // Offset to advance to next glyph
};

class TextHandler {
public:
	FT_Library textLib;

	std::map<char, Character> characters;

	TextHandler();

	~TextHandler();

	void drawText(std::string text, float x, float y, float scale, glm::vec3 color);

	void setUpShader(std::shared_ptr<ShaderNew> shader);

private:
	void init();

	//second value shpuld represent window height
	glm::mat4 projection = glm::ortho(0.0f, 800.0f, 0.0f, 600.0f);

	std::shared_ptr<ShaderNew>  textShader;

	unsigned int VAO;
	unsigned int VBO;
};