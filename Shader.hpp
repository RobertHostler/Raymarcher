#ifndef SHADER_H
#define SHADER_H


#include <glad/glad.h>
//#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <fstream>
#include <iostream>
#include <unordered_map>

class Shader{
public:
  unsigned int shaderProgramID;
  std::unordered_map<std::string, GLint> uniformCache;

  Shader(const std::string& vsPath, const std::string& fsPath);
  void UseShader();
  void SetUniformBool(const std::string name, const bool value);
  void SetUniform1i(const std::string name, const int value);
  void SetUniform1f(const std::string name, const float value);

  void SetUniform3fv(const std::string name, const glm::vec3 value);
  void SetUniformMatrix4fv(const std::string name, const glm::mat4 value);

  GLint GetUniformLocation(const std::string name);
};


#endif