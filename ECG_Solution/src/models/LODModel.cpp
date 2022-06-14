#include "LODModel.h"

LODModel::LODModel(std::shared_ptr<PhysxStaticEntity> model)
{
	addModel(model);
	_id = 0;
}

LODModel::LODModel(std::vector<std::shared_ptr<PhysxStaticEntity> > models)
	: _models(models)
{
}

LODModel::LODModel()
{
}

LODModel::~LODModel()
{
}


std::shared_ptr<PhysxStaticEntity> LODModel::getModel(int id) {
	if(_models.size() > 0 && id >= 0 && id < _models.size())
		return _models[id];
	return _models[_models.size() - 1];
}

std::shared_ptr<PhysxStaticEntity> LODModel::getActiveModel() {
	return getModel(_id);
}

void LODModel::drawActive(Camera& camera, DirectionalLight& dirL) {
	getActiveModel()->draw(camera, dirL);
}

void LODModel::draw(Camera& camera, DirectionalLight& dirL, uint32_t id) {
	getModel(id)->draw(camera, dirL);
}

void LODModel::setLodID(int id) {
	_id = id;
	/*
	for (uint32_t level = 0; level < _models.size(); ++level) {
		auto actor = _models[level]->getRigidStatic();
		const physx::PxU32 numShapes = actor->getNbShapes();		
		
		physx::PxShape** shapes;
		shapes = new physx::PxShape * [numShapes];
		actor->getShapes(shapes, numShapes);

		for (physx::PxU32 i = 0; i < numShapes; i++)
		{
			physx::PxShape* shape = shapes[i];
			shape->setFlag(physx::PxShapeFlag::eSCENE_QUERY_SHAPE, true);
		}	
	}
	*/
}

void LODModel::addModel(std::shared_ptr<PhysxStaticEntity> model) {
	_models.push_back(model);
}