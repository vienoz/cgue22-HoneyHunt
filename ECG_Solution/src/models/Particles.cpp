#include "Particles.h"

const float* ParticleHandler::vertex_buffer_data;

void ParticleHandler::init(std::shared_ptr<TextureMaterial> material, float maxPart) {
	_material = material;
	max_particles = maxPart;

	const float buffer_data[] = { -0.5f, -0.5f, 0.0f,
	 0.5f, -0.5f, 0.0f,
	 -0.5f, 0.5f, 0.0f,
	 0.5f, 0.5f, 0.0f, };
	vertex_buffer_data = buffer_data;

	glGenVertexArrays(1, &vao_Handle);
	glBindVertexArray(vao_Handle);
	
	glGenBuffers(1, &vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(buffer_data), buffer_data, GL_STATIC_DRAW);

	
	glGenBuffers(1, &position_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, position_buffer);
	
	glBufferData(GL_ARRAY_BUFFER, max_particles * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW);



	// vertices of particle
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER,vertex_buffer);
	glVertexAttribPointer(
		0,
		3, // size
		GL_FLOAT, // type
		GL_FALSE, // normalized?
		3 * sizeof(float), // stride
		(void*)0 // array buffer offset
	);

	//positions of particles' centers
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, position_buffer);
	glVertexAttribPointer(
		1,
		4, 
		GL_FLOAT, 
		GL_FALSE, 
		4 * sizeof(float), 
		(void*)0 
	);

	glVertexAttribDivisor(0, 0); 
	glVertexAttribDivisor(1, 1); 
}

void ParticleHandler::update(float* position_size_data, int pCount) {
	particle_count = pCount;
	glBindBuffer(GL_ARRAY_BUFFER, position_buffer);
	glBufferData(GL_ARRAY_BUFFER, max_particles * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW); // buffer orphaning
	glBufferSubData(GL_ARRAY_BUFFER, 0, particle_count * sizeof(GLfloat) * 4, position_size_data);
}

void ParticleHandler::draw(Camera camera) {
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	_material->getShader()->use();
	
	glBindVertexArray(vao_Handle);
	glEnableVertexAttribArray(0);

	_material->getTexture()->bind(0);
	_material->getShader()->setUniform(0, glm::ortho(0.0f, 800.0f, 0.0f, 600.0f));
	_material->getShader()->setUniform(1, camera.getViewMatrix());
	_material->getShader()->setUniform(2, camera.getProjMatrix());
	_material->getShader()->setUniform(3, camera.getCameraRight());

	glm::vec3 camBack = glm::vec3(-camera.getCameraFoward().x, 0, -camera.getCameraFoward().z);
	_material->getShader()->setUniform(4, glm::normalize(glm::cross(camera.getCameraRight(), camBack)));

	glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, particle_count);
}
