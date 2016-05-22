#pragma once

#include <GL/glew.h>
#include <string>

class CShader {

public:
	 CShader();
	 ~CShader();

	 void load(const char *vertex_file_path, const char *fragment_file_path);
	 void reload(const char *vertex_file_path, const char *fragment_file_path);
	 inline GLuint id() {return m_programId;}

private:
	GLuint m_programId;

	// string containing the source code of the input file
	std::string getCode(const char *file_path);

	// call it after each shader compilation
	void checkCompilation(GLuint shaderId);

	// call it after linking the program
	void checkLinks(GLuint programId);
};
