#pragma once

#include <glm/glm.hpp>

#include "GamePhysx.h"
#include "Model.h"
#include "Camera.h"

class PhysxEntity
{
protected:
  
    std::shared_ptr<Model> _model;
    GamePhysx& _physx;

public:
    PhysxEntity(GamePhysx& physx, std::shared_ptr<Model> model);
    //~PhysxEntity();

};
  
class PhysxDynamicEntity : public PhysxEntity
{
public:
    physx::PxRigidDynamic* _actor;
    virtual void draw(Camera& camera, DirectionalLight& dirL);
    virtual void setGlobalPose(glm::mat4 transform);
    virtual glm::vec3 getPosition();

    physx::PxRigidDynamic* getPhysxActor()
    {
        return _actor;
    }

    PhysxDynamicEntity(GamePhysx& physx, std::shared_ptr<Model> model, std::vector<physx::PxGeometry> shapes, bool isDynamic);
};

//TODO replace flower bool with enum for different geometries in scene
class PhysxStaticEntity : public PhysxEntity {
public: 
    bool flowerToBeVisited;
    physx::PxRigidStatic* _actor;
    virtual void draw(Camera& camera, DirectionalLight& dirL);
    virtual void setGlobalPose(glm::mat4 transform);
    virtual glm::vec3 getPosition();

    physx::PxRigidStatic* getPhysxActor()
    {
        return _actor;
    }

    PhysxStaticEntity(GamePhysx& physx, std::shared_ptr<Model> model, std::vector<physx::PxGeometry> shapes, bool isDynamic);
};