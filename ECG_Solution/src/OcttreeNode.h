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

#include "LODModel.h"

class OcttreeNode
{
protected:
    glm::vec3 _position;
    LODModel _lodModels;

public:
    OcttreeNode(LODModel lods);
    virtual ~OcttreeNode();

    glm::vec3 getPosition();
    void setLodID(int id);
    LODModel getLodModels();
    void print();
};