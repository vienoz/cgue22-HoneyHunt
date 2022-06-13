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

physx::PxTriangleMesh* Mesh::createPxMesh(GamePhysx& physx)
{
    physx::PxTriangleMeshDesc meshDesc;
    meshDesc.points.count           = _vertices.size();
    meshDesc.points.stride          = sizeof(glm::vec3);
    meshDesc.points.data            = _vertices.data();

    meshDesc.triangles.count        = _indices.size() / 3;
    meshDesc.triangles.stride       = 3*sizeof(uint32_t);
    meshDesc.triangles.data         = _indices.data();

    physx::PxDefaultMemoryOutputStream writeBuffer;
    physx::PxTriangleMeshCookingResult::Enum* result;
    bool status = physx.getCooking()->cookTriangleMesh(meshDesc, writeBuffer, result);
    if(!status)
        return NULL;

    physx::PxDefaultMemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());
    return physx.getPhysics()->createTriangleMesh(readBuffer);
}

/*
void Mesh::createConvexMeshes(GamePhysx& physics)
{
    const physx::PxU32 numVerts = _vertices.size();
    physx::PxVec3* vertices = new physx::PxVec3[numVerts];

    // Prepare random verts
    for (physx::PxU32 i = 0; i < numVerts; i++)
        vertices[i] = physx::PxVec3(_vertices[i].position.x, _vertices[i].position.y, _vertices[i].position.z);

    // Create convex mesh using the quickhull algorithm with different settings
    printf("-----------------------------------------------\n");
    printf("Create convex mesh using the quickhull algorithm: \n\n");

    // The default convex mesh creation serializing to a stream, useful for offline cooking.
    //createConvex(physics, numVerts, vertices, physx::PxConvexMeshCookingType::eQUICKHULL, false, 16);

    delete[] vertices;
}

void createConvex(GamePhysx& physx, physx::PxU32 numVerts, const physx::PxVec3* verts, physx::PxConvexMeshCookingType::Enum convexMeshCookingType, bool directInsertion, physx::PxU32 gaussMapLimit)
{
    physx::PxCookingParams params = physx.getCooking()->getParams();
    auto gCooking = physx.getCooking();

    // Use the new (default) PxConvexMeshCookingType::eQUICKHULL
    params.convexMeshCookingType = convexMeshCookingType;

    // If the gaussMapLimit is chosen higher than the number of output vertices, no gauss map is added to the convex mesh data (here 256).
    // If the gaussMapLimit is chosen lower than the number of output vertices, a gauss map is added to the convex mesh data (here 16).
    params.gaussMapLimit = gaussMapLimit;
    gCooking->setParams(params);

    // Setup the convex mesh descriptor
    physx::PxConvexMeshDesc desc;

    // We provide points only, therefore the PxConvexFlag::eCOMPUTE_CONVEX flag must be specified
    desc.points.data = verts;
    desc.points.count = numVerts;
    desc.points.stride = sizeof(physx::PxVec3);
    desc.flags = physx::PxConvexFlag::eCOMPUTE_CONVEX;

    physx::PxU32 meshSize = 0;
    physx::PxConvexMesh* convex = NULL;

    if (directInsertion)
    {
        // Directly insert mesh into PhysX
        convex = gCooking->createConvexMesh(desc, physx.getPhysics()->getPhysicsInsertionCallback());
        PX_ASSERT(convex);
    }
    else
    {
        // Serialize the cooked mesh into a stream.
        physx::PxDefaultMemoryOutputStream outStream;
        bool res = gCooking->cookConvexMesh(desc, outStream);
        PX_UNUSED(res);
        PX_ASSERT(res);
        meshSize = outStream.getSize();

        // Create the mesh from a stream.
        physx::PxDefaultMemoryInputData inStream(outStream.getData(), outStream.getSize());
        convex = physx.getPhysics()->createConvexMesh(inStream);
        PX_ASSERT(convex);
    }

    convex->release();
}
*/