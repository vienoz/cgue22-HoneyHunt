#pragma once

#include <glm/glm.hpp>
#include <cstdint>
#include <vector>
#include <memory>
#include <glm\glm.hpp>
#include <GL\glew.h>
#include <sstream>
#include <iostream>
#include <string>
#include <fstream>
#include <map>
#include "GamePhysx.h"
#include "Entity.h"

class LODModel
{
protected:
    std::vector < std::shared_ptr<PhysxStaticEntity> > _models;
    int _id;

public:
    LODModel();
    LODModel(std::shared_ptr<PhysxStaticEntity>);
    LODModel(std::vector < std::shared_ptr<PhysxStaticEntity> > models);
    virtual ~LODModel();

    std::shared_ptr<PhysxStaticEntity> getModel(int id);
    std::shared_ptr<PhysxStaticEntity> getActiveModel();
    void setLodID(int id);
    void draw(Camera camera, int id);
    void drawActive(Camera camera);
    void addModel(std::shared_ptr<PhysxStaticEntity> model);
};