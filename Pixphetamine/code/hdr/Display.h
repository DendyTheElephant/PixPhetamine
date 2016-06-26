/// \file		Display.h
///	\author		Daniel Huc
/// \date		June 2016
/// \brief		Functions to initialize Window and OpenGL context
#pragma once

/* Standard library includes */
#include <iostream>

/* External dependencies */
#include <GL/glew.h>
#include <SDL2/SDL.h>

/* Internal headers includes */
#include "HInternalTypesAliases.h"
#include "UErrorHandler.h"

/// Namespace used to group wrapped rendering primitives and the rendering engine.
namespace PixPhetamine {

	/// This part of the program is in charge of initialisation and destruction of Window and GLContext
	namespace Display {

		/// \brief		Will open the SDL Window (UI) and initialize OpenGL (PixPhetamine context) in it.
		/// \details	Use this before anything to set up the context of OpenGL and to display the window. \n
		///				Don't forget to shut down display with shutdown_SDL_GL()!
		/// \param		SDL_WindowReference		Out parameter, will reference the SDL Window that can be processed by this reference (Reference pointer of a pointer)
		/// \param		SDL_GL_Context			Out parameter, will reference GL context, in fact not very usefull (needed to shutdown)
		/// \param		windowTitle				Const char* that will appears in the window caption
		/// \param		width					Width of the window and openGL viewport
		/// \param		height					Height of the window and openGL viewport
		void openWindowAndInitializeOpenGL(SDL_Window*& SDL_WindowReference, SDL_GLContext* SDL_GLContextReference, const char* windowTitle, pxInt width, pxInt height);
		
		/// \brief		Will close and free the SDL Window (UI) and OpenGL (PixPhetamine context).
		/// \param		SDL_WindowReference		Out parameter, an opened and initialized window (by SDL)
		/// \param		SDL_GL_ContextReference	In parameter, an initilized context for OpenGL (by SDL)
		void shutdownSDL_GL(SDL_Window* SDL_WindowReference, SDL_GLContext SDL_GLContextReference);
	}

}