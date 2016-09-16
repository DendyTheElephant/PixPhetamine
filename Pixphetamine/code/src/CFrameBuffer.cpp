#include "CFrameBuffer.h"

namespace PixPhetamine {

	namespace LowLevelWrapper {

		void CFrameBuffer::checkFrameBufferErrors() {
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


		CFrameBuffer::CFrameBuffer(pxUInt16 const& a_width, pxUInt16 const& a_height, pxBool const& a_willBeMultisampled) {
			STACK_TRACE;
			m_isMultisampled = a_willBeMultisampled;
			m_width = a_width;
			m_height = a_height;

			glGenFramebuffers(1, &m_id);

			STACK_MESSAGE("Checking Framebuffer errors");
			checkFrameBufferErrors();
			UNSTACK_TRACE;
		}


		CFrameBuffer::~CFrameBuffer() {
			for (auto &it_texture : m_texture) {
				delete (it_texture.second);
			}
			for (auto &it_depthTexture : m_depthTexture) {
				delete (it_depthTexture.second);
			}
			glDeleteFramebuffers(1, &m_id);
		}


		void CFrameBuffer::addTexture(std::string const& a_textureName, ETextureType const& a_textureType) {
			STACK_TRACE;
			
			GLenum textureTarget;
			(m_isMultisampled) ? textureTarget = GL_TEXTURE_2D : textureTarget = GL_TEXTURE_2D_MULTISAMPLE;

			// Here we want special process for depth textures (because it's the way OpenGL handles it)
			if (a_textureType == DEPTH) {
				// Create depth attachment
				m_depthTextureAttachment[a_textureName] = m_depthTexture.size();
				
				// Create depth texture
				glActiveTexture(m_depthTextureAttachment[a_textureName]);
				m_depthTexture[a_textureName] = new CTexture(m_width, m_height, DEPTH, m_isMultisampled);

				// Framebuffer link
				glBindFramebuffer(GL_FRAMEBUFFER, m_id);
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, textureTarget, m_texture[a_textureName]->getID(), 0);
			}
			else {
				// Create attachment
				m_textureAttachment[a_textureName] = m_texture.size();

				// Create texture
				glActiveTexture(m_textureAttachment[a_textureName]);
				m_texture[a_textureName] = new CTexture(m_width, m_height, a_textureType, m_isMultisampled);

				// Framebuffer link
				glBindFramebuffer(GL_FRAMEBUFFER, m_id);
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + m_textureAttachment[a_textureName], textureTarget, m_texture[a_textureName]->getID(), 0);
			}

			STACK_MESSAGE("Checking Framebuffer errors");
			checkFrameBufferErrors();

			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			
			UNSTACK_TRACE;
		}

		/*void CFrameBuffer::resize(pxUInt16 a_width, pxUInt16 a_height, pxBool a_willBeInterpolated) {

		}*/
	}
}