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
}

void LODModel::addModel(std::shared_ptr<PhysxStaticEntity> model) {
	_models.push_back(model);
}