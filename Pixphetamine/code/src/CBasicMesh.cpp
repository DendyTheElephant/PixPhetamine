#include "CBasicMesh.h"

namespace PixPhetamine {

	namespace LowLevelWrapper {

		CBasicMesh::CBasicMesh(const pxFloat a_vertices[]) {
			STACK_TRACE;

			// Create the buffer inside the GPU memory
			glGenVertexArrays(1, &m_VAOid);
			glGenBuffers(1, &m_VBOVertices);
			// Activate VAO
			glBindVertexArray(m_VAOid);
			// Store the geometry in the VRAM
			glBindBuffer(GL_ARRAY_BUFFER, m_VBOVertices);
			glBufferData(GL_ARRAY_BUFFER, sizeof(a_vertices), a_vertices, GL_STATIC_DRAW);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);
			glEnableVertexAttribArray(0);
			// Desactivate the VAO and VBO
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindVertexArray(0);

			Utility::UErrorHandler::checkOpenGLErrors();
			UNSTACK_TRACE;
		}

		CBasicMesh::~CBasicMesh() {
			glDeleteBuffers(1, &m_VBOVertices);
			glDeleteVertexArrays(1, &m_VAOid);
		}
	}
}