#include "LowLevelRenderingWrapper.h"

namespace PixPhetamine {

	/* Anonymous namespace for RenderingUtilities private functions  ===========================================*/
	namespace {
		void checkSDLError() {
			const char *error = SDL_GetError();
			if (*error != '\0') {
				ERROR_CONTINUE("SDL Error: " + std::string(error));
				SDL_ClearError();
			}
		}

		void checkFrameBuffer() {
			GLenum status;
			if ((status = glCheckFramebufferStatus(GL_FRAMEBUFFER)) != GL_FRAMEBUFFER_COMPLETE) {
				switch (status) {

					case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
						ERROR("Framebuffer error: Framebuffer incomplete, Attachment is NOT complete");
						break;

					case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
						ERROR("Framebuffer error: Framebuffer incomplete, No image is attached to FBO");
						break;

					case GL_FRAMEBUFFER_UNSUPPORTED:
						ERROR("Framebuffer error: Unsupported FBO implementation");
						break;

					default:
						ERROR("Framebuffer error: Unknown error");
						break;
				}
			}
		}

		void createTexture(GLvramLocation &texture, GLenum textureType, GLenum iFormat, GLenum format, pxInt width, pxInt height) {
			glGenTextures(1, &texture);
			glBindTexture(textureType, texture);
			glTexParameteri(textureType, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(textureType, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(textureType, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(textureType, GL_TEXTURE_WRAP_T, GL_REPEAT);
			if (textureType == GL_TEXTURE_2D)
				glTexImage2D(GL_TEXTURE_2D, 0, iFormat, width, height, 0, format, GL_FLOAT, NULL);
			else
				glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 8, iFormat, width, height, GL_TRUE);
			glBindTexture(textureType, 0);
		}

		void createQuad(GLvramLocation &VAO, GLvramLocation &VBO) {
			/* Generation of the Quad VBO (to draw the Post Process image) */
			const pxFloat quadData[] = { -1.0f, -1.0f, 0.0f, 1.0f, -1.0f, 0.0f, -1.0f, 1.0f, 0.0f,
				-1.0f, 1.0f, 0.0f, 1.0f, -1.0f, 0.0f, 1.0f, 1.0f, 0.0f };
			glGenVertexArrays(1, &VAO);
			glGenBuffers(1, &VBO);
			glBindVertexArray(VAO);
			glBindBuffer(GL_ARRAY_BUFFER, VBO); // vertices
			glBufferData(GL_ARRAY_BUFFER, sizeof(quadData), quadData, GL_STATIC_DRAW);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);
			glEnableVertexAttribArray(0);
		}
	}
	/* End of Anonymous namespace ==========================================================================*/







	namespace LowLevelWrapper {
		/* ======================================================================================================================================================== */
		/* ======================     [VAO]     =================================================================================================================== */
		/* ======================================================================================================================================================== */
		void VAO::loadToGPU(pxFloatArray &vertices, pxFloatArray &normals, pxFloatArray &colors, pxUInt16Array &faces, GLenum mode) {
			// Create some buffers inside the GPU memory
			glGenVertexArrays(1, &id);
			glGenBuffers(1, &VBO_Vertices);
			glGenBuffers(1, &VBO_Normals);
			glGenBuffers(1, &VBO_Colors);
			glGenBuffers(1, &VBO_Indices);

			// Activate VAO
			glBindVertexArray(id);

			// Store mesh positions into buffer inside the GPU memory
			glBindBuffer(GL_ARRAY_BUFFER, VBO_Vertices);
			glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(pxFloat), vertices.data(), mode);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);
			glEnableVertexAttribArray(0);

