#pragma once

#include <glm/glm.hpp>

#include "GamePhysx.h"
#include "Model.h"
#include "Camera.h"

class PhysxEntity
{
protected:
    std::shared_ptr<Model> _model;
    physx::PxRigidActor* _actor;
    GamePhysx& _physx;

public:
    PhysxEntity(GamePhysx& physx, std::shared_ptr<Model> model);
    ~PhysxEntity();

    physx::PxRigidActor* getPhysxActor()
    {
        return _actor;
    }

    virtual void setGlobalPose(glm::mat4 transform);
    virtual void draw(Camera& camera);
};

class PhysxDynamicEntity : public PhysxEntity
{
public:
    PhysxDynamicEntity(GamePhysx& physx, std::shared_ptr<Model> model, std::vector<physx::PxGeometry> shapes, bool isDynamic);
};

// ==============================================;