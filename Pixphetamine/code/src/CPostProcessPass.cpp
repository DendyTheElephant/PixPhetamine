#include "CPostProcessPass.h"

// IMPORTANT: MAYBE WE NEED TO GLUSEPROGRAM(SHADERID) EVERY TIME THAT WE SEND SOMETHING... (FIXME)

namespace PixPhetamine {

	namespace PostProcess {

		void CPostProcessPass::sendTexture(LowLevelWrapper::CTexture* a_textureToSend, const char* a_correspondingVariableNameInShader, pxUInt16 a_location) {
			STACK_TRACE;
			
			// Verify that the current binded shader is this one! (To optimise performance, prefer not to switch between shaders)
			// Active the shader with activate(); before sending textures...
			GLint currentShader, currentFBOIn, currentFBOOut;
			glGetIntegerv(GL_CURRENT_PROGRAM, &currentShader);
			glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &currentFBOIn);
			glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING, &currentFBOOut);
			if (currentShader != m_shader->id() || currentFBOOut != m_frameBuffer->getId()) {
				activate();
			}


			// Active texture unit in shader and bind the correct texture
			glActiveTexture(GL_TEXTURE0 + a_location);
			if (a_textureToSend->isMultisampled()) {
				glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, a_textureToSend->getID());
			}
			else {
				glBindTexture(GL_TEXTURE_2D, a_textureToSend->getID());
			}
			
			// Send it to the shader
			glUniform1i(m_shader->getVariableAttachment(a_correspondingVariableNameInShader), a_location);

			STACK_MESSAGE("Sending " + std::string(a_correspondingVariableNameInShader));
			Utility::UErrorHandler::checkOpenGLErrors();

			UNSTACK_TRACE;
		}

		void CPostProcessPass::process(std::initializer_list<const char*> a_textureTargets) {
			STACK_TRACE;

			// Verify that the current binded shader is this one! (To optimise performance, prefer not to switch between shaders)
			// Active the shader with activate(); before sending textures...
			GLint currentShader, currentFBOIn, currentFBOOut;
			glGetIntegerv(GL_CURRENT_PROGRAM, &currentShader);
			glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &currentFBOIn);
			glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING, &currentFBOOut);
			if (currentShader != m_shader->id() || currentFBOOut != m_frameBuffer->getId()) {
				activate();
			}

			glClearColor(0.0f, 0.5f, 0.5f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);
			glDisable(GL_DEPTH_TEST);

			std::vector<GLenum> textureUnits;
			// Witch are the textures where we want to draw
			for (auto &it_target : a_textureTargets) {
				textureUnits.push_back(m_frameBuffer->getTextureAttachment(it_target));
			}
			glDrawBuffers(textureUnits.size(), textureUnits.data()); // Sending the output targets			

			// Draw call
			glBindVertexArray(m_quad.VAO_id);
			glDrawArrays(GL_TRIANGLES, 0, 6);
			glBindVertexArray(0);
 
			// Unbind the shader
			glUseProgram(0);

			// Unbind the FBO
			glBindFramebuffer(GL_FRAMEBUFFER, 0);

			UNSTACK_TRACE;
		}
	}
}