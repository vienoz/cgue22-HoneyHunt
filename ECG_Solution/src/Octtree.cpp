#include "Octtree.h"


Octtree::Octtree(glm::vec3 origin, glm::vec3 maxDimension, int capacity)
{
    _maxDimension = maxDimension;
    _capacity = capacity;
    _origin = origin;
    _hasDivided = false;
    _index = 0;
    IsLodActive = true;
}

Octtree::Octtree() {
}

Octtree::~Octtree()
{
    _subtrees.clear();
    _nodes.clear();
}

void Octtree::insert(OcttreeNode node) {
    if (!contains(node.getPosition())) {
        std::cout << "not contains Node.." << std::endl;
        return;
    }

    if (_nodes.size() < _capacity) {
        std::cout<< "Add Node.." << std::endl;
        _nodes.push_back(node);
    }
    else {
        if (!_hasDivided) {
            std::cout << "divide.." << std::endl;
            subdivide();
        }

        //std::cout << "Add Node to children.." << std::endl;
        for (auto& subtree : _subtrees) {
            subtree.insert(node);
        }
    }
}

void Octtree::subdivide() {
    float offsetX = _maxDimension.x / 2;
    float offsetY = _maxDimension.y / 2;
    float offsetZ = _maxDimension.z / 2;
    glm::vec3 newDimension = glm::vec3(offsetX, offsetY, offsetZ);
    _hasDivided = true;
    glm::vec3 newOrigin; // right = +x, up = +y, forward = +z


    //std::cout << "PRE TREE ADD" << std::endl;
    //std::cout << "_nodes size: " << _nodes.size() << ", ";
    //std::cout << "_subtrees size: " << _subtrees.size() << std::endl;

    // cube dimension calculations
    newOrigin = glm::vec3(_origin.x + offsetX, _origin.y - offsetY, _origin.z + offsetZ);
    _subtrees.push_back(Octtree(newOrigin, newDimension, _capacity));
    newOrigin = glm::vec3(_origin.x - offsetX, _origin.y - offsetY, _origin.z + offsetZ);
    _subtrees.push_back(Octtree(newOrigin, newDimension, _capacity));
    newOrigin = glm::vec3(_origin.x + offsetX, _origin.y - offsetY, _origin.z - offsetZ);
    _subtrees.push_back(Octtree(newOrigin, newDimension, _capacity));
    newOrigin = glm::vec3(_origin.x - offsetX, _origin.y - offsetY, _origin.z - offsetZ);
    _subtrees.push_back(Octtree(newOrigin, newDimension, _capacity));
    newOrigin = glm::vec3(_origin.x + offsetX, _origin.y + offsetY, _origin.z + offsetZ);
    _subtrees.push_back(Octtree(newOrigin, newDimension, _capacity));
    newOrigin = glm::vec3(_origin.x - offsetX, _origin.y + offsetY, _origin.z + offsetZ);
    _subtrees.push_back(Octtree(newOrigin, newDimension, _capacity));
    newOrigin = glm::vec3(_origin.x + offsetX, _origin.y + offsetY, _origin.z - offsetZ);
    _subtrees.push_back(Octtree(newOrigin, newDimension, _capacity));
    newOrigin = glm::vec3(_origin.x - offsetX, _origin.y + offsetY, _origin.z - offsetZ);
    _subtrees.push_back(Octtree(newOrigin, newDimension, _capacity));

    //std::cout << "POST TREE ADD" << std::endl;
    //std::cout<< "_nodes size: " << _nodes.size() << ", ";
    //std::cout<< "_subtrees size: " << _subtrees.size() << std::endl;

    for (auto& node: _nodes)
        for (auto& subtree : _subtrees) {
            //std::cout << "PRE GoDeeper:" << subtree._nodes.size();
            subtree.insert(node);
            //std::cout << "POST GoDeeper:"  << subtree._nodes.size() << std::endl;
        }

}

bool Octtree::contains(glm::vec3 point) {

    std::cout << "Check Contains:" << std::endl;
    std::cout << "Point: x:  " << point.x << ", y:  " << point.y << ", z:  " << point.z << std::endl;
    std::cout << "Min:   x:  " << _origin.x - _maxDimension.x << ", y:  " << _origin.y - _maxDimension.y << ", z:  " << _origin.z - _maxDimension.z << std::endl;
    std::cout << "Max:   x:  " << _origin.x + _maxDimension.x << ", y:  " << _origin.y + _maxDimension.y << ", z:  " << _origin.z + _maxDimension.z << std::endl << std::endl;

    if (point.x >= _origin.x - _maxDimension.x && point.x < _origin.x + _maxDimension.x &&
        point.y >= _origin.y - _maxDimension.y && point.y < _origin.y + _maxDimension.y &&
        point.z >= _origin.z - _maxDimension.z && point.z < _origin.z + _maxDimension.z)
        return true;
    return false;
}

void Octtree::print() {
    if (!_hasDivided)
        for (auto& node : _nodes)
            node.print();
    else
        for (auto& subtree : _subtrees)
            subtree.print();
}

void Octtree::draw(Camera& camera, DirectionalLight& dirL) {
    if (!_hasDivided)
        for (auto& node : _nodes) {
            if(IsLodActive)
                node.getLodModels()->drawActive(camera, dirL);
            else
                node.getLodModels()->draw(camera, dirL, 0); //0 is default lod
        }
    else
        for (auto& subtree : _subtrees)
            subtree.draw(camera, dirL);
}

void Octtree::setLodIDs(glm::vec3 playerPosition) {
    if (!_hasDivided) {
        int id;
        if (glm::distance(_origin, playerPosition) < _LOD_distance_1)
            id = 0;
        else if (glm::distance(_origin, playerPosition) < _LOD_distance_2)
            id = 1;
        else
            id = 2;

        for (auto& node : _nodes)
            node.setLodID(id);
    }   
    else
        for (auto& subtree : _subtrees)
            subtree.setLodIDs(playerPosition);
}