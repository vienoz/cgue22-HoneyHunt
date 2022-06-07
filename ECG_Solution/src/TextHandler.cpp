#include "TextHandler.h"



void TextHandler::init() {
    if (FT_Init_FreeType(&textLib))
    {
        std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
        return;
    }

    FT_Face face;
    if (FT_New_Face(textLib, "assets/fonts/valentine-bees.regular.ttf", 0, &face))
    {
        std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
        return;
    }

    FT_Set_Pixel_Sizes(face, 0, 48);

    if (FT_Load_Char(face, 'X', FT_LOAD_RENDER))
    {
        std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
        return;
    }

    FT_Bitmap skd = face->glyph->bitmap;
}

TextHandler::TextHandler() {
    init();
}