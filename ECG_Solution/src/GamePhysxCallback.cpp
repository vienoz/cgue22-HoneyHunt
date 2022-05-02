#pragma once
#include <PxPhysicsAPI.h>
#include <memory>

class GamePhysxCallback : public physx::PxSimulationEventCallback {

	void onConstraintBreak(physx::PxConstraintInfo* /*constraints*/, physx::PxU32 /*count*/) override { std::printf("onConstraintBreak\n"); }
	void onWake(physx::PxActor** /*actors*/, physx::PxU32 /*count*/) override { std::printf("onWake\n"); }
	void onSleep(physx::PxActor** /*actors*/, physx::PxU32 /*count*/) override { std::printf("onSleep\n"); }
	void onTrigger(physx::PxTriggerPair* /*pairs*/, physx::PxU32 /*count*/) override { std::printf("onTrigger\n"); }
	void onAdvance(const physx::PxRigidBody* const* /*bodyBuffer*/, const physx::PxTransform* /*poseBuffer*/, const physx::PxU32 /*count*/) override { std::printf("onAdvance\n"); }

	void onContact(const physx::PxContactPairHeader& pairHeader, const physx::PxContactPair* pairs, physx::PxU32 nbPairs) {
		std::printf("Colission detected\n");
	}

};

