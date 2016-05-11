#include "LowLevelRenderingWrapper.h"

namespace Render {

	/* Anonymous namespace for RenderingUtilities private functions  ===========================================*/
	namespace {
		/* A simple function that prints a message, the error code returned by SDL,
		* and quits the application */
		void quit(const char *msg) {
			std::cerr << msg << ", " << SDL_GetError() << std::endl;
			SDL_Quit();
			exit(1);
		}

		void checkSDLError(int line = -1) {
			const char *error = SDL_GetError();
			if (*error != '\0') {
				std::cerr << "SDL Error: " << error << std::endl;
				if (line != -1) {
					std::cerr << " + line: " << line << std::endl;
				}
				SDL_ClearError();
			}
		}

		void checkFrameBuffer(int line = -1) {
			GLenum status;
			if ((status = glCheckFramebufferStatus(GL_FRAMEBUFFER)) != GL_FRAMEBUFFER_COMPLETE) {
				std::cerr << "glCheckFramebufferStatus: error " << status << std::endl;
				switch (status)
				{
				case GL_FRAMEBUFFER_COMPLETE:
					std::cerr << "Framebuffer complete." << std::endl;
					break;

				case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
					std::cerr << "[ERROR] Framebuffer incomplete: Attachment is NOT complete." << std::endl;
					break;

				case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
					std::cerr << "[ERROR] Framebuffer incomplete: No image is attached to FBO." << std::endl;
					break;

				case GL_FRAMEBUFFER_UNSUPPORTED:
					std::cerr << "[ERROR] Unsupported by FBO implementation." << std::endl;
					break;

				default:
					std::cerr << "[ERROR] Unknown error." << std::endl;
					break;
				}
				if (line != -1) {
					std::cerr << " + line: " << line << std::endl;
				}
				exit(1);
			}
		}

		void createTexture(GLuint &texture, GLenum textureType, GLenum iFormat, GLenum format, int width, int height) {
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

		void createQuad(GLuint &VAO, GLuint &VBO) {
			/* Generation of the Quad VBO (to draw the Post Process image) */
			const GLfloat quadData[] = { -1.0f, -1.0f, 0.0f, 1.0f, -1.0f, 0.0f, -1.0f, 1.0f, 0.0f,
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
		void VAO::loadToGPU(std::vector<GLfloat> &vertices, std::vector<GLfloat> &normals, std::vector<GLfloat> &colors, std::vector<GLushort> &faces, GLenum mode) {
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
			glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), mode);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);
			glEnableVertexAttribArray(0);

			// Store mesh normals into buffer inside the GPU memory
			glBindBuffer(GL_ARRAY_BUFFER, VBO_Normals);
			glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(GLfloat), normals.data(), mode);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, 0, (void *)0);
			glEnableVertexAttribArray(1);

