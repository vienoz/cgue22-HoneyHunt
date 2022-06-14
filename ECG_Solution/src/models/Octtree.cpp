#include "Octtree.h"

bool Octtree::IsLodActive = true;

Octtree::Octtree(glm::vec3 origin, glm::vec3 maxDimension, int capacity, float lod_distance_min, float lod_distance_max)
{
    _lod_distance_min = lod_distance_min;
    _lod_distance_max = lod_distance_max;
    _maxDimension = maxDimension;
    _capacity = capacity;
    _origin = origin;
    _hasDivided = false;
    _index = 0;
}

Octtree::Octtree() {
}

Octtree::~Octtree()
{
    _subtrees.clear();
    _nodes.clear();
}

void Octtree::insert(OcttreeNode node) {
    if (!contains(node.getPosition()))
        return;

    if (_nodes.size() < _capacity) 
        _nodes.push_back(node);
    else 
    {
        if (!_hasDivided) 
            subdivide();
        for (auto& subtree : _subtrees)
            subtree.insert(node);
    }
}

void Octtree::subdivide() {
    float offsetX = _maxDimension.x / 2;
    float offsetY = _maxDimension.y / 2;
    float offsetZ = _maxDimension.z / 2;
    glm::vec3 newDimension = glm::vec3(offsetX, offsetY, offsetZ);
    _hasDivided = true;
    glm::vec3 newOrigin; // grid: right = +x, up = +y, forward = +z

    // cube dimension calculations
    newOrigin = glm::vec3(_origin.x + offsetX, _origin.y - offsetY, _origin.z + offsetZ);
    _subtrees.push_back(Octtree(newOrigin, newDimension, _capacity, _lod_distance_min, _lod_distance_max));
    newOrigin = glm::vec3(_origin.x - offsetX, _origin.y - offsetY, _origin.z + offsetZ);
    _subtrees.push_back(Octtree(newOrigin, newDimension, _capacity, _lod_distance_min, _lod_distance_max));
    newOrigin = glm::vec3(_origin.x + offsetX, _origin.y - offsetY, _origin.z - offsetZ);
    _subtrees.push_back(Octtree(newOrigin, newDimension, _capacity, _lod_distance_min, _lod_distance_max));
    newOrigin = glm::vec3(_origin.x - offsetX, _origin.y - offsetY, _origin.z - offsetZ);
    _subtrees.push_back(Octtree(newOrigin, newDimension, _capacity, _lod_distance_min, _lod_distance_max));
    newOrigin = glm::vec3(_origin.x + offsetX, _origin.y + offsetY, _origin.z + offsetZ);
    _subtrees.push_back(Octtree(newOrigin, newDimension, _capacity, _lod_distance_min, _lod_distance_max));
    newOrigin = glm::vec3(_origin.x - offsetX, _origin.y + offsetY, _origin.z + offsetZ);
    _subtrees.push_back(Octtree(newOrigin, newDimension, _capacity, _lod_distance_min, _lod_distance_max));
    newOrigin = glm::vec3(_origin.x + offsetX, _origin.y + offsetY, _origin.z - offsetZ);
    _subtrees.push_back(Octtree(newOrigin, newDimension, _capacity, _lod_distance_min, _lod_distance_max));
    newOrigin = glm::vec3(_origin.x - offsetX, _origin.y + offsetY, _origin.z - offsetZ);
    _subtrees.push_back(Octtree(newOrigin, newDimension, _capacity, _lod_distance_min, _lod_distance_max));

    //pass node to children
    for (auto& node: _nodes)
        for (auto& subtree : _subtrees) 
            subtree.insert(node);
}

bool Octtree::contains(glm::vec3 point) {
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
            if (Octtree::IsLodActive) {
                node.getLodModels()->drawActive(camera, dirL);
            }
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
        if (glm::distance(_origin, playerPosition) < _lod_distance_min)
            id = 0;
        else if (glm::distance(_origin, playerPosition) < _lod_distance_max)
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