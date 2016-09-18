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
			glDeleteFramebuffers(1, &m_id);
		}


		void CFrameBuffer::addTexture(std::string const& a_textureName, ETextureType const& a_textureType) {
			STACK_TRACE;
			
			GLenum textureTarget;
			(m_isMultisampled) ? textureTarget = GL_TEXTURE_2D : textureTarget = GL_TEXTURE_2D_MULTISAMPLE;

			// Resolve attachment
			GLvramLocation attachment = m_textureAttachment.size();

			// Create attachment
			m_textureAttachment[a_textureName] = attachment;

			// Create texture
			glActiveTexture(GL_TEXTURE + attachment);
			m_texture[a_textureName] = new CTexture(m_width, m_height, a_textureType, m_isMultisampled);
			
			// Framebuffer link
			glBindFramebuffer(GL_FRAMEBUFFER, m_id);
			if (a_textureType == DEPTH) {
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, textureTarget, m_texture[a_textureName]->getID(), 0);
				m_isDepthAttached = true;
			} 
			else {
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + attachment, textureTarget, m_texture[a_textureName]->getID(), 0);
			}

			STACK_MESSAGE("Checking Framebuffer errors");
			checkFrameBufferErrors();

			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			
			UNSTACK_TRACE;
		}


		pxUInt16 CFrameBuffer::getNumberOfColorAttachments() const {
			pxUInt16 number = m_texture.size();
			if (m_isDepthAttached)
				number--;
			return number;
		}
	}
}