			// Store mesh colors into buffer inside the GPU memory
			glBindBuffer(GL_ARRAY_BUFFER, VBO_Colors);
			glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(GLfloat), colors.data(), mode);
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
		void GBuffer::initialize(int width, int height, GLenum textureType) {
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

			checkFrameBuffer(__LINE__);

			glBindFramebuffer(GL_FRAMEBUFFER, 0);

			createQuad(VAO_id, VBO_Quad);
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
		void ImageBuffer::initialize(int width, int height) {
			/* Texture */
			glActiveTexture(GL_TEXTURE0);
			createTexture(texture, GL_TEXTURE_2D, GL_RGBA32F, GL_RGBA, width, height);

			/* Framebuffer to link everything together */
			glGenFramebuffers(1, &id);
			glBindFramebuffer(GL_FRAMEBUFFER, id);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

			checkFrameBuffer(__LINE__);

			glBindFramebuffer(GL_FRAMEBUFFER, 0);

			createQuad(VAO_id, VBO_Quad);
		}

		void ImageBuffer::free() {
			glDeleteBuffers(1, &VBO_Quad);
			glDeleteTextures(1, &texture);
			glDeleteVertexArrays(1, &VAO_id);
			glDeleteFramebuffers(1, &id);
		}



		/* ======================================================================================================================================================== */
		/* ======================     [CStaticMesh]     =========================================================================================================== */
		/* ======================================================================================================================================================== */
		CStaticMesh::CStaticMesh(const char* OBJpath) {

			/* Fills the RAM with the mesh */
			std::string line;

			std::ifstream OBJfile(OBJpath, std::ios::in);
			if (!OBJfile) {
				std::cerr << "Cannot open: " << OBJpath << std::endl;
				exit(1);
			}

			GLfloat posX, posY, posZ, normalX, normalY, normalZ, colorR, colorG, colorB;
			GLushort vertexNdx1, vertexNdx2, vertexNdx3;
			while (getline(OBJfile, line)) {
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
			m_VAO = new VAO();
			m_VAO->loadToGPU(m_vertices, m_normals, m_colors, m_faces);
		}

		CStaticMesh::~CStaticMesh() {
			m_VAO->free();
		}




		/* ======================================================================================================================================================== */
		/* ======================     [CShader]     =============================================================================================================== */
		/* ======================================================================================================================================================== */
		CShader::~CShader() {
			if (glIsProgram(m_programId)) {
				glDeleteProgram(m_programId);
			}
		}

		void CShader::load(const char *vertex_file_path,
			const char *fragment_file_path) {

			// create and compile vertex shader object
			std::string vertexCode = getCode(vertex_file_path);
			const char * vertexCodeC = vertexCode.c_str();
			GLuint vertexId = glCreateShader(GL_VERTEX_SHADER);
			glShaderSource(vertexId, 1, &(vertexCodeC), NULL);
			glCompileShader(vertexId);
			std::cerr << vertex_file_path << " :" << std::endl;
			checkCompilation(vertexId);

			// create and compile fragment shader object
			std::string fragmentCode = getCode(fragment_file_path);
			const char * fragmentCodeC = fragmentCode.c_str();
			GLuint fragmentId = glCreateShader(GL_FRAGMENT_SHADER);
			glShaderSource(fragmentId, 1, &(fragmentCodeC), NULL);
			glCompileShader(fragmentId);
			std::cerr << fragment_file_path << " :" << std::endl;
			checkCompilation(fragmentId);

			// create, attach and link program object
			m_programId = glCreateProgram();
			glAttachShader(m_programId, vertexId);
			glAttachShader(m_programId, fragmentId);
			glLinkProgram(m_programId);
			checkLinks(m_programId);

			// delete vertex and fragment ids
			glDeleteShader(vertexId);
			glDeleteShader(fragmentId);
		}


		void CShader::reload(const char *vertex_file_path,
			const char *fragment_file_path) {

			// check if the program already contains a shader 
			if (glIsProgram(m_programId)) {
				// delete it...
				glDeleteProgram(m_programId);
			}

			// ... and reload it
			load(vertex_file_path, fragment_file_path);
		}

		void CShader::checkCompilation(GLuint shaderId) {
			// check if the compilation was successfull (and display syntax errors)
			// call it after each shader compilation
			GLint result = GL_FALSE;
			int infoLogLength;

			glGetShaderiv(shaderId, GL_COMPILE_STATUS, &result);
			glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &infoLogLength);

			if (infoLogLength > 0) {
				std::vector<char> message(infoLogLength + 1);
				glGetShaderInfoLog(shaderId, infoLogLength, NULL, &message[0]);
				std::cerr << &message[0] << std::endl;
			}
		}

		void CShader::checkLinks(GLuint programId) {
			// check if links were successfull (and display errors)
			// call it after linking the program  
			GLint result = GL_FALSE;
			int infoLogLength;

			glGetProgramiv(programId, GL_LINK_STATUS, &result);
			glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &infoLogLength);

			if (infoLogLength > 0) {
				std::vector<char> message(infoLogLength + 1);
				glGetProgramInfoLog(programId, infoLogLength, NULL, &message[0]);
				std::cerr << &message[0] << std::endl;
			}
		}

		std::string CShader::getCode(const char *file_path) {
			// return a string containing the source code of the input file
			std::string   shaderCode;
			std::ifstream shaderStream(file_path, std::ios::in);

			if (!shaderStream.is_open()) {
				std::cerr << "Unable to open " << file_path << std::endl;
				return "";
			}

			std::string line = "";
			while (getline(shaderStream, line))
				shaderCode += "\n" + line;
			shaderStream.close();

			return shaderCode;
		}




		/* ======================================================================================================================================================== */
		/* ======================     [Routines]     ============================================================================================================== */
		/* ======================================================================================================================================================== */
		void openWindowAndInitializeOpenGL(SDL_Window* SDL_WindowReference, SDL_GLContext* SDL_GLContextReference, const char* windowTitle, int width, int height) {
			/* Initialize SDL's Video subsystem */
			if (SDL_Init(SDL_INIT_VIDEO) < 0) {
				quit("Unable to initialize SDL"); // Or die on error
			}

			/* Request opengl 3.2 context.
			* SDL doesn't have the ability to choose which profile at this time of writing,
			* but it should default to the core profile */
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

			/* Turn on double buffering with a 24bit Z buffer.
			* You may need to change this to 16 or 32 for your system */
			SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
			SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

			/* Multi sampling to prevent aliasing effect, here MSAA x2
			* MSAA2: Creates 2 images of the edges with a little offset
			* and samples values between these two images (SSAA>MSAA>FSAA)*/
			//SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
			//SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

			/* Enable Anti-Aliasing filtering */
			//glEnable(GL_MULTISAMPLE);

			/* Make shure that the powerful GPU will execute this context
			* (Need when two GPU with Optimus for example) */
			SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);



			/* Create our window centered */
			SDL_WindowReference = SDL_CreateWindow(windowTitle, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
			if (!SDL_WindowReference) { /* Die if creation failed */
				quit("Unable to create window");
			}

			/* Hide the cursor (it's an FPS!) */
			SDL_ShowCursor(0);

			/* This makes our buffer swap syncronized with the monitor's vertical refresh */
			SDL_GL_SetSwapInterval(1);

			//checkSDLError(__LINE__);



			/* Create our opengl context and attach it to our window */
			*SDL_GLContextReference = SDL_GL_CreateContext(SDL_WindowReference);

			/* Sometimes needed to run in "Experimental" to have access to VBO and stuff */
			glewExperimental = GL_TRUE;
			GLenum status = glewInit();
			if (status != GLEW_OK) {
				std::cerr << "GLEW Error: " << glewGetErrorString(status) << "\n";
				quit("Exit on GLEW Error");
			}
			std::cerr << "----------------------------------------------------------------" << std::endl;
			std::cerr << "Graphics Successfully Initialized" << std::endl;
			std::cerr << "OpenGL Info" << std::endl;
			std::cerr << "    Version: " << glGetString(GL_VERSION) << std::endl;
			std::cerr << "     Vendor: " << glGetString(GL_VENDOR) << std::endl;
			std::cerr << "   Renderer: " << glGetString(GL_RENDERER) << std::endl;
			std::cerr << "    Shading: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
			std::cerr << "----------------------------------------------------------------" << std::endl;

			checkSDLError(__LINE__);
		}


		void initialiseDrawIntoBuffer(GLuint shader, GLuint fbo, GLenum* targets, unsigned short numberOfTargets) {
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


		void multiSamplingAntiAliasing(GBuffer* Aliased, GBuffer* Output, int width, int height) {
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, Output->id); // Result is going in the non aliased GBuffer
			glBindFramebuffer(GL_READ_FRAMEBUFFER, Aliased->id); // From the multi sampled aliased GBuffer
			glClearColor(0.5, 0.5, 0.5, 1.0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glDisable(GL_DEPTH);

			// Resolve color multisampling
			glReadBuffer(GL_COLOR_ATTACHMENT0);
			glDrawBuffer(GL_COLOR_ATTACHMENT0);
			glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

			// Resolve normal multisampling
			glReadBuffer(GL_COLOR_ATTACHMENT1);
			glDrawBuffer(GL_COLOR_ATTACHMENT1);
			glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

			// Resolve type multisampling
			glReadBuffer(GL_COLOR_ATTACHMENT2);
			glDrawBuffer(GL_COLOR_ATTACHMENT2);
			glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

			// Resolve depth multisampling
			glReadBuffer(GL_DEPTH_ATTACHMENT);
			glDrawBuffer(GL_DEPTH_ATTACHMENT);
			glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
		}


		/* Delete our opengl context, destroy our window, and shutdown SDL */
		void shutdownSDL_GL(SDL_Window* SDLWindow, SDL_GLContext GLContext) {
			SDL_GL_DeleteContext(GLContext);
			SDL_DestroyWindow(SDLWindow);
			SDL_Quit();
		}
	}
}