#pragma once

#include <istream>
#include <memory>
#include <iostream>
#include <PxPhysicsAPI.h>




class GamePhysxCallback : public physx::PxSimulationEventCallback {

	
	 
	void onConstraintBreak(physx::PxConstraintInfo* /*constraints*/, physx::PxU32 /*count*/) override { std::printf("onConstraintBreak\n"); }
	void onWake(physx::PxActor** /*actors*/, physx::PxU32 /*count*/) override { std::printf("onWake\n"); }
	void onSleep(physx::PxActor** /*actors*/, physx::PxU32 /*count*/) override { std::printf("onSleep\n"); }
	void onTrigger(physx::PxTriggerPair* /*pairs*/, physx::PxU32 /*count*/) override { std::printf("onTrigger\n"); }
	void onAdvance(const physx::PxRigidBody* const* /*bodyBuffer*/, const physx::PxTransform* /*poseBuffer*/, const physx::PxU32 /*count*/) override { std::printf("onAdvance\n"); }

	void onContact(const physx::PxContactPairHeader& pairHeader, const physx::PxContactPair* pairs, physx::PxU32 nbPairs) {
		(void)(pairHeader);
		physx::PxVec3 a = pairHeader.actors[1]->getGlobalPose().p;
		//std::cout << a.x <<" "<< a.y << " " << a.z << std::endl;
		collisionObj = pairHeader.actors[1];
	
	}

public:
	physx::PxRigidActor* collisionObj;

};

