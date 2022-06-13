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

PhysxDynamicEntity::PhysxDynamicEntity(GamePhysx& gphysx, std::shared_ptr<Model> model, bool flower = false)
    : PhysxEntity(gphysx, model)
{
    physx::PxRigidDynamic* actor = gphysx.getPhysics()->createRigidDynamic(physx::PxTransform(physx::PxVec3(0.f, 0.f, 0.f)));

    physx::PxShape* aBoxShape = physx::PxRigidActorExt::createExclusiveShape(*actor, physx::PxCapsuleGeometry(1, 2), *gphysx.getMaterial());

    physx::PxQuat(1.570796, physx::PxVec3(0.0, 1.0, 0.0));
    aBoxShape->setLocalPose(physx::PxTransform(physx::PxQuat(1.570796, physx::PxVec3(0.0, 1.0, 0.0))));

  
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


PhysxStaticEntity::PhysxStaticEntity(GamePhysx& gphysx, physx::PxRigidActor* actor, std::shared_ptr<Model> model, bool flower, const char* name)
    : PhysxEntity(gphysx, model)
{
    flowerToBeVisited = flower;
    physx::PxRigidStatic* rbStatic = gphysx.getPhysics()->createRigidStatic(physx::PxTransform(physx::PxVec3(0.f, 0.f, 0.f)));
    rbStatic->setName(name);
    if (!flowerToBeVisited) {

        //create pxTriangleMesh for each Mesh per Model
        for(Mesh* mesh : *(model->getMeshes()))
        {
            auto pxMesh = mesh->createPxMesh(gphysx);
            physx::PxMeshScale scale( physx::PxVec3(1,1,1),  physx::PxQuat(physx::PxIdentity));
            physx::PxTriangleMeshGeometry geom(pxMesh, scale);
            physx::PxShape* shape = physx::PxRigidActorExt::createExclusiveShape(*actor, geom ,*gphysx.getMaterial());
            shape->setLocalPose(actor->getGlobalPose());
            rbStatic->attachShape(*shape);
        }        
    }

    if (flowerToBeVisited) {
        physx::PxShape* stem = physx::PxRigidActorExt::createExclusiveShape(*rbStatic, physx::PxBoxGeometry(0.5, 8, 0.5), *gphysx.getMaterial());
        physx::PxShape* base = gphysx.getPhysics()->createShape(physx::PxBoxGeometry(3.5, 0.2, 3.5), *gphysx.getMaterial(), true);
        base->setLocalPose(physx::PxTransform(physx::PxVec3(0.0, 8.1, 0.0)));
        physx::PxShape* blossom = gphysx.getPhysics()->createShape(physx::PxCapsuleGeometry(1, 1.5), *gphysx.getMaterial(), true);
        physx::PxTransform blossomTrans;
        blossomTrans.p = physx::PxVec3(0.0, 8.5, 0.0);
        blossomTrans.q= physx::PxQuat(1.570796, physx::PxVec3(0.0, 0.0, 1.0));
        blossom->setLocalPose(blossomTrans);
        //blossom->setLocalPose(physx::PxTransform(physx::PxVec3(0.0, 8.5, 0.0)));
        //physx::PxShape* shape = physx.getPhysics()->createShape(physx::PxSphereGeometry(1.0f), *physx.getMaterial(), true);
        rbStatic->attachShape(*base);
        rbStatic->attachShape(*blossom);
    }

    rbStatic->setActorFlag(physx::PxActorFlag::eDISABLE_GRAVITY, false);
    _actor = rbStatic;

    gphysx.getScene()->addActor(*_actor);
    std::cout << _actor->getName() << std::endl;
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
