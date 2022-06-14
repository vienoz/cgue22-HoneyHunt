#include "Mesh.h"

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<uint32_t> indices)
    : _vertices(vertices), _indices(indices)
{
    // Create VAO and check for validity
    _vaoID = 0;
    glGenVertexArrays(1, &_vaoID);
    if (_vaoID == 0)
        throw new std::exception("Failed to create VAO!");


    //std::cout << "Hello world, THIS IS ZE FAKIN VAO INIT OMG DONT ANNOY ME" << std::endl;
    glBindVertexArray(_vaoID);

    // Create data VBO and check for validity
    _vboIDData = 0;
    glGenBuffers(1, &_vboIDData);
    if (_vboIDData == 0)
        throw new std::exception("Failed to create data VBO!");

    glBindBuffer(GL_ARRAY_BUFFER, _vboIDData);

    // Setup data VBO structure
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, position));
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, uv));
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, normal));

    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

    // Create indicies VBO and check for validity
    _vboIDIndices = 0;
    glGenBuffers(1, &_vboIDIndices);
    if (_vboIDIndices == 0)
        throw new std::exception("Failed to create indicies VBO!");

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _vboIDIndices);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t), indices.data(), GL_STATIC_DRAW);

    glBindVertexArray(0);
}

Mesh::~Mesh()
{
    glDeleteBuffers(1, &_vboIDData);
    glDeleteBuffers(1, &_vboIDIndices);
    glDeleteVertexArrays(1, &_vaoID);
}

void Mesh::draw(glm::mat4 modelMatrix, Camera& camera)
{
    glBindVertexArray(_vaoID);
    glDrawElements(GL_TRIANGLES, _indices.size(), GL_UNSIGNED_INT, 0);
}

physx::PxTriangleMesh* Mesh::createPxMesh(GamePhysx& gphysx)
{
    physx::PxTriangleMeshDesc meshDesc;
    meshDesc.points.count           = _vertices.size();
    meshDesc.points.stride          = sizeof(physx::PxVec3);
    meshDesc.points.data            = _vertices.data();

    meshDesc.triangles.count        = _indices.size() / 3;
    meshDesc.triangles.stride       = 3*sizeof(physx::PxU32);
    meshDesc.triangles.data         = _indices.data();


    physx::PxDefaultMemoryOutputStream writeBuffer;
    bool status = gphysx.getCooking()->cookTriangleMesh(meshDesc, writeBuffer);
    if(!status)
        return NULL;

    physx::PxDefaultMemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());
    return gphysx.getPhysics()->createTriangleMesh(readBuffer);
}