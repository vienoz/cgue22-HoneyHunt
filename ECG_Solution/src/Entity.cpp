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

PhysxDynamicEntity::PhysxDynamicEntity(GamePhysx& physx, std::shared_ptr<Model> model, std::vector<physx::PxGeometry> shapes, bool flower = false)
    : PhysxEntity(physx, model)
{
    physx::PxRigidDynamic* me = physx.getPhysics()->createRigidDynamic(physx::PxTransform(physx::PxVec3(0.f, 0.f, 0.f)));

    physx::PxShape* aBoxShape = physx::PxRigidActorExt::createExclusiveShape(*me, physx::PxCapsuleGeometry(1, 2), *physx.getMaterial());

    physx::PxQuat(1.570796, physx::PxVec3(0.0, 1.0, 0.0));
    aBoxShape->setLocalPose(physx::PxTransform(physx::PxQuat(1.570796, physx::PxVec3(0.0, 1.0, 0.0))));

  

    me->setActorFlag(physx::PxActorFlag::eDISABLE_GRAVITY, false);
    _actor = me;
    _actor->setName("Player");

    physx.getScene()->addActor(*_actor);
}

glm::vec3 PhysxDynamicEntity::getPosition()
{
    auto pos = _actor->getGlobalPose().p;
    return glm::vec3(pos.x, pos.y, pos.z);
}


