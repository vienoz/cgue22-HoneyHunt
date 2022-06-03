#include "Entity.h"

static void physXMat4ToGlmMat4(const physx::PxMat44& mat4, glm::mat4& newMat)
{
    newMat[0][0] = mat4[0][0];
    newMat[0][1] = mat4[0][1];
    newMat[0][2] = mat4[0][2];
    newMat[0][3] = mat4[0][3];

    newMat[1][0] = mat4[1][0];
    newMat[1][1] = mat4[1][1];
    newMat[1][2] = mat4[1][2];
    newMat[1][3] = mat4[1][3];

    newMat[2][0] = mat4[2][0];
    newMat[2][1] = mat4[2][1];
    newMat[2][2] = mat4[2][2];
    newMat[2][3] = mat4[2][3];

    newMat[3][0] = mat4[3][0];
    newMat[3][1] = mat4[3][1];
    newMat[3][2] = mat4[3][2];
    newMat[3][3] = mat4[3][3];
}

static void glmMat4ToPhysxMat4(const glm::mat4& mat4, physx::PxMat44& newMat)
{
    newMat[0][0] = mat4[0][0];
    newMat[0][1] = mat4[0][1];
    newMat[0][2] = mat4[0][2];
    newMat[0][3] = mat4[0][3];

    newMat[1][0] = mat4[1][0];
    newMat[1][1] = mat4[1][1];
    newMat[1][2] = mat4[1][2];
    newMat[1][3] = mat4[1][3];

    newMat[2][0] = mat4[2][0];
    newMat[2][1] = mat4[2][1];
    newMat[2][2] = mat4[2][2];
    newMat[2][3] = mat4[2][3];

    newMat[3][0] = mat4[3][0];
    newMat[3][1] = mat4[3][1];
    newMat[3][2] = mat4[3][2];
    newMat[3][3] = mat4[3][3];
}

PhysxEntity::PhysxEntity(GamePhysx& physx, std::shared_ptr<Model> model)
    : _physx(physx), _model(model)
{
}

/*PhysxEntity::~PhysxEntity()
{
    if (_actor != nullptr && _actor->isReleasable())
    {
        // TODO check if stupid
        _physx.getScene()->removeActor(*_actor);
        _actor->release();
    }
}*/

void PhysxDynamicEntity::setGlobalPose(glm::mat4 transform)
{
    physx::PxMat44 mat;
    glmMat4ToPhysxMat4(transform, mat);

    _actor->setGlobalPose(physx::PxTransform(mat));
}

void PhysxDynamicEntity::draw(Camera& camera)
{
    glm::mat4 modelMatrix;
    physXMat4ToGlmMat4(_actor->getGlobalPose(), modelMatrix);

    _model->draw(modelMatrix, camera);
}

PhysxDynamicEntity::PhysxDynamicEntity(GamePhysx& physx, std::shared_ptr<Model> model, std::vector<physx::PxGeometry> shapes, bool isKinematic)
    : PhysxEntity(physx, model)
{
    //physx::PxRigidStatic
    physx::PxRigidDynamic* me = physx.getPhysics()->createRigidDynamic(physx::PxTransform(physx::PxVec3(0.f, 0.f, 0.f)));


    physx::PxShape* aBoxShape = physx::PxRigidActorExt::createExclusiveShape(*me, physx::PxBoxGeometry(1, 1, 1), *physx.getMaterial());
    physx::PxShape* shape = physx.gPhysics->createShape(physx::PxSphereGeometry(1.0f), *physx.getMaterial(), true);
    shape->setLocalPose(physx::PxTransform(physx::PxVec3(1.0, 1.0, -5.0)));
    me->attachShape(*shape);


    physx::PxTransform a;
    a.p = physx::PxVec3(0.0, 4.0, 0.0);
  

    me->setActorFlag(physx::PxActorFlag::eDISABLE_GRAVITY, false);
    _actor = me;

    physx.getScene()->addActor(*_actor);
}

PhysxStaticEntity::PhysxStaticEntity(GamePhysx& physx, std::shared_ptr<Model> model, std::vector<physx::PxGeometry> shapes, bool isKinematic)
    : PhysxEntity(physx, model)
{
    physx::PxRigidStatic* me = physx.getPhysics()->createRigidStatic(physx::PxTransform(physx::PxVec3(0.f, 0.f, 0.f)));


    physx::PxShape* aBoxShape = physx::PxRigidActorExt::createExclusiveShape(*me, physx::PxBoxGeometry(1, 1, 1), *physx.getMaterial());
    physx::PxShape* shape = physx.gPhysics->createShape(physx::PxSphereGeometry(1.0f), *physx.getMaterial(), true);
    shape->setLocalPose(physx::PxTransform(physx::PxVec3(1.0, 1.0, -5.0)));
    me->attachShape(*shape);


    physx::PxTransform a;
    a.p = physx::PxVec3(0.0, 4.0, 0.0);


    me->setActorFlag(physx::PxActorFlag::eDISABLE_GRAVITY, false);
    _actor = me;

    physx.getScene()->addActor(*_actor);
}

void PhysxStaticEntity::draw(Camera& camera)
{
    glm::mat4 modelMatrix;
    physXMat4ToGlmMat4(_actor->getGlobalPose(), modelMatrix);

    _model->draw(modelMatrix, camera);
}

void PhysxStaticEntity::setGlobalPose(glm::mat4 transform)
{
    physx::PxMat44 mat;
    glmMat4ToPhysxMat4(transform, mat);

    _actor->setGlobalPose(physx::PxTransform(mat));
}
