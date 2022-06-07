#pragma once
#include <ft2build.h>
#include FT_FREETYPE_H
#include <iostream>



class TextHandler {
public:
	FT_Library textLib;

	TextHandler();

private:
	void init();
};