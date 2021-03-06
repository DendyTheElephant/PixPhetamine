﻿#include "SMeshQuad.h"

namespace PixPhetamine {

	namespace LowLevelWrapper {

		SMeshQuad::SMeshQuad() {
			STACK_TRACE;
			/* Generation of the Quad VBO (to draw the Post Process image) */
			pxFloat quadData[] = {
				-1.0f, -1.0f, 0.0f, 
				1.0f, -1.0f, 0.0f, 
				-1.0f, 1.0f, 0.0f,

				-1.0f, 1.0f, 0.0f,
				1.0f, -1.0f, 0.0f,
				1.0f, 1.0f, 0.0f 
			};
			
			// Create the buffer inside the GPU memory
			glGenVertexArrays(1, &VAO_id);
			glGenBuffers(1, &VBO_Vertices);
			// Activate VAO
			glBindVertexArray(VAO_id);
			// Store the geometry in the VRAM
			glBindBuffer(GL_ARRAY_BUFFER, VBO_Vertices);
			glBufferData(GL_ARRAY_BUFFER, sizeof(quadData), quadData, GL_STATIC_DRAW);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);
			glEnableVertexAttribArray(0);
			// Desactivate the VAO and VBO
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindVertexArray(0);

			Utility::UErrorHandler::checkOpenGLErrors();
			UNSTACK_TRACE;
		}

		SMeshQuad::~SMeshQuad() {
			glDeleteBuffers(1, &VBO_Vertices);
			glDeleteVertexArrays(1, &VAO_id);
		}
	}
}