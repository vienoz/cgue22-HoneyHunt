#pragma once

#include <glm/glm.hpp>

#include "Model.h"
#include "Camera.h"
#include "GamePhysx.h"

class Entity
{
protected:
    std::shared_ptr<Model> _model;

    glm::vec3 _position;
    glm::vec3 _rotation; // Euler Angles - Pan (x), Tilt (y), Roll (z)
    glm::vec3 _scale;

    glm::mat4 _modelMatrix;
    bool _isModelMatrixDirty;

public:
    Entity(std::shared_ptr<Model> model)
        : _model(model), _isModelMatrixDirty(true)
    {}

    virtual glm::vec3 getPosition();
    virtual void setPosition(glm::vec3 value);

    virtual glm::vec3 getRotation();
    virtual void setRotation(glm::vec3 value);

    virtual glm::vec3 getScale();
    virtual void setScale(glm::vec3 value);

    virtual void draw(Camera& camera);
};

// ==============================================

class PhysxRigidEntity : public Entity
{
protected:
    physx::PxRigidBody* _pxObject;

public:
    PhysxRigidEntity(std::shared_ptr<Model> model, GamePhysx& physx, physx::PxShape* shape);
    ~PhysxRigidEntity();

    virtual glm::vec3 getPosition() override;
    virtual void setPosition(glm::vec3 value) override;

    virtual glm::vec3 getRotation() override;
    virtual void setRotation(glm::vec3 value) override;

    virtual glm::vec3 getScale() override;
    virtual void setScale(glm::vec3 value) override;

    virtual void draw(Camera& camera) override;
};