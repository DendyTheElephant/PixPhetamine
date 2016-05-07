/// \file		LowLevelRenderingWrapper.h
///	\author		Daniel Huc
/// \date		April 2016
/// \brief		Description principale
/// \details	Sous description
#pragma once

/* Standard library includes */
#include <iostream>
#include <vector>

/* External dependencies */
#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

/// Namespace used to group wrapped rendering primitives and the rendering engine.
namespace Render {

	namespace LowLevelWrapper {

		/// \brief		My VertexArrayObject structure to handle 3D models efficiently in VRAM
		/// \details	Allocate VBOs (buffers) to store vertices, normals and colors of the mesh \n
		///				Then load mesh attributes to the GPU (VRAM) \n 
		///				Use the ID of the VBO to display from the VRAM \n
		///				Then free ressources on the GPU!
		struct VAO {
			GLuint id;				///< ID of the VAO, used to reference the active VBO in the rendering pipeline
			GLuint VBO_Vertices;	///< Buffer containing the vertices of the 3D model
			GLuint VBO_Normals;		///< Buffer containing the normals of the 3D model
			GLuint VBO_Colors;		///< Buffer containing the colors of the 3D model
			GLuint VBO_Indices;		///< Buffer containing the indices of the vertices of the 3D model

			VAO() { ; }
			/// \brief		Allocate VBOs, send buffers to VRAM, so you can use VAO.
			/// \details    You have to specify the vertices, normals, colors, faces and then choose the buffer type (see parameters of mode).
			/// \param		vertices	Vector of GLfloat must be allocated.
			/// \param		normals		Vector of GLfloat must be allocated.
			/// \param		colors		Vector of GLfloat must be allocated.
			/// \param		faces		Vector of GLfloat must be allocated.
			/// \param		mode		GL_STATIC_DRAW or any parameters from https://www.opengl.org/sdk/docs/man/html/glBufferData.xhtml
			void loadToGPU(std::vector<GLfloat> &vertices, std::vector<GLfloat> &normals, std::vector<GLfloat> &colors, std::vector<GLushort> &faces, GLenum mode);
			void free();
		};

		struct GBuffer {
			GLuint id;
			GLuint colorTexture;
			GLuint normalTexture;
			GLuint depthTexture;
			GLuint typeTexture;
			GLuint VBO_Quad;
			GLuint VAO_id;

			GBuffer() { ; }
			void initialize(int width, int height, GLenum textureType);
			void free();
		};

		struct ImageBuffer {
			GLuint id;
			GLuint texture;
			GLuint VBO_Quad;
			GLuint VAO_id;

			ImageBuffer() { ; }
			void initialize(int width, int height);
			void free();
		};

		struct

			SDL_Window* createSDLWindow(const char* windowTitle, int width, int height);
		SDL_GLContext initRenderContext(SDL_Window* SDLWindow);
		GLuint createGBuffer();
		void initialiseDrawIntoBuffer(GLuint shader, GLuint fbo, GLenum* targets, unsigned short numberOfTargets);
		void multiSamplingAntiAliasing(GBuffer* Aliased, GBuffer* Output, int width, int height);
		void shutdownSDL_GL(SDL_Window* SDLWindow, SDL_GLContext GLContext);

	}
	
}