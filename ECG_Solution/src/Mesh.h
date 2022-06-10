#pragma once

#include <cstdint>
#include <vector>
#include <memory>
#include <glm\glm.hpp>
#include <GL\glew.h>

#include "Camera.h"
#include "Material.h"

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
    std::vector<std::shared_ptr<Material> > _materials;

    GLuint _vaoID;
    GLuint _vboIDData, _vboIDIndices;

public:
    Mesh(std::vector<Vertex> vertices, std::vector<uint32_t> indices, std::vector<std::shared_ptr<Material> > materials);
    ~Mesh();

    void addMaterial(std::shared_ptr<Material> material);

    void draw(glm::mat4 modelMatrix, Camera& camera);
};
