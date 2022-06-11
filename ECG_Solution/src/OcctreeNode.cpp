#include "OcttreeNode.h"

OcttreeNode::OcttreeNode(std::shared_ptr<LODModel> lodModels)
    : _lodModels(lodModels)
{
    _position = lodModels->getActiveModel()->getPosition();
}

OcttreeNode::~OcttreeNode()
{
}

glm::vec3 OcttreeNode::getPosition() {
    return _position;
}

void OcttreeNode::setLodID(int id) {
    _lodModels->setLodID(id);
}

void OcttreeNode::print() {
    std::cout << "Node Position x:" << _position.x << ", y:" << _position.y << ", z:" << _position.z << std::endl;
}

std::shared_ptr<LODModel> OcttreeNode::getLodModels() {
    return _lodModels;
}