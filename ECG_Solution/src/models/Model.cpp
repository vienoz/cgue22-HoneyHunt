#include "Model.h"


Model::Model(const std::string& path, std::shared_ptr<BaseMaterial> material)
    : _material(material)
{
    loadModel(path);
}

Model::~Model()
{
    for (size_t i = 0; i < _meshes.size(); ++i)
        delete _meshes[i];
}

void Model::loadModel(const std::string& path)
{
    // read file via ASSIMP
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
    // check for errors
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
    {
        std::cerr << importer.GetErrorString() << std::endl;
        exit(EXIT_FAILURE);
    }

    importer.ApplyPostProcessing(aiProcess_GenNormals);
    
    
    // retrieve the directory path of the filepath
    _directory = path.substr(0, path.find_last_of('/'));

    // process ASSIMP's root node recursively
    processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode* node, const aiScene* scene)
{
    // process each mesh located at the current node
    for (size_t i = 0; i < node->mNumMeshes; i++)
    {
        // the node object only contains indices to index the actual objects in the scene. 
        // the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        _meshes.push_back(processMesh(mesh, scene));
    }
    // after we've processed all of the meshes (if any) we then recursively process each of the children nodes
    for (size_t i = 0; i < node->mNumChildren; i++)
    {
        processNode(node->mChildren[i], scene);
    }
}

Mesh* Model::processMesh(aiMesh* mesh, const aiScene* scene)
{
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    std::vector<std::shared_ptr<Texture> > textures;

    for (size_t i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vertex;

        vertex.position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);

        if (mesh->mTextureCoords[0])
            vertex.uv = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
        else
            vertex.uv = glm::vec2(0, 0);

        if (mesh->HasNormals())
            vertex.normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
        else
            // TODO: Calculate normal from face normal
            vertex.normal = glm::vec3(0, 0, 0);

        vertices.push_back(vertex);

        //set texture
        //std::shared_ptr<TextureMaterial> tm = std::dynamic_pointer_cast<TextureMaterial>(_material);
        //if(tm)
        //    tm = tm->getShader(), tm->getTexture();
    }

    for (size_t i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        // retrieve all indices of the face and store them in the indices vector
        for (size_t j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }

    return new Mesh(vertices, indices);
}


std::shared_ptr<BaseMaterial> Model::getMaterial()
{
    return _material;
}

std::shared_ptr<std::vector<Mesh*> > Model::getMeshes()
{
    return std::make_shared<std::vector<Mesh*> >(_meshes);
}

/*
if (mesh->mMaterialIndex >= 0)
    {
        aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
        std::vector<std::shared_ptr<Texture> > diffuseMaps = loadMaterialTextures(material,
                                            aiTextureType_DIFFUSE, "texture_diffuse");

        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
        std::vector<std::shared_ptr<Texture> > specularMaps = loadMaterialTextures(material,
                                            aiTextureType_SPECULAR, "texture_specular");
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

        if (textures.size() > 0)
            ownMaterial = std::make_shared<TextureMaterial>(shader, glm::vec3(0.1f, 0.7f, 0.1f), 2.0f, textures[0]);
        else
            ownMaterial = AssetManager::getInstance()->defaultMaterial;
    }
    else
        ownMaterial = AssetManager::getInstance()->defaultMaterial;
*/

void Model::draw(glm::mat4 modelMatrix, Camera& camera, DirectionalLight& dirL)
{
    _material->getShader()->use();
    _material->setUniforms(modelMatrix, camera, dirL);

    for (size_t i = 0; i < _meshes.size(); ++i)
        _meshes[i]->draw(modelMatrix, camera);
}