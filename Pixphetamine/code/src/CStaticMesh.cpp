#include "CStaticMesh.h"

namespace PixPhetamine {
	CStaticMesh::CStaticMesh(const char* a_objectPath) throw (Exception::FileNotFound_Exception) {

		/* Fills the RAM with the mesh */
		std::string line;

		std::ifstream objetFile(a_objectPath, std::ios::in);
		if (!objetFile.is_open()) {
			throw Exception::FileNotFound_Exception(a_objectPath, __FILE__, __LINE__);
		}

		pxFloat posX, posY, posZ, normalX, normalY, normalZ, colorR, colorG, colorB;
		pxUInt16 vertexNdx1, vertexNdx2, vertexNdx3;
		while (getline(objetFile, line)) {
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
		m_VAO = new PixPhetamine::LowLevelWrapper::VAO();
		m_VAO->loadToGPU(m_vertices, m_normals, m_colors, m_faces, GL_STATIC_DRAW);
	}

	CStaticMesh::~CStaticMesh() {
		m_VAO->free();
	}
}