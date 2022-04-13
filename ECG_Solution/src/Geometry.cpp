/*
* Copyright 2020 Vienna University of Technology.
* Institute of Computer Graphics and Algorithms.
* This file is part of the ECG Lab Framework and must not be redistributed.
*/

#include "Geometry.h"


Geometry::Geometry(glm::mat4 modelMatrix, GeometryData& data, std::shared_ptr<Material> material) : _elements(data.indices.size()), _modelMatrix(glmMat4ToPhysxMat4(modelMatrix)), _material(material)
{
	//physx object
	physObj = data.physObj;

	// create VAO
	glGenVertexArrays(1, &_vao);
	glBindVertexArray(_vao);

	// create positions VBO
	glGenBuffers(1, &_vboPositions);
	glBindBuffer(GL_ARRAY_BUFFER, _vboPositions);
	glBufferData(GL_ARRAY_BUFFER, data.positions.size() * sizeof(glm::vec3), data.positions.data(), GL_STATIC_DRAW);

	// bind positions to location 0
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	// create normals VBO
	glGenBuffers(1, &_vboNormals);
	glBindBuffer(GL_ARRAY_BUFFER, _vboNormals);
	glBufferData(GL_ARRAY_BUFFER, data.normals.size() * sizeof(glm::vec3), data.normals.data(), GL_STATIC_DRAW);

	// bind normals to location 1
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

	// create uvs VBO
	glGenBuffers(1, &_vboUVs);
	glBindBuffer(GL_ARRAY_BUFFER, _vboUVs);
	glBufferData(GL_ARRAY_BUFFER, data.uvs.size() * sizeof(glm::vec2), data.uvs.data(), GL_STATIC_DRAW);

	// bind uvs to location 2
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);

	// create and bind indices VBO
	glGenBuffers(1, &_vboIndices);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _vboIndices);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, data.indices.size() * sizeof(unsigned int), data.indices.data(), GL_STATIC_DRAW);

	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

Geometry::~Geometry()
{
	glDeleteBuffers(1, &_vboPositions);
	glDeleteBuffers(1, &_vboNormals);
	glDeleteBuffers(1, &_vboUVs);
	glDeleteBuffers(1, &_vboIndices);
	glDeleteVertexArrays(1, &_vao);
}