			// Store mesh normals into buffer inside the GPU memory
			glBindBuffer(GL_ARRAY_BUFFER, VBO_Normals);
			glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(pxFloat), normals.data(), mode);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, 0, (void *)0);
			glEnableVertexAttribArray(1);

			// Store mesh colors into buffer inside the GPU memory
			glBindBuffer(GL_ARRAY_BUFFER, VBO_Colors);
			glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(pxFloat), colors.data(), mode);
			glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);
			glEnableVertexAttribArray(2);

			// Store mesh indices into buffer inside the GPU memory
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBO_Indices);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, faces.size() * sizeof(GLushort), faces.data(), mode);

			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindVertexArray(0);
		}


		void VAO::free() {
			glDeleteBuffers(1, &VBO_Vertices);
			glDeleteBuffers(1, &VBO_Normals);
			glDeleteBuffers(1, &VBO_Colors);
			glDeleteBuffers(1, &VBO_Indices);
			glDeleteVertexArrays(1, &id);
		}



		/* ======================================================================================================================================================== */
		/* ======================     [GBuffer]     =============================================================================================================== */
		/* ======================================================================================================================================================== */
		void GBuffer::initialize(pxInt width, pxInt height, GLenum textureType) {
			STACK_TRACE;
			/* Texture */
			glActiveTexture(GL_TEXTURE0);
			createTexture(colorTexture, textureType, GL_RGBA32F, GL_RGBA, width, height);

			glActiveTexture(GL_TEXTURE1);
			createTexture(normalTexture, textureType, GL_RGBA32F, GL_RGBA, width, height);

			glActiveTexture(GL_TEXTURE2);
			createTexture(typeTexture, textureType, GL_RGBA32F, GL_RGBA, width, height);

			glActiveTexture(GL_TEXTURE3);
			createTexture(depthTexture, textureType, GL_DEPTH_COMPONENT24, GL_DEPTH_COMPONENT, width, height);

			/* Framebuffer to link everything together */
			glGenFramebuffers(1, &id);
			glBindFramebuffer(GL_FRAMEBUFFER, id);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, textureType, colorTexture, 0);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, textureType, normalTexture, 0);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, textureType, typeTexture, 0);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, textureType, depthTexture, 0);

			STACK_MESSAGE("Checking Framebuffer errors");
			checkFrameBuffer();

			glBindFramebuffer(GL_FRAMEBUFFER, 0);

			createQuad(VAO_id, VBO_Quad);
			UNSTACK_TRACE;
		}


		void GBuffer::free() {
			glDeleteBuffers(1, &VBO_Quad);
			glDeleteTextures(1, &depthTexture);
			glDeleteTextures(1, &typeTexture);
			glDeleteTextures(1, &colorTexture);
			glDeleteTextures(1, &normalTexture);
			glDeleteVertexArrays(1, &VAO_id);
			glDeleteFramebuffers(1, &id);
		}



		/* ======================================================================================================================================================== */
		/* ======================     [ImageBuffer]     =========================================================================================================== */
		/* ======================================================================================================================================================== */
		void ImageBuffer::initialize(pxInt width, pxInt height) {
			STACK_TRACE;
			/* Texture */
			glActiveTexture(GL_TEXTURE0);
			createTexture(texture, GL_TEXTURE_2D, GL_RGBA32F, GL_RGBA, width, height);

			/* Framebuffer to link everything together */
			glGenFramebuffers(1, &id);
			glBindFramebuffer(GL_FRAMEBUFFER, id);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

			STACK_TRACE;
			checkFrameBuffer();

			glBindFramebuffer(GL_FRAMEBUFFER, 0);

			createQuad(VAO_id, VBO_Quad);
			UNSTACK_TRACE;
		}

		void ImageBuffer::free() {
			glDeleteBuffers(1, &VBO_Quad);
			glDeleteTextures(1, &texture);
			glDeleteVertexArrays(1, &VAO_id);
			glDeleteFramebuffers(1, &id);
		}







		/* ======================================================================================================================================================== */
		/* ======================     [Routines]     ============================================================================================================== */
		/* ======================================================================================================================================================== */
		void openWindowAndInitializeOpenGL(SDL_Window*& SDL_WindowReference, SDL_GLContext* SDL_GLContextReference, const char* windowTitle, pxInt width, pxInt height) {
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

			/* Create our window centered */
			SDL_WindowReference = SDL_CreateWindow(windowTitle, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
			if (!SDL_WindowReference) { /* Die if creation failed */
				STACK_TRACE;
				ERROR("Unable to create the window!");
			}

			/* Hide the cursor (it's an FPS!) */
			SDL_ShowCursor(0);

			/* Enable vertical synchronization */
			SDL_GL_SetSwapInterval(1);

			/* Create our opengl context and attach it to our window */
			*SDL_GLContextReference = SDL_GL_CreateContext(SDL_WindowReference);

			/* Sometimes needed to run in "Experimental" to have access to VBO and stuff */
			glewExperimental = GL_TRUE;
			GLenum status = glewInit();
			if (status != GLEW_OK) {
				STACK_TRACE;
				ERROR("GLEW Error: "); //+ std::string(glewGetErrorString(status)));
			}




			// Retrieve the GPU - OpenGL Current specs for the platform --> Log file
			std::cerr << "=============[ PixPhetamine log-file ]=========================" << std::endl;
			std::cerr << ">Graphics Successfully Initialized !" << std::endl;
			std::cerr << "OpenGL Info" << std::endl;
			std::cerr << "    Version: " << glGetString(GL_VERSION) << std::endl;
			std::cerr << "     Vendor: " << glGetString(GL_VENDOR) << std::endl;
			std::cerr << "   Renderer: " << glGetString(GL_RENDERER) << std::endl;
			std::cerr << "    Shading: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
			std::cerr << "----------------------------------------------------------------" << std::endl;
			std::cerr << ">SDL error messages:" << std::endl;
			STACK_MESSAGE("Checking for SDL errors");
			checkSDLError();
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

			std::cerr << "===============================================================" << std::endl;
			UNSTACK_TRACE;
		}


		void initialiseDrawIntoBuffer(GLvramLocation shader, GLvramLocation fbo, GLenum* targets, unsigned short numberOfTargets) {
			glBindFramebuffer(GL_FRAMEBUFFER, fbo);

			/* Set color to clear color */
			glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
			/* Clear buffer color and Z-buffer */
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			/* Enable the Z-Buffer */
			glEnable(GL_DEPTH_TEST);
			/* Enable multi sampled rendering */
			//glEnable(GL_MULTISAMPLE);

			// We want to draw into FBO!!!
			glDrawBuffers(numberOfTargets, targets);

			// activate the current shader 
			glUseProgram(shader);
		}


		void multiSamplingAntiAliasing(GBuffer* Aliased, GBuffer* Output, pxInt width, pxInt height) {
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, Output->id); // Result is going in the non aliased GBuffer
			glBindFramebuffer(GL_READ_FRAMEBUFFER, Aliased->id); // From the multi sampled aliased GBuffer
			glClearColor(0.5, 0.5, 0.5, 1.0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glDisable(GL_DEPTH);

			// Resolve color multisampling
			glReadBuffer(GL_COLOR_ATTACHMENT0);
			glDrawBuffer(GL_COLOR_ATTACHMENT0);
			glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_LINEAR);

			// Resolve normal multisampling
			glReadBuffer(GL_COLOR_ATTACHMENT1);
			glDrawBuffer(GL_COLOR_ATTACHMENT1);
			glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_LINEAR);

			// Resolve type multisampling
			glReadBuffer(GL_COLOR_ATTACHMENT2);
			glDrawBuffer(GL_COLOR_ATTACHMENT2);
			glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_LINEAR);

			// Resolve depth multisampling
			glReadBuffer(GL_DEPTH_ATTACHMENT);
			glDrawBuffer(GL_DEPTH_ATTACHMENT);
			glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_DEPTH_BUFFER_BIT, GL_LINEAR);
		}


		/* Delete our opengl context, destroy our window, and shutdown SDL */
		void shutdownSDL_GL(SDL_Window* SDLWindow, SDL_GLContext GLContext) {
			SDL_GL_DeleteContext(GLContext);
			SDL_DestroyWindow(SDLWindow);
			SDL_Quit();
		}
	}
}