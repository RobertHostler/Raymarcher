#include "./Model.hpp"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <string>
#include "stb_image.h"


unsigned int Model::TextureFromFile(const char *path, const std::string &directory, bool gamma){
  std::string filename = std::string(path);
  filename = directory + '/' + filename;

  unsigned int textureID;
  glGenTextures(1, &textureID);

  int width, height, nrComponents;
  unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
  if (data){
    GLenum format;
    if (nrComponents == 1)
      format = GL_RED;
    else if (nrComponents == 3)
      format = GL_RGB;
    else if (nrComponents == 4)
      format = GL_RGBA;

    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data);
  } else {
    std::cout << "Texture failed to load at path: " << path << std::endl;
    stbi_image_free(data);
  }

  return textureID;
}

void Model::Draw(Shader &shader){
  for(unsigned int i = 0; i < meshes.size(); i++){
    meshes[i].Draw(shader);
  }
}

std::vector<Texture> Model::loadMaterialTextures(aiMaterial *material, aiTextureType type, std::string typeName){
  std::vector<Texture> textures;
  for(unsigned int i = 0; i < material -> GetTextureCount(type); i++){
    aiString str;
    material -> GetTexture(type, i, &str);

    Texture texture;
    texture.id = TextureFromFile(str.C_Str(), directory);
    texture.type = typeName;

    //texture.path = str;
    //texture.path = str.C_Str().to_string();
    //texture.path = str.C_Str().c_str();
    texture.path = std::string( str.C_Str() );



    textures.push_back(texture);
  }
  return textures;
}

Mesh Model::processMesh(aiMesh *mesh, const aiScene *scene){
  std::vector<Vertex> vertices;
  std::vector<unsigned int> indices;
  std::vector<Texture> textures;

  // this is likely to be a very big problem, since it seems to increase the number
  // of vertices in a model by a factor of 4.
  for(unsigned int i = 0; i < mesh -> mNumVertices; i++){
    Vertex vertex;

    // process vertices
    glm::vec3 position;
    position.x = mesh -> mVertices[i].x;
    position.y = mesh -> mVertices[i].y;
    position.z = mesh -> mVertices[i].z;
    vertex.position = position;

    glm::vec3 normal;
    normal.x = mesh -> mNormals[i].x;
    normal.y = mesh -> mNormals[i].y;
    normal.z = mesh -> mNormals[i].z;
    vertex.normal = normal;

    glm::vec3 tangent;
    tangent.x = mesh -> mTangents[i].x;
    tangent.y = mesh -> mTangents[i].y;
    tangent.z = mesh -> mTangents[i].z;
    vertex.tangent = tangent;

    // If the mesh has UV coordinates
    if(mesh -> mTextureCoords[0]){
      glm::vec2 uv;
      uv.x = mesh -> mTextureCoords[0][i].x;
      uv.y = mesh -> mTextureCoords[0][i].y;
      vertex.uv = uv;
    } else {
      vertex.uv = glm::vec2(0.0f, 0.0f);
    }

    vertices.push_back(vertex);
  }

  // indices
  for(unsigned int i = 0; i < mesh -> mNumFaces; i++){
    aiFace face = mesh -> mFaces[i];
    for(unsigned int j = 0; j < face.mNumIndices; j++){
      indices.push_back(face.mIndices[j]);
    }
  }

  // material
  if(mesh -> mMaterialIndex >= 0){
    aiMaterial *material = scene -> mMaterials[mesh -> mMaterialIndex];
    std::vector<Texture> diffuseMaps = loadMaterialTextures(
      material, aiTextureType_DIFFUSE, "texture_diffuse"
    );

    textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
    std::vector<Texture> specularMaps = loadMaterialTextures(
      material, aiTextureType_SPECULAR, "texture_specular"
    );

    textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
  }

  return Mesh(vertices, indices, textures);
}

void Model::processNode(aiNode *node, const aiScene *scene){
  for(unsigned int i = 0; i < node -> mNumMeshes; i++){

    //unsigned int index = (node -> mNumMeshes)[i];
    aiMesh *mesh = scene -> mMeshes[ node -> mMeshes[i] ];
    meshes.push_back(processMesh(mesh, scene));
  }
  for(unsigned int i = 0; i < node -> mNumChildren; i++){
    processNode(node -> mChildren[i], scene);
  }
}

void Model::loadModel(std::string path){
  
  // an importer takes quite a while to set up, so it might be preferable
  // to have one single importer class that sets up each mesh.
  // the mesh data is a property of the importer object, and will be deallocated if the
  // importer is. So it might need to be a global object as part of the scene class.
  //
  // But if the importer is deallocated at the end of this function, is the model data
  // still available??
  // for the skeleton class, perhaps we need an importer member variable for now, so the
  // data does not go out of scope??
  //
  // also, they are NOT thread-safe, so if using several threads to load files, it is better
  // for each of them to have their own Importer object.
  Assimp::Importer import;
  //Importer import;

  // perhaps the issue I have is that I don't use the Assimp genSmoothNormals flag??
  // perhaps that is the issue with my normal maps??
  // use the aiProcess_GenSmoothNormals flag
  // and the HasTangentsAndBitangents() function
  const aiScene *scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace | aiProcess_GenSmoothNormals);
  
  if(!scene || scene -> mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene -> mRootNode){
    std::cout << "Assimp Failed to read the file: " << path << std::endl;
    return;
  }

  directory = path.substr(0, path.find_last_of('/'));
  processNode(scene -> mRootNode, scene);
}

// why don't we just put the code from loadModel into the constructor??
Model::Model(std::string path){
  loadModel(path);
}
