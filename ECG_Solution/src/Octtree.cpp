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
}

void Octtree::insert(OcttreeNode node) {
    if (!contains(node.getPosition()))
        return;

    if (_nodes.size() < _capacity)
        _nodes.push_back(node);
    else {
        if (!_hasDivided)
            subdivide();

        _subtrees[0].insert(node);
        _subtrees[1].insert(node);
        _subtrees[2].insert(node);
        _subtrees[3].insert(node);
        _subtrees[4].insert(node);
        _subtrees[5].insert(node);
        _subtrees[6].insert(node);
        _subtrees[7].insert(node);
    }
}

void Octtree::subdivide() {
    float offsetX = _maxDimension.x / 2;
    float offsetY = _maxDimension.y / 2;
    float offsetZ = _maxDimension.z / 2;
    glm::vec3 newDimension = glm::vec3(offsetX, offsetY, offsetZ);
    glm::vec3 newOrigin; // right = +x, up = +y, forward = +z

    // cube dimension calculations
    newOrigin = glm::vec3(_origin.x + offsetX, _origin.y - offsetY, _origin.z + offsetZ);
    auto lowNorthEast = Octtree(newOrigin, newDimension, _capacity);
    newOrigin = glm::vec3(_origin.x - offsetX, _origin.y - offsetY, _origin.z + offsetZ);
    auto lowNorthWest = Octtree(newOrigin, newDimension, _capacity);
    newOrigin = glm::vec3(_origin.x + offsetX, _origin.y - offsetY, _origin.z - offsetZ);
    auto lowSouthEast = Octtree(newOrigin, newDimension, _capacity);
    newOrigin = glm::vec3(_origin.x - offsetX, _origin.y - offsetY, _origin.z - offsetZ);
    auto lowSouthWest = Octtree(newOrigin, newDimension, _capacity);
    newOrigin = glm::vec3(_origin.x + offsetX, _origin.y + offsetY, _origin.z + offsetZ);
    auto upNorthEast = Octtree(newOrigin, newDimension, _capacity);
    newOrigin = glm::vec3(_origin.x - offsetX, _origin.y + offsetY, _origin.z + offsetZ);
    auto upNorthWest = Octtree(newOrigin, newDimension, _capacity);
    newOrigin = glm::vec3(_origin.x + offsetX, _origin.y + offsetY, _origin.z - offsetZ);
    auto upSouthEast = Octtree(newOrigin, newDimension, _capacity);
    newOrigin = glm::vec3(_origin.x - offsetX, _origin.y + offsetY, _origin.z - offsetZ);
    auto upSouthWest = Octtree(newOrigin, newDimension, _capacity);

    // set new trees as children 
    _subtrees[0] = lowNorthEast;
    _subtrees[1] = lowNorthWest;
    _subtrees[2] = lowSouthEast;
    _subtrees[3] = lowSouthWest;
    _subtrees[4] = upNorthEast;
    _subtrees[5] = upNorthWest;
    _subtrees[6] = upSouthEast;
    _subtrees[7] = upSouthWest;
    _hasDivided = true;
}

bool Octtree::contains(glm::vec3 point) {
    if (point.x > _origin.x - _maxDimension.x && point.x < _origin.x + _maxDimension.x &&
        point.y > _origin.y - _maxDimension.y && point.y < _origin.y + _maxDimension.y &&
        point.z > _origin.z - _maxDimension.z && point.z < _origin.z + _maxDimension.z)
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

void Octtree::draw(Camera camera) {
    if (!_hasDivided)
        for (auto& node : _nodes) {
            if(IsLodActive)
                node.getLodModels().drawActive(camera);
            else
                node.getLodModels().draw(camera, 0); //0 is default lod
        }
    else
        for (auto& subtree : _subtrees)
            subtree.draw(camera);
}

void Octtree::setLodIDs(glm::vec3 playerPosition) {
    if (!_hasDivided) {
        int id;
        if (glm::distance(_origin, playerPosition) < _max_LOD_distance)
            id = 0;
        else
            id = 1;

        for (auto& node : _nodes)
            node.setLodID(id);
    }   
    else
        for (auto& subtree : _subtrees)
            subtree.setLodIDs(playerPosition);
}