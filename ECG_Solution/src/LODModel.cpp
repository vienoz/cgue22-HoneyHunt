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
	return _models[id];
}

std::shared_ptr<PhysxStaticEntity> LODModel::getActiveModel() {
	return _models[_id];
}

void LODModel::drawActive(Camera camera) {
	getActiveModel()->draw(camera);
}

void LODModel::draw(Camera camera, int id) {
	getModel(id)->draw(camera);
}

void LODModel::setLodID(int id) {
	_id = id;
}

void LODModel::addModel(std::shared_ptr<PhysxStaticEntity> model) {
	_models.push_back(model);
}