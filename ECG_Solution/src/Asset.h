#pragma once

#include <map>
#include <memory>
#include "rendering/Texture.h"
#include "rendering/Material.h"

class AssetManager final
{
private:
    AssetManager();

    std::map<std::string, Texture*> _textures;
    std::map<std::string, ShaderNew*> _shaders;

    static AssetManager* _instance;

public:
    ~AssetManager();

    std::shared_ptr<BaseMaterial> defaultMaterial;
    
    std::shared_ptr<Texture> getTexture(std::string path);
    std::shared_ptr<ShaderNew> getShader(std::string path);

    static void init();
    static void destroy();
    static AssetManager* getInstance();
};