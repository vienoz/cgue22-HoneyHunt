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

void PhysxDynamicEntity::draw(Camera& camera, DirectionalLight& dirL)
{
    glm::mat4 modelMatrix;
    physXMat4ToGlmMat4(_actor->getGlobalPose(), modelMatrix);

    _model->draw(modelMatrix, camera, dirL);
}

PhysxDynamicEntity::PhysxDynamicEntity(GamePhysx& physx, std::shared_ptr<Model> model, std::vector<physx::PxGeometry> shapes, bool flower)
    : PhysxEntity(physx, model)
{
    //physx::PxRigidStatic
    physx::PxRigidDynamic* me = physx.getPhysics()->createRigidDynamic(physx::PxTransform(physx::PxVec3(0.f, 0.f, 0.f)));

    physx::PxShape* aBoxShape = physx::PxRigidActorExt::createExclusiveShape(*me, physx::PxCapsuleGeometry(1, 2), *physx.getMaterial());

    physx::PxQuat(1.570796, physx::PxVec3(0.0, 1.0, 0.0));
    aBoxShape->setLocalPose(physx::PxTransform(physx::PxQuat(1.570796, physx::PxVec3(0.0, 1.0, 0.0))));

  

    me->setActorFlag(physx::PxActorFlag::eDISABLE_GRAVITY, false);
    _actor = me;

    physx.getScene()->addActor(*_actor);
}

glm::vec3 PhysxDynamicEntity::getPosition()
{
    auto pos = _actor->getGlobalPose().p;
    return glm::vec3(pos.x, pos.y, pos.z);
}


//first shape should always be collision relevant one, which is the sphere in this instance for whatever reason
PhysxStaticEntity::PhysxStaticEntity(GamePhysx& physx, std::shared_ptr<Model> model, std::vector<physx::PxGeometry> shapes, bool flower)
    : PhysxEntity(physx, model)
{
    flowerToBeVisited = flower;
    physx::PxRigidStatic* me = physx.getPhysics()->createRigidStatic(physx::PxTransform(physx::PxVec3(0.f, 0.f, 0.f)));
    if (!flower) {
        physx::PxShape* aBoxShape = physx::PxRigidActorExt::createExclusiveShape(*me, physx::PxBoxGeometry(1, 1, 1), *physx.getMaterial());
        physx::PxShape* shape = physx.gPhysics->createShape(physx::PxSphereGeometry(1.0f), *physx.getMaterial(), true);
        shape->setLocalPose(physx::PxTransform(physx::PxVec3(0.0, 4.0, 0.0)));
        me->attachShape(*shape);
    }

    if (flower) {
        physx::PxShape* stem = physx::PxRigidActorExt::createExclusiveShape(*me, physx::PxBoxGeometry(0.5, 8, 0.5), *physx.getMaterial());
        physx::PxShape* base = physx.gPhysics->createShape(physx::PxBoxGeometry(3.5, 0.2, 3.5), *physx.getMaterial(), true);
        base->setLocalPose(physx::PxTransform(physx::PxVec3(0.0, 8.1, 0.0)));
        physx::PxShape* blossom = physx.gPhysics->createShape(physx::PxCapsuleGeometry(1, 1.5), *physx.getMaterial(), true);
        physx::PxTransform blossomTrans;
        blossomTrans.p = physx::PxVec3(0.0, 8.5, 0.0);
        blossomTrans.q= physx::PxQuat(1.570796, physx::PxVec3(0.0, 0.0, 1.0));
        blossom->setLocalPose(blossomTrans);
        //blossom->setLocalPose(physx::PxTransform(physx::PxVec3(0.0, 8.5, 0.0)));
        //physx::PxShape* shape = physx.gPhysics->createShape(physx::PxSphereGeometry(1.0f), *physx.getMaterial(), true);
        me->attachShape(*base);
        me->attachShape(*blossom);
    }





    physx::PxTransform a;
    a.p = physx::PxVec3(0.0, 4.0, 0.0);


    me->setActorFlag(physx::PxActorFlag::eDISABLE_GRAVITY, false);
    _actor = me;

    physx.getScene()->addActor(*_actor);
}

void PhysxStaticEntity::draw(Camera& camera, DirectionalLight& dirL)
{
    glm::mat4 modelMatrix;
    physXMat4ToGlmMat4(_actor->getGlobalPose(), modelMatrix);

    _model->draw(modelMatrix, camera, dirL);
}

void PhysxStaticEntity::setGlobalPose(glm::mat4 transform)
{
    physx::PxMat44 mat;
    glmMat4ToPhysxMat4(transform, mat);

    _actor->setGlobalPose(physx::PxTransform(mat));
}

glm::vec3 PhysxStaticEntity::getPosition()
{
    auto pos = _actor->getGlobalPose().p;
    return glm::vec3(pos.x, pos.y, pos.z);
}
