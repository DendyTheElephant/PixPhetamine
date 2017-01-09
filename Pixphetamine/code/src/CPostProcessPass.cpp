#include "CPostProcessPass.h"

// IMPORTANT: MAYBE WE NEED TO GLUSEPROGRAM(SHADERID) EVERY TIME THAT WE SEND SOMETHING... (FIXME)

namespace PixPhetamine {

	namespace PostProcess {

		void CPostProcessPass::checkContext() const { 
			// Verify that the current binded shader is this one! (To optimise performance, prefer not to switch between shaders)
			// Active the shader with activate(); before sending textures...
			GLint currentShader, currentFBOOut;
			glGetIntegerv(GL_CURRENT_PROGRAM, &currentShader);
			if (currentShader != m_shader->id()) {
				glUseProgram(m_shader->id());
			}
			if (m_frameBuffer != nullptr) {
				glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING, &currentFBOOut);
				if (currentFBOOut != m_frameBuffer->getId())
				glBindFramebuffer(GL_FRAMEBUFFER, m_frameBuffer->getId());
			}
			else {
				glBindFramebuffer(GL_FRAMEBUFFER, 0);
			}
			
		}

		void CPostProcessPass::bindTexture(LowLevelWrapper::CTexture* a_textureToBind, const char* a_correspondingVariableNameInShader, pxUInt16 a_location) {
			STACK_TRACE;
			
			checkContext();
			
			// Send it to the shader
			m_shader->bindVariableName(a_correspondingVariableNameInShader);
			glUniform1i(m_shader->getVariableAttachment(a_correspondingVariableNameInShader), a_location);

			m_inTextureUnits.push_back(STextureUnit(a_location, a_textureToBind->getID(), a_textureToBind->isMultisampled()));

			STACK_MESSAGE("Sending " + std::string(a_correspondingVariableNameInShader));
			Utility::UErrorHandler::checkOpenGLErrors();

			UNSTACK_TRACE;
		}

		void CPostProcessPass::process(std::initializer_list<const char*> a_textureTargets) {
			STACK_TRACE;

			checkContext();

			glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);
			glDisable(GL_DEPTH_TEST);

			// Texture unit in bindings
			for (auto i_inTextureUnit = 0; i_inTextureUnit < m_inTextureUnits.size(); ++i_inTextureUnit) {
				STextureUnit textureUnit = m_inTextureUnits[i_inTextureUnit];
				// Active texture unit in shader and bind the correct texture
				glActiveTexture(GL_TEXTURE0 + textureUnit.binding);
				if (textureUnit.isMultisampled) {
					glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, textureUnit.textureID);
				}
				else {
					glBindTexture(GL_TEXTURE_2D, textureUnit.textureID);
				}
			}

			// Texture unit out bindings (FBOs)
			std::vector<GLenum> textureUnitsOut;
			// Witch are the textures where we want to draw
			for (auto &it_target : a_textureTargets) {
				if (m_frameBuffer == nullptr) {
					ERROR("Post process pass Error : No FBO attached but trying to draw into texture");
				}
				textureUnitsOut.push_back(m_frameBuffer->getTextureAttachment(it_target));
			}
			if (m_frameBuffer != nullptr) {
				glDrawBuffers(textureUnitsOut.size(), textureUnitsOut.data()); // Sending the output targets
			}

			// Draw call
			glBindVertexArray(m_quad.getVAO());
			glDrawArrays(GL_TRIANGLES, 0, 6);
			glBindVertexArray(0);
 
			// Unbind the shader
			glUseProgram(0);

			UNSTACK_TRACE;
		}
	}
}