PhysxStaticEntity::PhysxStaticEntity(GamePhysx& physx, std::shared_ptr<Model> model, std::vector<physx::PxGeometry> shapes, bool flower, objType type = objType::Default)
    : PhysxEntity(physx, model)
{
    flowerToBeVisited = flower;
    physx::PxRigidStatic* me = physx.getPhysics()->createRigidStatic(physx::PxTransform(physx::PxVec3(0.f, 0.f, 0.f)));
    switch (type) {
    case objType::Tree: {
        me->setName("tree");
        physx::PxShape* aBoxShape = physx::PxRigidActorExt::createExclusiveShape(*me, physx::PxBoxGeometry(1, 5, 1), *physx.getMaterial());
        physx::PxShape* branchLeft = physx.gPhysics->createShape(physx::PxBoxGeometry(1, 8, 1), *physx.getMaterial(), true);
        physx::PxTransform blTrans;
        blTrans.p = physx::PxVec3(4.5, 5.0, 0.0);
        blTrans.q = physx::PxQuat(-0.360305, physx::PxVec3(0.0, 0.0, 1.0));
        branchLeft->setLocalPose(blTrans);

        physx::PxShape* branchLeft2 = physx.gPhysics->createShape(physx::PxBoxGeometry(1, 5, 1), *physx.getMaterial(), true);
        physx::PxTransform bl2Trans;
        bl2Trans.p = physx::PxVec3(10.0, 15.0, -1.0);
        bl2Trans.q = physx::PxQuat(-0.463225, physx::PxVec3(0.0, 0.0, 1.0));
        branchLeft2->setLocalPose(bl2Trans);

        physx::PxShape* branchRight = physx.gPhysics->createShape(physx::PxBoxGeometry(1, 5, 1), *physx.getMaterial(), true);
        physx::PxTransform brTrans;
        brTrans.p = physx::PxVec3(-3.0, 5.0, 0.0);
        brTrans.q = physx::PxQuat(0.663225, physx::PxVec3(0.0, 0.0, 1.0));
        branchRight->setLocalPose(brTrans);

        physx::PxShape* branchRight2 = physx.gPhysics->createShape(physx::PxBoxGeometry(1, 5, 1), *physx.getMaterial(), true);
        physx::PxTransform br2Trans;
        br2Trans.p = physx::PxVec3(-5.0, 13.0, 0.0);
        br2Trans.q = physx::PxQuat(0.05, physx::PxVec3(0.0, 0.0, 1.0));
        branchRight2->setLocalPose(br2Trans);

        physx::PxShape* leavesLarge = physx.gPhysics->createShape(physx::PxSphereGeometry(8), *physx.getMaterial(), true);
        physx::PxTransform llTrans;
        llTrans.p = physx::PxVec3(-8.0, 26.0, 0.0);
        llTrans.q = physx::PxQuat(0.05, physx::PxVec3(0.0, 0.9, 1.0));
        leavesLarge->setLocalPose(llTrans);
   
        physx::PxShape* leavesMedium = physx.gPhysics->createShape(physx::PxSphereGeometry(6), *physx.getMaterial(), true);
        physx::PxTransform lmTrans;
        lmTrans.p = physx::PxVec3(-15.0, 23.0, -4.0);              
        lmTrans.q = physx::PxQuat(0.05, physx::PxVec3(0.0, 0.9, 0.0)); 
        leavesMedium->setLocalPose(lmTrans);

        physx::PxShape* leavesSmall = physx.gPhysics->createShape(physx::PxSphereGeometry(3.4), *physx.getMaterial(), true);
        physx::PxTransform lsTrans;
        lsTrans.p = physx::PxVec3(3.0, 36.0, 1.0);
        lsTrans.q = physx::PxQuat(0.05, physx::PxVec3(0.0, 0.9, 0.0));
        leavesSmall->setLocalPose(lsTrans);

        physx::PxShape* leavesLarge2 = physx.gPhysics->createShape(physx::PxSphereGeometry(7), *physx.getMaterial(), true);
        physx::PxTransform ll2Trans;
        ll2Trans.p = physx::PxVec3(15.0, 30.0, -2.0);
        ll2Trans.q = physx::PxQuat(0.05, physx::PxVec3(0.0, 0.9, 1.0));
        leavesLarge2->setLocalPose(ll2Trans);

        physx::PxShape* leavesMedium2 = physx.gPhysics->createShape(physx::PxSphereGeometry(6), *physx.getMaterial(), true);
        physx::PxTransform lm2Trans;
        lm2Trans.p = physx::PxVec3(-15.0, 23.0, -4.0);
        lm2Trans.q = physx::PxQuat(0.05, physx::PxVec3(0.0, 0.9, 0.0));
        leavesMedium2->setLocalPose(lm2Trans);
        
        me->attachShape(*branchRight);
        me->attachShape(*branchRight2);
        me->attachShape(*branchLeft);
        me->attachShape(*branchLeft2);
        me->attachShape(*leavesLarge);
        me->attachShape(*leavesMedium);
        me->attachShape(*leavesSmall);
        me->attachShape(*leavesLarge2);
        me->attachShape(*leavesMedium2);
    }
            break;

    case objType::Flower: {
        me->setName("flower");
        physx::PxShape* stem = physx::PxRigidActorExt::createExclusiveShape(*me, physx::PxBoxGeometry(0.5, 8, 0.5), *physx.getMaterial());
        physx::PxShape* base = physx.gPhysics->createShape(physx::PxBoxGeometry(3.5, 0.2, 3.5), *physx.getMaterial(), true);
        base->setLocalPose(physx::PxTransform(physx::PxVec3(0.0, 8.1, 0.0)));
        physx::PxShape* blossom = physx.gPhysics->createShape(physx::PxCapsuleGeometry(1, 1.5), *physx.getMaterial(), true);
        physx::PxTransform blossomTrans;
        blossomTrans.p = physx::PxVec3(0.0, 8.5, 0.0);
        blossomTrans.q = physx::PxQuat(1.570796, physx::PxVec3(0.0, 0.0, 1.0));
        blossom->setLocalPose(blossomTrans);
        //blossom->setLocalPose(physx::PxTransform(physx::PxVec3(0.0, 8.5, 0.0)));
        //physx::PxShape* shape = physx.gPhysics->createShape(physx::PxSphereGeometry(1.0f), *physx.getMaterial(), true);
        me->attachShape(*base);
        me->attachShape(*blossom); 
    }
            break;
    case objType::Ground: {
        me->setName("ground"); 
    }
        break;
    case objType::Default:{me->setName("default"); }
        break;
    case objType::Stump:{
        me->setName("stump"); 
        physx::PxShape* aBoxShape = physx::PxRigidActorExt::createExclusiveShape(*me, physx::PxBoxGeometry(3.5, 3.7, 3.5), *physx.getMaterial());
      
    }
        break;

    }




    me->setActorFlag(physx::PxActorFlag::eDISABLE_GRAVITY, false);
    _actor = me;
    physx.getScene()->addActor(*_actor);
    //std::cout << _actor->getName() << std::endl;
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
