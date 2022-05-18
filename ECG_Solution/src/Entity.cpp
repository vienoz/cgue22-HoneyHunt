#include "Entity.h"

glm::vec3 Entity::getPosition()
{
    return _position;
}

void Entity::setPosition(glm::vec3 value)
{
    _position = value;
    _isModelMatrixDirty = true;
}

glm::vec3 Entity::getRotation()
{
    return _rotation;
}

void Entity::setRotation(glm::vec3 value)
{
    _rotation = value;
    _isModelMatrixDirty = true;
}

glm::vec3 Entity::getScale()
{
    return _scale;
}

void Entity::setScale(glm::vec3 value)
{
    _scale = value;
    _isModelMatrixDirty = true;
}

void Entity::draw(Camera& camera)
{
    if (_isModelMatrixDirty)
    {
        _modelMatrix = glm::mat4();

        _modelMatrix = glm::translate(_modelMatrix, _position);
        _modelMatrix = glm::scale(_modelMatrix, _scale);

        _modelMatrix = glm::rotate(_modelMatrix, _rotation.x, glm::vec3(0, 1, 0));
        _modelMatrix = glm::rotate(_modelMatrix, _rotation.z, glm::vec3(1, 0, 0));
        _modelMatrix = glm::rotate(_modelMatrix, _rotation.y, glm::vec3(0, 0, 1));

        _isModelMatrixDirty = false;
    }

    _model->draw(_modelMatrix, camera);
}

// ==============================================

PhysxRigidEntity::PhysxRigidEntity(std::shared_ptr<Model> model, GamePhysx& physx, physx::PxShape* shape)
    : Entity(model)
{
    _pxObject = physx.getPhysics()->createRigidDynamic(physx::PxTransform(physx::PxVec3(0, 0, 0)));
    _pxObject->attachShape(*shape);
}


void PhysxRigidEntity::setPosition(glm::vec3 value)
{
    Entity::setPosition(value);
}

void PhysxRigidEntity::setRotation(glm::vec3 value)
{
    Entity::setRotation(value);
}

void PhysxRigidEntity::setScale(glm::vec3 value)
{
    Entity::setScale(value);
}