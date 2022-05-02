#pragma once

#include "GamePhysxCallback.cpp";




class GamePhysx {
public:

	GamePhysx() {
		init();
	}

	~GamePhysx() {

	}

	physx::PxFoundation* getFoundation() const { return gFoundation; }
	physx::PxPhysics* getPhysics() const { return gPhysics; }
	physx::PxScene* getScene() const { return gScene; }
	physx::PxMaterial* getMaterial() const { return gMaterial; }

private:

	physx::PxDefaultAllocator		gAllocator;
	physx::PxDefaultErrorCallback	gErrorCallback;
	physx::PxFoundation* gFoundation = nullptr;
	physx::PxDefaultCpuDispatcher* gDispatcher = nullptr;
	physx::PxPvd* gPvd = nullptr;

	physx::PxScene* gScene = nullptr;
	physx::PxPhysics* gPhysics = nullptr;
	physx::PxMaterial* gMaterial = nullptr;

	GamePhysxCallback callback;

	void init() {
		gFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gAllocator, gErrorCallback);
		if (!gFoundation) throw ("failed to create PxCreateFoundation");

		// for visual debugger?
		gPvd = PxCreatePvd(*gFoundation);
		physx::PxPvdTransport* transport = physx::PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
		gPvd->connect(*transport, physx::PxPvdInstrumentationFlag::eALL);

		//create physics
		gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation, physx::PxTolerancesScale(), true, gPvd);
		if (!gPhysics) throw ("failed to create PxCreatePhysics");

		//setup scene
		physx::PxSceneDesc sceneDesc(gPhysics->getTolerancesScale());
		//sceneDesc.gravity = physx::PxVec3(0.0f, -9.81f, 0.0f);
		sceneDesc.gravity = physx::PxVec3(0.0f, .0f, 0.0f);
		gDispatcher = physx::PxDefaultCpuDispatcherCreate(2);
		sceneDesc.cpuDispatcher = gDispatcher;
		sceneDesc.filterShader = physx::PxDefaultSimulationFilterShader;
		//sceneDesc.filterShader = contactFilterB;

		sceneDesc.kineKineFilteringMode = physx::PxPairFilteringMode::eKEEP; // So kin-kin contacts with be reported
		sceneDesc.staticKineFilteringMode = physx::PxPairFilteringMode::eKEEP; // So static-kin constacts will be reported
		sceneDesc.simulationEventCallback = &callback;



		gScene = gPhysics->createScene(sceneDesc);


		//scene client, aslo for debugger
		physx::PxPvdSceneClient* pvdClient = gScene->getScenePvdClient();
		if (pvdClient)
		{
			pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
			pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
			pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
		}

		//create body 
		gMaterial = gPhysics->createMaterial(0.5f, 0.5f, 0.6f);

		physx::PxRigidStatic* groundPlane = PxCreatePlane(*gPhysics, physx::PxPlane(0, 1, 0, 0), *gMaterial);
		gScene->addActor(*groundPlane);
	}
};