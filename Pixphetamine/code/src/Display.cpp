#include "Display.h"

namespace PixPhetamine {

	namespace Display {

		/* Anonymous namespace for Display's private functions: checkSDLError */
		namespace {
			void checkSDLError() {
				const char *error = SDL_GetError();
				if (*error != '\0') {
					ERROR_CONTINUE("SDL Error: " + std::string(error));
					SDL_ClearError();
				}
			}
		}
		/* End of Anonymous namespace */

		void openWindowAndInitializeOpenGL(SDL_Window*& SDL_WindowReference, SDL_GLContext* SDL_GLContextReference, const char* windowTitle, pxInt const& width, pxInt const& height) {
			STACK_TRACE;
			/* Initialize SDL's Video subsystem */
			if (SDL_Init(SDL_INIT_VIDEO) < 0) {
				STACK_TRACE;
				ERROR("Unable to initialize SDL!");
			}

			/* Request opengl 3.2 context.
			* SDL doesn't have the ability to choose which profile at this time of writing,
			* but it should default to the core profile */
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

			/* Enable double buffering (one frame delay but complete rendering before update) */
			SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
			SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

			/* Multi sampling to prevent aliasing effect, here MSAA x2
			* MSAA2: Creates 2 images of the edges with a little offset
			* and samples values between these two images (SSAA>MSAA>FSAA)*/
			//SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
			//SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
			/* SDL MSAA is disabled since we do post-process and we have to MS ourselves*/

			/* Enable Anti-Aliasing filtering */
			//glEnable(GL_MULTISAMPLE);

			/* Make shure that the powerful GPU will execute this context
			* (Need when two GPU with Optimus for example) */
			SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);

			/* Hide the cursor (it's an FPS!) */
			SDL_ShowCursor(0);


			/* Create our window centered */
			SDL_WindowReference = SDL_CreateWindow(windowTitle, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
			if (!SDL_WindowReference) { /* Die if creation failed */
				STACK_TRACE;
				ERROR("Unable to create the window!");
			}

			/* Create our opengl context and attach it to our window */
			*SDL_GLContextReference = SDL_GL_CreateContext(SDL_WindowReference);

			/* Enable vertical synchronization */
			//SDL_GL_SetSwapInterval(1); 

			STACK_MESSAGE("Checking for OpenGL errors");
			Utility::UErrorHandler::checkOpenGLErrors();

			/* Sometimes needed to run in "Experimental" to have access to VBO and stuff */
			glewExperimental = GL_TRUE;
			GLenum status = glewInit();
			if (status != GLEW_OK) {
				STACK_TRACE;
				ERROR("GLEW Error: "); //+ std::string(glewGetErrorString(status)));
			}

			// Catch the GL_INVALID_ENUM error caused by glewInit()! It seems to be non harmfull...
			glGetError();

			STACK_MESSAGE("Checking for SDL errors");
			checkSDLError();

			// Retrieve the GPU - OpenGL Current specs for the platform --> Log file
			std::cerr << "=============[ PixPhetamine log-file ]=========================" << std::endl;
			std::cerr << ">Graphics Successfully Initialized !" << std::endl;
			std::cerr << "OpenGL Info" << std::endl;
			std::cerr << "    Version: " << glGetString(GL_VERSION) << std::endl;
			std::cerr << "     Vendor: " << glGetString(GL_VENDOR) << std::endl;
			std::cerr << "   Renderer: " << glGetString(GL_RENDERER) << std::endl;
			std::cerr << "    Shading: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
			std::cerr << "----------------------------------------------------------------" << std::endl;
			std::cerr << ">GPU Specifications for modern GLSL:" << std::endl;
			pxInt uboBindings, uboSize, uboVertex, uboFragment, uboGeometry;
			glGetIntegerv(GL_MAX_UNIFORM_BUFFER_BINDINGS, &uboBindings);
			glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, &uboSize);
			glGetIntegerv(GL_MAX_VERTEX_UNIFORM_BLOCKS, &uboVertex);
			glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_BLOCKS, &uboFragment);
			glGetIntegerv(GL_MAX_GEOMETRY_UNIFORM_BLOCKS, &uboGeometry);
			std::cerr << "  Max uniform block bindings: " << uboBindings << std::endl;
			std::cerr << "      Max uniform block size: " << uboSize << std::endl;
			std::cerr << "    Max uniform block vertex: " << uboVertex << std::endl;
			std::cerr << "  Max uniform block fragment: " << uboFragment << std::endl;
			std::cerr << "  Max uniform block geometry: " << uboGeometry << std::endl;
			std::cerr << "----------------------------------------------------------------" << std::endl;
			std::cerr << ">Texture specifications:" << std::endl;
			pxInt texUnits;
			glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &texUnits);
			//GL_MAX_COLOR_ATTACHMENTS
			//
			std::cerr << "     Max texture image units: " << uboBindings << std::endl;
			std::cerr << "----------------------------------------------------------------" << std::endl;
			UNSTACK_TRACE;
		}


		/* Delete our opengl context, destroy our window, and shutdown SDL */
		void shutdownSDL_GL(SDL_Window* SDLWindow, SDL_GLContext GLContext) {
			SDL_GL_DeleteContext(GLContext);
			SDL_DestroyWindow(SDLWindow);
			SDL_Quit();
		}
	}
}