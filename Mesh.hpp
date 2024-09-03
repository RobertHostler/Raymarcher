#ifndef MESH_H
#define MESH_H

#include <string>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "./Shader.hpp"


// struct Vertex {
//   glm::vec3 position;
//   glm::vec3 normal;
//   glm::vec2 uv;
// };

struct Vertex {
  glm::vec3 position;
  glm::vec3 normal;
  glm::vec3 tangent;
  glm::vec2 uv;
};


struct Texture {
  unsigned int id;
  std::string type;
  std::string path;
};


class Mesh {
public:
  std::vector<Vertex> vertices;
  std::vector<unsigned int> indices;
  std::vector<Texture> textures;
  Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);
  void Draw(Shader &shader);

//private:
  unsigned int VAO, VBO, IBO;
  void setupBuffers();
};


#endif