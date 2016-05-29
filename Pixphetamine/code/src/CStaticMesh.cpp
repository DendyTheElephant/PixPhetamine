#include "CStaticMesh.h"

CStaticMesh::CStaticMesh(const char* OBJpath) {

	/* Fills the RAM with the mesh */
	std::string line;
	
	std::ifstream OBJfile(OBJpath, std::ios::in);
	if (!OBJfile) {
		std::cerr << "Cannot open: " << OBJpath << std::endl; 
		exit(1);
	}
	
	GLfloat posX, posY, posZ, normalX, normalY, normalZ, colorR, colorG, colorB;
	GLushort vertexNdx1, vertexNdx2, vertexNdx3;
	while (getline(OBJfile, line)) {
		if (line.substr(0, 2) == "v ") {
			std::istringstream s(line.substr(2));
			s >> posX >> posY >> posZ >> normalX >> normalY >> normalZ >> colorR >> colorG >> colorB;
			m_vertices.push_back(posX); m_vertices.push_back(posY); m_vertices.push_back(posZ);
			m_normals.push_back(normalX); m_normals.push_back(normalY); m_normals.push_back(normalZ);
			m_colors.push_back(colorR); m_colors.push_back(colorG); m_colors.push_back(colorB);
		}
		else if (line.substr(0, 2) == "f ") {
			std::istringstream s(line.substr(2));
			s >> vertexNdx1 >> vertexNdx2 >> vertexNdx3;
			m_faces.push_back(vertexNdx1); m_faces.push_back(vertexNdx2); m_faces.push_back(vertexNdx3);
		}
	}

	/* Fills the VRAM with the mesh */
	m_VAO = new render::VAO();
	m_VAO->loadToGPU(m_vertices, m_normals, m_colors, m_faces);
}

CStaticMesh::~CStaticMesh() {
	m_VAO->free();
}