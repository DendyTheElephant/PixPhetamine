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
			if (textureType == GL_TEXTURE_2D) {
				glTexParameteri(textureType, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(textureType, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(textureType, GL_TEXTURE_WRAP_S, GL_REPEAT);
				glTexParameteri(textureType, GL_TEXTURE_WRAP_T, GL_REPEAT);
			}			
			
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

			STACK_MESSAGE("Checking Framebuffer errors");
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


		void initialiseDrawIntoBuffer(GLvramLocation shader, GLvramLocation fbo, GLenum* targets, unsigned short numberOfTargets) {
			glBindFramebuffer(GL_FRAMEBUFFER, fbo);

			/* Set color to clear color */
			//glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
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
			STACK_TRACE;
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, Output->id); // Result is going in the non aliased GBuffer
			glBindFramebuffer(GL_READ_FRAMEBUFFER, Aliased->id); // From the multi sampled aliased GBuffer
			glClearColor(0.5, 0.5, 0.5, 1.0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glDisable(GL_DEPTH_TEST);

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
			/*glReadBuffer(GL_DEPTH_ATTACHMENT);
			glDrawBuffer(GL_DEPTH_ATTACHMENT);
			glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
			if (GLenum status = glGetError() != GL_NO_ERROR) {
				switch (status) {
				case GL_INVALID_OPERATION:
					ERROR("Uniform error: See https://www.opengl.org/sdk/docs/man/html/glUniform.xhtml for more details");
					break;
				case GL_INVALID_FRAMEBUFFER_OPERATION:
					ERROR("ERROR HERE");
					break;
				default:
					ERROR("Uniform error: Unknown error");
					break;
				}
			}*/
			UNSTACK_TRACE;
		}
	}
}