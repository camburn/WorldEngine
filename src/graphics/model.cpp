#include "managers/state_manager.hpp"
#include "graphics/model.hpp"

// TODO: Each Mesh should have its own model matrix
// Also note that meshes can expressed as a parent child relationship
// Child meshes may use offsets from parent as a matrix

Model::Model() {}

Model::Model(std::string path, std::string filename) {
    this->loadModel(path.c_str(), filename.c_str());
}

Model::Model(const char* path, const char* filename) {
    this->loadModel(path, filename);
}

void Model::Draw(State &state, GLuint shader) {
//void Model::Draw(GLuint shader) {
    for (Mesh mesh: meshes) {
        if (state.renderer.active().get_shader_name().compare("default") == 0) {
            state.renderer.active().set_uniform("objectColor", mesh.material.ambient);
        }
        mesh.Draw(shader);
    }
}

int Model::loadModel(const char* path, const char* filename) {
    string _path = string(path);
    string _filename = string(filename); 
    Assimp::Importer import;
    const aiScene* scene = import.ReadFile(_path + _filename, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals);
    // NOTE: Consider adding aiProcess_GenNormals on models without normals
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        const char* error_msg = "ERROR:ASSIMP:: In load model:: ";
        fprintf(stderr, "%s", error_msg);
        fprintf(stderr, "%s\n", import.GetErrorString());
        return -1;
    }
    this->directory = _path;
    this->processNode(scene->mRootNode, scene);
    return 0;
}

void Model::processNode(aiNode* node, const aiScene* scene) {
    // Process all the node's meshes (if any)
    for (GLuint i = 0; i < node->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        this->meshes.push_back(this->processMesh(mesh, scene));
    }
    // Repeat for each child node
    for (GLuint i = 0; i < node->mNumChildren; i++) {
        this->processNode(node->mChildren[i], scene);
    }
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene) {
    vector<Vertex> vertices;
    vector<GLuint> indices;
    vector<Texture> textures;

    for (GLuint i = 0; i < mesh->mNumVertices; i++) {
        Vertex vertex;
        glm::vec3 vector;
        // Process vertex positions, normals, and texture coordinates here
        // Positions
        vector.x = mesh->mVertices[i].x;
        vector.y = mesh->mVertices[i].y;
        vector.z = mesh->mVertices[i].z;
        vertex.Position = vector;
        // Normals
        // Do we handle no normals?
        vector.x = mesh->mNormals[i].x;
        vector.y = mesh->mNormals[i].y;
        vector.z = mesh->mNormals[i].z;
        vertex.Normal = vector;
        // Texture Coordinates
        if (mesh->mTextureCoords[0]) {
            glm::vec2 vec;
            // NOTE: A vertex can contain up to 8 different texture coordinates
            // Currently only taking the first set of TexCoords
            // FIXME: Process all texture coords (for what?)
            vec.x = mesh->mTextureCoords[0][i].x;
            vec.y = mesh->mTextureCoords[0][i].y;
            vertex.TexCoord = vec;
        } else {
            vertex.TexCoord = glm::vec2(0.0f, 0.0f);
        }
        vertices.push_back(vertex);
    }
    // Process Indices
    for (GLuint i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for (GLuint j = 0; j < face.mNumIndices; j++) {
            indices.push_back(face.mIndices[j]);
        }
    }

    MeshMaterial mesh_material;
    // Process Materials
    if (mesh->mMaterialIndex >= 0) {
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        // TODO: Handle all texture types
        aiColor3D ambient (0.0f, 0.0f, 0.0f);
        material->Get(AI_MATKEY_COLOR_AMBIENT, ambient);
        mesh_material.ambient = glm::vec3(ambient.r, ambient.g, ambient.b);

        aiColor3D diffuse (0.0f, 0.0f, 0.0f);
        material->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse);
        mesh_material.diffuse = glm::vec3(diffuse.r, diffuse.g, diffuse.b);

        aiColor3D specular (0.0f, 0.0f, 0.0f);
        material->Get(AI_MATKEY_COLOR_SPECULAR, specular);
        mesh_material.specular = glm::vec3(specular.r, specular.g, specular.b);

        aiColor3D emissive (0.0f, 0.0f, 0.0f);
        material->Get(AI_MATKEY_COLOR_EMISSIVE, emissive);
        mesh_material.emissive = glm::vec3(emissive.r, emissive.g, emissive.b);

        float opacity = 0.0f;
        material->Get(AI_MATKEY_OPACITY, opacity);
        mesh_material.opacity = opacity;

        // Diffuse Maps
        vector<Texture> diffuseMaps = this->loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
        // Specular Maps
        vector<Texture> specularMaps = this->loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

    }
    return Mesh(mesh_material, vertices, indices, textures);
}

vector<Texture> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName) {
    vector<Texture> textures;
    for (GLuint i = 0; i < mat->GetTextureCount(type); i++) {
        aiString str;
        mat->GetTexture(type, i, &str);
        // Check if texture was loaded before and if so continue to next texture
        GLboolean skip = false;
        for (GLuint j = 0; j < textures_loaded.size(); j++) {
            if (std::strcmp(textures_loaded[j].path.C_Str(), str.C_Str()) == 0) {
                textures.push_back(textures_loaded[j]);
                skip = true;
                break;
            }
        }
        if (!skip) {
            Texture texture;
            texture.id = BufferTextureDataFromFile(str.C_Str(), this->directory);
            texture.type = typeName;
            texture.path = str;
            textures.push_back(texture);
            textures_loaded.push_back(texture);
        }
    }
    return textures;
}
