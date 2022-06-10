#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Mesh.h"
#include "ShaderNew.h"
#include "Asset.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>

class Model
{
protected:
    std::vector<Mesh*> _meshes;
    std::string _directory;

    // loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
    void loadModel(const std::string& path, std::shared_ptr<ShaderNew> shader);

    // processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
    void processNode(aiNode* node, const aiScene* scene, std::shared_ptr<ShaderNew> shader);

    Mesh* processMesh(aiMesh* mesh, const aiScene* scene, std::shared_ptr<ShaderNew> shader);

    std::vector<std::shared_ptr<Texture> > loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);

public:
    // constructor, expects a filepath to a 3D model.
    Model(const std::string& path, std::shared_ptr<ShaderNew> shader);
    ~Model();

    void addMaterial(std::shared_ptr<Material> material);

    void draw(glm::mat4 modelMatrix, Camera& camera);

    // draws the model, and thus all its meshes
    /*void Draw()
    {
        for (unsigned int i = 0; i < _meshes.size(); i++)
            _meshes[i].draw();
    }*/
};