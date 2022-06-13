#pragma once

#include "GamePhysx.h"

using namespace physx;

physx::PxFilterFlags contactFilter(
	physx::PxFilterObjectAttributes attributes0, physx::PxFilterData filterData0,
	physx::PxFilterObjectAttributes attributes1, physx::PxFilterData filterData1,
	physx::PxPairFlags& pairFlags, const void* constantBlock, physx::PxU32 constantBlockSize)
{
	// let triggers through
	if (physx::PxFilterObjectIsTrigger(attributes0) || physx::PxFilterObjectIsTrigger(attributes1))
	{
		pairFlags = physx::PxPairFlag::eTRIGGER_DEFAULT;
		return physx::PxFilterFlag::eDEFAULT;
	}
	// generate contacts for all that were not filtered above
	pairFlags = physx::PxPairFlag::eTRIGGER_DEFAULT | physx::PxPairFlag::eSOLVE_CONTACT;
	//pairFlags = physx::PxPairFlag::eCONTACT_DEFAULT;

	// trigger the contact callback for pairs (A,B) where 
	// the filtermask of A contains the ID of B and vice versa.
	if ((filterData0.word0 & filterData1.word1) && (filterData1.word0 & filterData0.word1))
		pairFlags |= physx::PxPairFlag::eNOTIFY_TOUCH_FOUND;

	return physx::PxFilterFlag::eDEFAULT;
}



GamePhysx::GamePhysx() {
	init();
}

GamePhysx::~GamePhysx() {

}

physx::PxFoundation* GamePhysx::getFoundation() const { return gFoundation; }
physx::PxPhysics* 	 GamePhysx::getPhysics() const { return gPhysics; }
physx::PxScene* 	 GamePhysx::getScene() const { return gScene; }
physx::PxMaterial* 	 GamePhysx::getMaterial() const { return gMaterial; }
physx::PxCooking* 	 GamePhysx::getCooking() const { return gCooking; }


void GamePhysx::init() {
	//init physx
	gFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gAllocator, gErrorCallback);
	if (!gFoundation) throw ("failed to create PxCreateFoundation");

	physx::PxTolerancesScale scale;
	scale.length = 100;        // typical length of an object
	scale.speed = 981;         // typical speed of an object, gravity*1s is a reasonable choice

	//gCooking = PxCreateCooking(PX_PHYSICS_VERSION, *gFoundation, PxCookingParams(scale));
	//if (!gCooking) throw ("PxCreateCooking failed!");

	// for visual debugger?
	//gPvd = PxCreatePvd(*gFoundation);
	//physx::PxPvdTransport* transport = physx::PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
	//gPvd->connect(*transport, physx::PxPvdInstrumentationFlag::eALL);

	//create physics
	gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation, physx::PxTolerancesScale(scale), true, gPvd);
	if (!gPhysics) throw ("failed to create PxCreatePhysics");

	//setup scene
	physx::PxSceneDesc sceneDesc(gPhysics->getTolerancesScale());
	sceneDesc.gravity = physx::PxVec3(0.0f, 0.0f, 0.0f);
	gDispatcher = physx::PxDefaultCpuDispatcherCreate(2);
	sceneDesc.cpuDispatcher = gDispatcher;
	sceneDesc.kineKineFilteringMode = physx::PxPairFilteringMode::eKEEP; // So kin-kin contacts with be reported
	sceneDesc.staticKineFilteringMode = physx::PxPairFilteringMode::eKEEP; // So static-kin constacts will be reported
	//sceneDesc.filterShader = physx::PxDefaultSimulationFilterShader;
	sceneDesc.filterShader = contactFilter;
	sceneDesc.simulationEventCallback = &callback;

	gScene = gPhysics->createScene(sceneDesc);

	/*
	//scene client, aslo for debugger
	physx::PxPvdSceneClient* pvdClient = gScene->getScenePvdClient();
	if (pvdClient)
	{
		pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
		pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
		pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
	}*/

	//create body 
	gMaterial = gPhysics->createMaterial(0.5f, 0.5f, 0.6f);

	physx::PxRigidStatic* groundPlane = PxCreatePlane(*gPhysics, physx::PxPlane(0, 1, 0, 0), *gMaterial);
	groundPlane->setName("ground");
	gScene->addActor(*groundPlane);
}