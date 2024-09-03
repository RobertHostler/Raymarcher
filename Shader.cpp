#include "./Shader.hpp"


Shader::Shader(const std::string& vertexPath, const std::string& fragmentPath){
  std::string vertexCode = "";
	std::string line = "";
	std::ifstream vertexFile(vertexPath.c_str());
	if (vertexFile.is_open()){
		while (std::getline(vertexFile, line)){
			vertexCode += line + "\n";
		}
		vertexFile.close();
	}

  std::string fragmentCode = "";
  line = "";
	std::ifstream fragmentFile(fragmentPath.c_str());
	if (fragmentFile.is_open()){
		while (std::getline(fragmentFile, line)){
			fragmentCode += line + "\n";
		}
		fragmentFile.close();
	}

  // a flag and an error log for compiling
  shaderProgramID = glCreateProgram();
  int success;
  char infoLog[512];

  GLuint vertexObject = glCreateShader(GL_VERTEX_SHADER);
  const char* vertexSource = vertexCode.c_str();
  glShaderSource(vertexObject, 1, &vertexSource, nullptr);
  glCompileShader(vertexObject);
  glGetShaderiv(vertexObject, GL_COMPILE_STATUS, &success);
	if (success == false){
		glGetShaderInfoLog(vertexObject, 512, NULL, infoLog);
		std::cout << "vertex shader compilation error: " << infoLog << std::endl;
	}

  GLuint fragmentObject = glCreateShader(GL_FRAGMENT_SHADER);
  const char* fragmentSource = fragmentCode.c_str();
  glShaderSource(fragmentObject, 1, &fragmentSource, nullptr);
  glCompileShader(fragmentObject);
  glGetShaderiv(fragmentObject, GL_COMPILE_STATUS, &success);
	if (success == false){
		glGetShaderInfoLog(fragmentObject, 512, NULL, infoLog);
		std::cout << "fragment shader compilation error: " << infoLog << std::endl;
	}

	glAttachShader(shaderProgramID, vertexObject);
	glAttachShader(shaderProgramID, fragmentObject);
	glLinkProgram(shaderProgramID);
	glValidateProgram(shaderProgramID);
  glGetProgramiv(shaderProgramID, GL_LINK_STATUS, &success);
	if (success == false){
		glGetProgramInfoLog(shaderProgramID, 512, NULL, infoLog);
		std::cout << "shader linking error: " << infoLog << std::endl;
	}

  glDeleteShader(vertexObject);
  glDeleteShader(fragmentObject);
}

void Shader::UseShader(){
  glUseProgram(shaderProgramID);
}

// These functions dont really use anything specific to this shader class. perhaps
// they could be put into a 'Uniform' class that manages the global shader uniforms.
// This shader class is more geared towards handling ONE shader / mesh, rather than
// managing every uniform in the entire application.

// these are never used anywhere, and can be safely removed.
void Shader::SetUniformBool(const std::string name, const bool value){
	GLint location = GetUniformLocation(name);
  glUniform1i(location, value);
}

void Shader::SetUniform1i(const std::string name, const int value){
	GLint location = GetUniformLocation(name);
  glUniform1i(location, value);
}

void Shader::SetUniform1f(const std::string name, const float value){
	GLint location = GetUniformLocation(name);
  glUniform1f(location, value);
}

void Shader::SetUniform3fv(const std::string name, const glm::vec3 value){
	GLint location = GetUniformLocation(name);
	glUniform3fv(location, 1, glm::value_ptr(value));
}

void Shader::SetUniformMatrix4fv(const std::string name, const glm::mat4 value){
	GLint location = GetUniformLocation(name);
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
}


GLint Shader::GetUniformLocation(const std::string name){
	//GLint loc = uniformCache.find(name);
	if(uniformCache.find(name) != uniformCache.end()){
		return uniformCache[name];
	} else {
		//int cameraPositionLocation = glGetUniformLocation(helmetShader.shaderProgramID, "cameraPosition");
		GLint location = glGetUniformLocation(shaderProgramID, name.c_str());
		if(location == -1){
			std::cout << "The shader uniform '" << name << "' could not be found. Perhaps it was misspelled, or optimized away?" << std::endl;
		}
		uniformCache[name] = location;
		return location;
	}
}