//TODO only call updateModelMatrix if physx object is dynamic
void Geometry::draw()
{
	updateModelMatrix();
	Shader* shader = _material->getShader();
	shader->use();

	shader->setUniform(0, _modelMatrix);
	_material->setUniforms();

	glBindVertexArray(_vao);
	glDrawElements(GL_TRIANGLES, _elements, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void Geometry::transform(glm::mat4 transformation)
{
	//_modelMatrix = transformation * _modelMatrix;
}

void Geometry::resetModelMatrix()
{
	//_modelMatrix = glm::mat4(1);
}

GeometryData Geometry::createCubeGeometry(float width, float height, float depth, glm::vec3 initPos, float mass, physx::PxMaterial* gMaterial, physx::PxPhysics* gPhysics)
{
	GeometryData data;

	data.positions = {
		// front
		glm::vec3(-width / 2.0f, -height / 2.0f,  depth / 2.0f),
		glm::vec3(width / 2.0f, -height / 2.0f,  depth / 2.0f),
		glm::vec3(width / 2.0f, height / 2.0f,  depth / 2.0f),
		glm::vec3(-width / 2.0f, height / 2.0f,  depth / 2.0f),
		// back
		glm::vec3(width / 2.0f, -height / 2.0f,  -depth / 2.0f),
		glm::vec3(-width / 2.0f, -height / 2.0f,  -depth / 2.0f),
		glm::vec3(-width / 2.0f, height / 2.0f,  -depth / 2.0f),
		glm::vec3(width / 2.0f, height / 2.0f,  -depth / 2.0f),
		// right
		glm::vec3(width / 2.0f, -height / 2.0f,  depth / 2.0f),
		glm::vec3(width / 2.0f, -height / 2.0f,  -depth / 2.0f),
		glm::vec3(width / 2.0f, height / 2.0f,  -depth / 2.0f),
		glm::vec3(width / 2.0f, height / 2.0f,  depth / 2.0f),
		// left
		glm::vec3(-width / 2.0f, -height / 2.0f,  -depth / 2.0f),
		glm::vec3(-width / 2.0f, -height / 2.0f,  depth / 2.0f),
		glm::vec3(-width / 2.0f, height / 2.0f,  depth / 2.0f),
		glm::vec3(-width / 2.0f, height / 2.0f,  -depth / 2.0f),
		// top
		glm::vec3(-width / 2.0f, height / 2.0f,  -depth / 2.0f),
		glm::vec3(-width / 2.0f, height / 2.0f,  depth / 2.0f),
		glm::vec3(width / 2.0f, height / 2.0f,  depth / 2.0f),
		glm::vec3(width / 2.0f, height / 2.0f,  -depth / 2.0f),
		// bottom
		glm::vec3(-width / 2.0f, -height / 2.0f,  -depth / 2.0f),
		glm::vec3(width / 2.0f, -height / 2.0f,  -depth / 2.0f),
		glm::vec3(width / 2.0f, -height / 2.0f,  depth / 2.0f),
		glm::vec3(-width / 2.0f, -height / 2.0f,  depth / 2.0f)
	};

	data.normals = {
		// front
		glm::vec3(0, 0, 1),
		glm::vec3(0, 0, 1),
		glm::vec3(0, 0, 1),
		glm::vec3(0, 0, 1),
		// back
		glm::vec3(0, 0, -1),
		glm::vec3(0, 0, -1),
		glm::vec3(0, 0, -1),
		glm::vec3(0, 0, -1),
		// right
		glm::vec3(1, 0, 0),
		glm::vec3(1, 0, 0),
		glm::vec3(1, 0, 0),
		glm::vec3(1, 0, 0),
		// left
		glm::vec3(-1, 0, 0),
		glm::vec3(-1, 0, 0),
		glm::vec3(-1, 0, 0),
		glm::vec3(-1, 0, 0),
		// top
		glm::vec3(0, 1, 0),
		glm::vec3(0, 1, 0),
		glm::vec3(0, 1, 0),
		glm::vec3(0, 1, 0),
		// bottom
		glm::vec3(0, -1, 0),
		glm::vec3(0, -1, 0),
		glm::vec3(0, -1, 0),
		glm::vec3(0, -1, 0)
	};

	data.uvs = {
		// front
		glm::vec2(0, 0),
		glm::vec2(1, 0),
		glm::vec2(1, 1),
		glm::vec2(0, 1),
		// back
		glm::vec2(1, 1),
		glm::vec2(0, 1),
		glm::vec2(0, 0),
		glm::vec2(1, 0),
		// right
		glm::vec2(0, 0),
		glm::vec2(1, 0),
		glm::vec2(1, 1),
		glm::vec2(0, 1),
		// left
		glm::vec2(0, 0),
		glm::vec2(1, 0),
		glm::vec2(1, 1),
		glm::vec2(0, 1),
		// top
		glm::vec2(0, 1),
		glm::vec2(0, 0),
		glm::vec2(1, 0),
		glm::vec2(1, 1),
		// bottom
		glm::vec2(0, 0),
		glm::vec2(1, 0),
		glm::vec2(1, 1),
		glm::vec2(0, 1)
	};

	data.indices = {
		// front
		0, 1, 2,
		2, 3, 0,
		// back
		4, 5, 6,
		6, 7, 4,
		// right
		8, 9, 10,
		10, 11, 8,
		// left
		12, 13, 14,
		14, 15, 12,
		// top
		16, 17, 18,
		18, 19, 16,
		// bottom
		20, 21, 22,
		22, 23, 20
	};

	data.physObj = gPhysics->createRigidDynamic(physx::PxTransform(physx::PxVec3(initPos.x, initPos.y, initPos.z))); 
	physx::PxShape* aBoxShape = physx::PxRigidActorExt::createExclusiveShape(*data.physObj, physx::PxBoxGeometry(width / 2, height / 2, depth / 2), *gMaterial);

	return std::move(data);
}

GeometryData Geometry::createCylinderGeometry(unsigned int segments, float height, float radius)
{
	GeometryData data;


	// center vertices
	data.positions.push_back(glm::vec3(0, -height / 2.0f, 0));
	data.normals.push_back(glm::vec3(0, -1, 0));
	data.uvs.push_back(glm::vec2(0.5f, 0.5f));
	data.positions.push_back(glm::vec3(0, height / 2.0f, 0));
	data.normals.push_back(glm::vec3(0, 1, 0));
	data.uvs.push_back(glm::vec2(0.5f, 0.5f));

	// circle segments
	float angle_step = 2.0f * glm::pi<float>() / float(segments);
	for (unsigned int i = 0; i < segments; i++) {
		glm::vec3 circlePos = glm::vec3(
			glm::cos(i * angle_step) * radius,
			-height / 2.0f,
			glm::sin(i * angle_step) * radius
		);

		glm::vec2 squareToCirlceUV = glm::vec2(
			(circlePos.x / radius) * 0.5f + 0.5f,
			(circlePos.z / radius) * 0.5f + 0.5f
		);

		// bottom ring vertex
		data.positions.push_back(circlePos);
		data.positions.push_back(circlePos);
		data.normals.push_back(glm::vec3(0, -1, 0));
		data.normals.push_back(glm::normalize(circlePos - glm::vec3(0, -height / 2.0f, 0)));
		data.uvs.push_back(squareToCirlceUV);
		data.uvs.push_back(glm::vec2(i * angle_step / (2.0f * glm::pi<float>()), 0));

		// top ring vertex
		circlePos.y = height / 2.0f;
		data.positions.push_back(circlePos);
		data.positions.push_back(circlePos);
		data.normals.push_back(glm::vec3(0, 1, 0));
		data.normals.push_back(glm::normalize(circlePos - glm::vec3(0, height / 2.0f, 0)));
		data.uvs.push_back(squareToCirlceUV);
		data.uvs.push_back(glm::vec2(i * angle_step / (2.0f * glm::pi<float>()), 1));

		// bottom face
		data.indices.push_back(0);
		data.indices.push_back(2 + i * 4);
		data.indices.push_back(i == segments - 1 ? 2 : 2 + (i + 1) * 4);

		// top face
		data.indices.push_back(1);
		data.indices.push_back(i == segments - 1 ? 4 : (i + 2) * 4);
		data.indices.push_back((i + 1) * 4);

		// side faces
		data.indices.push_back(3 + i * 4);
		data.indices.push_back(i == segments - 1 ? 5 : 5 + (i + 1) * 4);
		data.indices.push_back(i == segments - 1 ? 3 : 3 + (i + 1) * 4);

		data.indices.push_back(i == segments - 1 ? 5 : 5 + (i + 1) * 4);
		data.indices.push_back(3 + i * 4);
		data.indices.push_back(5 + i * 4);
	}


	return std::move(data);
}

void Geometry::updateModelMatrix() {
	physx::PxMat44 pxMat(physObj->getGlobalPose());
	_modelMatrix = pxMat;
}

GeometryData Geometry::createSphereGeometry(unsigned int longitudeSegments, unsigned int latitudeSegments, float radius, glm::vec3 initPos, physx::PxMaterial* gMaterial, physx::PxPhysics* gPhysics)
{
	GeometryData data;


	data.positions.push_back(glm::vec3(0.0f, radius, 0.0f));
	data.positions.push_back(glm::vec3(0.0f, -radius, 0.0f));
	data.normals.push_back(glm::vec3(0.0f, radius, 0.0f));
	data.normals.push_back(glm::vec3(0.0f, -radius, 0.0f));
	data.uvs.push_back(glm::vec2(0.0f, 0.0f));
	data.uvs.push_back(glm::vec2(0.0f, 1.0f));

	// first and last ring
	for (unsigned int j = 0; j < longitudeSegments; j++) {
		data.indices.push_back(0);
		data.indices.push_back(j == longitudeSegments - 1 ? 2 : (j + 3));
		data.indices.push_back(2 + j);

		data.indices.push_back(2 + (latitudeSegments - 2) * longitudeSegments + j);
		data.indices.push_back(j == longitudeSegments - 1 ? 2 + (latitudeSegments - 2) * longitudeSegments : 2 + (latitudeSegments - 2) * longitudeSegments + j + 1);
		data.indices.push_back(1);
	}

	// vertices and rings
	for (unsigned int i = 1; i < latitudeSegments; i++) {
		float verticalAngle = float(i) * glm::pi<float>() / float(latitudeSegments);
		for (unsigned int j = 0; j < longitudeSegments; j++) {
			float horizontalAngle = float(j) * 2.0f * glm::pi<float>() / float(longitudeSegments);
			glm::vec3 position = glm::vec3(
				radius * glm::sin(verticalAngle) * glm::cos(horizontalAngle),
				radius * glm::cos(verticalAngle),
				radius * glm::sin(verticalAngle) * glm::sin(horizontalAngle)
			);
			data.positions.push_back(position);
			data.normals.push_back(glm::normalize(position));
			data.uvs.push_back(glm::vec2(horizontalAngle / (2.0f * glm::pi<float>()), verticalAngle / glm::pi<float>()));

			if (i == 1) continue;

			data.indices.push_back(2 + (i - 1) * longitudeSegments + j);
			data.indices.push_back(j == longitudeSegments - 1 ? 2 + (i - 2) * longitudeSegments : 2 + (i - 2) * longitudeSegments + j + 1);
			data.indices.push_back(j == longitudeSegments - 1 ? 2 + (i - 1) * longitudeSegments : 2 + (i - 1) * longitudeSegments + j + 1);

			data.indices.push_back(j == longitudeSegments - 1 ? 2 + (i - 2) * longitudeSegments : 2 + (i - 2) * longitudeSegments + j + 1);
			data.indices.push_back(2 + (i - 1) * longitudeSegments + j);
			data.indices.push_back(2 + (i - 2) * longitudeSegments + j);
		}
	}

	data.physObj = gPhysics->createRigidDynamic(physx::PxTransform(physx::PxVec3(initPos.x, initPos.y, initPos.z)));
	physx::PxShape* sphereShape = physx::PxRigidActorExt::createExclusiveShape(*data.physObj, physx::PxSphereGeometry(radius), *gMaterial);

	return std::move(data);
}

void PhysXMat4ToglmMat4(const physx::PxMat44& mat4, glm::mat4& newMat)
{

	newMat[0][0] = mat4[0][0];
	newMat[0][1] = mat4[0][1];
	newMat[0][2] = mat4[0][2];
	newMat[0][3] = mat4[0][3];

	newMat[1][0] = mat4[1][0];
	newMat[1][1] = mat4[1][1];
	newMat[1][2] = mat4[1][2];
	newMat[1][3] = mat4[1][3];

	newMat[2][0] = mat4[2][0];
	newMat[2][1] = mat4[2][1];
	newMat[2][2] = mat4[2][2];
	newMat[2][3] = mat4[2][3];

	newMat[3][0] = mat4[3][0];
	newMat[3][1] = mat4[3][1];
	newMat[3][2] = mat4[3][2];
	newMat[3][3] = mat4[3][3];

}

physx::PxMat44 Geometry::glmMat4ToPhysxMat4(const glm::mat4& mat4)
{
	physx::PxMat44 newMat;

	newMat[0][0] = mat4[0][0];
	newMat[0][1] = mat4[0][1];
	newMat[0][2] = mat4[0][2];
	newMat[0][3] = mat4[0][3];

	newMat[1][0] = mat4[1][0];
	newMat[1][1] = mat4[1][1];
	newMat[1][2] = mat4[1][2];
	newMat[1][3] = mat4[1][3];

	newMat[2][0] = mat4[2][0];
	newMat[2][1] = mat4[2][1];
	newMat[2][2] = mat4[2][2];
	newMat[2][3] = mat4[2][3];

	newMat[3][0] = mat4[3][0];
	newMat[3][1] = mat4[3][1];
	newMat[3][2] = mat4[3][2];
	newMat[3][3] = mat4[3][3];


	return newMat;
}