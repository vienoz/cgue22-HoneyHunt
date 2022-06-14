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
    float _lod_distance_min;
    float _lod_distance_max;

    std::vector<OcttreeNode> _nodes;
    std::vector<Octtree> _subtrees;
    glm::vec3 _maxDimension;
    glm::vec3 _origin;
    int _capacity; // maximum content nodes (low value -> higher division -> higher lod accuracy)
    int _index;
    bool _hasDivided; // false as leaf node


public:
    static bool IsLodActive;

    Octtree(glm::vec3 origin, glm::vec3 maxDimension, int capacity, float lod_distance_min, float lod_distance_max);
    Octtree();
    virtual ~Octtree();

    void insert(OcttreeNode node);
    void subdivide();
    bool contains(glm::vec3 point);
    void print();
    void draw(Camera& camera, DirectionalLight& dirL);
    void setLodIDs(glm::vec3 playerPosition);
};