#include "Asset.h"

AssetManager* AssetManager::_instance = 0;

AssetManager::AssetManager()
{
}

AssetManager::~AssetManager()
{
    // Free all textures
    auto it = _textures.begin();
    while (it != _textures.end())
    {
        delete it->second;
        ++it;
    }
}

std::shared_ptr<Texture> AssetManager::getTexture(std::string path)
{
    if (_textures.count(path))
        return std::shared_ptr<Texture>(_textures[path]);
    else
        return std::shared_ptr<Texture>(_textures[path] = new Texture(path));
}

AssetManager* AssetManager::getInstance()
{
    return _instance; 
}

void AssetManager::init()
{
    _instance = new AssetManager();
}

void AssetManager::destroy()
{
    delete _instance;
}