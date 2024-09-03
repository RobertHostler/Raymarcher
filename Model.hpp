#ifndef MODEL_H
#define MODEL_H

#include "./Mesh.hpp"
#include "./Shader.hpp"
#include <string>

//#define STB_IMAGE_IMPLEMENTATION

// we only need this line
//#include "stb_image.h"


class Model{
public:
  Model(std::string path);
  
  void Draw(Shader &shader);

//private:
  std::vector<Mesh> meshes;
  std::string directory;

  unsigned int TextureFromFile(const char *path, const std::string &directory, bool gamma = false);

  void loadModel(std::string path);
  void processNode(aiNode *node, const aiScene *scene);
  Mesh processMesh(aiMesh *mesh, const aiScene *scene);
  std::vector<Texture> loadMaterialTextures(aiMaterial *material, aiTextureType type, std::string typeName);
};


#endif
