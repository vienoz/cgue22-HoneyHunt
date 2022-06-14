#pragma once

#include <glm/glm.hpp>
#include "../GamePhysx.h"
#include "../rendering/Camera.h"
#include "Model.h"


enum class objType { Tree, Flower, Stump, Bee, Ground, Default }; //used for generating appropriate physx actor

class PhysxEntity
{
protected:
    std::shared_ptr<Model> _model;
    GamePhysx& _physx;

public:
    PhysxEntity(GamePhysx& physx, std::shared_ptr<Model> model);
    ~PhysxEntity();
};
  
class PhysxDynamicEntity : public PhysxEntity
{
public:
    bool flowerToBeVisited =  false;
    physx::PxRigidDynamic* _actor;
    virtual void draw(Camera& camera, DirectionalLight& dirL);
    virtual void setGlobalPose(glm::mat4 transform);
    virtual glm::vec3 getPosition();

    physx::PxRigidDynamic* getPhysxActor()
    {
        return _actor;
    }

    PhysxDynamicEntity(GamePhysx& physx, std::shared_ptr<Model> model, bool flower);
};

//TODO replace flower bool with enum for different geometries in scene
class PhysxStaticEntity : public PhysxEntity {
public: 
    bool flowerToBeVisited;
    physx::PxRigidStatic* _rigidStatic;
    virtual void draw(Camera& camera, DirectionalLight& dirL);
    virtual void setGlobalPose(glm::mat4 transform);
    virtual glm::vec3 getPosition();

    physx::PxRigidStatic* getRigidStatic()
    {
        return _rigidStatic;
    }

    PhysxStaticEntity(GamePhysx& physx, std::shared_ptr<Model> model, bool flower, objType type);
};
