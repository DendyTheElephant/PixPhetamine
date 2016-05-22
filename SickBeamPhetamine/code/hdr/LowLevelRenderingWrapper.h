/// \file		LowLevelRenderingWrapper.h
///	\author		Daniel Huc
/// \date		April 2016
/// \brief		Custom wrapper to the OpenGL API
/// \details	Features: VAOs, GBuffers, ImageBuffers, context creation, structures, functions like AntiAliasing filtering and more ...
#pragma once

/* Standard library includes */
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>

/* External dependencies */
#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

/// Namespace used to group wrapped rendering primitives and the rendering engine.
namespace Render {

	namespace LowLevelWrapper {

		/// Unamed enum, to remplace OpenGL parameters by meaningfull names
		enum {
			// Textures
			TEXTURE =					GL_TEXTURE_2D,
			TEXTURE_FOR_ANTI_ALIASING = GL_TEXTURE_2D_MULTISAMPLE,
			// Buffer modes
			BUFFER_CONSTANT =			GL_STATIC_DRAW,
			BUFFER_DYNAMIC =			GL_DYNAMIC_DRAW
		};

		/// \brief		My VertexArrayObject structure to handle 3D models efficiently in VRAM
		/// \details	Allocate VBOs (buffers) to store vertices, normals and colors of the mesh \n
		///				Then load mesh attributes to the GPU (VRAM) \n 
		///				Use the ID of the VBO to display from the VRAM \n
		///				Then free ressources on the GPU!
		struct VAO {
			GLuint id{ 0 };				///< ID of the VAO, used to reference the active VBO in the rendering pipeline
			GLuint VBO_Vertices{ 0 };	///< Buffer containing the vertices of the 3D model
			GLuint VBO_Normals{ 0 };	///< Buffer containing the normals of the 3D model
			GLuint VBO_Colors{ 0 };		///< Buffer containing the colors of the 3D model
			GLuint VBO_Indices{ 0 };	///< Buffer containing the indices of the vertices of the 3D model

			/// \brief		Default constructor, to make static declaration then use with loadToGPU
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

		/// \brief		GBuffer is a custom FBO (image buffers) to render the scene and apply some postprocess job
		/// \details	The GBuffer is used to get the scene in the screen space, with colors, normals, depth, and
		///				type of objects in the screen. \n
		///				Post process effects and deffered shading can be applied on the color texture (a VBO is also provided).
		///				Don't forget to free() after using!
		struct GBuffer {
			int		width{ 0 };				///< Texture width
			int		height{ 0 };			///< Texture height
			GLuint	id{ 0 };				///< ID of the FBO, used to reference the active FBO in the rendering pipeline
			GLuint	colorTexture{ 0 };		///< ID of the color texture of the scene (classic rendering)
			GLuint	normalTexture{ 0 };		///< ID of the normal texture of the scene (in eyeview coord)
			GLuint	depthTexture{ 0 };		///< ID of the depth texture of the scene (Z Buffer rendering)
			GLuint	typeTexture{ 0 };		///< ID of the texture that references the object type (custom trick)
			GLuint	VBO_Quad{ 0 };			///< Buffer of the vertices used to display a texture (quad, 2 triangles)
			GLuint	VAO_id{ 0 };			///< ID of the VAO, usefull to render the quad texture

			/// \brief	Default constructor, to make static declaration then use with initialize
			GBuffer() { ; }
			/// \brief		Allocate the textures (create them in VRAM) 
			/// \details    Will creates sized textured of specified type, with depth special texture ready to be rendered in.
			/// \param		width			Width of the texture
			/// \param		height			Height of the texture
			/// \param		textureType		GL_TEXTURE_2D or any parameters from https://www.opengl.org/sdk/docs/man/html/glTexParameter.xhtml
			void initialize(int width, int height, GLenum textureType);
			void free();
		};

		/// \brief		ImageBuffer is a custom FBO to render in, usefull for many effects.
		/// \details	Use this to render in a custom texture, apply processes, render in other buffers then display on screen
		///				Don't forget to free() after using!
		struct ImageBuffer {
			int		width{ 0 };				///< Texture width
			int		height{ 0 };			///< Texture height
			GLuint	id{ 0 };				///< ID of the FBO, used to reference the active FBO in the rendering pipeline
			GLuint	texture{ 0 };			///< ID of the color texture of the scene (classic rendering)
			GLuint	VBO_Quad{ 0 };			///< Buffer of the vertices used to display a texture (quad, 2 triangles)
			GLuint	VAO_id{ 0 };			///< ID of the VAO, usefull to render the quad texture

			/// \brief	Default constructor, to make static declaration then use with initialize
			ImageBuffer() { ; }
			/// \brief		Allocate the textures (create them in VRAM) 
			/// \details    Will creates sized texture (GL_TEXTURE_2D), ready to be rendered in.
			/// \param		width			Width of the texture
			/// \param		height			Height of the texture
			void initialize(int width, int height);
			void free();
		};






		/// \brief		Will open the SDL Window (UI) and initialize OpenGL (render context) in it.
		/// \details	Use this before anything to set up the context of OpenGL and to display the window. \n
		///				Don't forget to shut down display with shutdown_SDL_GL()!
		/// \param		SDL_WindowReference		Out parameter, will reference the SDL Window that can be processed by this reference
		/// \param		SDL_GL_Context			Out parameter, will reference GL context, in fact not very usefull (needed to shutdown)
		/// \param		windowTitle				Const char* that will appears in the window caption
		/// \param		width					Width of the window and openGL viewport
		/// \param		height					Height of the window and openGL viewport
		void openWindowAndInitializeOpenGL(SDL_Window* SDL_WindowReference, SDL_GLContext* SDL_GLContextReference, const char* windowTitle, int width, int height);
		void initialiseDrawIntoBuffer(GLuint shader, GLuint fbo, GLenum* targets, unsigned short numberOfTargets);
		void multiSamplingAntiAliasing(GBuffer* Aliased, GBuffer* Output, int width, int height);
		void shutdownSDL_GL(SDL_Window* SDLWindow, SDL_GLContext GLContext);

	}
	
}