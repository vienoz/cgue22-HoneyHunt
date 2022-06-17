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
	static const int max_particles = 100;
	void init(std::shared_ptr<TextureMaterial> material);
	void update(float* position_size_data, unsigned char* color_data, int pCount);
	void draw(Camera camera);
private:
	unsigned int vertex_buffer;
	unsigned int position_buffer;
	unsigned int color_buffer;
	unsigned int vao_Handle;
};

struct Particle {
	glm::vec3 pos;
	glm::vec3 speed;
	unsigned char r, g, b, a;
	float size;
	float angle;
	float weight;
	float life;
	float cameradistance;

	bool operator<(const Particle& that) const {
		// Sort in reverse order : far particles drawn first.
		return this->cameradistance > that.cameradistance;
	}
};