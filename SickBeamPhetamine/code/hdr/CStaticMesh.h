#pragma once

/* Standard library includes */
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

/* External dependencies */
#include <GL/glew.h>
#include <glm/glm.hpp>

/* Internal headers includes */
#include "RenderingUtilities.h"

class CStaticMesh {
/* Members */
private:
	render::VAO*		   m_VAO;
	std::vector<GLfloat>   m_vertices;
	std::vector<GLfloat>   m_normals;
	std::vector<GLfloat>   m_colors;
	std::vector<GLushort>  m_faces;

/* Methods */
public:
	CStaticMesh(const char* OBJpath);
	~CStaticMesh();

	GLuint			getVBO() { return m_VAO->id; }
	unsigned int	getNumberOfFaces() const { return m_faces.size(); }
};
