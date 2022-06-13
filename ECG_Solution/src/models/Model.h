#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Mesh.h"
#include "../Asset.h"
#include "../rendering/ShaderNew.h"
#include "../rendering/Material.h"
#include "../rendering/Light.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
#include <memory>

class Model
{
protected:
    std::vector<Mesh*> _meshes;
    std::string _directory;
    std::shared_ptr<BaseMaterial> _material;

    // loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
    void loadModel(const std::string& path);

    // processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
    void processNode(aiNode* node, const aiScene* scene);

    Mesh* processMesh(aiMesh* mesh, const aiScene* scene);

public:
    // constructor, expects a filepath to a 3D model.
    Model(const std::string& path, std::shared_ptr<BaseMaterial> material);
    ~Model();

    std::shared_ptr<BaseMaterial> getMaterial();

    void draw(glm::mat4 modelMatrix, Camera& camera, DirectionalLight& dirL);
};