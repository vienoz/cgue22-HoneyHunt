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

PhysxEntity::~PhysxEntity()
{
}

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

PhysxDynamicEntity::PhysxDynamicEntity(GamePhysx& gphysx, std::shared_ptr<Model> model, bool flower = false)
    : PhysxEntity(gphysx, model)
{
    physx::PxRigidDynamic* actor = gphysx.getPhysics()->createRigidDynamic(physx::PxTransform(physx::PxVec3(0.f, 0.f, 0.f)));

    physx::PxShape* playerShape = physx::PxRigidActorExt::createExclusiveShape(*actor, physx::PxCapsuleGeometry(1, 2), *gphysx.getMaterial());
    physx::PxQuat(1.570796, physx::PxVec3(0.0, 1.0, 0.0));
    playerShape->setLocalPose(physx::PxTransform(physx::PxQuat(1.570796, physx::PxVec3(0.0, 1.0, 0.0))));

    actor->setActorFlag(physx::PxActorFlag::eDISABLE_GRAVITY, false);
    _actor = actor;
    _actor->setName("Player");
    gphysx.getScene()->addActor(*_actor);
}

glm::vec3 PhysxDynamicEntity::getPosition()
{
    auto pos = _actor->getGlobalPose().p;
    return glm::vec3(pos.x, pos.y, pos.z);
}

