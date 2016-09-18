#include "CPostProcessPass.h"

// IMPORTANT: MAYBE WE NEED TO GLUSEPROGRAM(SHADERID) EVERY TIME THAT WE SEND SOMETHING... (FIXME)

namespace PixPhetamine {

	namespace PostProcess {

		void CPostProcessPass::sendTexture(const char* a_correspondingVariableNameInShader, const char* a_textureName, pxUInt16 a_location) {
			STACK_TRACE;

			// Retrieve the texture in the FBO
			LowLevelWrapper::CTexture* texture = m_inputFrame->getTexture(a_textureName);
			
			// Active and bind the correct texture
			glActiveTexture(GL_TEXTURE0 + m_inputFrame->getTextureAttachment(a_textureName));
			if (texture->isMultisampled()) {
				glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, texture->getID());
			}
			else {
				glBindTexture(GL_TEXTURE_2D, texture->getID());
			}
			
			// Send it to the shader
			glUniform1i(m_shader->getVariableAttachment(a_correspondingVariableNameInShader), a_location);

			STACK_MESSAGE("Sending " + std::string(a_correspondingVariableNameInShader));
			Utility::UErrorHandler::checkOpenGLErrors();
			UNSTACK_TRACE;
		}

		void CPostProcessPass::process() {
			STACK_TRACE;

			glBindFramebuffer(GL_FRAMEBUFFER, m_processedFrame->getId());
			glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);
			glDisable(GL_DEPTH_TEST);

			// Witch are the textures where we want to draw
			std::vector<GLenum> attachments;
			for (int i = 0; i < m_processedFrame->getNumberOfColorAttachments(); ++i) { // Adding GLenums for base textures
				attachments.push_back(GL_COLOR_ATTACHMENT0 + i);
			}
			if (m_processedFrame->isDepthAttached())	// Adding one more if there is a depth texture
				attachments.push_back(GL_DEPTH_ATTACHMENT);
			glDrawBuffers(attachments.size(), attachments.data()); // Sending the information to the GPU

			// Activate the current shader 
			glUseProgram(m_shader->id());

			// Draw call
			glBindVertexArray(m_quad.VAO_id);
			glDrawArrays(GL_TRIANGLES, 0, 6);
			glBindVertexArray(0);
 
			glUseProgram(0);

			UNSTACK_TRACE;
		}
	}
}