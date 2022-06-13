#pragma once

#include <cstdint>
#include <vector>
#include <memory>
#include <glm\glm.hpp>
#include <GL\glew.h>
#include "../GamePhysx.h"

#include "../rendering/Camera.h"
#include "../rendering/Material.h"

struct Vertex
{
public:
    glm::vec3 position;
    glm::vec2 uv;
    glm::vec3 normal;
};

class Mesh
{
protected:
    std::vector<Vertex> _vertices;
    std::vector<uint32_t> _indices;

    GLuint _vaoID;
    GLuint _vboIDData, _vboIDIndices;

public:
    Mesh(std::vector<Vertex> vertices, std::vector<uint32_t> indices);
    ~Mesh();

    void draw(glm::mat4 modelMatrix, Camera& camera);
    
    void Mesh::createConvex(GamePhysx& physx, physx::PxU32 numVerts, const physx::PxVec3* verts, 
        physx::PxConvexMeshCookingType::Enum convexMeshCookingType, bool directInsertion, physx::PxU32 gaussMapLimit);
    void Mesh::createConvexMeshes(GamePhysx& physics);
    
    physx::PxTriangleMesh* Mesh::createPxMesh(GamePhysx& physx);

    //physx::PxRigidDynamic* generatePxRigidbodyShape();
    //physx::PxTriangleMesh generateTriangleMesh(physx::PxPhysics, physx::PxCooking cooking);
};