PhysxStaticEntity::PhysxStaticEntity(GamePhysx& gphysx, std::shared_ptr<Model> model, bool flower, objType type = objType::Default)
    : PhysxEntity(gphysx, model)
{
    objectType = type;
    flowerToBeVisited = flower;
    physx::PxRigidStatic* rbStatic = gphysx.getPhysics()->createRigidStatic(physx::PxTransform(physx::PxVec3(0.f, 0.f, 0.f)));
    physx::PxTransform transform;

    switch (type) {
    case objType::Tree: {
        rbStatic->setName("tree");
        physx::PxShape* branchLeft = gphysx.getPhysics()->createShape(physx::PxBoxGeometry(1, 8, 1), *gphysx.getMaterial(), true);
        transform.p = physx::PxVec3(4.5, 5.0, 0.0);
        transform.q = physx::PxQuat(-0.360305, physx::PxVec3(0.0, 0.0, 1.0));
        branchLeft->setLocalPose(transform);

        physx::PxShape* branchLeft2 = gphysx.getPhysics()->createShape(physx::PxBoxGeometry(1, 5, 1), *gphysx.getMaterial(), true);
        transform.p = physx::PxVec3(10.0, 15.0, -1.0);
        transform.q = physx::PxQuat(-0.463225, physx::PxVec3(0.0, 0.0, 1.0));
        branchLeft2->setLocalPose(transform);

        physx::PxShape* branchRight = gphysx.getPhysics()->createShape(physx::PxBoxGeometry(1, 5, 1), *gphysx.getMaterial(), true);
        transform.p = physx::PxVec3(-3.0, 5.0, 0.0);
        transform.q = physx::PxQuat(0.663225, physx::PxVec3(0.0, 0.0, 1.0));
        branchRight->setLocalPose(transform);

        physx::PxShape* branchRight2 = gphysx.getPhysics()->createShape(physx::PxBoxGeometry(1, 5, 1), *gphysx.getMaterial(), true);
        transform.p = physx::PxVec3(-5.0, 13.0, 0.0);
        transform.q = physx::PxQuat(0.05, physx::PxVec3(0.0, 0.0, 1.0));
        branchRight2->setLocalPose(transform);

        physx::PxShape* leavesLarge = gphysx.getPhysics()->createShape(physx::PxSphereGeometry(8), *gphysx.getMaterial(), true);
        transform.p = physx::PxVec3(-8.0, 26.0, 0.0);
        transform.q = physx::PxQuat(0.05, physx::PxVec3(0.0, 0.9, 1.0));
        leavesLarge->setLocalPose(transform);
   
        physx::PxShape* leavesMedium = gphysx.getPhysics()->createShape(physx::PxSphereGeometry(6), *gphysx.getMaterial(), true);
        transform.p = physx::PxVec3(-15.0, 23.0, -4.0);
        transform.q = physx::PxQuat(0.05, physx::PxVec3(0.0, 0.9, 0.0));
        leavesMedium->setLocalPose(transform);

        physx::PxShape* leavesSmall = gphysx.getPhysics()->createShape(physx::PxSphereGeometry(3.4), *gphysx.getMaterial(), true);
        transform.p = physx::PxVec3(3.0, 36.0, 1.0);
        transform.q = physx::PxQuat(0.05, physx::PxVec3(0.0, 0.9, 0.0));
        leavesSmall->setLocalPose(transform);

        physx::PxShape* leavesLarge2 = gphysx.getPhysics()->createShape(physx::PxSphereGeometry(7), *gphysx.getMaterial(), true);
        transform.p = physx::PxVec3(15.0, 30.0, -2.0);
        transform.q = physx::PxQuat(0.05, physx::PxVec3(0.0, 0.9, 1.0));
        leavesLarge2->setLocalPose(transform);

        physx::PxShape* leavesMedium2 = gphysx.getPhysics()->createShape(physx::PxSphereGeometry(6), *gphysx.getMaterial(), true);
        transform.p = physx::PxVec3(-15.0, 23.0, -4.0);
        transform.q = physx::PxQuat(0.05, physx::PxVec3(0.0, 0.9, 0.0));
        leavesMedium2->setLocalPose(transform);
        
        rbStatic->attachShape(*branchRight);
        rbStatic->attachShape(*branchRight2);
        rbStatic->attachShape(*branchLeft);
        rbStatic->attachShape(*branchLeft2);
        rbStatic->attachShape(*leavesLarge);
        rbStatic->attachShape(*leavesMedium);
        rbStatic->attachShape(*leavesSmall);
        rbStatic->attachShape(*leavesLarge2);
        rbStatic->attachShape(*leavesMedium2);
        }
        break;

    case objType::Flower: 
        {
        rbStatic->setName("flower");
        physx::PxShape* stem = physx::PxRigidActorExt::createExclusiveShape(*rbStatic, physx::PxBoxGeometry(0.5, 8, 0.5), *gphysx.getMaterial());
        physx::PxShape* base = gphysx.getPhysics()->createShape(physx::PxBoxGeometry(3.5, 0.2, 3.5), *gphysx.getMaterial(), true);
        base->setLocalPose(physx::PxTransform(physx::PxVec3(0.0, 8.1, 0.0)));
        
        physx::PxShape* blossom = gphysx.getPhysics()->createShape(physx::PxCapsuleGeometry(1, 1.5), *gphysx.getMaterial(), true);
        physx::PxTransform blossomTrans;
        blossomTrans.p = physx::PxVec3(0.0, 8.5, 0.0);
        blossomTrans.q = physx::PxQuat(1.570796, physx::PxVec3(0.0, 0.0, 1.0));
        blossom->setLocalPose(blossomTrans);
        
        rbStatic->attachShape(*base);
        rbStatic->attachShape(*blossom);
        }
        break;

    case objType::Ground: {
        rbStatic->setName("ground");
        }
        break;

    case objType::Default:{
        rbStatic->setName("default"); 
        }
        break;

    case objType::Stump:{
        rbStatic->setName("stump");
        physx::PxShape* aBoxShape = physx::PxRigidActorExt::createExclusiveShape(*rbStatic, physx::PxBoxGeometry(3.5, 3.7, 3.5), *gphysx.getMaterial());
        }
        break;
    case objType::PowerUp: {
        rbStatic->setName("powerUp");
        physx::PxShape* aBoxShape = physx::PxRigidActorExt::createExclusiveShape(*rbStatic, physx::PxSphereGeometry(1.6), *gphysx.getMaterial());
    }
    case objType::Hose : {
        rbStatic->setName("garden hose");
        physx::PxShape* aBoxShape = physx::PxRigidActorExt::createExclusiveShape(*rbStatic, physx::PxSphereGeometry(0.6), *gphysx.getMaterial());
        //TODO create rigidstatic
    }
    }

    
    /* // dynamic mesh cooking
    physx::PxTriangleMeshCookingResult::Enum result;
    auto pxMesh = model->getMeshes()->at(i)->createPxMesh(gphysx);
    physx::PxMeshScale scale( physx::PxVec3(1,1,1),  physx::PxQuat(physx::PxIdentity));
    physx::PxTriangleMeshGeometry geom(pxMesh, scale);
    physx::PxShape* shape = physx::PxRigidActorExt::createExclusiveShape(*rbStatic, geom ,*gphysx.getMaterial());
    */

    rbStatic->setActorFlag(physx::PxActorFlag::eDISABLE_GRAVITY, false);
    _rigidStatic = rbStatic;
    gphysx.getScene()->addActor(*_rigidStatic);
    std::cout << "Init:" << _rigidStatic->getName() << ", ";
}

void PhysxStaticEntity::draw(Camera& camera, DirectionalLight& dirL)
{
    glm::mat4 modelMatrix;
    physXMat4ToGlmMat4(_rigidStatic->getGlobalPose(), modelMatrix);
    _model->draw(modelMatrix, camera, dirL);
}

void PhysxStaticEntity::setGlobalPose(glm::mat4 transform)
{
    physx::PxMat44 mat;
    glmMat4ToPhysxMat4(transform, mat);
    _rigidStatic->setGlobalPose(physx::PxTransform(mat));
}

glm::vec3 PhysxStaticEntity::getPosition()
{
    auto pos = _rigidStatic->getGlobalPose().p;
    return glm::vec3(pos.x, pos.y, pos.z);
}
