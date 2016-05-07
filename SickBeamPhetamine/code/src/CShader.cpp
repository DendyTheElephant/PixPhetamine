#include "CShader.h"

#include <stdio.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <stdlib.h>
#include <string.h>

using namespace std;

CShader::CShader(): m_programId(0) {

}

CShader::~CShader() {
	if (glIsProgram(m_programId)) {
		glDeleteProgram(m_programId);
	}
}

void CShader::load(const char *vertex_file_path,
	const char *fragment_file_path) {

	// create and compile vertex shader object
	std::string vertexCode = getCode(vertex_file_path);
	const char * vertexCodeC = vertexCode.c_str();
	GLuint vertexId = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexId, 1, &(vertexCodeC), NULL);
	glCompileShader(vertexId);
	cerr << vertex_file_path << " :" << endl;
	checkCompilation(vertexId);

	// create and compile fragment shader object
	std::string fragmentCode = getCode(fragment_file_path);
	const char * fragmentCodeC = fragmentCode.c_str();
	GLuint fragmentId = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentId, 1, &(fragmentCodeC), NULL);
	glCompileShader(fragmentId);
	cerr << fragment_file_path << " :" << endl;
	checkCompilation(fragmentId);

	// create, attach and link program object
	m_programId = glCreateProgram();
	glAttachShader(m_programId, vertexId);
	glAttachShader(m_programId, fragmentId);
	glLinkProgram(m_programId);
	checkLinks(m_programId);

	// delete vertex and fragment ids
	glDeleteShader(vertexId);
	glDeleteShader(fragmentId);
}


void CShader::reload(const char *vertex_file_path,
	const char *fragment_file_path) {

	// check if the program already contains a shader 
	if (glIsProgram(m_programId)) {
		// delete it...
		glDeleteProgram(m_programId);
	}

	// ... and reload it
	load(vertex_file_path, fragment_file_path);
}




void CShader::checkCompilation(GLuint shaderId) {
	// check if the compilation was successfull (and display syntax errors)
	// call it after each shader compilation
	GLint result = GL_FALSE;
	int infoLogLength;

	glGetShaderiv(shaderId, GL_COMPILE_STATUS, &result);
	glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &infoLogLength);

	if (infoLogLength > 0) {
		std::vector<char> message(infoLogLength + 1);
		glGetShaderInfoLog(shaderId, infoLogLength, NULL, &message[0]);
		std::cerr << &message[0] << std::endl;
	}
}

void CShader::checkLinks(GLuint programId) {
	// check if links were successfull (and display errors)
	// call it after linking the program  
	GLint result = GL_FALSE;
	int infoLogLength;

	glGetProgramiv(programId, GL_LINK_STATUS, &result);
	glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &infoLogLength);

	if (infoLogLength > 0) {
		std::vector<char> message(infoLogLength + 1);
		glGetProgramInfoLog(programId, infoLogLength, NULL, &message[0]);
		std::cerr << &message[0] << std::endl;
	}
}

std::string CShader::getCode(const char *file_path) {
	// return a string containing the source code of the input file
	std::string   shaderCode;
	std::ifstream shaderStream(file_path, std::ios::in);

	if (!shaderStream.is_open()) {
		std::cerr << "Unable to open " << file_path << std::endl;
		return "";
	}

	std::string line = "";
	while (getline(shaderStream, line))
		shaderCode += "\n" + line;
	shaderStream.close();

	return shaderCode;
}
