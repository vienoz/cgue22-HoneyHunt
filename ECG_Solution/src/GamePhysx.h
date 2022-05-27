#pragma once

#include "GamePhysxCallback.cpp";


class GamePhysx {
public:
	GamePhysx();
	~GamePhysx();

	physx::PxFoundation* getFoundation() const;
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
	physx::PxPhysics* gPhysics = nullptr;
	physx::PxMaterial* gMaterial = nullptr;

	void init();
};
