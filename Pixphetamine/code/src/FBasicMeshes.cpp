#include "FBasicMeshes.h"

namespace PixPhetamine {

	namespace LowLevelWrapper {

		VBasicMesh::~VBasicMesh() { /***/ }

		void VBasicMesh::sendToVRAM(std::vector<pxFloat> a_vertices) {
			STACK_TRACE;

			// Create the buffer inside the GPU memory
			glGenVertexArrays(1, &m_VAOid);
			glGenBuffers(1, &m_VBOVertices);
			// Activate VAO
			glBindVertexArray(m_VAOid);
			// Store the geometry in the VRAM
			glBindBuffer(GL_ARRAY_BUFFER, m_VBOVertices);
			glBufferData(GL_ARRAY_BUFFER, sizeof(pxFloat) * a_vertices.size(), a_vertices.data(), GL_STATIC_DRAW);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);
			glEnableVertexAttribArray(0);
			// Desactivate the VAO and VBO
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindVertexArray(0);

			Utility::UErrorHandler::checkOpenGLErrors();
			UNSTACK_TRACE;
		}

		void VBasicMesh::deleteFromVRAM() {
			glDeleteBuffers(1, &m_VBOVertices);
			glDeleteVertexArrays(1, &m_VAOid);
		}

		CBasicMeshQuad::CBasicMeshQuad() {
			std::vector<pxFloat> vertices {
				-1.0f, -1.0f, 0.0f,
				1.0f, -1.0f, 0.0f,
				-1.0f, 1.0f, 0.0f,

				-1.0f, 1.0f, 0.0f,
				1.0f, -1.0f, 0.0f,
				1.0f, 1.0f, 0.0f
			};
			sendToVRAM(vertices);
		}

		CBasicMeshCube::CBasicMeshCube() {
			std::vector<pxFloat> vertices {
				-1.0f, 1.0f, -1.0f,
				-1.0f, -1.0f, -1.0f,
				1.0f, -1.0f, -1.0f,
				1.0f, -1.0f, -1.0f,
				1.0f, 1.0f, -1.0f,
				-1.0f, 1.0f, -1.0f,

				-1.0f, -1.0f, 1.0f,
				-1.0f, -1.0f, -1.0f,
				-1.0f, 1.0f, -1.0f,
				-1.0f, 1.0f, -1.0f,
				-1.0f, 1.0f, 1.0f,
				-1.0f, -1.0f, 1.0f,

				1.0f, -1.0f, -1.0f,
				1.0f, -1.0f, 1.0f,
				1.0f, 1.0f, 1.0f,
				1.0f, 1.0f, 1.0f,
				1.0f, 1.0f, -1.0f,
				1.0f, -1.0f, -1.0f,

				-1.0f, -1.0f, 1.0f,
				-1.0f, 1.0f, 1.0f,
				1.0f, 1.0f, 1.0f,
				1.0f, 1.0f, 1.0f,
				1.0f, -1.0f, 1.0f,
				-1.0f, -1.0f, 1.0f,

				-1.0f, 1.0f, -1.0f,
				1.0f, 1.0f, -1.0f,
				1.0f, 1.0f, 1.0f,
				1.0f, 1.0f, 1.0f,
				-1.0f, 1.0f, 1.0f,
				-1.0f, 1.0f, -1.0f,

				-1.0f, -1.0f, -1.0f,
				-1.0f, -1.0f, 1.0f,
				1.0f, -1.0f, -1.0f,
				1.0f, -1.0f, -1.0f,
				-1.0f, -1.0f, 1.0f,
				1.0f, -1.0f, 1.0f
			};
			sendToVRAM(vertices);
		}
	}
}