#pragma once

#include <glm/glm.hpp>
#include <GL\glew.h>
#include <cstdint>
#include <vector>
#include <memory>
#include <sstream>
#include <iostream>
#include <string>
#include <fstream>
#include <map>
#include "../OcttreeNode.h"


class Octtree
{

protected:
    float _LOD_distance_1 = 40.0f;
    float _LOD_distance_2 = 80.0f;

    std::vector<OcttreeNode> _nodes;
    std::vector<Octtree> _subtrees;
    glm::vec3 _maxDimension;
    glm::vec3 _origin;
    int _capacity;
    int _index;
    bool _hasDivided;


public:
    static bool IsLodActive;

    Octtree(glm::vec3 origin, glm::vec3 maxDimension, int capacity);
    Octtree();
    virtual ~Octtree();

    void insert(OcttreeNode node);
    void subdivide();
    bool contains(glm::vec3 point);
    void print();
    void draw(Camera& camera, DirectionalLight& dirL);
    void setLodIDs(glm::vec3 playerPosition);
};