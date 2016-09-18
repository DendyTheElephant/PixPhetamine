#include "CPostProcessPass.h"

// IMPORTANT: MAYBE WE NEED TO GLUSEPROGRAM(SHADERID) EVERY TIME THAT WE SEND SOMETHING... (FIXME)

namespace PixPhetamine {

	namespace PostProcess {

		void CPostProcessPass::sendTexture(LowLevelWrapper::CTexture* a_textureToSend, const char* a_correspondingVariableNameInShader, pxUInt16 a_location) {
			STACK_TRACE;
			
			// Verify that the current binded shader is this one! (To optimise performance, prefer not to switch between shaders)
			// Active the shader with activate(); before sending textures...
			GLint currentShader;
			glGetIntegerv(GL_CURRENT_PROGRAM, &currentShader);
			if (currentShader != m_shader->id()) {
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

		void CPostProcessPass::process(pxUInt16 a_numberOfOutputTextures, std::string a_outputTextureName ...) {
			STACK_TRACE;

			// Verify that the current binded shader is this one! (To optimise performance, prefer not to switch between shaders)
			// Active the shader with activate(); before sending textures...
			GLint currentShader;
			glGetIntegerv(GL_CURRENT_PROGRAM, &currentShader);
			if (currentShader != m_shader->id()) {
				activate();
			}

			glBindFramebuffer(GL_FRAMEBUFFER, m_frameBuffer->getId());
			glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);
			glDisable(GL_DEPTH_TEST);

			std::vector<GLenum> textureUnits;
			// Witch are the textures where we want to draw
			va_list targets;						// Args storage
			va_start(targets, a_outputTextureName);	// Init arguments list
			for (int i_target = 0; i_target < a_numberOfOutputTextures; ++i_target) {
				textureUnits.push_back(m_frameBuffer->getTextureAttachment(va_arg(targets, std::string).c_str()));
			}
			va_end(targets);						// Cleans up the list
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