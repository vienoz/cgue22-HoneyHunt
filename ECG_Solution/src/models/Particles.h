#pragma once
#include <memory>
#include <glm\glm.hpp>
#include <GL\glew.h>
#include "../rendering/ShaderNew.h"
#include "../rendering/Camera.h"
#include "../rendering/Material.h"


class ParticleHandler {
protected:
	static const float* vertex_buffer_data;
	int particle_count;
public:
	std::shared_ptr<TextureMaterial> _material;
	int max_particles;
	void init(std::shared_ptr<TextureMaterial> material, float maxPart);
	void update(float* position_size_data, int pCount);
	void draw(Camera camera);
private:
	unsigned int vertex_buffer;
	unsigned int position_buffer;
	unsigned int vao_Handle;
};

struct Particle {
	glm::vec3 pos;
	glm::vec3 speed;

	float size;
	float life;
	float cameradistance;

	bool operator<(const Particle& that) const {
		return this->cameradistance > that.cameradistance;
	}
};