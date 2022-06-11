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
    std::shared_ptr<LODModel> _lodModels;

public:
    OcttreeNode(std::shared_ptr<LODModel> lods);
    virtual ~OcttreeNode();

    glm::vec3 getPosition();
    void setLodID(int id);
    std::shared_ptr<LODModel> getLodModels();
    void print();
};