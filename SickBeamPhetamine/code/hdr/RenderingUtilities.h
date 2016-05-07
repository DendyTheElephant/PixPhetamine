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


namespace render {

	struct VAO {
		GLuint id;
		GLuint VBO_Vertices;
		GLuint VBO_Normals;
		GLuint VBO_Colors;
		GLuint VBO_Indices;

		VAO() { ; }
		void loadToGPU(std::vector<GLfloat> &vertices, std::vector<GLfloat> &normals, std::vector<GLfloat> &colors, std::vector<GLushort> &faces);
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