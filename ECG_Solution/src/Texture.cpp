#include "Texture.h"
#include "iostream"


int Texture::texUnit = 0;

Texture::Texture(std::string file) {
	const char* path = file.c_str();
	DDSImage img = loadDDS(path);

	glGenTextures(1, &_texHandle); // generate texture
	glActiveTexture(GL_TEXTURE0 + texUnit); //activate before binding; GL_TEXTURE0 is activated by default
	glBindTexture(GL_TEXTURE_2D, _texHandle); //binding texture
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); //GL_NEAREST //magnification filter
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // minification filter:  mipmap linear interpolation
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);  // behaviour if tex coordinates >1 or <0
	glCompressedTexImage2D(GL_TEXTURE_2D, 0, GL_COMPRESSED_RGBA_S3TC_DXT1_EXT, img.width, img.height, 0, img.size, img.data);
	glGenerateMipmap(GL_TEXTURE_2D);
	std::cout << texUnit << std::endl;
	texUnit++;


}

Texture::~Texture()
{
}


void Texture::bind(unsigned int unint) {

	//glUniform1i(10, texUnit);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, _texHandle);
}