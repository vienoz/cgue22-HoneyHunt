#pragma once

#include <cstdio>

#include <PxPhysicsAPI.h>
#include <geometry/PxGeometry.h>
#include <geometry/PxBoxGeometry.h>
#include <PxRigidActor.h>
#include <PxRigidDynamic.h>
#include <PxRigidStatic.h>
#include <foundation/PxTransform.h>
#include <foundation/PxVec3.h>
#include <PxActor.h>
#include <iostream>

class GamePhysxCallback : public physx::PxSimulationEventCallback {

	void onConstraintBreak(physx::PxConstraintInfo* /*constraints*/, physx::PxU32 /*count*/) override { std::printf("onConstraintBreak\n"); }
	void onWake(physx::PxActor** /*actors*/, physx::PxU32 /*count*/) override { std::printf("onWake\n"); }
	void onSleep(physx::PxActor** /*actors*/, physx::PxU32 /*count*/) override { std::printf("onSleep\n"); }
	void onTrigger(physx::PxTriggerPair* /*pairs*/, physx::PxU32 /*count*/) override { std::printf("onTrigger\n"); }
	void onAdvance(const physx::PxRigidBody* const* /*bodyBuffer*/, const physx::PxTransform* /*poseBuffer*/, const physx::PxU32 /*count*/) override { std::printf("onAdvance\n"); }

	void onContact(const physx::PxContactPairHeader& pairHeader, const physx::PxContactPair* pairs, physx::PxU32 nbPairs) {
		(void)(pairHeader);
		physx::PxVec3 a = pairHeader.actors[1]->getGlobalPose().p;
		collisionObj = pairHeader.actors[1];
		collisionShapes = pairs->shapes[1];
		//std::printf("Colission detected\n");
	}
public:
	physx::PxRigidActor* collisionObj;
	physx::PxShape* collisionShapes;

};

class GamePhysx
{
public:
	GamePhysx();
	~GamePhysx();

	physx::PxFoundation* getFoundation() const;
	physx::PxPhysics* gPhysics = nullptr;
	physx::PxPhysics* getPhysics() const;
	physx::PxScene* getScene() const;
	physx::PxMaterial* getMaterial() const;
	GamePhysxCallback callback;

private:
	physx::PxDefaultAllocator		gAllocator;
	physx::PxDefaultErrorCallback	gErrorCallback;
	physx::PxFoundation* gFoundation = nullptr;
	physx::PxDefaultCpuDispatcher* gDispatcher = nullptr;
	physx::PxPvd* gPvd = nullptr;

	physx::PxScene* gScene = nullptr;
	physx::PxMaterial* gMaterial = nullptr;



	void init();
};
