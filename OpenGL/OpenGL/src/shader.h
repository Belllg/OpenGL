#pragma once
#define SHADER_H

#include <glad/glad.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader
{
public:
	unsigned int ID; // Program ID created on glCreateProgram

	Shader(const char* vertexPath, const char* fragmentPath); //Constructor generates the shader on the fly

	void use(); // activate the shader 

	void setBool(const std::string& name, bool value) const; // utility uniform functions

	void setInt(const std::string& name, int value) const; // utility uniform functions

	void setFloat(const std::string& name, float value) const; // utility uniform functions

	void deleteProgram();

private:
	void checkCompileErrors(unsigned int shader, std::string type);  // utility function for checking shader compilation/linking errors.

};

