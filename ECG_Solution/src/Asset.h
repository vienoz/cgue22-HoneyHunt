#pragma once

#include <map>
#include <memory>
#include "Texture.h"
#include "Material.h"

class AssetManager final
{
private:
    AssetManager();

    std::map<std::string, Texture*> _textures;

    static AssetManager* _instance;

public:
    ~AssetManager();

    std::shared_ptr<Material> defaultMaterial;
    
    std::shared_ptr<Texture> getTexture(std::string path);

    static void init();
    static void destroy();
    static AssetManager* getInstance();
};