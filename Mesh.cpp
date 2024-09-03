#include "./Mesh.hpp"


Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures){
  this -> vertices = vertices;
  this -> indices = indices;
  this -> textures = textures;
  setupBuffers();
}

// relies on textures using a very specific naming convention.
// This seems like VERY INEFFICIENT code to run each frame. there
// has to be a better way to handle this.
void Mesh::Draw(Shader &shader){
  // determines how many of each type of texture there are

  // unsigned int diffuseNum = 1;
  // unsigned int specularNum = 1;
  // for(unsigned int i = 0; i < textures.size(); i++){
  //   // GL_TEXTURE0 is an enum, or an integer. We can add i to it.
  //   glActiveTexture(GL_TEXTURE0 + i);
  //
  //   std::string number;
  //   std::string name = textures[i].type;
  //   if(name == "texture_diffuse") {
  //     number = std::to_string(diffuseNum++);
  //   } else if (name == "texture_specular") {
  //     number = std::to_string(specularNum++);
  //   }
  //   shader.setUniformInt(("material." + name + number).c_str(), i);
  //   glBindTexture(GL_TEXTURE0);
  // }


  // perhaps I should give each mesh a handle to a shader, so that it can set its
  // own shader before it draws. That way I dont have to manually handle that in code.



  // Draw the mesh
  //This is the ONLY part of the function that should be run each frame
  
  //Manual rendering
  glActiveTexture(GL_TEXTURE0);
  glBindVertexArray(VAO);

  // draws the mesh
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

  // draws the wireframe
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

  glBindVertexArray(0);
}

void Mesh::setupBuffers(){
  glGenVertexArrays(1, &VAO);

  glGenBuffers(1, &VBO);
  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

  glGenBuffers(1, &IBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()*sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

  // Vertex positions
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

  // Normals
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

  // tangents
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tangent));

  // UVs
  glEnableVertexAttribArray(3);
  glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));

  glBindVertexArray(0);
}